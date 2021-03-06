#include "nosdki2s.h"
#include "user_interface.h"
#include "pin_mux_register.h"
#include "broadcast.h"

//Change these if you need!
#define WS_I2S_BCK 1	
#define WS_I2S_DIV 2

volatile uint32_t * IOMUX_BASE = (volatile uint32_t *)0x60000800;

#define nosdk8266_configio( port, FUNC, pd, pu ) \
	IOMUX_BASE[(port-PERIPHS_IO_MUX)/4] = ((((FUNC&0x04)<<2)|(FUNC&0x3))<<PERIPHS_IO_MUX_FUNC_S) | (pu<<7) | (pd<<6);

void pico_i2c_writereg_asm( uint32_t a, uint32_t b);
#define pico_i2c_writereg( reg, hostid, par, val ) pico_i2c_writereg_asm( (hostid<<2) + 0x60000a00 + 0x300, (reg | (par<<8) | (val<<16) | 0x01000000 ) )

#define I2S_BUFSIZE   160

static unsigned int i2sData[I2S_BUFSIZE];
struct sdio_queue i2sBufDesc[1] = {
	{ .owner = 1, .eof = 1, .sub_sof = 0, .datalen = I2S_BUFSIZE*4,  .blocksize = I2S_BUFSIZE*4, .buf_ptr = (uint32_t)&i2sData[0], .next_link_ptr = (uint32_t)&i2sBufDesc[1] },
};

volatile uint32_t * DR_REG_I2S_BASEL = (volatile uint32_t*)0x60000e00;
volatile uint32_t * DR_REG_SLC_BASEL = (volatile uint32_t*)0x60000B00;

volatile uint16_t isrs = 0;

#define I2S_INTERRUPTS 1

#if I2S_INTERRUPTS

LOCAL void slc_isr(void) {
	WRITE_PERI_REG(SLC_INT_CLR, 0xffffffff);//slc_intr_status);

	//This is a little wacky.  This function actually gets called twice.
	//Once for the initial transfer, but by the time we tell it to stop
	//The other zero transfer's already begun.
  pal_broadcast(isrs);
  SendI2S();
	isrs++;
}

#endif


void InitI2S()
{
	int i;

	DR_REG_SLC_BASEL[4] = 0;  //Reset DMA
	SLC_CONF0L = (1<<SLC_MODE_S);		//Configure DMA
	SLC_RX_DSCR_CONFL = SLC_INFOR_NO_REPLACE|SLC_TOKEN_NO_REPLACE;

	//Initialize buffers
	for( i = 0; i < I2S_BUFSIZE; i++ )
	{
		i2sData[i] = 0x00000000;
	}
	//Set address for buffer descriptor.
	SLC_RX_LINKL = ((uint32)&i2sBufDesc[0]) & SLC_RXLINK_DESCADDR_MASK;

#if I2S_INTERRUPTS

	//Attach the DMA interrupt
	ets_isr_attach(ETS_SLC_INUM, slc_isr);
	//Enable DMA operation intr
		//WRITE_PERI_REG(SLC_INT_ENA,  SLC_RX_EOF_INT_ENA);
	SLC_INT_ENAL = SLC_RX_EOF_INT_ENA; //Select the interrupt.

	//clear any interrupt flags that are set
		//WRITE_PERI_REG(SLC_INT_CLR, 0xffffffff);
	SLC_INT_CLRL = 0xffffffff;
	///enable DMA intr in cpu
	ets_isr_unmask(1<<ETS_SLC_INUM);

#endif
	//Init pins to i2s functions
	nosdk8266_configio(PERIPHS_IO_MUX_U0RXD_U, FUNC_I2SO_DATA, 0, 0);
  //PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_I2SO_WS);
  //PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_I2SO_BCK);

	//Enable clock to i2s subsystem
	//From 13 to 93, reconfigure the BBPLL to output i2c_bbpll_en_audio_clock_out
	//Code was originally: //i2c_writeReg_Mask_def(i2c_bbpll, i2c_bbpll_en_audio_clock_out, 1);
	pico_i2c_writereg(103,4,4,0x93);

	//Reset I2S subsystem
	I2SCONFL = I2S_I2S_RESET_MASK;
	I2SCONFL = 0;

	//Select 16bits per channel (FIFO_MOD=0)
	//CLEAR_PERI_REG_MASK(I2S_FIFO_CONF, I2S_I2S_DSCR_EN|(I2S_I2S_RX_FIFO_MOD<<I2S_I2S_RX_FIFO_MOD_S)|(I2S_I2S_TX_FIFO_MOD<<I2S_I2S_TX_FIFO_MOD_S));
	//SET_PERI_REG_MASK(I2S_FIFO_CONF, I2S_I2S_DSCR_EN);
	I2S_FIFO_CONFL = I2S_I2S_DSCR_EN; 	//Enable DMA in i2s subsystem

	//tx/rx binaureal
  //CLEAR_PERI_REG_MASK(I2SCONF_CHAN, (I2S_TX_CHAN_MOD<<I2S_TX_CHAN_MOD_S)|(I2S_RX_CHAN_MOD<<I2S_RX_CHAN_MOD_S));

#if I2S_INTERRUPTS

	//Clear int
	I2SINT_CLRL = 0;  //Clear interrupts in I2SINT_CLR
#endif
	//Configure the I2SConf mode.
	I2SCONFL = I2S_RIGHT_FIRST|I2S_MSB_RIGHT|I2S_RECE_SLAVE_MOD|
						I2S_RECE_MSB_SHIFT|I2S_TRANS_MSB_SHIFT|
						(((WS_I2S_BCK)&I2S_BCK_DIV_NUM )<<I2S_BCK_DIV_NUM_S)|
						(((WS_I2S_DIV)&I2S_CLKM_DIV_NUM)<<I2S_CLKM_DIV_NUM_S);


	//enable int
	//SET_PERI_REG_MASK(I2SINT_ENA,  I2S_I2S_RX_REMPTY_INT_ENA|I2S_I2S_RX_TAKE_DATA_INT_ENA);
	I2SINT_ENAL = I2S_I2S_RX_REMPTY_INT_ENA|I2S_I2S_RX_TAKE_DATA_INT_ENA;

	//Start transmission
	//SET_PERI_REG_MASK(I2SCONF,I2S_I2S_TX_START);
	I2SCONFL |= I2S_I2S_TX_START;
}

void SendI2S()
{	
	SLC_RX_LINKL = SLC_RXLINK_STOP;
	SLC_RX_LINKL = (((uint32)&i2sBufDesc[0]) & SLC_RXLINK_DESCADDR_MASK) | SLC_RXLINK_START;
//	while(!(SLC_INT_RAWL & (1<<16)));
//	SET_PERI_REG_MASK(SLC_RX_LINK, SLC_RXLINK_STOP);
//	CLEAR_PERI_REG_MASK(SLC_RX_LINK,SLC_RXLINK_DESCADDR_MASK);
//	SET_PERI_REG_MASK(SLC_RX_LINK, ((uint32)&i2sData[0]) & SLC_RXLINK_DESCADDR_MASK);
//	SET_PERI_REG_MASK(SLC_RX_LINK, SLC_RXLINK_START);
}

//NOTE: See if frame is complete:
// (SLC_INT_RAWL & (1<<16))?event happened.
// To clear:	SLC_INT_CLRL = -1;

