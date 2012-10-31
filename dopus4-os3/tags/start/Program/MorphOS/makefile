vpath %.c ../
CC = ppc-morphos-gcc
CCOPTS = -mcpu=604
CDEFS = 
INC = -I../../include-mos -I/gg/mos-include

OBJS = about.o arbiter.o archive.o assembly.o buffers.o complete.o\
custom.o cycling.o data.o dirtree.o doerror.o doidcmp.o dopusfunctions.o\
getusage.o icondata.o iffload.o launchexternal.o main.o main1.o main2.o\
main3.o main4.o main5.o main6.o main7.o main8.o main9.o main10.o main11.o\
main12.o main13.o main14.o main15.o main16.o main17.o main18.o main19.o\
main20.o main21.o main22.o parentmulti.o protectdata.o remember.o\
renamedata.o rexx.o screendata.o searchdata.o select.o selectdata.o\
strings.o tasks.o view.o chipimage.o

LIBS = -L./ -lm -lpm -lasync
# -ldebug

APPNAME = ../../../DirectoryOpus.elf

All : $(APPNAME)

$(APPNAME) : $(OBJS)
	$(CC) -o $(APPNAME) $(OBJS) $(CCOPTS) -noixemul $(LIBS)
# /gg/lib/libb/libm020/libnix/detach.o
# -s

.c.o:
	$(CC) -c ../$*.c $(CCOPTS) -O3 -fomit-frame-pointer $(CDEFS) -Wall $(INC)
# -g

#chipimage.o: chipimage.asm
#	$(ASM) $*.asm $(ASMOPTS)

#assembly.o: assembly.asm
#	$(ASM) $*.asm $(ASMOPTS)
#	@hunk2aout $*.o >NIL:
#	@delete $*.o QUIET
#	@rename obj.* $*.o

#getusage.o: getusage.asm
#	$(ASM) $*.asm $(ASMOPTS)
#	@hunk2aout $*.o >NIL:
#	@delete $*.o QUIET
#	@rename obj.* $*.o

PCHGSRCS = HuffComp.c PCHGLib.c ParsePCHG.c SHAM2PCHG.c SetUserCopList.c

PCHGOBJS = HuffComp.o PCHGLib.o ParsePCHG.o SHAM2PCHG.o SetUserCopList.o\
           BuildFreqTable.o FastDecomp.o

libpchg.a: ../../pchg/BuildFreqTable.o ../../pchg/FastDecomp.o
	cd ../../pchg; $(CC) -c *.c -I../include -fomit-frame-pointer -m68020 -O2
	ar qc libpchg.a ../../pchg/*.o
	ranlib libpchg.a

../../pchg/BuildFreqTable.o: ../../pchg/BuildFreqTable.a
	cd ../../pchg; $(ASM) BuildFreqTable.a $(ASMOPTS)
	cd ../../pchg; hunk2aout $*.o >NIL:; rm $*.o; mv obj.* $*.o

../pchg/FastDecomp.o: ../../pchg/FastDecomp.a
	cd ../../pchg; $(ASM) FastDecomp.a $(ASMOPTS)
	cd ../../pchg; hunk2aout $*.o >NIL:; rm $*.o; mv obj.* $*.o

