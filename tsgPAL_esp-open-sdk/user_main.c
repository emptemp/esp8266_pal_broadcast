#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "c_types.h"
#include "driver/uart.h"

#include "mem.h"
#include "c_types.h"
#include "user_interface.h"
#include "espconn.h"
#include "tsgp.h"
#include "broadcast.h"
#include <string.h>

/* ***************   TODO   ******************  
   ============= PAL_BROADCAST ==============
 * revisit LEVELS in broadcast.h
   runs quite smooth on MATSUI but is still
   wacky on LQ 42" and possible other TV's
 * use 256 instead of 128 px width !!
   ============== UDP SERVER ================
 * basic udp server + packet handling
 * should we get the whole frame via udp?
   - this could be smaller than actual fbuffer
**********************************************/

#define procTaskPrio 0
#define procTaskQueueLen    1
os_event_t    procTaskQueue[procTaskQueueLen];

void user_rf_pre_init(void)
{
	//nothing.
}

// update screen
void ICACHE_FLASH_ATTR procTask(os_event_t *events)
{
  char printbuf[32] = {0x00};

  int stat = wifi_station_get_connect_status();
  struct ip_info ipi;
  wifi_get_ip_info(0, &ipi);
	if( ipi.ip.addr || stat == 255 )
	{
    ets_sprintf(printbuf, "IP: %d.%d.%d.%d", (ipi.ip.addr>>0)&0xff,
                                             (ipi.ip.addr>>8)&0xff,
                                             (ipi.ip.addr>>16)&0xff,
                                             (ipi.ip.addr>>24)&0xff);
    print_str(4, 230, printbuf, 1, 2);
  }
  system_os_post(procTaskPrio, 0, 0 );
}

void ICACHE_FLASH_ATTR user_init()
{
  // init gpio subsytem
  gpio_init();

  // init uart
  uart_init(BIT_RATE_115200, BIT_RATE_115200);
	uart0_sendStr("\r\nhello from the esp :)\r\n");

  // init wifi
  #define SSID ""
  #define PSWD ""
	struct station_config stationConf;
	wifi_station_get_config(&stationConf);
	os_strcpy((char*)&stationConf.ssid, SSID );
	os_strcpy((char*)&stationConf.password, PSWD );
	stationConf.bssid_set = 0;
	wifi_station_set_config(&stationConf);
	wifi_set_opmode(1);

  system_update_cpu_freq( SYS_CPU_160MHZ );

  memset(fbuf, 0x00, sizeof(fbuf));  
  print_str(4, 48, "IS THIS", 2,2);
  print_str(4, 48+30, "MULTI", 2,0);
  print_str(4, 48+60, "TASKING?", 2,2);
  print_str(32, 48+150, "*butterfly*", 1,1);

  // append task to queue
  system_os_task(procTask, procTaskPrio, procTaskQueue, procTaskQueueLen);
  system_os_post(procTaskPrio, 0, 0 );

  // setup i2s and send empty buffer to start
  // pal_broadcast() is called by i2s interrupt handler
	InitI2S();
  SendI2S();
}
