/*
 *  $VER: GetSliderPos.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/GetSliderPos ******************************************
*
*   NAME
*      GetSliderPos -- Description
*
*   SYNOPSIS
*      int GetSliderPos(struct Gadget * gadget, int count, int lines);
*
*   FUNCTION
*
*   INPUTS
*       gadget - 
*       count - 
*       lines - 
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

int _DOpus_GetSliderPos(struct DOpusIFace *Self, struct Gadget *gad, int count, int lines)
{
	int i;
	USHORT vertpot;
	struct PropInfo *pinfo;

	pinfo = (struct PropInfo *)gad->SpecialInfo;
	if(pinfo->Flags & FREEVERT)
		vertpot = pinfo->VertPot;
	else
		vertpot = pinfo->HorizPot;

	if(count < lines || vertpot == 0)
		return (0);
	i = (((count - lines) + 1) * (long long)vertpot) >> 16 /*/0xffff */ ;
	if(i > (count - lines))
		i = count - lines;
	if(i < 0)
		i = 0;
	return (i);
}
