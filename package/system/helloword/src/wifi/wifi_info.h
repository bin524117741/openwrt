#ifndef __WIFI_INFO_H
#define __WIFI_INFO_H

#include <stdio.h>

struct wifi_info {
	char name[64];
    char password[64];
    char isconnected;
    int single_level;
};
int wifi_probe(void);
int wifi_remove(void);
int wifi_connect(char *name, char *password);
int wifi_disconnect(void);
int wifi_get_ssid(void);
int wifi_get(void);
#endif // !__WIFI_INFO_H
