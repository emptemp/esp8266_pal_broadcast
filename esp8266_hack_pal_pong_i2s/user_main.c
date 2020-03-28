#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "c_types.h"
#include "driver/uart.h"
#include "esp8266_auxrom.h"
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

static volatile os_timer_t dot_timer;


char stats_buf[32] = {0x00};

// pong stuff
#define TOP_BORDER 25
#define BOTTOM_BORDER 15
#define RWIDTH 30
#define STEP  7

volatile bool start=0;
uint8_t p1_points = 0;
uint8_t p2_points = 0;
volatile bool draw=1;
// p1 coordinates
volatile uint16_t y_pos1=156;
volatile uint16_t y_pos2=156;
// dot coordinates and vector
volatile uint16_t x=64;
volatile uint16_t y=156;
volatile int8_t vx=2;
volatile int8_t vy=0;


void user_rf_pre_init(void)
{
	//nothing.
}

int32 ICACHE_FLASH_ATTR esp_atoi(const char* in)
{
	int positive = 1; //1 if negative.
	int hit = 0;
	int val = 0;
	while( *in && hit < 11 	)
	{
		if( *in == '-' )
		{
			if( positive == -1 ) return val*positive;
			positive = -1;
		} else if( *in >= '0' && *in <= '9' )
		{
			val *= 10;
			val += *in - '0';
			hit++;
		} else if (!hit && ( *in == ' ' || *in == '\t' ) )
		{
			//okay
		} else
		{
			//bad.
			return val*positive;
		}
		in++;
	}
	return val*positive;
}

// called player got point
static void ICACHE_FLASH_ATTR game_rst()
{
  y_pos1=156; y_pos2=156;
  x=64; y=156; vy=0;
}


void dot_timer_func(void *arg)
{
  if(start) {
    if(x<=4)
    {
      p2_points++;
      game_rst();
      vx=vx*-1;
      start=0;
    }
    if(x>=WIDTH-8)
    {
      p1_points++;
      game_rst();
      vx=vx*-1;
      start=0;
    }
    if(y > HEIGHT-TOP_BORDER || y < BOTTOM_BORDER)
    {
      vy=vy*-1;
    }
    if(x <= 8 && y_pos1-RWIDTH < y && y < y_pos1+RWIDTH)
    {
      vy=(y-y_pos1)/4;
      vx=vx*-1;
    }
    if(x >= WIDTH-12 && y_pos2-RWIDTH < y && y < y_pos2+RWIDTH)
    {
      vy=(y-y_pos2)/4;
      vx=vx*-1;
    }
    x = x + vx;
    y = y + vy;
    draw=1;
  }
}

// update screen  
void ICACHE_FLASH_ATTR procTask(os_event_t *events)
{
  printf("HELLO\n");
  if(draw)
  {
    memset(fbuf, 0x00, sizeof(fbuf));
    // draw dot
    rect(x, y-4, x+2, y+4, 0);
    // draw p1 racket
    rect(0, y_pos1-RWIDTH, 4, y_pos1+RWIDTH, 0);
    // draw p2 racket
    rect(WIDTH-8, y_pos2-RWIDTH, WIDTH-4, y_pos2+RWIDTH, 0);
    // print stats    
    ets_sprintf(stats_buf, "%d", p1_points);
    print_str(32, 4+TOP_BORDER, stats_buf, 1, 2);
    ets_sprintf(stats_buf, "%d", p2_points);
    print_str(96, 4+TOP_BORDER, stats_buf, 1, 2);
    draw=0;
  }
  system_os_post(procTaskPrio, 0, 0 );
}

// called when new packet comes in.
static void ICACHE_FLASH_ATTR udpserver_recv(void *arg, char *data, unsigned short len)
{
  uart0_sendStr("X");
  //struct espconn *pespconn = (struct espconn *)arg;
  //ets_sprintf(stats_buf, "%s", pespconn->proto.udp->remote_ip);
  //draw=1;
  ets_intr_lock();
  if(data)  {
    uint16_t y_udp = esp_atoi(data);
    if(TOP_BORDER < y_udp && y_udp < HEIGHT-BOTTOM_BORDER-RWIDTH)
      y_pos1 = y_udp;
    else if(*data == 'r') {
      game_rst();
      start=1;
    }
    draw=1; 
  }
  ets_intr_unlock();
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

	//Pattern Timer example
  os_timer_setfn(&dot_timer, (os_timer_func_t *)dot_timer_func, NULL);
  os_timer_arm(&dot_timer, 50, 1);

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
