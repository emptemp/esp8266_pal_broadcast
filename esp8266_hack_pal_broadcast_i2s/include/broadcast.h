#define PORCH_LEVEL   0xeee773bb  //0xeff777fb
#define BLACK_LEVEL   0xeff777fb  //0xeff777fb
#define SYNC_LEVEL    0xb9dddcee   //0xee77773b
#define WHITE_LEVEL   0xFFFFFFFF  //0xFFFFFFFF
#define LOW_LEVEL     0xbb9ddcee    //0xbb9ddcee

//Tricky: We do it this way to force GCC to emit the code for the 'call' command instead of trying to do a callx0 or something.
#define call_delay_us( time ) { asm volatile( "mov.n a2, %0\n_call0 delay4clk" : : "r"(time*13) : "a2" ); }

void ICACHE_FLASH_ATTR pal_broadcast(uint16_t isrs);
void ICACHE_FLASH_ATTR fill_buf(uint32_t* buf, uint16_t k0, uint16_t k1, uint32_t byte);
void ICACHE_FLASH_ATTR empty_sync(uint32_t* buf);
void ICACHE_FLASH_ATTR ll_sync(uint32_t* buf);
void ICACHE_FLASH_ATTR ss_sync(uint32_t* buf);
void ICACHE_FLASH_ATTR sl_sync(uint32_t* buf);
void ICACHE_FLASH_ATTR ls_sync(uint32_t* buf);
