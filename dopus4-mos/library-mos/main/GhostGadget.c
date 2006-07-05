/*
 *  $VER: GhostGadget.c $Revision$ (01-Feb-2006)
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
#include <proto/graphics.h>
#include <graphics/gfxmacros.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/GhostGadget ******************************************
*
*   NAME
*      GhostGadget -- Description
*
*   SYNOPSIS
*      void GhostGadget(struct Gadget * gad, struct RastPort * rp, int xoff, 
*          int yoff);
*
*   FUNCTION
*
*   INPUTS
*       gad - 
*       rp - 
*       xoff - 
*       yoff - 
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

void _DOpus_GhostGadget(struct Gadget *gad, struct RastPort *rp, int xoff, int yoff)
{
	ULONG old_drmd = GetDrMd(rp);
	STATIC CONST UWORD ditherdata[] = { 0x8888, 0x2222 };

	SetDrMd(rp, COMPLEMENT);
	SetAfPt(rp, ditherdata, 1);
	RectFill(rp, gad->LeftEdge+xoff, gad->TopEdge + yoff, gad->LeftEdge+gad->Width-1-xoff, gad->TopEdge+gad->Height-1-yoff);
	SetAfPt(rp, NULL, 0);
	SetDrMd(rp, old_drmd);

	return;
}

