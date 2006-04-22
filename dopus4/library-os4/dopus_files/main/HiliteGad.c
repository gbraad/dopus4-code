/*
 *  $VER: HiliteGad.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/HiliteGad ******************************************
*
*   NAME
*      HiliteGad -- Description
*
*   SYNOPSIS
*      void HiliteGad(struct Gadget * gadget, struct RastPort * a1arg);
*
*   FUNCTION
*
*   INPUTS
*       gadget - 
*       a1arg - 
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

void _DOpus_HiliteGad(struct DOpusIFace *Self, struct Gadget *gad, struct RastPort *rp)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *GfxBase = IExec->OpenLibrary("graphics.library", 50L);
	struct GraphicsIFace *IGraphics = (struct GraphicsIFace *)IExec->GetInterface(GfxBase, "main", 1, NULL);
	UBYTE old_drmd = IGraphics->GetDrMd(rp);

	IGraphics->SetDrMd(rp, COMPLEMENT);
	IGraphics->RectFill(rp, gad->LeftEdge, gad->TopEdge, gad->LeftEdge + gad->Width - 1, gad->TopEdge + gad->Height - 1);
	IGraphics->SetDrMd(rp,old_drmd);
}

