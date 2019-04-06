#define BMP_NAME      "1.bmp"

#define WIDTH         (512)
#define HEIGHT        (512)

#define WORDSIZE      8
#define BUFFERSIZE    (HEIGHT * (WIDTH / WORDSIZE))
#define GETY(i)       (i/(WIDTH/WORDSIZE))
#define GETX(i)       (i%(WIDTH/WORDSIZE))
#define GETBUFPOS(x,y)  (y*(WIDTH/WORDSIZE) + x/WORDSIZE)

uint8_t fbuf[BUFFERSIZE] = {0xFF};
