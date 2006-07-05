/*
 *  $VER: init.c $Revision$ (01-Feb-2006)
 *
 *  This file is part of dopus.
 *
 * $Id$
 *
 * $Log$
 *
 *
 */


#include <exec/exec.h>
#include <proto/exec.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

#include "Library.h"

/* Version Tag */
#include "dopus.library_rev.h"
CONST TEXT verstag[] = VERSTAG " MOS";

struct ExecBase *SysBase;
struct DosLibrary *DOSBase;
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *UtilityBase;

struct DOpusBase
{
	struct Library LibNode;
	BPTR SegList;

#if 0
	UBYTE Flags;
	UBYTE pad;

	/* Private stuff */
	PLANEPTR pdb_cycletop;
	PLANEPTR pdb_cyclebot;
	PLANEPTR pdb_check;
	ULONG pad1;
	ULONG pad2;
	ULONG pad3;
	ULONG pad4;
	ULONG pdb_Flags;
#endif
};

struct Library *DOpusBase;

STATIC ULONG libReserved(void)
{
	return 0;
}

/**********************************************************************
	FreeLib
**********************************************************************/

STATIC VOID FreeLib(struct DOpusBase *LibBase)
{
	CloseLibrary((struct Library *)DOSBase);
	CloseLibrary((struct Library *)GfxBase);
	CloseLibrary((struct Library *)IntuitionBase);
	CloseLibrary(UtilityBase);
	FreeMem((APTR)((ULONG)(LibBase) - (ULONG)(LibBase->LibNode.lib_NegSize)), LibBase->LibNode.lib_NegSize + LibBase->LibNode.lib_PosSize);
}

/**********************************************************************
	LibInit
**********************************************************************/

STATIC struct Library *LibInit(struct DOpusBase *LibBase, BPTR SegList, struct ExecBase *MySysBase)
{
	DOpusBase		= (struct Library *)LibBase;
	SysBase			= MySysBase;
	DOSBase			= (struct DosLibrary *)OpenLibrary("dos.library", 36);
	GfxBase			= (struct GfxBase *)OpenLibrary("graphics.library", 36);
	IntuitionBase	= (struct IntuitionBase *)OpenLibrary("intuition.library", 36);
	UtilityBase		= OpenLibrary("utility.library", 36);

	if (DOSBase && GfxBase && IntuitionBase && UtilityBase)
	{
		LibBase->SegList	= SegList;
	}
	else
	{
		FreeLib(LibBase);
		LibBase	= NULL;
	}

	return (struct Library *)LibBase;
}

/**********************************************************************
	DeleteLib
**********************************************************************/

STATIC BPTR DeleteLib(struct DOpusBase *LibBase)
{
	BPTR	SegList	= 0;

	if (LibBase->LibNode.lib_OpenCnt == 0)
	{
		SegList	= LibBase->SegList;
		REMOVE(&LibBase->LibNode.lib_Node);
		FreeLib(LibBase);
	}

	return SegList;
}

STATIC struct Library *libOpen(void)
{
	struct Library *libBase = (struct Library *)REG_A6;
	libBase->lib_OpenCnt++;
	libBase->lib_Flags &= ~LIBF_DELEXP;
	return libBase;
}


STATIC BPTR libClose(void)
{
	struct Library *libBase = (struct Library *)REG_A6;
	libBase->lib_OpenCnt--;

	if (libBase->lib_Flags & LIBF_DELEXP)
	{
		return DeleteLib((struct DOpusBase *)libBase);
	}

	return 0;
}

STATIC BPTR libExpunge(void)
{
	struct Library *libBase = (struct Library *)REG_A6;
	libBase->lib_Flags	|= LIBF_DELEXP;
	return DeleteLib((struct DOpusBase *)libBase);
}

/**********************************************************************
	Functions
**********************************************************************/

#include "stubs.c"

/**********************************************************************
	Function table
**********************************************************************/

STATIC CONST APTR FuncTable[] =
{
	(APTR)	FUNCARRAY_32BIT_NATIVE, 

	(APTR)	libOpen,
	(APTR)	libClose,
	(APTR)	libExpunge,
	(APTR)	libReserved,

	(APTR) DOPUS_FileRequest,
	(APTR) DOPUS_Do3DBox,
	(APTR) DOPUS_Do3DStringBox,
	(APTR) DOPUS_Do3DCycleBox,
	(APTR) DOPUS_DoArrow,
	(APTR) DOPUS_LSprintf,
	(APTR) DOPUS_LCreateExtIO,
	(APTR) DOPUS_LCreatePort,
	(APTR) DOPUS_LDeleteExtIO,
	(APTR) DOPUS_LDeletePort,
	(APTR) DOPUS_LToUpper,
	(APTR) DOPUS_LToLower,
	(APTR) DOPUS_LStrCat,
	(APTR) DOPUS_LStrnCat,
	(APTR) DOPUS_LStrCpy,
	(APTR) DOPUS_LStrnCpy,
	(APTR) DOPUS_LStrCmp,
	(APTR) DOPUS_LStrnCmp,
	(APTR) DOPUS_LStrCmpI,
	(APTR) DOPUS_LStrnCmpI,
	(APTR) DOPUS_StrCombine,
	(APTR) DOPUS_StrConcat,
	(APTR) DOPUS_LParsePattern,
	(APTR) DOPUS_LMatchPattern,
	(APTR) DOPUS_LParsePatternI,
	(APTR) DOPUS_LMatchPatternI,
	(APTR) DOPUS_BtoCStr,
	(APTR) DOPUS_Assign,
	(APTR) DOPUS_BaseName,
	(APTR) DOPUS_CompareLock,
	(APTR) DOPUS_PathName,
	(APTR) DOPUS_SendPacket,
	(APTR) DOPUS_TackOn,
	(APTR) DOPUS_StampToStr,
	(APTR) DOPUS_StrToStamp,
	(APTR) DOPUS_AddListView,
	(APTR) DOPUS_ListViewIDCMP,
	(APTR) DOPUS_RefreshListView,
	(APTR) DOPUS_RemoveListView,
	(APTR) DOPUS_DrawCheckMark,
	(APTR) DOPUS_FixSliderBody,
	(APTR) DOPUS_FixSliderPot,
	(APTR) DOPUS_GetSliderPos,
	(APTR) DOPUS_LAllocRemember,
	(APTR) DOPUS_LFreeRemember,
	(APTR) DOPUS_SetBusyPointer,
	(APTR) DOPUS_GetWBScreen,
	(APTR) DOPUS_SearchPathList,
	(APTR) DOPUS_CheckExist,
	(APTR) DOPUS_CompareDate,
	(APTR) DOPUS_Seed,
	(APTR) DOPUS_Random,
	(APTR) DOPUS_StrToUpper,
	(APTR) DOPUS_StrToLower,
	(APTR) DOPUS_RawkeyToStr,
	(APTR) DOPUS_DoRMBGadget,
	(APTR) DOPUS_AddGadgets,
	(APTR) DOPUS_ActivateStrGad,
	(APTR) DOPUS_RefreshStrGad,
	(APTR) DOPUS_CheckNumGad,
	(APTR) DOPUS_CheckHexGad,
	(APTR) DOPUS_Atoh,
	(APTR) DOPUS_HiliteGad,
	(APTR) DOPUS_DoSimpleRequest,
	(APTR) DOPUS_ReadConfig,
	(APTR) DOPUS_SaveConfig,
	(APTR) DOPUS_DefaultConfig,
	(APTR) DOPUS_GetDevices,
	(APTR) DOPUS_AssignGadget,
	(APTR) DOPUS_AssignMenu,
	(APTR) DOPUS_FindSystemFile,
	(APTR) DOPUS_Do3DFrame,
	(APTR) DOPUS_FreeConfig,
	(APTR) DOPUS_DoCycleGadget,
	(APTR) DOPUS_UScoreText,
	(APTR) DOPUS_DisableGadget,
	(APTR) DOPUS_EnableGadget,
	(APTR) DOPUS_GhostGadget,
	(APTR) DOPUS_DrawRadioButton,
	(APTR) DOPUS_GetButtonImage,
	(APTR) DOPUS_ShowSlider,
	(APTR) DOPUS_CheckConfig,
	(APTR) DOPUS_GetCheckImage,
	(APTR) DOPUS_OpenRequester,
	(APTR) DOPUS_CloseRequester,
	(APTR) DOPUS_AddRequesterObject,
	(APTR) DOPUS_RefreshRequesterObject,
	(APTR) DOPUS_ObjectText,
	(APTR) DOPUS_DoGlassImage,
	(APTR) DOPUS_Decode_RLE,
	(APTR) DOPUS_ReadStringFile,
	(APTR) DOPUS_FreeStringFile,
	(APTR) DOPUS_LFreeRemEntry,
	(APTR) DOPUS_AddGadgetBorders,
	(APTR) DOPUS_CreateGadgetBorders,
	(APTR) DOPUS_SelectGadget,
	(APTR) DOPUS_FSSetMenuStrip,

	(APTR)	-1,
};

STATIC CONST struct MyInitData InitData =
{
	{ 0xa0, 8, NT_LIBRARY, 0 },
	{ 0xa0, 9, -10, 0 },
	{ 0x80, 10 }, (ULONG)"dopus.library",
	{ 0xa0, 14,	LIBF_SUMUSED|LIBF_CHANGED, 0 },
	{ 0x90, 20 }, VERSION,
	{ 0x90, 22 }, REVISION,
	{ 0x80, 24 }, (ULONG)VSTRING,
	0
};

STATIC CONST ULONG InitTable[] =
{
	sizeof(struct DOpusBase),
	(ULONG)	FuncTable,
	(ULONG)	&InitData,
	(ULONG)	LibInit
};

CONST struct Resident RomTag	=
{
	RTC_MATCHWORD,
	(struct Resident *)&RomTag,
	(struct Resident *)&RomTag+1,
	RTF_AUTOINIT | RTF_PPC | RTF_EXTENDED,
	VERSION,
	NT_LIBRARY,
	0,
	"dopus.library",
	VSTRING,
	(APTR)&InitTable,

	REVISION, NULL
};

/**********************************************************************
	Not needed but recommended (avoid confusion to PowerUp libraries)
**********************************************************************/

CONST ULONG __abox__	= 1;
