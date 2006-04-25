/*
 *  $VER: RefreshStrGad.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/RefreshStrGad ******************************************
*
*   NAME
*      RefreshStrGad -- Description
*
*   SYNOPSIS
*      void RefreshStrGad(struct Gadget * gadget, struct Window * window);
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

void _DOpus_RefreshStrGad(struct DOpusIFace *Self, struct Gadget *gad, struct Window *win)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *IntuitionBase = IExec->OpenLibrary("intuition.library", 50L);
	struct IntuitionIFace *IIntuition = (struct IntuitionIFace *)IExec->GetInterface(IntuitionBase, "main", 1, NULL);

	((struct StringInfo *)(gad->SpecialInfo))->BufferPos = 0;

	gad->Flags |= GFLG_GADGHNONE;
	IIntuition->RefreshGList(gad, win, NULL, 1);
	gad->Flags &= ~GFLG_GADGHNONE;

	IExec->DropInterface((struct Interface *)IIntuition);
	IExec->CloseLibrary(IntuitionBase);
	return;
}

