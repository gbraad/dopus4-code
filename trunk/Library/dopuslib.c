/*
 Directory Opus 4
 Original GPL release version 4.12
 Copyright 1993-2000 Jonathan Potter

 This program is free software you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

 All users of Directory Opus 4 (including versions distributed
 under the GPL) are entitled to upgrade to the latest version of
 Directory Opus version 5 at a reduced price. Please see
 http://www.gpsoft.com.au for more information.

 The release of Directory Opus 4 under the GPL in NO WAY affects
 the existing commercial status of Directory Opus 5.
*/

//#include <clib/alib_stdio_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/dos.h>
#include <proto/layers.h>
#include <proto/utility.h>
#include <proto/mathieeesingbas.h>

#include <exec/resident.h>
#include <exec/initializers.h>
#include <intuition/intuition.h>
#include <graphics/layers.h>
#include <utility/utility.h>

#include "dopuslib.h"

struct ExecBase *SysBase = NULL;
struct DOpusBase *DOpusBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct GfxBase *GfxBase = NULL;
struct DosLibrary *DOSBase = NULL;
//struct LayersBase *LayersBase = NULL;
struct UtilityBase *UtilityBase = NULL;
struct Library *__MathIeeeSingBasBase = NULL;

__saveds struct DOpusBase *Lib_Init(register BPTR seglist __asm("a0"),register struct ExecBase *sysbase __asm("a6"),register struct DOpusBase *dopusbase __asm("d0"));
struct DOpusBase *Lib_Open(struct DOpusBase *libbase __asm("a6"));
BPTR Lib_Close(struct DOpusBase *libbase __asm("a6"));
__saveds BPTR Lib_Expunge(struct DOpusBase *libbase __asm("a6"));
int Lib_Ext(void);

extern __chip USHORT pdb_cycletop[];
extern __chip USHORT pdb_cyclebot[];
extern __chip USHORT pdb_check[];

extern const char dopusname[];
extern const char idstring[];
extern const struct DefaultString default_strings[];

#define DOPUSNAME "dopus.library"
#define VERSION 22
#define REVISION 21

int Lib_Ext(void)
{
    return 0;
}
/*
void __stdargs _XCEXIT(long lcode) { }
*/
static const struct Resident romtag = {
    RTC_MATCHWORD,
    &romtag,
    (APTR)(&romtag+1)/*endcode*/,
    0,
    VERSION,
    NT_LIBRARY,
    0,
    dopusname,
    idstring,
    Lib_Init
};

const char dopusname[] = DOPUSNAME;
const char idstring[] = DOPUSNAME " 22.21 (" __DATE__ ") made under GPL license by Jacek Rzeuski\r\n";

static const APTR functable[] = {
    Lib_Open,
    Lib_Close,
    Lib_Expunge,
    Lib_Ext,
    DoFileRequest,
    Do3DBox,
    Do3DStringBox,
    Do3DCycleBox,
    DoDoArrow,
    LSprintf,
    DoCreateExtIO,
    DoCreatePort,
    DoDeleteExtIO,
    DoDeletePort,
    LToUpper,
    LToLower,
    LStrCat,
    LStrnCat,
    LStrCpy,
    LStrnCpy,
    LStrCmp,
    LStrnCmp,
    LStrCmpI,
    LStrnCmpI,
    DoStrCombine,
    DoStrConcat,
    LParsePattern,
    LMatchPattern,
    LParsePatternI,
    LMatchPatternI,
    BtoCStr,
    DoAssign,
    DoBaseName,
    DoCompareLock,
    DoPathName,
    DoSendPacket,
    DoTackOn,
    DoStampToStr,
    DoStrToStamp,
    DoAddListView,
    DoListViewIDCMP,
    DoRefreshListView,
    DoRemoveListView,
    DrawCheckMark,
    DoFixSliderBody,
    DoFixSliderPot,
    DoGetSliderPos,
    DoAllocRemember,
    DoFreeRemember,
    DoSetBusyPointer,
    GetWBScreen,
    DoSearchPathList,
    DoCheckExist,
    DoCompareDate,
    Seed,
    Random,
    StrToUpper,
    StrToLower,
    DoRawkeyToStr,
    DoDoRMBGadget,
    DoAddGadgets,
    ActivateStrGad,
    RefreshStrGad,
    DoCheckNumGad,
    DoCheckHexGad,
    DoAtoh,
    HiliteGad,
    DoDoSimpleRequest,
    DoReadConfig,
    DoSaveConfig,
    DoDefaultConfig,
    DoGetDevices,
    DoAssignGadget,
    DoAssignMenu,
    DoFindSystemFile,
    DoDo3DFrame,
    DoFreeConfig,
    DoDoCycleGadget,
    DoUScoreText,
    DisableGadget,
    EnableGadget,
    GhostGadget,
    DoDrawRadioButton,
    DoGetButtonImage,
    DoShowSlider,
    DoCheckConfig,
    DoGetCheckImage,
    R_OpenRequester,
    R_CloseRequester,
    R_AddRequesterObject,
    R_RefreshRequesterObject,
    R_ObjectText,
    DoDoGlassImage,
    DoDecode_RLE,
    DoReadStringFile,
    DoFreeStringFile,
    DoFreeRemEntry,
    DoAddGadgetBorders,
    DoCreateGadgetBorders,
    DoSelectGadget,
    DoFSSetMenuStrip,
    (APTR)-1L
};

struct StringData stringdata = {default_strings,STR_STRING_COUNT,NULL,NULL,0,STRING_VERSION,NULL,NULL};

__regargs void cleanup(struct DOpusBase *DOpusBase)
 {
  DoFreeStringFile(&stringdata);
  CloseLibrary((struct Library *)DOpusBase->DOSBase);
  CloseLibrary((struct Library *)DOpusBase->IntuitionBase);
  CloseLibrary((struct Library *)DOpusBase->GfxBase);
  CloseLibrary((struct Library *)DOpusBase->LayersBase);
  CloseLibrary((struct Library *)UtilityBase);
  CloseLibrary(__MathIeeeSingBasBase);
//D(bug("cleanup: leaving\n"));
 }

__saveds struct DOpusBase *Lib_Init(register BPTR seglist __asm("a0"),register struct ExecBase *sysbase __asm("a6"),register struct DOpusBase *dopusbase __asm("d0"))
{
    SysBase = sysbase;
//D(bug("LibInit: seglist=%lx,sysbase=%lx,dopusbase=%lx\n",seglist,sysbase,dopusbase));
    if (dopusbase == NULL)
     {
      if (DOpusBase = (struct DOpusBase *)MakeLibrary(functable,NULL,NULL,sizeof(struct DOpusBase),NULL))
       {
//D(bug("LibInit: MakeLibrary succeeded\n"));
        DOpusBase->LibNode.lib_Node.ln_Type = NT_LIBRARY;
        DOpusBase->LibNode.lib_Node.ln_Name = dopusname;
        DOpusBase->LibNode.lib_Flags        = LIBF_CHANGED | LIBF_SUMUSED;
        DOpusBase->LibNode.lib_Version      = VERSION;
        DOpusBase->LibNode.lib_Revision     = REVISION;
        DOpusBase->LibNode.lib_IdString     = idstring;

        AddLibrary((struct Library *)DOpusBase);
       }
     }
    else DOpusBase = dopusbase;

//D(bug("LibInit: DOpusBase=%lx\n",DOpusBase));
    if (DOpusBase)
     {
      DOpusBase->SegList = seglist;

      DOpusBase->ExecBase = SysBase;
      DOSBase = DOpusBase->DOSBase = (struct DosLibrary *)OpenLibrary(DOSNAME,39);
      IntuitionBase = DOpusBase->IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",39);
      GfxBase = DOpusBase->GfxBase = (struct GfxBase *)OpenLibrary(GRAPHICSNAME,39);
      /*LayersBase = */DOpusBase->LayersBase = (struct LayersBase *)OpenLibrary("layers.library",39);
      UtilityBase = (struct UtilityBase *)OpenLibrary(UTILITYNAME,39);
      __MathIeeeSingBasBase = OpenLibrary("mathieeesingbas.library",0);

      DOpusBase->pdb_cycletop = pdb_cycletop;
//D(bug("LibInit: pdb_cycletop=%lx\n",DOpusBase->pdb_cycletop));
      DOpusBase->pdb_cyclebot = pdb_cyclebot;
//D(bug("LibInit: pdb_cyclebot=%lx\n",DOpusBase->pdb_cyclebot));
      DOpusBase->pdb_check = pdb_check;
//D(bug("LibInit: pdb_check=%lx\n",DOpusBase->pdb_check));
      DOpusBase->pdb_Flags = 0;
/*
      stringdata.default_table = default_strings;
      stringdata.string_count = STR_STRING_COUNT;
      stringdata.min_version = STRING_VERSION;
*/
      if (DoReadStringFile(&stringdata,"dopus4_lib.catalog"))
       {
        string_table=stringdata.string_table;
        initstrings();
       }
      else
       {
        cleanup(DOpusBase);
        DOpusBase = NULL;
       }
     }
    return DOpusBase;
}

/*__saveds*/ struct DOpusBase *Lib_Open(struct DOpusBase *libbase __asm("a6"))
{
//D(bug("LibOpen:\n"));
    libbase->LibNode.lib_OpenCnt++;
    libbase->Flags &= ~LIBF_DELEXP;
    return libbase;
}

/*__saveds*/ BPTR Lib_Close(struct DOpusBase *libbase __asm("a6"))
{
//D(bug("LibClose:\n"));
    libbase->LibNode.lib_OpenCnt--;
    if ((!(libbase->LibNode.lib_OpenCnt)) && (libbase->Flags & LIBF_DELEXP))
      return Lib_Expunge(libbase);
    return NULL;
}

__saveds BPTR Lib_Expunge(struct DOpusBase *libbase __asm("a6"))
{
//D(bug("LibExpunge:\n"));
    if (libbase->LibNode.lib_OpenCnt)
     {
      libbase->Flags |= LIBF_DELEXP;
      return NULL;
     }
    else
     {
      BPTR seglist = libbase->SegList;

//D(bug("LibExpunge: expunging...\n"));
      Remove((struct Node *)libbase);
//D(bug("LibExpunge: library removed\n"));

      cleanup(libbase);

      FreeMem((APTR)((ULONG)libbase - libbase->LibNode.lib_NegSize),libbase->LibNode.lib_NegSize + libbase->LibNode.lib_PosSize);
//D(bug("LibExpunge: libbase freed at %lx - %ld bytes\n",(ULONG)libbase - libbase->LibNode.lib_NegSize,libbase->LibNode.lib_NegSize + libbase->LibNode.lib_PosSize));
      return seglist;
     }
}

/*
;DoObjectText:
;    movem.l a2-a6/d2-d7,-(sp)
;    move.l d4,-(sp)
;    move.l a1,-(sp)
;    move.l d3,-(sp)
;    move.l d2,-(sp)
;    move.l d1,-(sp)
;    move.l d0,-(sp)
;    move.l a0,-(sp)
;    jsr _R_ObjectText
;    add.l #28,sp
;    movem.l (sp)+,a2-a6/d2-d7
;    rts

;FixSliderBody:
;    movem.l a2-a6/d2-d7,-(sp)
;    move.l d2,-(sp)
;    move.l d1,-(sp)
;    move.l d0,-(sp)
;    move.l a1,-(sp)
;    move.l a0,-(sp)
;    jsr _DoFixSliderBody
;    add.l #20,sp
;    movem.l (sp)+,a2-a6/d2-d7
;    rts

;FixSliderPot:
;    movem.l a2-a6/d2-d7,-(sp)
;    move.l d3,-(sp)
;    move.l d2,-(sp)
;    move.l d1,-(sp)
;    move.l d0,-(sp)
;    move.l a1,-(sp)
;    move.l a0,-(sp)
;    jsr _DoFixSliderPot
;    add.l #24,sp
;    movem.l (sp)+,a2-a6/d2-d7
;    rts

;GetSliderPos:
;    movem.l a2-a6/d2-d7,-(sp)
;    move.l d1,-(sp)
;    move.l d0,-(sp)
;    move.l a0,-(sp)
;    jsr _DoGetSliderPos
;    add.l #12,sp
;    movem.l (sp)+,a2-a6/d2-d7
;    rts

    END
*/
