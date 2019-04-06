#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "font.h"

#define WIDTH         (256)
#define HEIGHT        (256)

#define WIDTH_0       WIDTH & 0xFF
#define WIDTH_1       (WIDTH & 0xFF00) >> 8
#define HEIGHT_0      0xFF -(HEIGHT & 0xFF)
#define HEIGHT_1      0xFF -((HEIGHT & 0xFF00) >> 8)
#define BITS_PIXEL    1
#define RAWSIZE       (WIDTH*HEIGHT)/(8/BITS_PIXEL)+62
#define RAWSIZE_0     RAWSIZE & 0xFF
#define RAWSIZE_1     (RAWSIZE & 0xFF00) >> 8

#define WORDSIZE      8
#define BUFFERSIZE    (HEIGHT * (WIDTH / WORDSIZE))
#define GETY(i)       (i/(WIDTH/WORDSIZE))
#define GETX(i)       (i%(WIDTH/WORDSIZE))
#define GETBUFPOS(x,y)  (y*(WIDTH/WORDSIZE) + x/WORDSIZE)
  
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
                                          
uint8_t bmp_array[BUFFERSIZE] = {0x00};
char txt_buff[128] = "\0";

void outputBmp()
{
  FILE* fp;
  fp = fopen("1.bmp","w");
  fwrite(header, 1, 62, fp);
  fwrite(bmp_array, 1, sizeof(bmp_array), fp);
  fclose(fp);
}

void drawDot(uint16_t x, uint16_t y)
{
  bmp_array[GETBUFPOS(x,y)] |= 0x01 << (WORDSIZE-1-(x%WORDSIZE));
}


void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  int16_t dx = abs(x1-x0);
  int16_t sx = x0<x1 ? 1 : -1;
  int16_t dy = abs(y1-y0);
  int16_t sy = y0<y1 ? 1 : -1; 
  int16_t err = (dx>dy ? dx : -dy)/2, e2;

  while(1)
  {
    drawDot(x0,y0);
    if (x0 == x1 && y0 == y1) 
      break;
    e2 = err;
    if (e2 > -dx) { 
      err -= dy; 
      x0 += sx; 
    }
    if (e2 < dy) { 
      err += dx; 
      y0 += sy; 
    }
  }
}

void drawRect(uint16_t p1x, uint16_t p1y, uint16_t p2x, uint16_t p2y)
{
  for(uint16_t y=p1y; y<=p2y; y++)
    for(uint16_t x=p1x; x<=p2x; x++)
      drawDot(x,y);
}

void printChar(uint8_t chr, uint16_t x, uint16_t y)
{
  for(uint8_t cs=0; cs<8*12; cs++)
  {
    uint16_t y_ = y + cs/8;
    uint16_t x_ = x + cs%8;
    if((char_lut[chr][cs/8] >> (8-(cs%8))) & 0x01)
    {
      //printf("%u %u - %u %u\n", cs/8, cs%8, x_, y_);
      drawDot(x_,y_);
    }
  }
}

void pprint(uint16_t x, uint16_t y)
{
  uint8_t c = 0;
  while(txt_buff[c] != 0x0D)  {
    printChar(txt_buff[c]-32, x, y);
    printf("%c", txt_buff[c]);
    c++;  
    x+=8;
  }
  printf("\n");

/*
  while(c != "\0")
  {
    printf("%c", c);
    c++;
  }
*/
}

int main()
{
  printf("pinsel v0.1\n");

//  drawLine(128,1,1,128);
//  drawLine(1,1,128,128);

//  drawDot(32,32);
//  drawDot(34,32);
//  drawDot(36,32);
//  drawDot(38,32);
//  drawDot(40,32);

//  uint8_t chr = 0;
//  for(uint16_t y = 0; y < HEIGHT; y+=12)  {
//    for(uint16_t x = 0; x < WIDTH; x+=8)  {
//      printChar(chr%100, x,y);
//      chr++;    
//    }
//  }

  sprintf(txt_buff, "hello there x:<\r");
  pprint(1,1); 
  drawLine(1,12, 118,12);

  outputBmp();
  return 0;
}
