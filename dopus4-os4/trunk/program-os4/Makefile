# Makefile for the DirectoryOpus program

CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip
TOUCH = touch

OPTIMIZE = -O3
WARNINGS = -Wall -Wwrite-strings
DEBUGOUT = #-DDEBUG
DEBUG = -gstabs $(DEBUGOUT)
CFLAGS = $(DEBUG) $(OPTIMIZE) $(WARNINGS) -I../include-os4 -I../external
LDFLAGS = -gstabs -lauto -lraauto -lm
FLEXCAT = flexcat

ifdef REVISE
BUMPREV:= revision
else
REVISE = sub
endif

OBJS = strings.o about.o arbiter.o archive.o assembly.o buffers.o\
complete.o custom.o data.o dirtree.o doerror.o doidcmp.o recurse.o\
dopusfunctions.o getusage.o icondata.o iffload.o launchexternal.o main.o\
main1.o main2.o main3.o main4.o main5.o main6.o main7.o main8.o main9.o\
main10.o main11.o main12.o main13.o main14.o main15.o main16.o main17.o\
main18.o main19.o main20.o main21.o main22.o parentmulti.o protectdata.o\
remember.o renamedata.o rexx.o screendata.o searchdata.o select.o\
selectdata.o tasks.o view.o popupmenu.o makelinkdata.o chipimage.o\
reaction.o strstri.o

all: $(BUMPREV) DirectoryOpus

%.o : %.c
	@echo ">>>>>Compiling $<"
	@$(CC) -c $< $(CFLAGS) -o $@

DirectoryOpus: $(OBJS)
	$(CC) -o $@.debug $^ $(LDFLAGS)
	$(STRIP) -s $@.debug -o $@

about.o: about.c DirectoryOpus_rev.h
tasks.o: tasks.c DirectoryOpus_rev.h

strings.c: ../locale/DOpus4.cd dopusstrings.h strings_c.sd
	$(FLEXCAT) ../locale/DOpus4.cd strings.c=strings_c.sd

dopusstrings.h: ../locale/DOpus4.cd dopusstrings_h.sd
	$(FLEXCAT) ../locale/DOpus4.cd dopusstrings.h=dopusstrings_h.sd

# revision targets
.PHONY: revision
revision:
	revbump DirectoryOpus $(REVISE)

# clean targets
clean:
	@rm -f *.o DirectoryOpus DirectoryOpus.debug
	@rm -f dopusstrings.h strings.c
