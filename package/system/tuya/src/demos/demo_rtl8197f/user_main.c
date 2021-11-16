#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "sys_timer.h"
#include "tuya_gw_infra_api.h"

/* must apply for uuid, authkey and product key from tuya iot develop platform */
// #define UUID         "tuya461dbc63aeeb991f"
// #define AUTHKEY      "c8X4PR4wx1gMFaQlaZu5dfgVvVRwB8Ug"
// #define PRODUCT_KEY  "fljmamufiym5fktz"
#define UUID         "65df704231d1b1b5"
#define AUTHKEY      "RjEDo7swB9cFIvB5R5eViw2E2OLPBTdz"

static int engr_mode = 0;

static void usage(void)
{
    printf("usage:\r\n");
    printf("  u: unactive gw unittest\r\n");
    printf("  p: permit device join unittest\r\n");
    printf("  q: exit \r\n");
    printf("\r\n"); 
}

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

STATIC VOID eth_send_dhcp(VOID)
{
    system("ps | grep 'udhcpc -i eth0' | grep -v grep | awk '{print $1}' |xargs kill -9");
    system("ps | grep 'udhcpc -b -i eth0' | grep -v grep | awk '{print $1}' |xargs kill -9");
    system("busybox udhcpc -b -i eth0 -A 3 -T 3 -s /tmp/tuya/8197wlan0_udhcpc.script &");
}

STATIC VOID ethernet_link_status(INT_T *status)
{
    CHAR_T buffer[10] = {0};

    run_command("sed -n '3p' /proc/rtl865x/port_status|grep 'LinkUp'", buffer, SIZEOF(buffer));
    if (0 == strlen(buffer)){
        *status = false;
    } else {
        *status = true;
    }
}

STATIC VOID _ethernet_ip_monitor_cb(UINT_T timerID, PVOID_T pTimerArg)
{
    STATIC INT_T pre_status = 0xff;
    INT_T cur_status = 0;

    ethernet_link_status(&cur_status);

    if (pre_status == cur_status)
        return;

    log_debug("ethernet status changed");

    if (cur_status) {
        /* up */
        log_debug("link is connected, send dhcp request");
        eth_send_dhcp();
    } else {
        /* down */
        log_debug("link is disconnected, clear IP address");
        linux_exec_cmd("ifconfig eth0 0.0.0.0", NULL, 0);
    }

    pre_status = cur_status;
}

static int _iot_get_uuid_authkey_cb(char *uuid, int uuid_size, char *authkey, int authkey_size)
{
    strncpy(uuid, UUID, uuid_size);
    strncpy(authkey, AUTHKEY, authkey_size);

    return 0;
}

static int _iot_get_product_key_cb(char *pk, int pk_size)
{
    if (engr_mode) {
        strncpy(pk, "gpy8uw1iqwooyg9j", pk_size);
    } else {
        strncpy(pk, "dn6an7mvbrnxgpi7", pk_size);
    }

    return 0;
}

static void _gw_reboot_cb(void)
{
    log_debug("gw reboot callback");
    /* USER TODO */

    exit(0);

    return;
}

static void _gw_reset_cb(void)
{
    log_debug("gw reset callback");
    /* USER TODO */

    return;
}

static void _gw_upgrade_cb(const char *img)
{
    log_debug("gw upgrade callback");
    
    if (img != NULL) {
        log_debug("img: %s", img);
    }

    return;
}

static int _gw_active_status_changed_cb(ty_gw_status_t status)
{
    log_debug("active status changed, status: %d", status);

    return 0;
}

static int _gw_online_status_changed_cb(bool online)
{
    log_debug("online status changed, online: %d", online);

    return 0;
}

static int _gw_fetch_local_log_cb(char *path, int path_len)
{
    char cmd[256] = {0};

    log_debug("gw fetch local log callback");
    /* USER TODO */

    /* snprintf(path, path_len, "/tmp/tuya.log"); */

    return 0;
}

static void test_tuya_user_iot_unactive_gw(void)
{
    tuya_user_iot_unactive_gw();

    return;
}

static void test_tuya_user_iot_permit_join(void)
{
    static bool permit = false;
    int ret = 0;

    permit ^= 1;

    ret = tuya_user_iot_permit_join(permit, 256);
    if (ret != 0) {
        log_err("tuya_user_iot_permit_join error, ret: %d", ret);
        return;
    }

    return;
}

static void _gw_engineer_finished_cb(void)
{
    log_debug("reboot the program, and switch to normal mode");
}

int main(int argc, char **argv)
{
    int opt = 0, ret = 0;
    TIMER_ID ip_monitor_tm = 0;
    char line[256] = {0};

    while ((opt = getopt(argc, argv, "e")) != -1) {
        switch (opt) {
        case 'e':
            /* switch to engineering mode */
            engr_mode = 1;
            break;
        default:
            break;
        }
    }

    /* must reset the opt */
    optind = 0;

    ty_gw_attr_s gw_attr = {
        .storage_path   = "./",
        .cache_path     = "/tmp/",
        .tty_device     = "/dev/ttyS2",
        .tty_baudrate   = 115200,
        .is_fork        = 1,
        .ver            = "1.0.0",
        .is_engr        = engr_mode,
        .log_level      = TY_LOG_DEBUG
    };

    ty_gw_infra_cbs_s gw_infra_cbs = {
        .get_uuid_authkey_cb         = _iot_get_uuid_authkey_cb,
        .get_product_key_cb          = _iot_get_product_key_cb,
        .gw_fetch_local_log_cb       = _gw_fetch_local_log_cb,
        .gw_engineer_finished_cb     = _gw_engineer_finished_cb,
        .gw_reboot_cb                = _gw_reboot_cb,
        .gw_reset_cb                 = _gw_reset_cb,
        .gw_upgrade_cb               = _gw_upgrade_cb,
        .gw_active_status_changed_cb = _gw_active_status_changed_cb,
        .gw_online_status_changed_cb = _gw_online_status_changed_cb,
    };

    tuya_user_iot_pre_init();

    ret = tuya_user_iot_init(&gw_attr, &gw_infra_cbs);
    if (ret != 0) {
        log_err("tuya_user_iot_init failed");
        return ret;
    }

    ret = sys_add_timer(_ethernet_ip_monitor_cb, NULL, &ip_monitor_tm);
    if (ret != 0) {
        log_err("sys_add_timer error, ret: %d", ret);
        return ret;
    }

    ret = sys_start_timer(ip_monitor_tm, (2*1000), TIMER_CYCLE); // 2 seconds
    if (ret != 0) {
        log_err("sys_start_timer error, ret: %d", ret);
        return ret;
    }

    while (1) {
        sleep(10);
    }

    return 0;
}
