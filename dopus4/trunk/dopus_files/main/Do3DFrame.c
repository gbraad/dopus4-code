/*
 *  $VER: Do3DFrame.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/Do3DFrame ******************************************
*
*   NAME
*      Do3DFrame -- Description
*
*   SYNOPSIS
*      void Do3DFrame(struct RastPort * a0arg, int x, int y, int w, int h, 
*          char * title, int tp, int bp);
*
*   FUNCTION
*
*   INPUTS
*       a0arg - 
*       x - 
*       y - 
*       w - 
*       h - 
*       title - 
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

void _DOpus_Do3DFrame(struct DOpusIFace *Self, struct RastPort *rp, int x, int y, int w, int h, char *title, int hi, int lo)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *GfxBase = IExec->OpenLibrary("graphics.library", 50L);
	struct GraphicsIFace *IGraphics = (struct GraphicsIFace *)IExec->GetInterface(GfxBase, "main", 1, NULL);
	char of,ob;
	int a,l;

	of = rp->FgPen;
	ob = rp->BgPen;
	Self->Do3DBox(rp, x + 2, y + 1, w - 4, h - 2, lo, hi);
	Self->Do3DBox(rp, x + 4, y + 2, w - 8, h - 4, hi, lo);
	IGraphics->SetAPen(rp, of);
	IGraphics->SetBPen(rp, ob);
	if(title)
	{
		a = strlen(title);
		x += ((w - (l = (IGraphics->TextLength(rp, title, a)))) / 2);
		IGraphics->SetAPen(rp, ob);
		IGraphics->RectFill(rp, x - rp->Font->tf_XSize / 2, y + 2 - rp->Font->tf_Baseline, x + l + rp->Font->tf_XSize / 2, y + 2 - rp->Font->tf_Baseline + rp->Font->tf_YSize);
		IGraphics->SetAPen(rp, of);
		IGraphics->Move(rp, x, y + 2);
		IGraphics->Text(rp, title, a);
	}
	IExec->DropInterface((struct Interface *)IGraphics);
	IExec->CloseLibrary(GfxBase);
	return;
}
