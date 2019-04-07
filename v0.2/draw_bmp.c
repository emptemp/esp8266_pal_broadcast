////////////////////////////////////////////////////////////////////////////////
/// @file       TSGP_font.h
/// @brief      contains 8x12 ascii charset 0x20 to 0x7F
/// 
/// @author     Gerald Tschinkl
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "tsgp.h"

#define WIDTH_0       WIDTH & 0xFF
#define WIDTH_1       (WIDTH & 0xFF00) >> 8
#define HEIGHT_0      0xFF - (HEIGHT & 0xFF) //+ 1      // needs to be 1px smaller
#define HEIGHT_1      0xFF - ((HEIGHT & 0xFF00) >> 8)
#define BITS_PIXEL    1
#define RAWSIZE       (WIDTH*HEIGHT)/(8/BITS_PIXEL)+62
#define RAWSIZE_0     RAWSIZE & 0xFF
#define RAWSIZE_1     (RAWSIZE & 0xFF00) >> 8

// header info for .bmp file format
uint8_t header[62] = {  0x42,0x4D,    // ID field
    RAWSIZE_0,RAWSIZE_1,0x00,0x00,    // size of bmp
              0x00,0x00,0x00,0x00,    // unused
              0x3E,0x00,0x00,0x00,    // offset of pixel array
              0x28,0x00,0x00,0x00,    // bytes in DIB header
        WIDTH_0,WIDTH_1,0x00,0x00,    // width of bmp
      HEIGHT_0,HEIGHT_1,0xFF,0xFF,    // width of bmp
                        0x01,0x00,    // nr of color planes
                        0x01,0x00,    // bits per pixel
              0x00,0x00,0x00,0x00,    // no compression
              0x00,0x00,0x00,0x00,    // size of raw bitmap data
              0x00,0x00,0x00,0x00,    // print resolution / horizontal     
              0x00,0x00,0x00,0x00,    // print resolution / vertical
              0x00,0x00,0x00,0x00,    // number of colors in palette 
              0x00,0x00,0x00,0x00,    // all colors important
              0x00,0x00,0x00,0x00,    // 
              0xFF,0xFF,0xFF,0x00};   // color palette 

// outputs fbuf[] to BMP_NAME
void draw_bmp()
{
//  printf("HEIGHT 1: %X HEIGHT 0: %X\n", HEIGHT_1, HEIGHT_0);
//  printf("WIDTH: %X\n", (WIDTH_1 << 8) | WIDTH_0);
  FILE* fp;
  fp = fopen(BMP_NAME,"w");
  fwrite(header, 1, 62, fp);
  fwrite(fbuf, 1, sizeof(fbuf), fp);
  fclose(fp);
}
