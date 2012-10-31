ASM = Work:Dev/PhxAss/bin/PhxAss
ASMOPTS = M=68020 SYMDEBUG SC SD 4
CCOPTS = -m68020 -resident

OBJS = data.o main.o doiconinfo.o lsprintf.o strings.o

All : ../../DOpus_Icon

../../DOpus_Icon : $(OBJS)
	gcc -o ../../DOpus_Icon -noixemul $(OBJS) $(CCOPTS) -s -ldebug

.c.o:
	gcc -c $*.c $(CCOPTS) -I../include -msmall-code -Os

lsprintf.o: lsprintf.asm
	$(ASM) lsprintf.asm $(ASMOPTS)
	@hunk2aout >NIL: $*.o
	@delete $*.o QUIET
	@rename obj.0000.* $*.o
