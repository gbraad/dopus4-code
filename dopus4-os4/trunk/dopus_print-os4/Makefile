CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip

CFLAGS = -mcrt=clib2 -Wall -O3 -I../include-os4
LDFLAGS = -mcrt=clib2 -lauto -lm

OBJS = print.o printreq.o doprint.o data.o printdir.o strings.o

FLEXCAT = flexcat

DOpus_Print : printstrings.h strings.c $(OBJS)
	$(CC) $(OBJS) -o $@.debug $(LDFLAGS)
	$(STRIP) -s $@.debug -o $@

printstrings.h: ../catalogs/DOpus4_Print.cd printstrings_h.sd
	$(FLEXCAT) ../catalogs/DOpus4_Print.cd printstrings.h=printstrings_h.sd

strings.c: ../catalogs/DOpus4_Print.cd printstrings.h strings_c.sd
	$(FLEXCAT) ../catalogs/DOpus4_Print.cd strings.c=strings_c.sd

revision:
	revbump DOpus_Print sub

clean:
	@rm -rf *.o DOpus_Print DOpus_Print.debug
	@rm -rf printstrings.h strings.c
