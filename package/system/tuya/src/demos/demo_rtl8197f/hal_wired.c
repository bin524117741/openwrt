#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#include "tuya_gw_infra_api.h"
#include "tuya_gw_base_types.h"

#define WAN_IFNAME "eth0"

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

int hal_wired_get_ip(ip_info_s *ip)
{
    int sock = 0;
    char ipaddr[50] = {0};

    struct sockaddr_in *sin;
    struct ifreq ifr;

    if (ip == NULL) {
        log_err("invalid param");
        return -1;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
         log_err("socket create failed");
         return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, WAN_IFNAME, sizeof(ifr.ifr_name) - 1);

    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0 ) {
         log_err("ioctl failed");
         close(sock);
         return -1;
    }

    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    strcpy(ip->ip, inet_ntoa(sin->sin_addr)); 
    close(sock);

    return 0;
}

int hal_wired_get_status(int *stat)
{
    if (stat == NULL) {
        log_err("invalid param");
        return -1;
    }

    if (!tuya_user_iot_get_engineer_mode()) {
        get_wired_link_status(stat);
    } else {
        *stat = 1;
    }

    return 0;
}

int hal_wired_get_mac(mac_info_s *mac)
{
    int sock;
    struct sockaddr_in *sin;
    struct ifreq ifr;

    if (mac == NULL) {
        log_err("invalid param");
        return -1;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
         log_err("socket failed");
         return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, WAN_IFNAME, sizeof(ifr.ifr_name) - 1);

    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
        log_err("ioctl failed");
        close(sock);
        return -1;
    }

    memcpy(mac->mac, ifr.ifr_hwaddr.sa_data, sizeof(mac->mac));

    log_debug("WIFI MAC: %02X-%02X-%02X-%02X-%02X-%02X\r\n",
               mac->mac[0],mac->mac[1],mac->mac[2],mac->mac[3],mac->mac[4],mac->mac[5]);
    
    close(sock);

    return 0;
}
