/**
 * @file    tuya_gw_infra_api.h
 * @brief   This file contains gateway base interface
 * @version 0.1
 * 
 * @copyright Copyright 2021 Tuya Inc. www.tuya.com
 */
#ifndef TUYA_GW_INFRA_API_H
#define TUYA_GW_INFRA_API_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void log_base(const char *file, 
    const char *func, 
    int line, 
    int level, 
    const char *fmt, ...);
#if 0
#define log_err(fmt, ...) \
    log_base(__FILE__, __func__, __LINE__, TY_LOG_ERR,    fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...) \
    log_base(__FILE__, __func__, __LINE__, TY_LOG_WARN,   fmt, ##__VA_ARGS__)
#define log_notice(fmt, ...) \
    log_base(__FILE__, __func__, __LINE__, TY_LOG_NOTICE, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...) \
    log_base(__FILE__, __func__, __LINE__, TY_LOG_INFO,   fmt, ##__VA_ARGS__)
#define log_debug(fmt, ...) \
    log_base(__FILE__, __func__, __LINE__, TY_LOG_DEBUG,  fmt, ##__VA_ARGS__)
#else
#define log_debug(...)     printf("\033[45m [debug][%s:%s(%d)]\033[0m", __FILE__, __FUNCTION__, __LINE__); \
                            printf("\033[45m"); \
                            printf(__VA_ARGS__); \
                            printf("\033[0m"); \
                            printf("\n");

#define log_err(...)   printf("\033[31m [error][%s:%s(%d)]\033[0m", __FILE__, __FUNCTION__, __LINE__); \
                            printf("\033[31m"); \
                            printf(__VA_ARGS__); \
                            printf("\033[0m"); \
                            printf("\n");

#define log_notice(...)   printf("\033[34m [notice][%s:%s(%d)]\033[0m", __FILE__, __FUNCTION__, __LINE__); \
                            printf("\033[34m"); \
                            printf(__VA_ARGS__); \
                            printf("\033[0m"); \
                            printf("\n");

#define log_info(...)    printf("\033[32m [info][%s:%s(%d)]\033[0m", __FILE__, __FUNCTION__, __LINE__); \
                            printf("\033[32m"); \
                            printf(__VA_ARGS__); \
                            printf("\033[0m"); \
                            printf("\n");

#define log_warn(...)    printf("\033[33m [warn][%s:%s(%d)]\033[0m", __FILE__, __FUNCTION__, __LINE__); \
                            printf("\033[33m"); \
                            printf(__VA_ARGS__); \
                            printf("\033[0m"); \
                            printf("\n");
#endif
typedef enum {
    TY_LOG_ERR     = 0,
    TY_LOG_WARN    = 1,
    TY_LOG_NOTICE  = 2,
    TY_LOG_INFO    = 3,
    TY_LOG_DEBUG   = 4,
    TY_LOG_TRACE   = 5
} ty_log_lovel_t;

typedef enum {
    TY_CONN_MODE_AP_ONLY      = 0,
    TY_CONN_MODE_EZ_ONLY      = 1,
    TY_CONN_MODE_AP_FIRST     = 2,
    TY_CONN_MODE_EZ_FIRST     = 3,
} ty_conn_mode_t;

typedef enum {
    TY_GW_STATUS_UNREGISTERED = 0,
    TY_GW_STATUS_REGISTERED   = 1,
} ty_gw_status_t;

typedef struct {
    char          *storage_path;
    char          *cache_path;
    char          *tty_device;
    int            tty_baudrate;
    char          *country_code;  // CN/US
    char          *ssid;
    char          *password;
    char          *ver;
    int            is_engr;
    int            is_fork;
    int            dev_max;
    ty_conn_mode_t wifi_mode;
    ty_log_lovel_t log_level;
} ty_gw_attr_s;

typedef struct {
    int  (*get_uuid_authkey_cb)(char *uuid, int uuid_size, char *authkey, int authkey_size);
    int  (*get_product_key_cb)(char *pk, int pk_size);
    void (*gw_upgrade_cb)(const char *img_file);
    void (*gw_reboot_cb)(void);
    void (*gw_reset_cb)(void);
    void (*gw_engineer_finished_cb)(void);
    int  (*gw_fetch_local_log_cb)(char *path, int path_len);
    int  (*gw_active_status_changed_cb)(ty_gw_status_t status);
    int  (*gw_online_status_changed_cb)(bool online);
} ty_gw_infra_cbs_s;

/**
 * @brief    pre-initiate sdk, must call at first.
 *
 * @retval   0: success
 * @retval  !0: failure
 */
int tuya_user_iot_pre_init(void);

/**
 * @brief    initiate sdk.
 *
 * @retval   0: success
 * @retval  !0: failure
 */
int tuya_user_iot_init(const ty_gw_attr_s *attr, const ty_gw_infra_cbs_s *cbs);

/**
 * @brief    unactive gateway.
 *
 * @retval   0: success
 * @retval  !0: failure
 */
int tuya_user_iot_unactive_gw(void);

/**
 * @brief    reset gateway.
 *
 * @retval   0: success
 * @retval  !0: failure
 */
int tuya_user_iot_reset_gw(void);

/**
 * @brief    active gateway with token.
 *
 * @retval   0: success
 * @retval  !0: failure
 */
int tuya_user_iot_active_gw(const char *token);

/**
 * @brief    allow or disallow subdevice to join network.
 *
 * @param permit. 0: disallow, 1: allow.
 * 
 * @retval   0: success
 * @retval  !0: failure
 */
int tuya_user_iot_permit_join(bool permit, uint32_t timeout);

/**
 * @brief    set time zone.
 * 
 * @param time_zone. an identifier for a time offset from UTC, such as "+08:00"
 * 
 * @retval   0: success
 * @retval  !0: failure
 */
int tuya_user_iot_set_time_zone(const char *time_zone);

/**
 * @brief    get time zone with seconds.
 * 
 * @param time. convert timezone to seconds, can use it with `tuya_user_iot_get_posix`.
 * 
 * @retval   0: success
 * @retval  !0: failure
 */
int tuya_user_iot_get_time_zone(uint32_t *time);

/**
 * @brief    update sdk inside time.
 * 
 * @param time. the time as the number of seconds since 1970-01-01 00:00:00 +0000 (UTC)
 * 
 * @retval   0: success
 * @retval  !0: failure
 */
int tuya_user_iot_set_posix(uint32_t time);

/**
 * @brief    get sdk inside time.
 * 
 * @param time. the time as the number of seconds since 1970-01-01 00:00:00 +0000 (UTC)
 *
 * @retval   0: success
 * @retval  !0: failure
 */
int tuya_user_iot_get_posix(uint32_t *time);

/**
 * @brief    check if current work mode is engineering mode.
 * 
 * @retval   true : engineer mode
 * @retval   false: normal mode
 */
bool tuya_user_iot_get_engineer_mode(void);

#ifdef __cplusplus
}
#endif
#endif