ASM = Work:Dev/PhxAss/PhxAss
ASMOPTS = M=68020 SYMDEBUG SC SD 4
CCOPTS = -m68020 -resident

OBJS = main.o main1.o main2.o main3.o main4.o main5.o\
screen.o screen_arrows.o screen_colors.o screen_fonts.o screen_palette.o \
screen_sliders.o screen_screenmode.o \
main7.o main8.o main9.o data.o gadgets.o lsprintf.o strings.o paint_mode.o\
chipdata.o

All : ../../ConfigOpus

../../ConfigOpus : $(OBJS)
	gcc -o ../../ConfigOpus *.o -noixemul $(CCOPTS) -ldebug -s

.c.o:
	gcc -c $*.c $(CCOPTS) -I../include -msmall-code -fomit-frame-pointer -O3 -Wall
# -DDEBUG

lsprintf.o: lsprintf.asm
	$(ASM) $*.asm $(ASMOPTS)
	@hunk2aout >NIL: $*.o
	@delete $*.o QUIET
	@rename obj.0000.* $*.o

chipdata.o: chipdata.asm
	$(ASM) $*.asm $(ASMOPTS)
