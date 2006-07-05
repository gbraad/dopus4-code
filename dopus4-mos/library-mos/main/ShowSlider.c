/*
 *  $VER: ShowSlider.c $Revision$ (01-Feb-2006)
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
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/ShowSlider ******************************************
*
*   NAME
*      ShowSlider -- Description
*
*   SYNOPSIS
*      void ShowSlider(struct Window * window, struct Gadget * gadget);
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

void _DOpus_ShowSlider(struct Window *win, struct Gadget *gad)
{
	int ob, ot, y, old_pen, old_mode;
	struct Image *image;
	struct PropInfo *pinfo;
	struct RastPort *r;

	pinfo = (struct PropInfo *)gad->SpecialInfo;

	if(pinfo->Flags & AUTOKNOB)
	{
		RefreshGList(gad, win, NULL, 1);
		return;
	}

	image = (struct Image *)gad->GadgetRender;

	r = win->RPort;
	old_pen = r->FgPen;
	old_mode = r->DrawMode;

	SetAPen(r, gad->MutualExclude);
	SetDrMd(r, JAM1);

	if(pinfo->Flags & FREEVERT)
	{
		ob = image->TopEdge + image->Height;
		ot = image->TopEdge;
		y = gad->LeftEdge + gad->Width + 1;
		if(ot >= 0)
			RectFill(r, gad->LeftEdge - 2, gad->TopEdge - 1, y, gad->TopEdge + ot - 1);
		if(ob <= gad->Height)
			RectFill(r, gad->LeftEdge - 2, gad->TopEdge + ob, y, gad->TopEdge + gad->Height);
		if(gad->MutualExclude)
		{
			RectFill(r, gad->LeftEdge - 2, gad->TopEdge + ot, gad->LeftEdge - 1, gad->TopEdge + ob);
			RectFill(r, y - 1, gad->TopEdge + ot, y, gad->TopEdge + ob);
		}
	}
	else
	{
		ob = image->LeftEdge + image->Width;
		ot = image->LeftEdge;
		y = gad->TopEdge + gad->Height + 1;
		if(ot >= 0)
			RectFill(r, gad->LeftEdge - 2, gad->TopEdge - 1, gad->LeftEdge + ot - 1, y - 1);
		if(ob <= gad->Width)
			RectFill(r, gad->LeftEdge + ob, gad->TopEdge - 1, gad->LeftEdge + gad->Width + 1, y - 1);
		if(gad->MutualExclude)
		{
			RectFill(r, gad->LeftEdge + ot, gad->TopEdge - 1, gad->LeftEdge + ob, gad->TopEdge - 1);
			RectFill(r, gad->LeftEdge + ot, y - 1, gad->LeftEdge + ob, y - 1);
		}
	}
	DrawImage(r, image, gad->LeftEdge, gad->TopEdge);

	SetAPen(r, old_pen);
	SetDrMd(r, old_mode);

	return;
}
