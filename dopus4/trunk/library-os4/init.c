/*
 *  $VER: init.c $Revision$ (01-Feb-2006)
 *
 *  This file is part of dopus.
 *
 *  (C) Copyright 2006 Hyperion Entertainment
 *      All rights reserved
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

/* Version Tag */
#include "dopus.library_rev.h"
STATIC CONST UBYTE __attribute__ ((used)) verstag[] = VERSTAG;

struct DOpusBase
{
	struct Library LibNode;
	BPTR SegList;

	UBYTE Flags;
	UBYTE pad;
//    struct ExecBase *ExecBase;
//    struct DosLibrary *DOSBase;
//    struct IntuitionBase *IntuitionBase;
//    struct GfxBase *GfxBase;
//    struct LayersBase *LayersBase;

	/* Private stuff */
	PLANEPTR pdb_cycletop;
	PLANEPTR pdb_cyclebot;
	PLANEPTR pdb_check;
	ULONG pad1;
	ULONG pad2;
	ULONG pad3;
	ULONG pad4;
	ULONG pdb_Flags;
};

/*
 * The system (and compiler) rely on a symbol named _start which marks
 * the beginning of execution of an ELF file. To prevent others from 
 * executing this library, and to keep the compiler/linker happy, we
 * define an empty _start symbol here.
 *
 * On the classic system (pre-AmigaOS4) this was usually done by
 * moveq #0,d0
 * rts
 *
 */
int32 _start(void);

int32 _start(void)
{
	/* If you feel like it, open DOS and print something to the user */
	return 100;
}


/* Open the library */
STATIC struct Library *libOpen(struct LibraryManagerInterface *Self, ULONG version)
{
	struct Library *libBase = (struct Library *)Self->Data.LibBase;

	if(version > VERSION)
	{
		return NULL;
	}

	/* Add any specific open code here. Return 0 before incrementing OpenCnt to fail opening */


	/* Add up the open count */
	libBase->lib_OpenCnt++;
	return (struct Library *)libBase;

}


/* Close the library */
STATIC APTR libClose(struct LibraryManagerInterface *Self)
{
	struct Library *libBase = (struct Library *)Self->Data.LibBase;
	/* Make sure to undo what open did */


	/* Make the close count */
	((struct Library *)libBase)->lib_OpenCnt--;

	return 0;
}


/* Expunge the library */
STATIC APTR libExpunge(struct LibraryManagerInterface * Self)
{
	/* If your library cannot be expunged, return 0 */
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	APTR result = (APTR) 0;
	struct Library *libBase = (struct Library *)Self->Data.LibBase;
	struct DOpusBase *DOBase = (struct DOpusBase *)libBase;
	if(libBase->lib_OpenCnt == 0)
	{
		result = (APTR) DOBase->SegList;
		/* Undo what the init code did */

		IExec->Remove((struct Node *)libBase);
		IExec->DeleteLibrary((struct Library *)libBase);
	}
	else
	{
		result = (APTR) 0;
		libBase->lib_Flags |= LIBF_DELEXP;
	}
	return result;
}

/* The ROMTAG Init Function */
STATIC struct Library *libInit(struct Library *LibraryBase, APTR seglist, struct Interface *exec)
{
	struct DOpusBase *libBase = (struct DOpusBase *)LibraryBase;
	struct ExecIFace *IExec __attribute__ ((unused)) = (struct ExecIFace *)exec;

	libBase->LibNode.lib_Node.ln_Type = NT_LIBRARY;
	libBase->LibNode.lib_Node.ln_Pri = 0;
	libBase->LibNode.lib_Node.ln_Name = "dopus.library";
	libBase->LibNode.lib_Flags = LIBF_SUMUSED | LIBF_CHANGED;
	libBase->LibNode.lib_Version = VERSION;
	libBase->LibNode.lib_Revision = REVISION;
	libBase->LibNode.lib_IdString = VSTRING;

	libBase->SegList = (BPTR) seglist;

	return (struct Library *)libBase;
}

/* ------------------- Manager Interface ------------------------ */
/* These are generic. Replace if you need more fancy stuff */
STATIC LONG _manager_Obtain(struct LibraryManagerInterface *Self)
{
	return ++Self->Data.RefCount;
}

STATIC ULONG _manager_Release(struct LibraryManagerInterface * Self)
{
	return --Self->Data.RefCount;
}

/* Manager interface vectors */
STATIC CONST APTR lib_manager_vectors[] =
{
	_manager_Obtain,
	_manager_Release,
	NULL,
	NULL,
	libOpen,
	libClose,
	libExpunge,
	NULL,
	(APTR) - 1
};

/* "__library" interface tag list */
STATIC CONST struct TagItem lib_managerTags[] =
{
	{MIT_Name, (Tag) "__library"},
	{MIT_VectorTable, (Tag) lib_manager_vectors},
	{MIT_Version, 1},
	{TAG_DONE, 0}
};

/* ------------------- Library Interface(s) ------------------------ */

#include "dopus_vectors.c"

/* Uncomment this line (and see below) if your library has a 68k jump table */
/* extern APTR VecTable68K[]; */

STATIC CONST struct TagItem mainTags[] =
{
	{MIT_Name, (Tag) "main"},
	{MIT_VectorTable, (Tag) main_vectors},
	{MIT_Version, 1},
	{TAG_DONE, 0}
};

STATIC CONST CONST_APTR libInterfaces[] =
{
	lib_managerTags,
	mainTags,
	NULL
};

STATIC CONST struct TagItem libCreateTags[] =
{
	{CLT_DataSize, sizeof(struct Library)},
	{CLT_InitFunc, (Tag) libInit},
	{CLT_Interfaces, (Tag) libInterfaces},
	/* Uncomment the following line if you have a 68k jump table */
	/* { CLT_Vector68K, (Tag)VecTable68K }, */
	{TAG_DONE, 0}
};


/* ------------------- ROM Tag ------------------------ */
STATIC CONST struct Resident lib_res __attribute__ ((used)) =
{
	RTC_MATCHWORD, (struct Resident *)&lib_res,
	(APTR) (&lib_res + 1), RTF_NATIVE | RTF_AUTOINIT,	/* Add RTF_COLDSTART if you want to be resident */
	VERSION, NT_LIBRARY,					/* Make this NT_DEVICE if needed */
	0,							/* PRI, usually not needed unless you're resident */
	"dopus.library", VSTRING,
	(APTR) libCreateTags
};
