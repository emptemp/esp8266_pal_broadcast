////////////////////////////////////////////////////////////////////////////////
/// @file       TSGP_font.h
/// @version    v0.3
/// @brief      contains 8x12 ascii charset 0x20 to 0x7F
/// 
/// @author     Gerald Tschinkl
////////////////////////////////////////////////////////////////////////////////

#define BMP_NAME            "1.bmp"

#define ROTATE              (0)
#define BACKCOLOR           (0x00)      // BMP: 0x00 = Black, 0xFF = White
                                        // EPD: 0x00 = White, 0xFF = Black

#define WIDTH               (128)
#define HEIGHT              (312)

#define WORDSIZE            (8)
#define BUFFERSIZE          (HEIGHT  * (WIDTH / WORDSIZE))
#define BLOCK_INDEX         (WIDTH/WORDSIZE) - 1
#if ROTATE
  #define GETBUFPOS(x,y)    (WIDTH*x)/WORDSIZE + BLOCK_INDEX - (y/WORDSIZE)
#else
  #define GETBUFPOS(x,y)    (y*(WIDTH/WORDSIZE) + x/WORDSIZE)
#endif

#if !(WIDTH%WORDSIZE==0)
  #error WIDTH IS NOT DIVISIBLE BY WORDSIZE
#endif

uint8_t fbuf[BUFFERSIZE];

// inverts pixeldot
extern void xdot(uint16_t x, uint16_t y);
// clears pixeldot
extern void dot(uint16_t x, uint16_t y);
// ©bresenham line algorithm
extern void line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
// fills rect with dots
extern void rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t fill);
// draws beautiful sine waves
extern void sine(uint16_t x, uint16_t y, uint16_t tlen, uint8_t ampl, uint8_t freq);
// also draws beautiful sine waves but rotated by 90 degrees ;-)
extern void sine_rot(uint16_t x, uint16_t y, uint16_t tlen, uint8_t ampl, uint8_t freq);
// gets info from char_lut and sets dots accordingly
//void print_chr(uint16_t x, uint16_t y, uint8_t chr);

// gets info from char_lut and sets dots accordingly
extern void print_chr(uint16_t x, uint16_t y, uint8_t chr, uint8_t size);
// puts string into fbuffer using print_chr()
extern void print_str(uint16_t x, uint16_t y, char* str, uint8_t size, uint8_t spacing);


