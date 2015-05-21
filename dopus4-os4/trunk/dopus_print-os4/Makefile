CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip

CFLAGS = -gstabs -mcrt=newlib -Wall -O3 -I../include-os4
LDFLAGS = -mcrt=newlib -lauto -lm

OBJS = print.o printreq.o doprint.o data.o printdir.o strings.o

FLEXCAT = flexcat

ifdef REVISE
BUMPREV:= revision
else
REVISE = sub
endif

all: $(BUMPREV) DOpus_Print

%.o : %.c
	@echo ">>>>>Compiling $<"
	@$(CC) -c $< $(CFLAGS) -o $@

DOpus_Print : printstrings.h strings.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@.debug $(LDFLAGS)
	$(STRIP) -s $@.debug -o $@

data.o: data.c DOpus_Print_rev.h

printstrings.h: ../locale/DOpus4_Print.cd printstrings_h.sd
	$(FLEXCAT) ../locale/DOpus4_Print.cd printstrings.h=printstrings_h.sd

strings.c: ../locale/DOpus4_Print.cd printstrings.h strings_c.sd
	$(FLEXCAT) ../locale/DOpus4_Print.cd strings.c=strings_c.sd

revision:
	revbump DOpus_Print $(REVISE)

clean:
	@rm -rf *.o DOpus_Print DOpus_Print.debug
	@rm -rf printstrings.h strings.c
