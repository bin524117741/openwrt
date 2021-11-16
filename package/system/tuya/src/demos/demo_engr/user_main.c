#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

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

static void _gw_engineer_finished_cb(void)
{
    log_debug("reboot the program, and switch to normal mode");
}

int main(int argc, char **argv)
{
    int opt = 0, ret = 0;

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
        .storage_path = "./",
        .cache_path = "/tmp/",
        .tty_device = "/dev/ttyUSB0",
        .tty_baudrate = 57600,
        .ver = "1.0.0",
        .is_engr = engr_mode,
        .log_level = TY_LOG_DEBUG
    };

    ty_gw_infra_cbs_s gw_infra_cbs = {
        .get_uuid_authkey_cb = _iot_get_uuid_authkey_cb,
        .get_product_key_cb = _iot_get_product_key_cb,
        .gw_fetch_local_log_cb = _gw_fetch_local_log_cb,
        .gw_engineer_finished_cb = _gw_engineer_finished_cb,
        .gw_reboot_cb = _gw_reboot_cb,
        .gw_reset_cb = _gw_reset_cb,
        .gw_upgrade_cb = _gw_upgrade_cb,
        .gw_active_status_changed_cb = _gw_active_status_changed_cb,
        .gw_online_status_changed_cb = _gw_online_status_changed_cb,
    };

    tuya_user_iot_pre_init();

    ret = tuya_user_iot_init(&gw_attr, &gw_infra_cbs);
    if (ret != 0) {
        log_err("tuya_user_iot_init failed");
        return ret;
    }

    while (1) {
        sleep(10);
    }

    return 0;
}
