////////////////////////////////////////////////////////////////////////////////
/// @file       TSGP_font.h
/// @version    v0.3
/// @brief      contains 8x12 ascii charset 0x20 to 0x7F
/// 
/// @author     Gerald Tschinkl
////////////////////////////////////////////////////////////////////////////////

#define BMP_NAME            "1.bmp"

#define ROTATE              (0)

#define WIDTH               (512)
#define HEIGHT              (512)

#define WORDSIZE            (8)
#define BUFFERSIZE          (HEIGHT  * (WIDTH / WORDSIZE))
#define BLOCK_INDEX         (WIDTH/WORDSIZE) - 1
#if ROTATE
  #define GETBUFPOS(x,y)    (WIDTH*x)/WORDSIZE + BLOCK_INDEX - (y/WORDSIZE)
#else
  #define GETBUFPOS(x,y)      (y*(WIDTH/WORDSIZE) + x/WORDSIZE)
#endif

#if !(WIDTH%WORDSIZE==0 && HEIGHT%WORDSIZE==0)
  #error WIDTH AND HEIGHT ARE NOT DIVISIBLE BY WORDSIZE
#endif

uint8_t fbuf[BUFFERSIZE] = {0xFF};

