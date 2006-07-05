/*
 *  $VER: DrawRadioButton.c $Revision$ (01-Feb-2006)
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
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

#include "extras.h"

/****** dopus/main/DrawRadioButton ******************************************
*
*   NAME
*      DrawRadioButton -- Description
*
*   SYNOPSIS
*      void DrawRadioButton(struct RastPort * rp, int x, int y, int w, 
*          int h, int hi, int lo);
*
*   FUNCTION
*
*   INPUTS
*       rp - 
*       x - 
*       y - 
*       w - 
*       h - 
*       hi - 
*       lo - 
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

#define drawline(rp,x1,y1,x2,y2) Move(rp,x1,y1);Draw(rp,x2,y2)

void _DOpus_DrawRadioButton(struct RastPort *rp, int x, int y, int w, int h, int hi, int lo)
{
	SetAPen(rp,hi);
	drawline(rp,x,y-1,x+w-3,y-1);
	drawline(rp,x-1,y,x,y);
	drawline(rp,x-2,y+1,x-2,y+h-4);
	drawline(rp,x-1,y+1,x-1,y+h-4);
	drawline(rp,x-1,y+h-3,x,y+h-3);

	SetAPen(rp,lo);
	drawline(rp,x,y+h-2,x+w-3,y+h-2);
	drawline(rp,x+w-3,y+h-3,x+w-2,y+h-3);
	drawline(rp,x+w-1,y+h-4,x+w-1,y+1);
	drawline(rp,x+w-2,y+h-4,x+w-2,y+1);
	drawline(rp,x+w-3,y,x+w-2,y);
}
