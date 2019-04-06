
from PIL import Image
im = Image.open("8x12.png", "r")
pixlist = list(im.getdata())

text = ["", ]

for i, pix in enumerate(pixlist):
  if(pix < 3):
    print "#",
  else:
    print "_",
  if(i%128 is 0):
    print ""

print "\n\n"
char = []
cols = 12
rows = 100
font = [[0 for i in range(cols)] for j in range(rows)]
ch = 0
for i, pix in enumerate(pixlist):
  if(pix < 3):
    print "#",
    ch |= 0x01 << (8-i%8)
  else:
    print "_",
  if(i%8 is 0):
    line_index = ((i/8)/16)%12
    letter_index = (i/8)%16 +((i/1536)*16)
    print hex(ch), line_index, letter_index, 
    font[letter_index][line_index] = ch
    char.append(ch)
    ch = 0
  if(i%128 is 0):
    print ""

print "\n\n"
for i,c in enumerate(font):
  print i,"{"
  for k,b in enumerate(c):
    print format(b, '#04x'),",",
    if((k+1)%4 is 0):
      print ""
  print "},"


