// gcc -o printSTR printstr.c -fbaserel -noixemul -ldebug -s
/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include <exec/types.h>
#include <exec/memory.h>
#include <libraries/iffparse.h>

#include <proto/dos.h>
#include <proto/powerpacker.h>

#include "include/dopus/stringdata.h"

typedef struct {
    ULONG ckID;
    ULONG ckSize;
} ChunkHeader;

__saveds ReadStringFile(register struct StringData *stringdata __asm("a0"),
    register char *filename __asm("a1"))
{
    int a;
    struct DefaultString *defstr;
    BPTR file;

    if (!stringdata->string_table) {
        if (!(stringdata->string_table=AllocMem(stringdata->string_count*4,MEMF_CLEAR)))
            return(0);
    }
kprintf("ReadStringFile(%s)\n",filename);

    if (filename && (file=Open(filename,MODE_OLDFILE))) {
        struct Library *PPBase;
        struct Process *myproc;
        APTR wsave;
        ULONG test[3];

        Read(file,(char *)test,12);
        if (test[0]=='PX20' || test[0]=='PP11' || test[0]=='PP20') {
            Close(file);
            if ((PPBase=OpenLibrary("powerpacker.library",0))) {
                myproc=(struct Process *)FindTask(NULL);
                wsave=myproc->pr_WindowPtr;
                myproc->pr_WindowPtr=(APTR)-1;

                ppLoadData(filename,
                    DECR_NONE,
                    MEMF_CLEAR,
                    &stringdata->string_buffer,
                    &stringdata->string_size,
                    NULL);

                myproc->pr_WindowPtr=wsave;
                CloseLibrary(PPBase);
            }
        }
        else {
            if (test[0]==ID_FORM && test[2]==ID_OSTR) {
                Seek(file,0,OFFSET_END);
                if ((stringdata->string_size=Seek(file,0,OFFSET_BEGINNING))>0 &&
                    (stringdata->string_buffer=AllocMem(stringdata->string_size,0)))
                    Read(file,stringdata->string_buffer,stringdata->string_size);
            }
            Close(file);
        }
    }
    if (stringdata->string_buffer) {
        int size,*ptr;
        StringHeader *header;
        String *string;
        char *cptr;

        ptr=(int *)stringdata->string_buffer;
        header=(StringHeader *)&stringdata->string_buffer[12];

        if (ptr[0]!=ID_FORM || ptr[2]!=ID_OSTR ||
            header->header_id!=ID_STHD) {
            FreeMem(stringdata->string_buffer,stringdata->string_size);
            stringdata->string_buffer=NULL;
        }
        else {
            cptr=(char *)header+sizeof(StringHeader);

            for (a=0;a<header->stringcount;a++) {
                string=(String *)cptr;
                if (string->chunk_id==ID_STRN) {
printf("%ld\t%s\n",string->string_id,cptr+sizeof(String));
               }
               size=string->chunk_size+sizeof(ChunkHeader);
                if (size%2) ++size;
                cptr+=size;
                if (cptr>=(stringdata->string_buffer+stringdata->string_size)) break;
            }
        }
    }
    return(1);
}

void __saveds FreeStringFile(register struct StringData *stringdata __asm("a0"))
{
    if (stringdata) {
        if (stringdata->string_buffer) {
            FreeMem(stringdata->string_buffer,stringdata->string_size);
            stringdata->string_buffer=NULL;
        }
        if (stringdata->string_table) {
            FreeMem(stringdata->string_table,stringdata->string_count*4);
            stringdata->string_table=NULL;
        }
    }
}

int main(int argc, char *argv[])
{
struct StringData stringdata = {0};

    stringdata.string_count = 1000;
    if (ReadStringFile(&stringdata,argv[1])) FreeStringFile(&stringdata);
}
