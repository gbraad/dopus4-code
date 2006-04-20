ASM = Work:Dev/PhxAss/PhxAss
ASMOPTS = M=68020 SYMDEBUG SC SD 4

CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip

CFLAGS = -O3 -Wall -I../include-os4
LDFLAGS = -lauto

FLEXCAT = flexcat

OBJS = main.o main1.o main2.o main3.o main4.o main5.o\
screen.o screen_arrows.o screen_colors.o screen_fonts.o screen_palette.o \
screen_sliders.o screen_screenmode.o \
main7.o main8.o main9.o data.o gadgets.o strings.o paint_mode.o\
chipdata.o\
autoinit_layers_base.o autoinit_layers_main.o

APPNAME = ConfigOpus

all : $(APPNAME)

$(APPNAME) : $(OBJS)
	$(CC) -o $@.debug $^ $(LDFLAGS)
	$(STRIP) -s $@.debug -o $@

configstrings.h: ../catalogs/DOpus4_Config.cd configstrings_h.sd
	$(FLEXCAT) ../catalogs/DOpus4_Config.cd configstrings.h=configstrings_h.sd

strings.c: ../catalogs/DOpus4_Config.cd configstrings.h strings_c.sd
	$(FLEXCAT) ../catalogs/DOpus4_Config.cd strings.c=strings_c.sd

clean:
	rm -f *.o ConfigOpus
