/*
 *  $VER: DrawCheckMark.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/DrawCheckMark ******************************************
*
*   NAME
*      DrawCheckMark -- Description
*
*   SYNOPSIS
*      void DrawCheckMark(struct RastPort * rp, int x, int y, int clear);
*
*   FUNCTION
*
*   INPUTS
*       rp - 
*       x - 
*       y - 
*       clear - 
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

extern USHORT pdb_check[];

void _DOpus_DrawCheckMark(struct DOpusIFace *Self, struct RastPort *rp, int x, int y, int clear)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *GfxBase = IExec->OpenLibrary("graphics.library", 50L);
	struct GraphicsIFace *IGraphics = (struct GraphicsIFace *)IExec->GetInterface(GfxBase, "main", 1, NULL);
//	struct DOpusBase *dobase = (struct DOpusBase *)Self->Data.LibBase;
	UBYTE old_apen;
	UBYTE old_drmd = IGraphics->GetDrMd(rp);

	IGraphics->SetDrMd(rp,JAM1);

	if(clear == 0)
	{
		old_apen = IGraphics->GetAPen(rp);
		IGraphics->SetAPen(rp, IGraphics->GetBPen(rp));
	}
	if(/*DOpusBase->*/pdb_check)
	{
		IGraphics->BltTemplate(/*DOpusBase->*/pdb_check, 0, 2, rp, x, y, 13, 7);
	}
	if(clear == 0)
	{
		IGraphics->SetAPen(rp, old_apen);
	}
	IGraphics->SetDrMd(rp, old_drmd);
}

