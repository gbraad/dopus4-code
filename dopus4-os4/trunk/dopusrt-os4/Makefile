CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip

CFLAGS = -mcrt=clib2 -O3 -Wall -I../include-os4
LDFLAGS = -mcrt=clib2 -lauto

ALL: revision DOpusRT

DOpusRT: main.o
	$(CC) -o $@.debug $< $(LDFLAGS)
	$(STRIP) -s $@.debug -o $@
	@rm -f main.o

revision: DOpusRT_rev.h
	revbump DOpusRT

clean:
	@rm -f *.o DOpusRT DOpusRT.debug
