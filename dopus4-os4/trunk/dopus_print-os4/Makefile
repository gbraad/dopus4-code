CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip

CFLAGS = -mcrt=clib2 -Wall -O3 -I../include-os4
LDFLAGS = -mcrt=clib2 -lauto -lm

OBJS = print.o printreq.o doprint.o data.o printdir.o strings.o

FLEXCAT = flexcat

All : DOpus_Print

DOpus_Print : $(OBJS)
	$(CC) $^ -o $@.debug $(LDFLAGS)
	$(STRIP) -s $@.debug -o $@
	@rm -f data.o

data.o: data.c DOpus_Print_rev.h
	revbump DOpus_Print
	$(CC) -c $(CFLAGS) $< -o $@

printstrings.h: ../catalogs/DOpus4_Print.cd printstrings_h.sd
	$(FLEXCAT) ../catalogs/DOpus4_Print.cd printstrings.h=printstrings_h.sd

strings.c: ../catalogs/DOpus4_Print.cd printstrings.h strings_c.sd
	$(FLEXCAT) ../catalogs/DOpus4_Print.cd strings.c=strings_c.sd

clean:
	@rm -rf *.o DOpus_Print DOpus_Print.debug
