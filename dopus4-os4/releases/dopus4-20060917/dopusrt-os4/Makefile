CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip

CFLAGS = -mcrt=clib2 -O3 -Wall -I../include-os4
LDFLAGS = -mcrt=clib2 -lauto

ifdef REVISE
BUMPREV:= revision
else
REVISE = sub
endif

all: $(BUMPREV) DOpusRT

DOpusRT: main.o
	$(CC) -o $@.debug $< $(LDFLAGS)
	$(STRIP) -s $@.debug -o $@

main.o: main.c DOpusRT_rev.h

revision:
	revbump DOpusRT $(REVISE)

clean:
	@rm -f *.o DOpusRT DOpusRT.debug
