CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip

CFLAGS = -Wall -O3 -I../include-os4
LDFLAGS = -lauto -lm

OBJS = print.o printreq.o doprint.o data.o printdir.o strings.o

FLEXCAT = flexcat

All : DOpus_Print

DOpus_Print : $(OBJS)
	$(CC) $^ -o $@.debug $(LDFLAGS)
	$(STRIP) -s $@.debug -o $@

printstrings.h: ../catalogs/DOpus4_Print.cd printstrings_h.sd
	$(FLEXCAT) ../catalogs/DOpus4_Print.cd printstrings.h=printstrings_h.sd

strings.c: ../catalogs/DOpus4_Print.cd printstrings.h strings_c.sd
	$(FLEXCAT) ../catalogs/DOpus4_Print.cd strings.c=strings_c.sd

clean:
	@rm -rf *.o DOpus_Print DOpus_Print.debug
