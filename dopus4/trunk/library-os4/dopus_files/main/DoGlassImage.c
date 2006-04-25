/*
 *  $VER: DoGlassImage.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/DoGlassImage ******************************************
*
*   NAME
*      DoGlassImage -- Description
*
*   SYNOPSIS
*      void DoGlassImage(struct RastPort * rp, struct Gadget * gadget, 
*          int shine, int shadow, int type);
*
*   FUNCTION
*
*   INPUTS
*       rp - 
*       gadget - 
*       shine - 
*       shadow - 
*       type - 
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

extern USHORT glass_image1[];
extern USHORT glass_image2[];

void _DOpus_DoGlassImage(struct DOpusIFace *Self, struct RastPort *rp, struct Gadget *gadget, int shine, int shadow, int type)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *GfxBase = IExec->OpenLibrary("graphics.library", 50L);
	struct GraphicsIFace *IGraphics = (struct GraphicsIFace *)IExec->GetInterface(GfxBase, "main", 1, NULL);
	int x, y;
	char op, om;

	om = rp->DrawMode;
	op = rp->FgPen;
	IGraphics->SetDrMd(rp, JAM1);

	if(type == 0)
	{
		x = gadget->LeftEdge + ((gadget->Width - 16) / 2);
		y = gadget->TopEdge + ((gadget->Height - 8) / 2);

		IGraphics->SetAPen(rp, shine);
		IGraphics->BltTemplate((UBYTE *)glass_image2, 0, 2, rp, x, y, 16, 8);
		IGraphics->SetAPen(rp, shadow);
		IGraphics->BltTemplate((UBYTE *)glass_image1, 0, 2, rp, x, y, 16, 8);
	}

	IGraphics->SetAPen(rp, op);
	IGraphics->SetDrMd(rp, om);

	IExec->DropInterface((struct Interface *)IGraphics);
	IExec->CloseLibrary(GfxBase);
	return;
}
