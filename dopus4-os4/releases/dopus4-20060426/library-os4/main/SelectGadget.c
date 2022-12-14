/*
 *  $VER: SelectGadget.c $Revision$ (01-Feb-2006)
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
#include <proto/dos.h>
#include <proto/intuition.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/SelectGadget ******************************************
*
*   NAME
*      SelectGadget -- Description
*
*   SYNOPSIS
*      void SelectGadget(struct Window * window, struct Gadget * gadget);
*
*   FUNCTION
*
*   INPUTS
*       window - 
*       gadget - 
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

void _DOpus_SelectGadget(struct DOpusIFace *Self, struct Window *window, struct Gadget *gadget)
{
	if(gadget->GadgetRender && gadget->SelectRender && !(gadget->Flags&GFLG_GADGIMAGE))
	{
		IIntuition->DrawBorder(window->RPort, gadget->SelectRender, gadget->LeftEdge, gadget->TopEdge);
		IDOS->Delay(5);
		IIntuition->DrawBorder(window->RPort, gadget->GadgetRender, gadget->LeftEdge, gadget->TopEdge);
	}
	else
	{
		int old;

		old = window->RPort->DrawMode;
		IGraphics->SetDrMd(window->RPort, COMPLEMENT);
		Self->HiliteGad(gadget, window->RPort);
		IDOS->Delay(5);
		Self->HiliteGad(gadget,window->RPort);
		IGraphics->SetDrMd(window->RPort, old);
	}

	return;
}

