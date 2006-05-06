CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip

CFLAGS = -O3 -Wall -I../include-os4
LDFLAGS = -lauto

DOpusRT: main.o
	$(CC) -o $@.debug $^ $(LDFLAGS)
	$(STRIP) -s $@.debug -o $@

clean:
	@rm -f *.o DOpusRT DOpusRT.debug
