/*
 *  $VER: ActivateStrGad.c $Revision$ (01-Feb-2006)
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

#include <string.h>
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

/****** dopus/main/ActivateStrGad ******************************************
*
*   NAME
*      ActivateStrGad -- Description
*
*   SYNOPSIS
*      void ActivateStrGad(struct Gadget * gadget, struct Window * window);
*
*   FUNCTION
*
*   INPUTS
*       gadget - 
*       window - 
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

void _DOpus_ActivateStrGad(struct DOpusIFace *Self, struct Gadget *gad, struct Window *win)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *IntuitionBase = IExec->OpenLibrary("intuition.library", 50L);
	struct IntuitionIFace *IIntuition = (struct IntuitionIFace *)IExec->GetInterface(IntuitionBase, "main", 1, NULL);
	struct StringInfo *si;

	if(gad->Flags & GFLG_DISABLED)
		return;

	si = (struct StringInfo *)(gad->SpecialInfo);

	si->BufferPos = (gad->Activation & GACT_STRINGRIGHT) ? 0 : strlen((char *)si->Buffer);

	IIntuition->RefreshGList(gad,win,NULL,1);
	IIntuition->ActivateGadget(gad,win,NULL);
	IExec->DropInterface((struct Interface *)IIntuition);
	IExec->CloseLibrary(IntuitionBase);
}

