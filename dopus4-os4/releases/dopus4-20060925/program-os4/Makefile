CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip
TOUCH = touch

OBJS = strings.o about.o arbiter.o archive.o assembly.o buffers.o\
complete.o custom.o data.o dirtree.o doerror.o doidcmp.o\
dopusfunctions.o getusage.o icondata.o iffload.o launchexternal.o main.o\
main1.o main2.o main3.o main4.o main5.o main6.o main7.o main8.o main9.o\
main10.o main11.o main12.o main13.o main14.o main15.o main16.o main17.o\
main18.o main19.o main20.o main21.o main22.o parentmulti.o protectdata.o\
remember.o renamedata.o rexx.o screendata.o searchdata.o select.o\
selectdata.o tasks.o view.o popupmenu.o makelinkdata.o chipimage.o\
autoinit_amigaguide_base.o autoinit_amigaguide_main.o\
autoinit_commodities_base.o autoinit_commodities_main.o\
autoinit_gadtools_base.o autoinit_gadtools_main.o\
autoinit_layers_base.o autoinit_layers_main.o\
autoinit_pm_base.o autoinit_pm_main.o\
autoinit_rexxsyslib_base.o autoinit_rexxsyslib_main.o\
autoinit_xadmaster_base.o autoinit_xadmaster_main.o\
autoinit_application_application.o autoinit_application_base.o autoinit_application_prefsobjects.o

CFLAGS = -mcrt=clib2 -Wall -O3 -I../include-os4
LDFLAGS = -mcrt=clib2 -lauto -lm
FLEXCAT = flexcat

ifdef REVISE
BUMPREV:= revision
else
REVISE = sub
endif

all: $(BUMPREV) DirectoryOpus

# DirectoryOpus itself
DirectoryOpus: $(OBJS)
	$(CC) -o $@.debug $^ $(LDFLAGS)
	$(STRIP) -s $@.debug -o $@

about.o: about.c DirectoryOpus_rev.h
tasks.o: tasks.c DirectoryOpus_rev.h

strings.c: ../catalogs/DOpus4.cd dopusstrings.h strings_c.sd
	$(FLEXCAT) ../catalogs/DOpus4.cd strings.c=strings_c.sd

dopusstrings.h: ../catalogs/DOpus4.cd dopusstrings_h.sd
	$(FLEXCAT) ../catalogs/DOpus4.cd dopusstrings.h=dopusstrings_h.sd

# revision targets
.PHONY: revision
revision:
	revbump DirectoryOpus $(REVISE)

# clean targets
clean:
	@rm -f *.o $(APPNAME) $(APPNAME).debug
	@rm -f dopusstrings.h strings.c
