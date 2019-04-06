char =  [
                                0x00, 0x00, 0x00, 0x00, 
                                0xf8, 0x24, 0x24, 0x24, 
                                0x24, 0x24, 0x00, 0x00,  
        ]

for c in char:
  c = (c >> 1) ^ 0x80
  print format(c ,"#010b"), " ", hex(c);

print "\n"
for i, c in enumerate(char):
  c = (c >> 1) ^ 0x80
  print hex(c),
  if((i+1) % 4 is 0):
    print ","  
  else:
    print ",",

