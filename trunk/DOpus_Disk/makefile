ASM = Work:Dev/PhxAss/PhxAss
ASMOPTS = M=68020 SYMDEBUG SC SD 4
CCOPTS = -m68020 -resident -Wall
CC = m68k-amigaos-gcc
FLEXCAT = Work:Dev/FlexCat/FlexCat

OBJS = strings.o diskop.o data.o lsprintf.o format.o diskcopy.o install.o

All : ../../DOpus_Disk

../../DOpus_Disk : $(OBJS)
	$(CC) -o ../../DOpus_Disk $(OBJS) -noixemul $(CCOPTS) -ldebug

diskstrings.h: ../catalogs/DOpus4_Disk.cd diskstrings_h.sd
	$(FLEXCAT) /catalogs/DOpus4_Disk.cd diskstrings.h=diskstrings_h.sd

strings.c: ../catalogs/DOpus4_Disk.cd diskstrings.h strings_c.sd
	$(FLEXCAT) /catalogs/DOpus4_Disk.cd strings.c=strings_c.sd

.c.o: diskop.h
	$(CC) -c $*.c $(CCOPTS) -I../include -msmall-code -Os -DDEBUG

lsprintf.o: lsprintf.asm
	$(ASM) lsprintf.asm $(ASMOPTS)
	@hunk2aout >NIL: $*.o
	@delete $*.o QUIET
	@rename obj.0000.* $*.o
