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

#include "dopus.h"

#define BETAREV

#ifdef __MORPHOS__
char _ProgramName[] = "DirectoryOpus";
#endif

char *version="$VER: Directory Opus 4.17" BETAREV " (" __DATE__ ")";

void about()
{
    char buf[1024];

    lsprintf(buf,globstring[STR_ABOUT],"Directory Opus 4.17" BETAREV ,"Jacek Rzeuski\nCopyright 1993-2000 Jonathan Potter");
    simplerequest(buf,globstring[STR_CONTINUE],NULL);
}


char *comp_date=__DATE__,*comp_time=__TIME__;

void give_version_info()
{
    char buf[700];
    char
        prog_ver_buf[20],
        lib_ver_buf[20],
        kick_ver_buf[20],
        wb_ver_buf[20],
        proc_buf[10],
        coproc_buf[10],
        gfx_buf[10];
    UWORD ver,rev;
    int proc,fpu;

    lsprintf(prog_ver_buf,"v%s" BETAREV,str_version_string);
    lsprintf(lib_ver_buf,"v%ld.%ld",
        DOpusBase->LibNode.lib_Version,DOpusBase->LibNode.lib_Revision);

    if (/*DOSBase->dl_lib.lib_Version<36 ||*/ (GetVar("Kickstart",&kick_ver_buf[1],19,GVF_GLOBAL_ONLY))<1)
    {
        char *ptr3;
        ULONG ptr,*ptr2;

        ptr2=(ULONG *)0xffffec;
        ptr=0x1000000-(*ptr2);
        ptr3=(char *)ptr+12;
        ptr2=(ULONG *)ptr3;
        ptr=*ptr2;

        ver=ptr>>16;
        rev=ptr&0xFFFF;//(((1<<32)-(1<<16))-1);

        lsprintf(kick_ver_buf,"v%ld.%ld",ver,rev);
    }
    else kick_ver_buf[0]='v';

    if (/*DOSBase->dl_lib.lib_Version<36 ||*/ (GetVar("Workbench",&wb_ver_buf[1],19,GVF_GLOBAL_ONLY))<1)
    {
        struct Library *VersionBase;

        if (VersionBase=OpenLibrary("version.library",0)) {
            ver=VersionBase->lib_Version;
            rev=VersionBase->lib_Revision;
            CloseLibrary(VersionBase);
        }
        else ver=rev=0;

        lsprintf(wb_ver_buf,"v%ld.%ld",ver,rev);
    }
    else wb_ver_buf[0]='v';

    if (SysBase->AttnFlags&AFF_68010) {
        if (SysBase->AttnFlags&AFF_68020) {
            if (SysBase->AttnFlags&AFF_68030) {
                if (SysBase->AttnFlags&AFF_68040) {
                    if (SysBase->AttnFlags&AFF_68060) {
                        proc=60;
                    }
                    else proc=40;
                }
                else proc=30;
            }
            else proc=20;
        }
        else proc=10;
    }
    else proc=0;

    lsprintf(proc_buf,"680%02ld",proc);

    fpu = 0;
    if (SysBase->AttnFlags & AFF_68881)
     {
      fpu = 881;
      if (SysBase->AttnFlags & AFF_68882)
       {
        fpu = 882;
        if (SysBase->AttnFlags & AFF_FPU40)
         {
          fpu = 40;
          if (SysBase->AttnFlags & AFF_68060) fpu = 60;
         }
       }
     }
    switch (fpu)
     {
      case 881: strcpy(coproc_buf,"68881"); break;
      case 882: strcpy(coproc_buf,"68882"); break;
      case  40: strcpy(coproc_buf,"68040"); break;
      case  60: strcpy(coproc_buf,"68060");  break;
//      default:  strcpy(coproc_buf,"None");
      default:  strcpy(coproc_buf,globstring[STR_PROTECT_NONE]); //HUX
     }

    if (FindName(&SysBase->LibList,"rtg.library")) strcpy(gfx_buf,"P96");
    else if (FindName(&SysBase->LibList,"cybergraphics.library")) strcpy(gfx_buf,"CGX");
//    else strcpy(gfx_buf,"Native");
    else strcpy(gfx_buf,globstring[STR_GFX_NATIVE]);

    lsprintf(buf,globstring[STR_VERSION_CONTENTS],globstring[STR_VERSION_HEADER],
        prog_ver_buf,
        lib_ver_buf,
        comp_time,comp_date,
        str_arexx_portname,
        kick_ver_buf,
        wb_ver_buf,
        proc_buf,
        coproc_buf,
        (GfxBase->ChipRevBits0&GFXF_AA_ALICE)?"AGA":
            ((GfxBase->ChipRevBits0&GFXF_HR_AGNUS)?"ECS":"OLD"),
        gfx_buf);

    simplerequest(buf,globstring[STR_CONTINUE],NULL);
}
