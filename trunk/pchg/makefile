CC = m68k-amigaos-gcc
ASM = Work:Dev/PhxAss/PhxAss
ASMOPTS = INCPATH Include: MACHINE=68020 SYMDEBUG SD 4

PCHGSRCS = HuffComp.c PCHGLib.c ParsePCHG.c SHAM2PCHG.c SetUserCopList.c

PCHGOBJS = HuffComp.o PCHGLib.o ParsePCHG.o SHAM2PCHG.o SetUserCopList.o\
           BuildFreqTable.o FastDecomp.o

libpchg.a: BuildFreqTable.o FastDecomp.o
	$(CC) -c *.c -I../include -fomit-frame-pointer -m68020 -O2
	ar qc libpchg.a *.o
	ranlib libpchg.a

BuildFreqTable.o: BuildFreqTable.a
	$(ASM) BuildFreqTable.a $(ASMOPTS)
	hunk2aout $*.o >NIL:
	rm $*.o
	mv obj.* $*.o

FastDecomp.o: FastDecomp.a
	$(ASM) FastDecomp.a $(ASMOPTS)
	hunk2aout $*.o >NIL:
	rm $*.o
	mv obj.* $*.o
