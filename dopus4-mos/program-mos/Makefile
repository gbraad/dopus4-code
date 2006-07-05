CC = gcc -noixemul
STRIP = strip

OBJS = strings.o about.o arbiter.o archive.o assembly.o buffers.o\
complete.o custom.o cycling.o data.o dirtree.o doerror.o doidcmp.o\
dopusfunctions.o getusage.o icondata.o iffload.o launchexternal.o main.o\
main1.o main2.o main3.o main4.o main5.o main6.o main7.o main8.o main9.o\
main10.o main11.o main12.o main13.o main14.o main15.o main16.o main17.o\
main18.o main19.o main20.o main21.o main22.o parentmulti.o protectdata.o\
remember.o renamedata.o rexx.o screendata.o searchdata.o select.o\
selectdata.o tasks.o view.o popupmenu.o makelinkdata.o chipimage.o\
init_libs.o

CFLAGS = -Wall -O3 -I../include-os4 -Iusr:local/include -DUSE_INLINE_STDARG
LDFLAGS = -Llib
FLEXCAT = flexcat

APPNAME = DirectoryOpus

all : $(APPNAME).debug

$(APPNAME).debug : $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) -lpopupmenu
	$(STRIP) --remove-section=.comment $@ -o $(APPNAME)
	@chmod 744 $(APPNAME)

strings.c: ../catalogs/DOpus4.cd dopusstrings.h strings_c.sd
	$(FLEXCAT) ../catalogs/DOpus4.cd strings.c=strings_c.sd

dopusstrings.h: ../catalogs/DOpus4.cd dopusstrings_h.sd
	$(FLEXCAT) ../catalogs/DOpus4.cd dopusstrings.h=dopusstrings_h.sd

clean:
	@rm -f *.o $(APPNAME) $(APPNAME).debug $(APPNAME).s

dump:
	objdump --disassemble-all --reloc $(APPNAME).debug >$(APPNAME).s