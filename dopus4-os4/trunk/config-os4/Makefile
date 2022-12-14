# Makefile for ConfigOpus module

NAME := ConfigOpus
DESTDIR  := ../.bin
TARGET   := $(DESTDIR)/$(NAME)

CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip
TOUCH = touch
MAKEDIR = mkdir -p
REMOVE = rm -r -f

OPTIMIZE = -O3
WARNINGS = -Wall -Wwrite-strings
DEBUG = -gstabs
CFLAGS = -mcrt=newlib $(DEBUG) $(OPTIMIZE) $(WARNINGS) -I../include-os4
LDFLAGS = -mcrt=newlib -lauto

FLEXCAT = flexcat

ifdef REVISE
BUMPREV:= revision
else
REVISE = sub
endif

OBJS = main.o main1.o main2.o main3.o main4.o main5.o screen.o \
screen_arrows.o screen_colors.o screen_fonts.o screen_palette.o \
screen_sliders.o screen_screenmode.o data.o main7.o main8.o \
main9.o gadgets.o strings.o paint_mode.o chipdata.o swapmem.o \
autoinit_layers_base.o autoinit_layers_main.o

all: $(BUMPREV) $(TARGET)

.PHONY: debug setdebug
debug: setdebug all
setdebug:
	$(eval override CFLAGS += -DDEBUG)

%.o : %.c
	@echo ">>>>>Compiling $<"
	@$(CC) -c $< $(CFLAGS) -o $@

$(TARGET): configstrings.h strings.c $(OBJS)
	@$(MAKEDIR) $(DESTDIR)
	$(CC) -o $@.debug $(OBJS) $(LDFLAGS)
	$(STRIP) -s $@.debug -o $@

data.o: data.c ConfigOpus_rev.h

configstrings.h: ../locale/DOpus4_Config.cd configstrings_h.sd
	$(FLEXCAT) ../locale/DOpus4_Config.cd configstrings.h=configstrings_h.sd

strings.c: ../locale/DOpus4_Config.cd configstrings.h strings_c.sd
	$(FLEXCAT) ../locale/DOpus4_Config.cd strings.c=strings_c.sd

# revision target
.PHONY: revision
revision:
	revbump ConfigOpus $(REVISE)

# clean target
clean:
	@$(REMOVE) *.o $(TARGET) $(TARGET).debug
	@$(REMOVE) configstrings.h strings.c
