/*
 *  $VER: CheckNumGad.c $Revision$ (01-Feb-2006)
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


#include <stdlib.h>
#include <exec/exec.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

#include "extras.h"

/****** dopus/main/CheckNumGad ******************************************
*
*   NAME
*      CheckNumGad -- Description
*
*   SYNOPSIS
*      int CheckNumGad(struct Gadget * gadget, struct Window * window, 
*          int min, int max);
*
*   FUNCTION
*
*   INPUTS
*       gadget - 
*       window - 
*       min - 
*       max - 
*
*   RESULT
*       The result ...
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

int _DOpus_CheckNumGad(struct DOpusIFace *Self, struct Gadget *gad, struct Window *win, int min, int max)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *DOSBase = IExec->OpenLibrary("dos.library", 50L);
	struct DOSIFace *IDOS = (struct DOSIFace *)IExec->GetInterface(DOSBase, "main", 1, NULL);
	struct StringInfo *sinfo;
	int a, b;

	sinfo = (struct StringInfo *)gad->SpecialInfo;
	a = b = atoi((char *)sinfo->Buffer);
	if(a < min)
		a = min;
	else if(a > max)
		a = max;
	LSprintf((char *)sinfo->Buffer, "%ld", a);
	if(a != b)
	{
		if(win)
			Self->RefreshStrGad(gad, win);
		return(1);
	}
	IExec->DropInterface((struct Interface *)IDOS);
	IExec->CloseLibrary(DOSBase);
	return(0);
}
