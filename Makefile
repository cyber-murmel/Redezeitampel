MCU=msp430g2553
CC=msp430-elf-gcc -mmcu=$(MCU)
SUPPORTFILES=$(HOME)/ti/gcc/include
CFLAGS=-I$(SUPPORTFILES) -Os
LDFLAGS=-L$(SUPPORTFILES) -minrt

.c.s:
	$(CC) $(CFLAGS)	-S -o $@ $<

.c.i:
	$(CC) $(CFLAGS) -E $<

.SUFFIXES: .i
