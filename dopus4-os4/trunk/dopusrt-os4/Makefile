CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip

CFLAGS = -mcrt=clib2 -O3 -Wall -I../include-os4
LDFLAGS = -mcrt=clib2 -lauto

ifdef REVISE
BUMPREV:= revision
endif

all: $(BUMPREV) DOpusRT

DOpusRT: main.o DOpusRT_rev.h
	$(CC) -o $@.debug $< $(LDFLAGS)
	$(STRIP) -s $@.debug -o $@

revision:
	revbump DOpusRT $(REVISE)

clean:
	@rm -f *.o DOpusRT DOpusRT.debug
