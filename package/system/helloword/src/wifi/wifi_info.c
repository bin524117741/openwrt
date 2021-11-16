#include "wifi_info.h"
#include "../log.h"
#include <stdlib.h>
#include <string.h>
int wifi_probe(void)
{
    db_warn("wifi_probe");
    return 0;
}

int wifi_remove(void)
{
    db_warn("wifi_remove");
    return 0;
}
int wifi_connect(char *name, char *password)
{
    char str[100] = {0};
    sprintf(str,"uci set wireless.@wifi-iface[0].ssid=%s",name);
    db_warn("%s",str);
    sprintf(str,"uci set wireless.@wifi-iface[0].key=%s",password);
    db_warn("%s",str);
    system("wifi");
    db_warn("wifi_connect");
    return 0;
}
int wifi_disconnect(void)
{
    db_warn("wifi_disconnect");
    system("wifi down");
    return 0;
}
int wifi_get(void)
{
    FILE* fp=NULL;
    char buf[100] = {0};    
    //char command[300] = ;
    char str1[100];
    char *q = NULL;
    int num = 0;
    memset(str1, '\0', sizeof(str1));
    if((fp = popen("iwlist wlan0 scan | grep ESSID", "r")) != NULL)
    {
        printf("popen command success\n");
        while(fgets(buf, sizeof(buf), fp) != NULL)
        {
            q = strstr(buf, "ESSID:");
            if (q != NULL)
            {
                sscanf(q, "ESSID:\"%[^\"]\"", str1);
                num++;
                db_warn("ESSID[%d]:%s",num,str1);
                q = NULL;
            }
            // Signal level
            // q = strstr(buf, "Signal level=");
            // if(q!=NULL)
            // {
            //     sscanf(q,"Signal level=%[^/]",str2);
            //     q = NULL;        
            //     strcat(str1,"");
            //     strcat(str1,str2);
            // }
            
        }
        pclose(fp);
    }

}