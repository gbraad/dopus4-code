# Makefile for DOpus_Print module.

NAME := DOpus_Print
DESTDIR  := ../.bin
TARGET   := $(DESTDIR)/$(NAME)

CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip
TOUCH = touch
MAKEDIR = mkdir -p
REMOVE = rm -r -f
FLEXCAT = flexcat

CFLAGS = -gstabs -mcrt=newlib -Wall -O3 -I../include-os4
LDFLAGS = -mcrt=newlib -lauto -lm

OBJS = print.o printreq.o doprint.o data.o printdir.o strings.o

ifdef REVISE
BUMPREV:= revision
else
REVISE = sub
endif

all: $(BUMPREV) $(TARGET)

.PHONY: debug setdebug
debug: setdebug all
setdebug:
	$(eval override CFLAGS += -DDEBUG)

%.o : %.c
	@echo ">>>>>Compiling $<"
	@$(CC) -c $< $(CFLAGS) -o $@

$(TARGET) : printstrings.h strings.c $(OBJS)
	@$(MAKEDIR) $(DESTDIR)
	$(CC) $(OBJS) -o $@.debug $(LDFLAGS)
	$(STRIP) -s $@.debug -o $@

data.o: data.c DOpus_Print_rev.h

printstrings.h: ../locale/DOpus4_Print.cd printstrings_h.sd
	$(FLEXCAT) ../locale/DOpus4_Print.cd printstrings.h=printstrings_h.sd

strings.c: ../locale/DOpus4_Print.cd printstrings.h strings_c.sd
	$(FLEXCAT) ../locale/DOpus4_Print.cd strings.c=strings_c.sd

revision:
	revbump DOpus_Print $(REVISE)

clean:
	@$(REMOVE) *.o $(TARGET) $(TARGET).debug
	@$(REMOVE) printstrings.h strings.c
