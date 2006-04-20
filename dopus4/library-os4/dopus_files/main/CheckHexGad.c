/*
 *  $VER: CheckHexGad.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/CheckHexGad ******************************************
*
*   NAME
*      CheckHexGad -- Description
*
*   SYNOPSIS
*      int CheckHexGad(struct Gadget * gadget, struct Window * window, 
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

int _DOpus_CheckHexGad(struct DOpusIFace *Self, struct Gadget *gad, struct Window *win, int min, int max)
{
	struct StringInfo *sinfo;
	int a, b;

	sinfo = (struct StringInfo *)gad->SpecialInfo;
	b = Self->Atoh((char *)sinfo->Buffer, 0);
	if(sinfo->Buffer[0] == 0)
		a = 0;
	else
	{
		a = b;
		if(a < min)
			a = min;
		else if(a > max)
			a = max;
	}
	LSprintf((char *)sinfo->Buffer, "%lx", a);
	if(a != b)
	{
		if(win)
			Self->RefreshStrGad(gad, win);
		return(1);
	}
	return(0);
}
