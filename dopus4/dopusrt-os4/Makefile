CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip

CFLAGS = -O3 -Wall -I../include-os4
LDFLAGS = -lauto

DOpusRT: DOpusRT.o autoinit_dopus_base.o autoinit_dopus_main.o
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) -s $@

clean:
	rm -f *.o DOpusRT
