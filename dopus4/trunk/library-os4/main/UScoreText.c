/*
 *  $VER: UScoreText.c $Revision$ (01-Feb-2006)
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


#include <string.h>

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

/****** dopus/main/UScoreText ******************************************
*
*   NAME
*      UScoreText -- Description
*
*   SYNOPSIS
*      void UScoreText(struct RastPort * rp, char * text, int x, int y, 
*          int upos);
*
*   FUNCTION
*
*   INPUTS
*       rp - 
*       text - 
*       x - 
*       y - 
*       upos - 
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

void _DOpus_UScoreText(struct DOpusIFace *Self, struct RastPort *rp, char *buf, int xp, int yp, int up)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *GfxBase = IExec->OpenLibrary("graphics.library", 50L);
	struct GraphicsIFace *IGraphics = (struct GraphicsIFace *)IExec->GetInterface(GfxBase, "main", 1, NULL);
	int a, x, x1, y1;

	IGraphics->Move(rp, xp, yp);
	IGraphics->Text(rp, buf,(a = strlen(buf)));
	x1 = rp->cp_x;
	y1 = rp->cp_y;
	if((up > -1) && (up < a))
	{
		x = IGraphics->TextLength(rp, buf, up);
		IGraphics->Move(rp, xp + x, yp + 2);
		IGraphics->Draw(rp, rp->cp_x + (IGraphics->TextLength(rp, &buf[up], 1)) - 1, rp->cp_y);
	}
	IGraphics->Move(rp, x1, y1);

	IExec->DropInterface((struct Interface *)IGraphics);
	IExec->CloseLibrary(GfxBase);
	return;
}

