CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip

CFLAGS = -mcrt=clib2 -O3 -Wall -I../include-os4
LDFLAGS = -mcrt=clib2 -lauto

DOpusRT: main.o
	$(CC) -o $@.debug $< $(LDFLAGS)
	$(STRIP) -s $@.debug -o $@

revision: DOpusRT_rev.h
	revbump DOpusRT sub

clean:
	@rm -f *.o DOpusRT DOpusRT.debug
