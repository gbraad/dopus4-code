ASM = Work:Dev/PhxAss/PhxAss
ASMOPTS = M=68020 SYMDEBUG SC SD 4

CC = m68k-amigaos-gcc
CCOPTS = -m68020 -resident

OBJS = print.o printreq.o doprint.o data.o printdir.o lsprintf.o strings.o

All : ../../DOpus_Print

../../DOpus_Print : $(OBJS)
	$(CC) -o ../../DOpus_Print $(OBJS) -noixemul $(CCOPTS) -ldebug -s

.c.o:
	$(CC) -c $*.c $(CCOPTS) -I../include -msmall-code -Os

lsprintf.o: lsprintf.asm
	$(ASM) lsprintf.asm $(ASMOPTS)
	@hunk2aout >NIL: $*.o
	@delete $*.o QUIET
	@rename obj.0000.* $*.o
