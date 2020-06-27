Example subblock:

0x8C, 0x2D, 0x99, 0x87, 0x2A, 0x1C, 0xDC, 0x33, 0xA0, 0x02, 0x75, 0xEC, 0x95, 0xFA, 0xA8, 0xDE, 0x60, 0x8C, 0x04, 0x91, 0x4C, 0x01 

First 8 bytes reversed, binary:

Hexadecimal       0x33     0xDC     0x1C     0x2A     0x87     0x99     0x2D     0x8C
Binary        00110011 11011100 00011100 00101010 10000111 10011001 00101101 10001100
Pack break        .     .    .     .   .    .   .    .   .    .   .    .  .   .  .  .
Pack size                          5                                   4            3
                    
Unpack from right to left:

00110 01111 01110 0001 1100 0010 1010 1000 0111 1001 1001 0010 110 110 001 100
 
Code stream left to right decimal:
 
Code    4 1 6 6 2 9 9 7 8 10 2 12 1 14 15 6
Bits    3       4                 5

Correponding string insertions:

4 1 6 6 2 9  9  7  8 10  2 12  1 14 15  6
- 1 6 7 8 9 10 11 12 13 14 15 16 17 18 19


