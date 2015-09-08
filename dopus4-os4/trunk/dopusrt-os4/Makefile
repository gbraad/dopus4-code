# Makefile for DOpusRT runtime program

NAME := DOpusRT
DESTDIR  := ../.bin
TARGET   := $(DESTDIR)/$(NAME)

CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip
TOUCH = touch
MAKEDIR = mkdir -p
REMOVE = rm -r -f

CFLAGS = -gstabs -mcrt=newlib -O3 -Wall -I../include-os4
LDFLAGS = -mcrt=newlib -lauto

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

main.o: main.c DOpusRT_rev.h
	@echo ">>>>>Compiling $<"
	@$(CC) -c $< $(CFLAGS) -o $@

$(TARGET): main.o
	@$(MAKEDIR) $(DESTDIR)
	$(CC) -o $@.debug $< $(LDFLAGS)
	@$(STRIP) -s $@.debug -o $@

# revision target
.PHONY: revision
revision:
	revbump $(NAME) $(REVISE)

clean:
	@$(REMOVE) *.o $(TARGET) $(TARGET).debug
