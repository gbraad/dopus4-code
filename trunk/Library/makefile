ASM = Work:Dev/PhxAss/PhxAss
ASMOPTS = INCPATH Include: M=68020 SYMDEBUG

CC = m68k-amigaos-gcc
CCOPTS = -m68020 -fbaserel

OBJS = dopuslib.o borders.o dates.o dopus_stuff.o dospaths.o dos_exec.o \
filereq.o gui.o imagery.o language.o listview.o misc.o requesters.o \
simplerequest.o strings.o wildcard.o menus.o functions.o data.o

FLEXCAT = Work:Dev/FlexCat/FlexCat

all: ../../dopus.library

../../dopus.library: $(OBJS)
	$(CC) -o ../../dopus.library -nostartfiles $(OBJS) -noixemul $(CCOPTS) -lamiga -ldebug
# -s

dopuslib.h: ../catalogs/dopus4_lib.cd dopuslib_h.sd
	$(FLEXCAT) /catalogs/dopus4_lib.cd dopuslib.h=dopuslib_h.sd

language.c: ../catalogs/dopus4_lib.cd dopuslib.h language_c.sd
	$(FLEXCAT) /catalogs/dopus4_lib.cd language.c=language_c.sd

.c.o:
	$(CC) -c $*.c $(CCOPTS) -I../include -Os -fomit-frame-pointer -msmall-code -Wall -DDEBUG

data.o: data.a
	$(ASM) data.a $(ASMOPTS)

.a.o:
	$(ASM) $*.a $(ASMOPTS)
	hunk2aout >NIL: $*.o
	delete $*.o QUIET
	rename obj.0000.* $*.o
