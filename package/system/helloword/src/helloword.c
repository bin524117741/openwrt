#include "log.h"
#include "wifi/wifi_info.h"
#include "mytool1.h"
#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <linux/input.h>
int main(void)
{
     asdas();
     wifi_probe();
     db_msg("WERWRWTE");
     db_info("HGJHJHJHJ");
     db_warn("DFGDGDGDFG");
     db_error("SDASDASDAS");
     wifi_remove();
     wifi_get();
     while(1){
          
     }
}