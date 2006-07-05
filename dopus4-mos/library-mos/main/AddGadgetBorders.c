/*
 *  $VER: AddGadgetBorders.c $Revision$ (01-Feb-2006)
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
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/AddGadgetBorders ******************************************
*
*   NAME
*      AddGadgetBorders -- Description
*
*   SYNOPSIS
*      void AddGadgetBorders(struct DOpusRemember ** key, 
*          struct Gadget * gadget, int count, int shine, int shadow);
*
*   FUNCTION
*
*   INPUTS
*       key - 
*       gadget - 
*       count - 
*       shine - 
*       shadow - 
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

void _DOpus_AddGadgetBorders(struct DOpusRemember **key, struct Gadget *gadget, int count, int shine, int shadow)
{
	struct Border *selborder, *unselborder;
	int width, height;

	width = gadget->Width;
	height = gadget->Height;

	CreateGadgetBorders(key, gadget->Width, gadget->Height, &selborder, &unselborder, 0, shine, shadow);

	while(gadget && count)
	{
		if(gadget->Width==width && gadget->Height==height && (gadget->GadgetType==GTYP_BOOLGADGET || gadget->GadgetType&GTYP_SYSGADGET))
		{
			if(unselborder)
			{
				gadget->Flags |= GFLG_GADGHIMAGE;
				gadget->Flags &= ~GFLG_GADGHCOMP;
				gadget->GadgetRender = unselborder;
				gadget->SelectRender = selborder;
			}
			else
			{
				gadget->Flags &= ~GFLG_GADGHIMAGE;
				gadget->Flags |= GFLG_GADGHCOMP;
				gadget->GadgetRender = NULL;
				gadget->SelectRender = NULL;
			}
		}
		gadget = gadget->NextGadget;
		--count;
	}
}

