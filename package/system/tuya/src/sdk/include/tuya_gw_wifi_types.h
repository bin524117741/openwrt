#ifndef TUYA_GW_WIFI_TYPES_H
#define TUYA_GW_WIFI_TYPES_H

#include <stdint.h>

#include "tuya_gw_base_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BSSID_MAX_LEN   6
#define SSID_MAX_LEN    32
#define KEY_MAX_LEN     64

typedef void (*sniffer_callback)(uint8_t *buf, uint16_t len, uint8_t rssi);

typedef enum {
    STATION = 0,
    AP      = 1,
} wifi_type_t;

typedef enum {
    WMODE_LOWPOWER    = 0, /* wifi work in lowpower mode */
    WMODE_SNIFFER,         /* wifi work in sniffer mode */
    WMODE_STATION,         /* wifi work in station mode */
    WMODE_SOFTAP,          /* wifi work in ap mode */
    WMODE_STATIONAP,       /* wifi work in station+ap mode */
    WMODE_UNKNOWN,         /* wifi work in unknown mode */
} wifi_work_mode_t;

typedef enum {
    STAT_IDLE         = 0, /* disconnected */
    STAT_CONNECTING,       /* connecting */
    STAT_PASSWD_WRONG,     /* passwd do not match */
    STAT_NO_AP_FOUND,      /* ap not found */
    STAT_CONN_FAIL,        /* connect ap fail */
    STAT_CONN_SUCCESS,     /* connect ap success */
    STAT_GOT_IP,           /* connected and got ip success */
} station_conn_stat_t;

typedef enum {
    TYPE_OPEN         = 0, /* open */
    TYPE_WEP,              /* WEP */
    TYPE_WPA_PSK,          /* WPA—PSK  */
    TYPE_WPA2_PSK,         /* WPA2—PSK */
    TYPE_WPA_WPA2_PSK,     /* WPA/WPA2 */
} ap_encryption_type_t;

typedef struct {
    uint8_t channel;                /* AP channel */
    int8_t rssi;                    /* AP rssi */
    uint8_t bssid[BSSID_MAX_LEN];   /* AP bssid */
    uint8_t ssid[SSID_MAX_LEN+1];   /* AP ssid */
    uint8_t s_len;                  /* AP ssid len */
    uint8_t security;
    uint8_t resv1;
    uint8_t data_len;
    uint8_t data[0];
} ap_scan_info_s;

typedef struct {
    uint8_t ssid[SSID_MAX_LEN+1];   /* ssid */
    uint8_t s_len;                  /* len of ssid */
    uint8_t key[KEY_MAX_LEN+1];     /* passwd */
    uint8_t p_len;                  /* len of passwd */
    uint8_t channel;                /* channel */
    ap_encryption_type_t type;      /* encryption type */
    uint8_t hidden;                 /* ssid hidden */
    uint8_t max_conn;               /* max sta connect nums */
    uint16_t ms_interval;           /* broadcast interval */
    ip_info_s ip;                   /* ip info for ap mode */
} ap_cfg_info_s;

#ifdef __cplusplus
}
#endif
#endif
