/*
 *  $VER: AddGadgets.c $Revision$ (01-Feb-2006)
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
#include <proto/intuition.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/AddGadgets ******************************************
*
*   NAME
*      AddGadgets -- Description
*
*   SYNOPSIS
*      int AddGadgets(struct Window * window, struct Gadget * gadget, 
*          char ** text, int count, int fg, int bg, int add);
*
*   FUNCTION
*
*   INPUTS
*       window - 
*       gadget - 
*       text - 
*       count - 
*       fg - 
*       bg - 
*       add - 
*
*   RESULT
*       The result ...
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

int _DOpus_AddGadgets(struct DOpusIFace *Self, struct Window *win, struct Gadget *firstgad, char **text, int count, int fg, int bg, int add)
{
	int num = 0, realcount = 0, of, ob, a, x, y, b, c, up, xp, yp, bl, len, offset;
	char buf[80];
	struct RastPort *rp;
	struct Gadget *gad;

	rp = win->RPort;
	gad = firstgad;
	of = rp->FgPen;
	ob = rp->BgPen;
	x = rp->Font->tf_XSize;
	y = rp->Font->tf_YSize;
	bl = rp->Font->tf_Baseline;

	while (gad && num < count)
	{
		while (text && text[realcount] && text[realcount] != (char *)-1 && !text[realcount][0])
			++realcount;
		if(text && !text[realcount])
			text = NULL;

		if(gad->GadgetType == GTYP_STRGADGET)
		{
			Self->Do3DStringBox(rp, gad->LeftEdge - (gad->MutualExclude * 2), gad->TopEdge - gad->MutualExclude, gad->Width + (gad->MutualExclude * 4), gad->Height + (gad->MutualExclude * 2), fg, bg);
		}
		else if(gad->GadgetType == GTYP_PROPGADGET)
		{
			Self->Do3DBox(rp, gad->LeftEdge - 2, gad->TopEdge - 1, gad->Width + 4, gad->Height + 2, fg, bg);
		}
		else if(gad->MutualExclude == GAD_CYCLE)
		{
			Self->Do3DCycleBox(rp, gad->LeftEdge + 2, gad->TopEdge + 1, gad->Width - 4, gad->Height - 2, fg, bg);
		}
		else if(gad->MutualExclude == GAD_CHECK)
		{
			Self->Do3DBox(rp, gad->LeftEdge + 2, gad->TopEdge + 1, 22, 9, fg, bg);
		}
		else if(gad->MutualExclude != GAD_NONE && gad->MutualExclude != GAD_RADIO)
		{
			Self->Do3DBox(rp, gad->LeftEdge + 2, gad->TopEdge + 1, gad->Width - 4, gad->Height - 2, fg, bg);
		}

		IGraphics->SetAPen(rp, of);
		IGraphics->SetBPen(rp, ob);

		if(text && text[realcount] && text[realcount] != (char *)-1 && text[realcount][0])
		{
			a = strlen(text[realcount]);
			up = -1;
			for(b = 0, c = 0; b < a; b++)
			{
				if(text[realcount][b] == '_' && up == -1)
					up = c;
				else
					buf[c++] = text[realcount][b];
			}
			buf[c] = 0;
			if(up > -1)
				--a;
			len = IGraphics->TextLength(rp, text[realcount], a);

			if(gad->GadgetType == GTYP_STRGADGET || (gad->GadgetType == GTYP_PROPGADGET && !(((struct PropInfo *)gad->SpecialInfo)->Flags & FREEHORIZ)) || gad->MutualExclude == GAD_GLASS || gad->MutualExclude == GAD_CYCLE)
			{

				if(gad->GadgetType == GTYP_STRGADGET)
					offset = gad->MutualExclude;
				else
					offset = 0;

				xp = gad->LeftEdge - (offset * 2) - (len + x);
				yp = gad->TopEdge + ((gad->Height + (offset * 2) - y) / 2) + bl - offset;
			}
			else if(gad->GadgetType == GTYP_PROPGADGET)
			{
				xp = gad->LeftEdge + ((gad->Width - len) / 2);
				yp = gad->TopEdge - (y * 2) + bl;
			}
			else
			{
				switch (gad->MutualExclude)
				{
				case GAD_CHECK:
					xp = gad->LeftEdge + 36;
					break;
				case GAD_RADIO:
					xp = gad->LeftEdge + gad->Width + 8;
					break;
				default:
					xp = gad->LeftEdge + ((gad->Width - len) / 2);
					break;
				}
				yp = gad->TopEdge + ((((gad->MutualExclude == GAD_CHECK) ? 12 : gad->Height) - y) / 2) + bl;
			}

			Self->UScoreText(rp, buf, xp, yp, up);
		}

		if(gad->GadgetType == GTYP_BOOLGADGET && gad->MutualExclude != GAD_CHECK && gad->MutualExclude != GAD_RADIO && gad->Activation & GACT_TOGGLESELECT && gad->Flags & GFLG_SELECTED)
			Self->HiliteGad(gad, rp);

		gad = gad->NextGadget;
		++num;
		++realcount;
	}
	if(add)
	{
		IIntuition->AddGList(win, firstgad, -1, realcount, NULL);
		gad = firstgad;
		for(a = 0; a < realcount && gad; a++)
		{
			if(gad->GadgetType != GTYP_PROPGADGET)
			{
				if(gad->GadgetType == GTYP_STRGADGET || gad->MutualExclude == GAD_RADIO || (gad->GadgetType == GTYP_BOOLGADGET && gad->MutualExclude == GAD_CHECK && gad->Activation & GACT_TOGGLESELECT && gad->Flags & GFLG_SELECTED))
					IIntuition->RefreshGList(gad, win, NULL, 1);
			}
			gad = gad->NextGadget;
		}
	}
	return (realcount);
}

