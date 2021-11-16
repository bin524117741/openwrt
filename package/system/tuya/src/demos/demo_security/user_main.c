#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "tuya_gw_hs_api.h"
#include "tuya_gw_infra_api.h"

/* must apply for uuid, authkey and product key from tuya iot develop platform */
#define UUID         "tuya461dbc63aeeb991f"
#define AUTHKEY      "c8X4PR4wx1gMFaQlaZu5dfgVvVRwB8Ug"
#define PRODUCT_KEY  "fljmamufiym5fktz"

static int _iot_get_uuid_authkey_cb(char *uuid, int uuid_size, char *authkey, int authkey_size)
{
    strncpy(uuid, UUID, uuid_size);
    strncpy(authkey, AUTHKEY, authkey_size);

    return 0;
}

static int _iot_get_product_key_cb(char *pk, int pk_size)
{
    strncpy(pk, PRODUCT_KEY, pk_size);

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

void _home_security_alarm_cb(void)
{
    log_debug("home security alarm callback");

    return;
}

void _home_security_alarm_cancel_cb(void)
{
    log_debug("home security alarm cancel callback");

    return;
}

void _home_security_alarm_mute_cb(int mute_on)
{
    log_debug("home security alarm mute callback");

    return;
}

void _home_security_disarmed_cb(void)
{
    log_debug("home security disarmed callback");

    return;
}

void _home_security_away_armed_cb(void)
{
    log_debug("home security away armed callback");

    return;
}

void _home_security_stay_armed_cb(void)
{
    log_debug("home security stay armed callback");

    return;
}

void _home_security_arm_ignore_cb(void)
{
    log_debug("home security arm ignore callback");

    return;
}

void _home_security_arm_countdown_cb(uint32_t time)
{
    log_debug("home security arm countdown callback");
    log_debug("countdown time: %d", time);

    return;
}

void _home_security_alarm_countdown_cb(uint32_t time)
{
    log_debug("home security alarm countdown callback");
    log_debug("countdown time: %d", time);

    return;
}

void _home_security_door_opened_cb(void)
{
    log_debug("home security door opened callback");

    return;
}

void _home_security_alarm_dev_cb(char *dev)
{
    log_debug("home security alarm dev callback");

    return;
}

int main(int argc, char **argv)
{
    int ret = 0;

    ty_gw_attr_s gw_attr = {
        .storage_path     = "./",
        .cache_path       = "/tmp/",
        .tty_device       = "/dev/ttyUSB0",
        .tty_baudrate     = 57600,
        .ver              = "1.0.0",
        .log_level        = TY_LOG_DEBUG
    };

    ty_gw_infra_cbs_s gw_infra_cbs = {
        .get_uuid_authkey_cb         = _iot_get_uuid_authkey_cb,
        .get_product_key_cb          = _iot_get_product_key_cb,
        .gw_fetch_local_log_cb       = _gw_fetch_local_log_cb,
        .gw_reboot_cb                = _gw_reboot_cb,
        .gw_reset_cb                 = _gw_reset_cb,
        .gw_upgrade_cb               = _gw_upgrade_cb,
        .gw_active_status_changed_cb = _gw_active_status_changed_cb,
        .gw_online_status_changed_cb = _gw_online_status_changed_cb,
    };

    ty_home_security_cbs_s security_cbs = {
    	.home_security_alarm_cb           = _home_security_alarm_cb,
    	.home_security_alarm_cancel_cb    = _home_security_alarm_cancel_cb,
        .home_security_alarm_mute_cb      = _home_security_alarm_mute_cb,
    	.home_security_disarmed_cb        = _home_security_disarmed_cb,
    	.home_security_away_armed_cb      = _home_security_away_armed_cb,
    	.home_security_stay_armed_cb      = _home_security_stay_armed_cb,
    	.home_security_arm_ignore_cb      = _home_security_arm_ignore_cb,
    	.home_security_arm_countdown_cb   = _home_security_arm_countdown_cb,
    	.home_security_alarm_countdown_cb = _home_security_alarm_countdown_cb,
    	.home_security_door_opened_cb     = _home_security_door_opened_cb,
    	.home_security_alarm_dev_cb       = _home_security_alarm_dev_cb,
    };

    tuya_user_iot_pre_init();

    ret = tuya_user_iot_reg_home_security_cb(&security_cbs);
    if (ret != 0) {
        log_err("tuya_user_iot_reg_home_security_cb err: %d", ret);
        return ret;
    }

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
