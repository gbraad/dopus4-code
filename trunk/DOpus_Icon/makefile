ASM = Work:Dev/PhxAss/PhxAss
ASMOPTS = M=68020 SYMDEBUG SC SD 4

CC = m68k-amigaos-gcc
CCOPTS = -m68020 -resident

FLEXCAT = Work:Dev/FlexCat/FlexCat

OBJS = data.o main.o doiconinfo.o lsprintf.o strings.o

All : ../../DOpus_Icon

../../DOpus_Icon : $(OBJS)
	$(CC) -o ../../DOpus_Icon -noixemul $(OBJS) $(CCOPTS) -s -ldebug

iconstrings.h: ../catalogs/DOpus4_Icon.cd iconstrings_h.sd
	$(FLEXCAT) /catalogs/DOpus4_Icon.cd iconstrings.h=iconstrings_h.sd

strings.c: ../catalogs/DOpus4_Icon.cd iconstrings.h strings_c.sd
	$(FLEXCAT) /catalogs/DOpus4_Icon.cd strings.c=strings_c.sd

.c.o:
	$(CC) -c $*.c $(CCOPTS) -I../include -msmall-code -Os

lsprintf.o: lsprintf.asm
	$(ASM) lsprintf.asm $(ASMOPTS)
	@hunk2aout >NIL: $*.o
	@delete $*.o QUIET
	@rename obj.0000.* $*.o
