CC = gcc

OBJS = about.o arbiter.o archive.o assembly.o buffers.o complete.o\
custom.o cycling.o data.o dirtree.o doerror.o doidcmp.o dopusfunctions.o\
getusage.o icondata.o iffload.o launchexternal.o main.o main1.o main2.o\
main3.o main4.o main5.o main6.o main7.o main8.o main9.o main10.o main11.o\
main12.o main13.o main14.o main15.o main16.o main17.o main18.o main19.o\
main20.o main21.o main22.o parentmulti.o protectdata.o remember.o\
renamedata.o rexx.o screendata.o searchdata.o select.o selectdata.o\
strings.o tasks.o view.o popupmenu.o chipimage.o

CCOPTS = -m68020 -fbaserel
INC = -I../include
LIBS = -L./ -lm -lpm -lpchg -lasync -ldebug
ASM = Work:Dev/PhxAss/PhxAss
ASMOPTS = INCPATH Include: MACHINE=68020 SYMDEBUG SD 4

APPNAME = ../../DirectoryOpus

All : $(APPNAME)

$(APPNAME) : $(OBJS)
	$(CC) -o $(APPNAME) $(OBJS) $(CCOPTS) -noixemul /gg/lib/libb/libm020/libnix/detach.o $(LIBS)
# -s

.c.o:
	$(CC) -c $*.c $(CCOPTS) -O3 -fomit-frame-pointer $(INC) -DWITHPCHG -DDEBUG -Wall
# -g

chipimage.o: chipimage.asm
	$(ASM) $*.asm $(ASMOPTS)

assembly.o: assembly.asm
	$(ASM) $*.asm $(ASMOPTS)
	@hunk2aout $*.o >NIL:
	@delete $*.o QUIET
	@rename obj.* $*.o

#getusage.o: getusage.asm
#	$(ASM) $*.asm $(ASMOPTS)
#	@hunk2aout $*.o >NIL:
#	@delete $*.o QUIET
#	@rename obj.* $*.o
