#ifndef __LOG_H
#define __LOG_H

#include <stdio.h>

#define db_msg(...)     printf("\033[37m [%s:%s(%d)]\033[0m", __FILE__, __FUNCTION__, __LINE__); \
                            printf("\033[37m"); \
                            printf(__VA_ARGS__); \
                            printf("\033[0m"); \
                            printf("\n");

#define db_error(...)   printf("\033[31m [%s:%s(%d)]\033[0m", __FILE__, __FUNCTION__, __LINE__); \
                            printf("\033[31m"); \
                            printf(__VA_ARGS__); \
                            printf("\033[0m"); \
                            printf("\n");
 
#define db_info(...)    printf("\033[32m [%s:%s(%d)]\033[0m", __FILE__, __FUNCTION__, __LINE__); \
                            printf("\033[32m"); \
                            printf(__VA_ARGS__); \
                            printf("\033[0m"); \
                            printf("\n");

#define db_warn(...)    printf("\033[33m [%s:%s(%d)]\033[0m", __FILE__, __FUNCTION__, __LINE__); \
                            printf("\033[33m"); \
                            printf(__VA_ARGS__); \
                            printf("\033[0m"); \
                            printf("\n");
#endif // !__LOG_H

