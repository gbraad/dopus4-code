ASM = Work:Dev/PhxAss/PhxAss
ASMOPTS = M=68020 SYMDEBUG SC SD 4
CCOPTS = -m68020 -resident
CC = m68k-amigaos-gcc

OBJS = diskop.o data.o lsprintf.o format.o strings.o diskcopy.o install.o

All : ../../DOpus_Disk

../../DOpus_Disk : $(OBJS)
	$(CC) -o ../../DOpus_Disk $(OBJS) -noixemul $(CCOPTS) -ldebug

.c.o:
	$(CC) -c $*.c $(CCOPTS) -I../include -msmall-code -Os -DDEBUG

lsprintf.o: lsprintf.asm
	$(ASM) lsprintf.asm $(ASMOPTS)
	@hunk2aout >NIL: $*.o
	@delete $*.o QUIET
	@rename obj.0000.* $*.o
