/*
 *  $VER: Do3DCycleBox.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/Do3DCycleBox ******************************************
*
*   NAME
*      Do3DCycleBox -- Description
*
*   SYNOPSIS
*      void Do3DCycleBox(struct RastPort * rp, int x, int y, int w, int h, 
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
extern USHORT pdb_cycletop[];
extern USHORT pdb_cyclebot[];

void _DOpus_Do3DCycleBox(struct DOpusIFace *Self, struct RastPort *rp, int x, int y, int w, int h, int tp, int bp)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *GfxBase = IExec->OpenLibrary("graphics.library", 50L);
	struct GraphicsIFace *IGraphics = (struct GraphicsIFace *)IExec->GetInterface(GfxBase, "main", 1, NULL);
	UBYTE old_drmd;

	Self->Do3DBox(rp, x, y, w, h, tp, bp);

	old_drmd = IGraphics->GetDrMd(rp);
	IGraphics->SetDrMd(rp, JAM1);

	IGraphics->SetAPen(rp, bp);

	if(/*DOpusBase->*/pdb_cycletop)
	{
		IGraphics->BltTemplate(/*DOpusBase->*/pdb_cycletop, 0, 2, rp, x + 4, y + 1, 11, 6);
	}
	if (/*DOpusBase->*/pdb_cyclebot)
	{
		IGraphics->BltTemplate(/*DOpusBase->*/pdb_cyclebot, 0, 2, rp, x + 4, y + 1 + h - 12 + 7, 11, 2);
	}

	drawline(rp, x + 4, y + 7, x + 4, /*y + 7 + */y + 1 + h - 12 + 7/* -12*/);
	drawline(rp, x + 5, y + 7, x + 5, /*y + 7 + */y + 1 + h - 12 + 7/* -12*/);
	drawline(rp, x + 18, y + 1, x + 18, y + h - 2);

	IGraphics->SetAPen(rp, tp);

	drawline(rp, x + 19, y + 1, x + 19, y + h - 2);

	IGraphics->SetDrMd(rp, old_drmd);
	IExec->DropInterface((struct Interface *)IGraphics);
	IExec->CloseLibrary(GfxBase);
	return;
}

