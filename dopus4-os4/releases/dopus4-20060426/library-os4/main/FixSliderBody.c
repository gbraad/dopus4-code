/*
 *  $VER: FixSliderBody.c $Revision$ (01-Feb-2006)
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
#include <proto/intuition.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/FixSliderBody ******************************************
*
*   NAME
*      FixSliderBody -- Description
*
*   SYNOPSIS
*      void FixSliderBody(struct Window * window, struct Gadget * gadget, 
*          int count, int lines, int show);
*
*   FUNCTION
*
*   INPUTS
*       window - 
*       gadget - 
*       count - 
*       lines - 
*       show - 
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

void _DOpus_FixSliderBody(struct DOpusIFace *Self, struct Window *win, struct Gadget *gad, int count, int lines, int show)
{
	USHORT body, gh, ih, min;
	struct Image *image;
	struct PropInfo *pinfo;
	float div;
	int vh, oh;

	pinfo = (struct PropInfo *)gad->SpecialInfo;
	if(pinfo->Flags & FREEVERT)
	{
		vh = 0;
		gh = gad->Height;
		min = KNOBVMIN;
	}
	else
	{
		vh = 1;
		gh = gad->Width;
		min = KNOBHMIN;
	}
	if(count <= lines)
		body = 0xffff;
	else
	{
		body = ((float)0x7fff) / (((float)count) / ((float)lines));
		body <<= 1;
	}
	image = (struct Image *)gad->GadgetRender;
	if(body == 0)
		ih = min;
	else
	{
		div = (float)0xffff / (float)body;
		if(div == 0)
			div = 1;
		ih = (int)((float)((float)gh / div));
		if(ih < min)
			ih = min;
	}
	if(vh)
		pinfo->HorizBody = body;
	else
		pinfo->VertBody = body;
	if(pinfo->Flags & AUTOKNOB)
	{
		if(show && win)
			IIntuition->NewModifyProp(gad, win, NULL, pinfo->Flags, pinfo->HorizPot, pinfo->VertPot, pinfo->HorizBody, pinfo->VertBody, 1);
	}
	else
	{
		if(vh)
		{
			oh = image->Width;
			image->Width = ih;
		}
		else
		{
			oh = image->Height;
			image->Height = ih;
		}
		if(show && win && (oh != ih || show == 2))
			Self->ShowSlider(win, gad);
	}

	return;
}
