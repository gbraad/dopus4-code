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

#include <stdio.h>
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
	struct StringInfo *sinfo;
	int a, b;

	sinfo = (struct StringInfo *)gad->SpecialInfo;
	a = b = atoi((char *)sinfo->Buffer);
	if(a < min)
		a = min;
	else if(a > max)
		a = max;
//	LSprintf((char *)sinfo->Buffer, "%ld", a);
	snprintf((char *)sinfo->Buffer, sinfo->MaxChars, "%d", a);
	if(a != b)
	{
		if(win)
			Self->RefreshStrGad(gad, win);
		return(1);
	}
	return(0);
}
