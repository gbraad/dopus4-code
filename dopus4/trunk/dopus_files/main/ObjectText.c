/*
 *  $VER: ObjectText.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/ObjectText ******************************************
*
*   NAME
*      ObjectText -- Description
*
*   SYNOPSIS
*      void ObjectText(struct RequesterBase * reqbase, short left, 
*          short top, short width, short height, char * text, 
*          unsigned short textpos);
*
*   FUNCTION
*
*   INPUTS
*       reqbase - 
*       left - 
*       top - 
*       width - 
*       height - 
*       text - 
*       textpos - 
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

void _DOpus_ObjectText(struct DOpusIFace *Self, struct RequesterBase *reqbase, short left, short top, short width, short height, char *text, uint16 textpos)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *GfxBase = IExec->OpenLibrary("graphics.library", 50L);
	struct GraphicsIFace *IGraphics = (struct GraphicsIFace *)IExec->GetInterface(GfxBase, "main", 1, NULL);
	struct RastPort *rp;
	short x, y, text_width, text_height, cx, cy, len, got_uscore = -1, uscoreok = 1;
	char *ptr, textbuf[82];

	rp = reqbase->rb_window->RPort;

	IGraphics->SetAPen(rp, reqbase->rb_fg);
	IGraphics->SetBPen(rp, reqbase->rb_bg);

	if(textpos & TEXTPOS_F_NOUSCORE)
	{
		uscoreok = 0;
		textpos &= ~TEXTPOS_F_NOUSCORE;
	}

	for(len = 0, ptr = text; *ptr && len < 80; len++, ptr++)
	{
		if(uscoreok && *ptr == '_' && got_uscore == -1)
		{
			got_uscore = len;
			--len;
		}
		else
			textbuf[len] = *ptr;
	}
	textbuf[len] = 0;

	text_width = len*rp->Font->tf_XSize;
	text_height = rp->Font->tf_YSize;

	cx = left + ((width - text_width) / 2);
	cy = top + ((height - text_height) / 2);

	switch(textpos)
	{
		case TEXTPOS_ABSOLUTE:
			x = left;
			y = top;
			break;
		case TEXTPOS_CENTER:
			x = cx;
			y = cy;
			break;
		case TEXTPOS_LEFT:
			x = left - text_width-rp->Font->tf_XSize;
			y = cy;
			break;
		case TEXTPOS_RIGHT:
			x = left + width + rp->Font->tf_XSize;
			y = cy;
			break;
		case TEXTPOS_ABOVE:
			x = cx;
			y = top - rp->Font->tf_YSize - 2;
			break;
		case TEXTPOS_BELOW:
			x = cx;
			y = top + height + 2;
			break;
	}

	Self->UScoreText(rp, textbuf, x, y + rp->Font->tf_Baseline, got_uscore);

	IExec->DropInterface((struct Interface *)IGraphics);
	IExec->CloseLibrary(GfxBase);
	return;
}
