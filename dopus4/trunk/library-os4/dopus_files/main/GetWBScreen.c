/*
 *  $VER: GetWBScreen.c $Revision$ (01-Feb-2006)
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
#include <proto/intuition.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/GetWBScreen ******************************************
*
*   NAME
*      GetWBScreen -- Description
*
*   SYNOPSIS
*      void GetWBScreen(struct Screen * scrbuf);
*
*   FUNCTION
*
*   INPUTS
*       scrbuf - 
*
*   RESULT
*       This function does not return a result
*
*   EXAMPLE
*
*   NOTES
*
*   BUGS
*
*   SEE ALSO
*
*****************************************************************************
*
*/

void _DOpus_GetWBScreen(struct DOpusIFace *Self, struct Screen *scrbuf)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *IntuitionBase = IExec->OpenLibrary("intuition.library", 50L);
	struct IntuitionIFace *IIntuition = (struct IntuitionIFace *)IExec->GetInterface(IntuitionBase, "main", 1, NULL);
	struct Screen *scr;

	if((scr = IIntuition->LockPubScreen(NULL)))
	{
		IExec->CopyMem(scr, scrbuf, sizeof(struct Screen));
		IIntuition->UnlockPubScreen(NULL, scr);
	}

	IExec->DropInterface((struct Interface *)IIntuition);
	IExec->CloseLibrary(IntuitionBase);
	return;
}

