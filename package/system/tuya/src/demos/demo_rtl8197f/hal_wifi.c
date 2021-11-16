#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>

#include "pcap.h"

#include "tuya_gw_wifi_types.h"
#include "tuya_gw_infra_api.h"

#define default_wifi_ip  "192.168.133.1"

struct rtl_wifi_header {
    uint16_t pkt_len;
    uint16_t payload_len;
    uint8_t data[252];
} __attribute__((packed));

typedef struct wlan_80211_head {
    uint8_t frmae_ctl1;
    uint8_t frmae_ctl2;
    uint16_t duration;
    uint8_t addr1[6];
    uint8_t addr2[6];
    uint8_t addr3[6];
    uint16_t seq_ctl;
    uint8_t addr4[6];
} WLAN_80211_HEAD_S;

#define WLAN_DEV       "wlan0"
#define MAX_SCAN_NUM   35
#define MAX_BUF        128
#define MAX_REV_BUF    512
#define MAX_REV_BUFFER 1024

static int g_cur_mode        = 0;
static int g_cur_channel     = 1;
static int g_sniffer_enabled = 0;
static int g_sniffer_thr_run = 0;

static pcap_t *device        = NULL;
static pthread_t sniffer_tid = 0;

static sniffer_callback sniffer_cb = NULL;

static int run_command(const char *cmd, char *data, int len)
{
    FILE *fp = NULL;

    fp = popen(cmd, "r");
    if (fp == NULL) {
        log_err("popen failed");
        return -1;
    }

    if (data != NULL) {
        memset(data, 0, len);

        fread(data, len, 1, fp);
        len = strlen(data);
        if (data[len - 1] == '\n') {
            data[len - 1] = '\0';
        }    
    }

    pclose(fp);

    return 0;
}

static int exec_cmd(const char *format, ...)
{
    va_list valist;
    char cmd[MAX_BUF]={0};

    va_start(valist, format);
    vsnprintf((char *)cmd, sizeof(cmd), format, valist);
    va_end (valist);

    system((char *)cmd);

    return 0;
}

static void get_wired_link_status(int *status)
{
    char buffer[10] = {0};

    run_command("sed -n '3p' /proc/rtl865x/port_status|grep 'LinkUp'", buffer, sizeof(buffer));
    if (0 == strlen(buffer)) {
        *status = 0;
    } else {
        *status = 1;
    }
}

static void enable_monitor_mode(void)
{
    int dl = 0;
    int status = 0;
    char errbuf[PCAP_ERRBUF_SIZE] = {0};

    device = pcap_create(WLAN_DEV, errbuf); 
    if (device == NULL) {
        log_err("pcap_create failed");
        return;
    }

    pcap_set_rfmon(device, 1);
    pcap_set_snaplen(device, 2048);
    pcap_set_promisc(device, 1);      	 
    
    status = pcap_activate(device);
    if (status < 0) {
        log_warn("pcap status: %d", status);
    }

    dl = pcap_datalink(device);

    log_debug("data link type: %s", pcap_datalink_val_to_name(dl));        
}

static void disable_monitor_mode(void)
{
    if (device != NULL) {         
        pcap_close(device);
        device = NULL;
    }
}

/**
 * frame_ctl1 (uint8_t)
 *     Verion: mask 0x03
 *     Type: mask 0x0C
 *         00: Management Frame
 *         01: Control Frame
 *         10: Data Frame
 *         11: Reserved
 *     Subtype: mask 0xF0
 *         0000: Association Request
 *         0100: Probe Request
 *         1000: Beacon
 *         1010: Disassociation
 *
 * frame_ctl2 (uint8_t) - Frame Control Flags
 *     To DS: mask 0x01
 *     From DS: mask 0x02
 *         +-----------------------------------------------------------------+
 *         | To Ds | From Ds | Address 1 | Address 2 | Address 3 | Address 4 |
 *         +-----------------------------------------------------------------+
 *         | 0     | 0       | dest      | src       | bssid     | N/A       |
 *         +-----------------------------------------------------------------+
 *         | 0     | 1       | dest      | bssid     | src       | N/A       |
 *         +-----------------------------------------------------------------+
 *         | 1     | 0       | bssid     | src       | dest      | N/A       |
 *         +-----------------------------------------------------------------+
 *         | 1     | 1       | RA        | TA        | DA        | SA        |
 *         +-----------------------------------------------------------------+
 *     More Frag: mask 0x04
 *     Retry: mask 0x08
 *     Power Mgmt: mask 0x10
 *     More Data: mask 0x20
 *     Protected Frame: mask 0x40
 *     Order: mask 0x80
 *
 */
static int wlan_80211_packet_parse(uint8_t *buf, uint32_t len)
{
    WLAN_80211_HEAD_S *wbuf = (WLAN_80211_HEAD_S *)buf;

    /* version must be 0x00 */
    if ((wbuf->frmae_ctl1 & 0x03) != 0x00) {
        return -1;
    }

    switch ((wbuf->frmae_ctl1) & 0x0c) {
        case 0x08: {
            /* Data Frame */
            break;
        }
        default: {
            return -1;
        }
    }

    /* From DS = 1 */
    if ((wbuf->frmae_ctl2 & 0x3) == 0x2) {
        /* Destination Address */
        if (!memcmp(wbuf->addr1, "\xff\xff\xff\xff\xff\xff", 6) || \
            !memcmp(wbuf->addr1, "\x01\x00\x5e", 3)) {
            return 0;
        }
    }

    return -1;
}

static void wlan_80211_packet_process(uint8_t *user, const struct pcap_pkthdr *h, const uint8_t *buf)
{
    struct rtl_wifi_header *rh = (struct rtl_wifi_header *)buf;

    if ((rh->pkt_len > 0) && (rh->data[0] != 0xff)) {
        if (wlan_80211_packet_parse(rh->data, rh->pkt_len) == 0) {
            sniffer_cb(rh->data, rh->pkt_len, 99);
        }
    }
}

static void *sniffer_process(void *arg)
{
    g_sniffer_thr_run = 1;

    if (device != NULL) {
        log_debug("pcap loop");
        pcap_loop(device, -1, wlan_80211_packet_process, NULL);
    }

    g_sniffer_thr_run = 0;
}

int hal_wifi_scan_all_ap(ap_scan_info_s **aps, uint32_t *num)
{
    FILE *fp = NULL;
    int idx = -1;
    ap_scan_info_s s_aps[MAX_SCAN_NUM];

    *aps = s_aps;
    *num = 0;

    log_debug("scanning AP...");

    memset(s_aps, 0, sizeof(s_aps));

    exec_cmd("iwpriv wlan0 at_ss && sleep 2");

    fp = fopen("/proc/wlan0/SS_Result", "r");
    if (fp == NULL) {
        log_err("fopen failed");
        return -1;
    }
    
    while (1) {
        char *ret = NULL;
        uint8_t rbuf[MAX_BUF] = {0};

        ret = fgets(rbuf, MAX_BUF, fp);
        if (ret == NULL) {
            break;
        }

        char *op_ptr = &rbuf[0];

        op_ptr = strstr(op_ptr, "HwAddr");
        if (op_ptr != NULL) {
            idx++;

            /* bssid */
            int x1,x2,x3,x4,x5,x6;
            sscanf(op_ptr + strlen("HwAddr: "), "%02x%02x%02x%02x%02x%02x", &x1, &x2, &x3, &x4, &x5, &x6);
            s_aps[idx].bssid[0] = x1 & 0xFF;
            s_aps[idx].bssid[1] = x2 & 0xFF;
            s_aps[idx].bssid[2] = x3 & 0xFF;
            s_aps[idx].bssid[3] = x4 & 0xFF;
            s_aps[idx].bssid[4] = x5 & 0xFF;
            s_aps[idx].bssid[5] = x6 & 0xFF;

            /* channel */
            memset(rbuf, 0, MAX_BUF);
            ret = fgets(rbuf, MAX_BUF, fp);
            if (ret == NULL) {
                break;
            }

            op_ptr = strstr(op_ptr,"Channel");
            if (op_ptr == NULL) {
                break;
            }

            sscanf(op_ptr + strlen("Channel: "), "%d", &(s_aps[idx].channel));

            /* ssid */
            memset(rbuf, 0, MAX_BUF);
            ret = fgets(rbuf, MAX_BUF, fp);
            if (ret == NULL) {
                break;
            }

            op_ptr = strstr(op_ptr, "SSID");
            if (op_ptr == NULL) {
                break;
            }

            sscanf(op_ptr + strlen("SSID: "), "%s", s_aps[idx].ssid);
            s_aps[idx].s_len = strlen(s_aps[idx].ssid);
        } else {
            continue;
        }
    }
   
    if (idx == -1) {
        log_warn("Not AP Found");
        return -1;
    }

    *num = idx + 1;

    return 0;
}

int hal_wifi_scan_assigned_ap(char *ssid, ap_scan_info_s **ap)
{
    ap_scan_info_s *aps = NULL;
    uint32_t num = 0;
    int index = 0, ret = 0;

    log_debug("scanning AP...");

    *ap = NULL;

    ret = hal_wifi_scan_all_ap(&aps, &num);
    if (ret != 0) {
        return ret;
    }

    for (index = 0; index < num; index++) {
        if (strncmp(aps[index].ssid, ssid, aps[index].s_len) == 0) {
            *ap = aps + index;
            return 0;
        }
    }

    return -1;
}

int hal_wifi_release_ap(ap_scan_info_s *ap)
{
    return 0;
}

int hal_wifi_set_cur_channel(uint8_t channel)
{
    exec_cmd("iwconfig wlan0 channel %d", channel);

    g_cur_channel = channel;

    return 0;
}

int hal_wifi_get_cur_channel(uint8_t *channel)
{
    if (channel == NULL) {
        log_err("invalid param");
        return -1;
    }

    *channel = g_cur_channel;

    return 0;
}

int hal_wifi_set_sniffer(int en, sniffer_callback cb)
{
    int ret = 0;

    log_debug("set sniffer, enable: %d", en);

    if (en) {
        if (g_sniffer_enabled) {
            if (!g_sniffer_thr_run) {
                log_debug("start sniffer");

                disable_monitor_mode();
                enable_monitor_mode();

                ret = pthread_create(&sniffer_tid, NULL, sniffer_process, NULL);
                if (ret != 0) {
                    log_err("pthread_create failed");
                    return ret;
                }
            }
        } else {
            log_debug("start sniffer");

            exec_cmd("ifconfig wlan0 0.0.0.0");
            exec_cmd("iwpriv wlan0 set_mib opmode=16");
            exec_cmd("ifconfig wlan0 up");

            sniffer_cb = cb;

            enable_monitor_mode();

            ret = pthread_create(&sniffer_tid, NULL, sniffer_process, NULL);
            if (ret != 0) {
                log_err("pthread_create failed");
                return ret;
            }

            g_sniffer_enabled = 1;
        }
    } else {
        if (g_sniffer_enabled) {
            g_sniffer_enabled = 0;

            exec_cmd("ifconfig wlan0 down");

            /* wait pthread exit */
            while (1) {
                if (!g_sniffer_thr_run) {
                    disable_monitor_mode();
                    break;
                }

                usleep(1000*10);
            }

            log_debug("stop sniffer");
        }
    }

    return 0;
}

int hal_wifi_get_ip(wifi_type_t type, ip_info_s *ip)
{
    int status = 0;
    char buf[256] = {0};
    char *pstart = NULL;
    FILE *pp = NULL;
    
    if (tuya_user_iot_get_engineer_mode()) {
        get_wired_link_status(&status);
        if (status) {
            // ethernet is connected
            pp = popen("ifconfig eth0", "r");
        } else {
            pp = popen("ifconfig wlan0", "r");
        }
    } else {
        pp = popen("ifconfig wlan0", "r");
    }

    if (pp == NULL) {
        log_err("popen failed");
        return -1;
    }

    while (fgets(buf, sizeof(buf), pp) != NULL) {
        pstart = strstr(buf, "inet ");
        if (pstart != NULL) {
            break;
        }
    }

    pstart = strstr(buf, "inet addr:");
    if (pstart != NULL) {
        sscanf(pstart + strlen("inet addr:"), "%s", ip->ip);
    } else {        
        pclose(pp);
        return -1;
    }

    pclose(pp);

    if (!tuya_user_iot_get_engineer_mode()) {
        if (!strcmp(ip->ip, default_wifi_ip)) {
            return -1;
        }
    }

    return 0;
}

int hal_wifi_get_mac(wifi_type_t type, mac_info_s *mac)
{
    char buf[256] = {0};
    char *pstart = NULL;
    FILE *pp = NULL;
    
    log_debug("get mac address");

    pp = popen("ifconfig wlan0", "r");
    if (pp == NULL) {
        log_err("popen failed");
        return -1;
    }

    while (fgets(buf, sizeof(buf), pp) != NULL) {
        pstart = strstr(buf, "HWaddr ");
        if (pstart != NULL) {
            int x1, x2, x3, x4, x5, x6;
            sscanf(pstart + strlen("HWaddr "), "%x:%x:%x:%x:%x:%x", &x1, &x2, &x3, &x4, &x5, &x6);
            mac->mac[0] = x1 & 0xFF;
            mac->mac[1] = x2 & 0xFF;
            mac->mac[2] = x3 & 0xFF;
            mac->mac[3] = x4 & 0xFF;
            mac->mac[4] = x5 & 0xFF;
            mac->mac[5] = x6 & 0xFF;
            break;
        }
    }

    pclose(pp);

    log_debug("MAC: %02X-%02X-%02X-%02X-%02X-%02X", mac->mac[0], mac->mac[1], mac->mac[2], mac->mac[3], mac->mac[4], mac->mac[5]);

    return 0;
}

int hal_wifi_set_mac(wifi_type_t type, mac_info_s *mac)
{
    return 0;
}

int hal_wifi_set_work_mode(wifi_work_mode_t mode)
{
    log_debug("set work mode, mode: %d", mode);

    switch (mode) {
        case WMODE_SNIFFER:
            g_cur_mode = WMODE_SNIFFER;
            break;
        case WMODE_STATION:
            g_cur_mode = WMODE_STATION;
            break;
        case WMODE_SOFTAP:
            g_cur_mode = WMODE_SOFTAP;
            break;
        default:
            break;
    }

    return 0;
}

int hal_wifi_get_work_mode(wifi_work_mode_t *mode)
{
    if (mode == NULL) {
        log_err("invalid param");
        return -1;
    }

    *mode = g_cur_mode;

    return 0;
}

int hal_wifi_connect_station(char *ssid, char *passwd)
{
    log_debug("connect station");

    if (ssid == NULL) {
        log_err("ssid is null");
        return -1;
    }

    g_cur_mode = WMODE_STATION;	

    exec_cmd("killall udhcpd");            
    exec_cmd("ifconfig wlan0 down");    
    exec_cmd("iwconfig wlan0 mode managed");
    exec_cmd("iwpriv wlan0 set_mib opmode=8");
    exec_cmd("iwpriv wlan0 set_mib band=11");
    exec_cmd("iwpriv wlan0 set_mib deny_legacy=0");
    exec_cmd("iwpriv wlan0 set_mib use40M=1");
    exec_cmd("iwpriv wlan0 set_mib ssid=\"%s\"",ssid);
    exec_cmd("iwpriv wlan0 set_mib 802_1x=0");
   
    if (!passwd || (strlen(passwd) == 0)) {
        log_debug(">>>>>>>> CONNECT AP WITHOUT PASSWORD");
        exec_cmd("iwpriv wlan0 set_mib authtype=0");
        exec_cmd("iwpriv wlan0 set_mib encmode=0");
    } else {
        log_debug(">>>>>>>> CONNECT AP WITH PASSWORD");
        exec_cmd("iwpriv wlan0 set_mib authtype=2");
        exec_cmd("iwpriv wlan0 set_mib psk_enable=3");
        exec_cmd("iwpriv wlan0 set_mib encmode=2");
        exec_cmd("iwpriv wlan0 set_mib wpa2_cipher=10");
        exec_cmd("iwpriv wlan0 set_mib wpa_cipher=10");
        exec_cmd("iwpriv wlan0 set_mib passphrase=\"%s\"", passwd);
    }
    
    exec_cmd("ifconfig wlan0 up");

    exec_cmd("ps | grep 'udhcpc -i wlan0' | grep -v grep | awk '{print $1}' | xargs kill -9");    
    exec_cmd("ps | grep 'udhcpc -b -i wlan0' | grep -v grep | awk '{print $1}' | xargs kill -9");
    exec_cmd("busybox udhcpc -b -i wlan0 -A 3 -T 3 -s /tmp/tuya/8197wlan0_udhcpc.script&");

    return 0;
}

int hal_wifi_disconnect_station(void)
{
    log_debug("disconnect station");

    return 0;
}

int hal_wifi_get_station_rssi(char *rssi)
{
    int value = 0;
    char buf[MAX_BUF] = {0};
    char *pstart = NULL;

    if (rssi == NULL) {
        log_err("invalid param");
        return -1;
    }

    *rssi = 0;
      
    run_command("cat /proc/wlan0/sta_info | grep rssi", buf, MAX_BUF);

    pstart = &buf[0];
    pstart = strstr(pstart, "rssi");
    if (pstart == NULL) {
        log_err("get rssi failed");
        return -1;
    }

    sscanf(pstart + strlen("rssi: "), "%d", &value);

    *rssi = value;    

    log_debug("AP RSSI: %d", *rssi);

    return 0;
}

int hal_wifi_get_station_mac(mac_info_s *mac)
{
    return 0;
}

int hal_wifi_get_station_conn_stat(station_conn_stat_t *stat)
{
    ip_info_s ip = {0};
    char buf[MAX_BUF] = {0};

    if (stat == NULL) {
        log_err("invalid param");
        return -1;
    }

    if (hal_wifi_get_ip(STATION, &ip) != 0) {
        *stat = STAT_CONN_FAIL;
    } else {
        *stat = STAT_GOT_IP;
    }

    return 0;
}

int hal_wifi_ap_start(ap_cfg_info_s *cfg)
{
    log_debug("start ap");

    if (cfg == NULL) {
        log_err("invalid param");
        return -1;
    }

    g_cur_mode = WMODE_SOFTAP;

    exec_cmd("killall udhcpd");    
    exec_cmd("udhcpd /tmp/tuya/udhcpd.conf");    
    exec_cmd("ifconfig wlan0 down");
    exec_cmd("iwconfig wlan0 mode master");
    exec_cmd("iwpriv wlan0 set_mib opmode=16");
    exec_cmd("iwpriv wlan0 set_mib band=11");
    exec_cmd("iwpriv wlan0 set_mib deny_legacy=0");
    exec_cmd("iwpriv wlan0 set_mib use_40M=1");
    exec_cmd("iwpriv wlan0 set_mib 802_1x=0");  
    exec_cmd("iwpriv wlan0 set_mib ssid=\"%s\"", cfg->ssid);

    if (cfg->p_len > 0) {        
        exec_cmd("iwpriv wlan0 set_mib authtype=2");        
        exec_cmd("iwpriv wlan0 set_mib psk_enable=3");
        exec_cmd("iwpriv wlan0 set_mib encmode=2");        
        exec_cmd("iwpriv wlan0 set_mib wpa2_cipher=10");
        exec_cmd("iwpriv wlan0 set_mib wpa_cipher=10");        
        exec_cmd("iwpriv wlan0 set_mib passphrase=\"%s\"", cfg->key);
    } else {
        exec_cmd("iwpriv wlan0 set_mib authtype=0");
        exec_cmd("iwpriv wlan0 set_mib encmode=0");
    }

    exec_cmd("ifconfig wlan0 192.168.133.1");
    exec_cmd("killall udhcpd");    
    exec_cmd("udhcpd /tmp/tuya/udhcpd.conf"); 

    log_debug("AP started, SSID: %s", cfg->ssid);

    return 0;
}

int hal_wifi_ap_stop(void)
{
    log_debug("stop ap");

    exec_cmd("ifconfig wlan0 down");

    return 0;
}

int hal_wifi_set_country_code(int code)
{
    return 0;
}
