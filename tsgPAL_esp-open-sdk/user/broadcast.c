#include <string.h>
#include "nosdki2s.h"
#include "pin_mux_register.h"
#include "tsgp.h"
#include "broadcast.h"
 
struct sdio_queue i2sBufDesc[1];

// gets called by slc_isr() i2s interrupt handler
// fills required line with sync, porch and data
void ICACHE_FLASH_ATTR pal_broadcast(uint16_t isrs)
{
  uint32_t* p_buffer = (uint32_t*)i2sBufDesc[0].buf_ptr;
  switch(isrs%312) {
    case 1:   ll_sync(p_buffer); break;
    case 2:   ll_sync(p_buffer); break;
    case 3:   ls_sync(p_buffer); break;
    case 4:   ss_sync(p_buffer); break;
    case 5:   ss_sync(p_buffer); break;
    case 310: ss_sync(p_buffer); break;
    case 311: ss_sync(p_buffer); break;
    case 312: ss_sync(p_buffer); break;
    default:  empty_sync(p_buffer);
              uint16_t bufl=(isrs%312)*16;
              uint8_t k;
              for(k=0; k<128; k++)  {
                bool w = (fbuf[bufl+(k/8)] >> (WORDSIZE-1-(k%8))) & 0x01;
                if(w)                    
                  fill_buf(p_buffer, k+32, k+33, WHITE_LEVEL);
              }
              break;  
  }
}

// fills i2s buffer (one PAL line) from k0 to k1 with byte
void ICACHE_FLASH_ATTR fill_buf(uint32_t* buf, uint16_t k0, uint16_t k1, uint32_t byte)
{
  uint16_t k = 0;
  for(k = k0; k < k1; k++)
  {
    buf[k] = byte;
  }
}

// creates empty sync line
void ICACHE_FLASH_ATTR empty_sync(uint32_t* buf)
{
  fill_buf(buf, 0, 4, PORCH_LEVEL);
  fill_buf(buf, 4, 16, SYNC_LEVEL);
  fill_buf(buf, 16, 30, PORCH_LEVEL);
  fill_buf(buf, 30, 160, BLACK_LEVEL);
}

// 2x ( 30us low, 2us high ) sync line
void ICACHE_FLASH_ATTR ll_sync(uint32_t* buf)
{
    fill_buf(buf, 0, 75, LOW_LEVEL);
    fill_buf(buf, 75, 80, BLACK_LEVEL);
    fill_buf(buf, 80, 155, LOW_LEVEL);
    fill_buf(buf, 155, 160, BLACK_LEVEL);
}

// 2x ( 2us low, 30us high ) sync line
void ICACHE_FLASH_ATTR ss_sync(uint32_t* buf)
{
    fill_buf(buf, 0, 5, LOW_LEVEL);
    fill_buf(buf, 5, 80, BLACK_LEVEL);
    fill_buf(buf, 80, 85, LOW_LEVEL);
    fill_buf(buf, 85, 160, BLACK_LEVEL);
}

// (2us low, 30us high, 30us low, 2us high) sync line
void ICACHE_FLASH_ATTR sl_sync(uint32_t* buf)
{
    fill_buf(buf, 0, 5, LOW_LEVEL);
    fill_buf(buf, 5, 80, BLACK_LEVEL);
    fill_buf(buf, 80, 155, LOW_LEVEL);
    fill_buf(buf, 155, 160, BLACK_LEVEL);
}

// (30us low, 2us high, 2us low, 30us high) sync line
void ICACHE_FLASH_ATTR ls_sync(uint32_t* buf)
{
    fill_buf(buf, 0, 75, LOW_LEVEL);
    fill_buf(buf, 75, 80, BLACK_LEVEL);
    fill_buf(buf, 80, 85, LOW_LEVEL);
    fill_buf(buf, 85, 160, BLACK_LEVEL);
}

