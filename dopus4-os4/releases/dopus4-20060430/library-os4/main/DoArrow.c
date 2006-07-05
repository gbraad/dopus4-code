/*
 *  $VER: DoArrow.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/DoArrow ******************************************
*
*   NAME
*      DoArrow -- Description
*
*   SYNOPSIS
*      void DoArrow(struct RastPort * rp, int x, int y, int w, int h, 
*          int fg, int bg, int dir);
*
*   FUNCTION
*
*   INPUTS
*       rp - 
*       x - 
*       y - 
*       w - 
*       h - 
*       fg - 
*       bg - 
*       dir - 
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

void _DOpus_DoArrow(struct DOpusIFace *Self, struct RastPort *rp, int x, int y, int w, int h, int fg, int bg, int d)
{
	int x1,y1,x2,y2,x3,y3,x4,y4,xb;

	IGraphics->SetAPen(rp,bg);
	xb = x + w;
	IGraphics->RectFill(rp,x,y,xb-1,y+h-1);
	IGraphics->SetAPen(rp,fg);
	switch(d)
	{
		case 0:
			y1=y+h-2;
			y3=y+1;
			x1=x+2;
			x2=xb-3;
			x3=x+(w/2)-1;
			x4=xb-(w/2);
			y2=y1;
			y4=y3;
			break;
		case 1:
			y1=y+1;
			y3=y+h-2;
			x1=x+2;
			x2=xb-3;
			x3=x+(w/2)-1;
			x4=xb-(w/2);
			y2=y1;
			y4=y3;
			break;
		case 2:
			x1=x+1;
			x3=xb-2;
			x2=x1;
			y1=y+1;
			y2=y+h-2;
			y3=y+(h/2);
			if(h % 2 == 0)
				--y3;
			x4=x3;
			y4=y3;
			break;
		default:
			x1=xb-2;
			x3=x+1;
			x2=x1;
			y1=y+1;
			y2=y+h-2;
			y3=y+(h/2);
			if(h % 2 == 0)
				--y3;
			x4=x3;
			y4=y3;
			break;
	}
	drawline(rp,x1,y1,x3,y3);
	drawline(rp,x2,y2,x4,y4);

	return;
}

