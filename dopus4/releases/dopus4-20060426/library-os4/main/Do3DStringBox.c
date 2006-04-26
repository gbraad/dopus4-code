/*
 *  $VER: Do3DStringBox.c $Revision$ (01-Feb-2006)
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
#include <proto/graphics.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/Do3DStringBox ******************************************
*
*   NAME
*      Do3DStringBox -- Description
*
*   SYNOPSIS
*      void Do3DStringBox(struct RastPort * rp, int x, int y, int w, int h, 
*          int tp, int bp);
*
*   FUNCTION
*
*   INPUTS
*       rp - 
*       x - 
*       y - 
*       w - 
*       h - 
*       tp - 
*       bp - 
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

#define drawline(rp,x1,y1,x2,y2) IGraphics->Move(rp,x1,y1);IGraphics->Draw(rp,x2,y2)

void _DOpus_Do3DStringBox(struct DOpusIFace *Self, struct RastPort *rp, int x, int y, int w, int h, int tp, int bp)
{
	Self->Do3DBox(rp,x-2,y-1,w+4,h+2,tp,bp);

	IGraphics->SetAPen(rp,bp);

	drawline(rp,x-2,y-1,x+w+1,y-1);
	drawline(rp,x-2,y-1,x-2,y+h+1);
	drawline(rp,x-1,y-1,x-1,y+h-1);

	IGraphics->SetAPen(rp,tp);

	drawline(rp,x-1,y+h,x+w+1,y+h);
	IGraphics->Draw(rp,x+w+1,y-1);

	drawline(rp,x+w,y+h,x+w,y);

	return;
}

