CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip

CFLAGS = -maltivec -mcrt=newlib -Wall -Os -I../include-os4
LDFLAGS = -maltivec -mcrt=newlib -lauto -lm

OBJS = print.o printreq.o doprint.o data.o printdir.o strings.o

FLEXCAT = flexcat

ifdef REVISE
BUMPREV:= revision
else
REVISE = sub
endif

all: $(BUMPREV) DOpus_Print

DOpus_Print : printstrings.h strings.c $(OBJS)
	$(CC) $(OBJS) -o $@.debug $(LDFLAGS)
	$(STRIP) -s $@.debug -o $@

data.o: data.c DOpus_Print_rev.h

printstrings.h: ../catalogs/DOpus4_Print.cd printstrings_h.sd
	$(FLEXCAT) ../catalogs/DOpus4_Print.cd printstrings.h=printstrings_h.sd

strings.c: ../catalogs/DOpus4_Print.cd printstrings.h strings_c.sd
	$(FLEXCAT) ../catalogs/DOpus4_Print.cd strings.c=strings_c.sd

revision:
	revbump DOpus_Print $(REVISE)

clean:
	@rm -rf *.o DOpus_Print DOpus_Print.debug
	@rm -rf printstrings.h strings.c
