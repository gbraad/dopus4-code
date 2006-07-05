/*
 *  $VER: FixSliderPot.c $Revision$ (01-Feb-2006)
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
#include <proto/intuition.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/FixSliderPot ******************************************
*
*   NAME
*      FixSliderPot -- Description
*
*   SYNOPSIS
*      void FixSliderPot(struct Window * window, struct Gadget * gadget, 
*          int offset, int count, int lines, int show);
*
*   FUNCTION
*
*   INPUTS
*       window - 
*       gadget - 
*       offset - 
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

void _DOpus_FixSliderPot(struct Window *win, struct Gadget *gad, int off, int count, int lines, int show)
{
	USHORT vert, vh, gh, ih, te, oh;
	DOUBLE div;
	struct Image *image;
	struct PropInfo *pinfo;

	image = (struct Image *)gad->GadgetRender;
	pinfo = (struct PropInfo *)gad->SpecialInfo;
	if(pinfo->Flags & FREEVERT)
	{
		vh = 0;
		gh = gad->Height;
		ih = image->Height;
	}
	else
	{
		vh = 1;
		gh = gad->Width;
		ih = image->Width;
	}

	if(count <= lines)
		vert = 0;
	else
		vert = ((DOUBLE)off * 0xffff) / (count - lines);
	if(vert == 0)
		te = 0;
	else
	{
		div = (DOUBLE)0xffff / (DOUBLE)vert;
		if(div == 0)
			div = 1;
		te = (int)((DOUBLE)((DOUBLE)(gh - ih) / div));
	}
	if(vh)
		pinfo->HorizPot = vert;
	else
		pinfo->VertPot = vert;
	if(pinfo->Flags & AUTOKNOB)
	{
		if(show && win)
			NewModifyProp(gad, win, NULL, pinfo->Flags, pinfo->HorizPot, pinfo->VertPot, pinfo->HorizBody, pinfo->VertBody, 1);
	}
	else
	{
		if(vh)
		{
			oh = image->LeftEdge;
			image->LeftEdge = te;
		}
		else
		{
			oh = image->TopEdge;
			image->TopEdge = te;
		}
		if(show && win && (oh != te || show == 2))
			ShowSlider(win, gad);
	}

	return;
}
