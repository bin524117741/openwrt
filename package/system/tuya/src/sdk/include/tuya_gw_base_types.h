#ifndef TUYA_GW_BASE_TYPES_H
#define TUYA_GW_BASE_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAC_MAX_LEN     6
#define ADDR_MAX_LEN    16

typedef struct {
    char ip[ADDR_MAX_LEN];
    char mask[ADDR_MAX_LEN];
    char gateway[ADDR_MAX_LEN];
} ip_info_s;

typedef struct {
    uint8_t mac[MAC_MAX_LEN];
} mac_info_s;

#ifdef __cplusplus
}
#endif
#endif