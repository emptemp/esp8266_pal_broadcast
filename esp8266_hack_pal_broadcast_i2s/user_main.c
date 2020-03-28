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
static struct espconn *pUdpServer;

char udp_buf[32] = {0x00};

void user_rf_pre_init(void)
{
	//nothing.
}

// update screen  
void ICACHE_FLASH_ATTR procTask(os_event_t *events)
{
  char ip_buf[20] = {0x00};
  int stat = wifi_station_get_connect_status();
  struct ip_info ipi;
  wifi_get_ip_info(0, &ipi);
  if( ipi.ip.addr || stat == 255 )
	{
    ets_sprintf(ip_buf, "IP: %d.%d.%d.%d", (ipi.ip.addr>>0)&0xff,
                                           (ipi.ip.addr>>8)&0xff,
                                           (ipi.ip.addr>>16)&0xff,
                                           (ipi.ip.addr>>24)&0xff);
    print_str(4, 32, ip_buf, 1, 2);
  }
  print_str(4,64, udp_buf, 1, 2);
  system_os_post(procTaskPrio, 0, 0 );
}

// called when new packet comes in.
static void ICACHE_FLASH_ATTR udpserver_recv(void *arg, char *data, unsigned short len)
{
	//uart0_sendStr("X");
  memset(fbuf, 0x00, sizeof(fbuf));
  ets_sprintf(udp_buf, "%s", data);
}

// this is the entry point, setup things go here
void ICACHE_FLASH_ATTR user_init()
{
  // init gpio subsytem
  gpio_init();

  // init uart
  uart_init(BIT_RATE_115200, BIT_RATE_115200);
	uart0_sendStr("\r\nhello from the esp :)\r\n");

  // init wifi
  #define SSID "UPC8142634"
  #define PSWD "GF7mcumhfhmb"
	struct station_config stationConf;
	wifi_station_get_config(&stationConf);
	os_strcpy((char*)&stationConf.ssid, SSID );
	os_strcpy((char*)&stationConf.password, PSWD );
	stationConf.bssid_set = 0;
	wifi_station_set_config(&stationConf);
	wifi_set_opmode(1);

  // init udp server
  pUdpServer = (struct espconn *)os_zalloc(sizeof(struct espconn));
	ets_memset( pUdpServer, 0, sizeof( struct espconn ) );
	espconn_create( pUdpServer );
	pUdpServer->type = ESPCONN_UDP;
	pUdpServer->proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
	pUdpServer->proto.udp->local_port = 7777;
	espconn_regist_recvcb(pUdpServer, udpserver_recv);

	if(espconn_create(pUdpServer)) {
		while(1) { uart0_sendStr( "\r\nFAULT\r\n" ); }
	}

  // clear framebuffer
  memset(fbuf, 0x00, sizeof(fbuf));  

  // append task to queue
  system_os_task(procTask, procTaskPrio, procTaskQueue, procTaskQueueLen);
  // update cpu frequency
  system_update_cpu_freq( SYS_CPU_160MHZ );

  system_os_post(procTaskPrio, 0, 0 );

  // setup i2s and send empty buffer to start
  // pal_broadcast() is called by i2s interrupt handler
	InitI2S();
  SendI2S();
}
