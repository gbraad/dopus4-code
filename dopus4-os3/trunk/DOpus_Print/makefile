ASM = Work:Dev/PhxAss/PhxAss
ASMOPTS = M=68020 SYMDEBUG SC SD 4

CC = m68k-amigaos-gcc
CCOPTS = -m68020 -resident

OBJS = print.o printreq.o doprint.o data.o printdir.o lsprintf.o strings.o

FLEXCAT = Work:Dev/FlexCat/FlexCat

All : ../../DOpus_Print

../../DOpus_Print : $(OBJS)
	$(CC) -o ../../DOpus_Print $(OBJS) -noixemul $(CCOPTS) -ldebug -s

printstrings.h: ../catalogs/DOpus4_Print.cd printstrings_h.sd
	$(FLEXCAT) /catalogs/DOpus4_Print.cd printstrings.h=printstrings_h.sd

strings.c: ../catalogs/DOpus4_Print.cd printstrings.h strings_c.sd
	$(FLEXCAT) /catalogs/DOpus4_Print.cd strings.c=strings_c.sd

.c.o:
	$(CC) -c $*.c $(CCOPTS) -I../include -msmall-code -Os

lsprintf.o: lsprintf.asm
	$(ASM) lsprintf.asm $(ASMOPTS)
	@hunk2aout >NIL: $*.o
	@delete $*.o QUIET
	@rename obj.0000.* $*.o
