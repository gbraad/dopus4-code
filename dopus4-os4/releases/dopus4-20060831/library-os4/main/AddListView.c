/*
 *  $VER: AddListView.c $Revision$ (01-Feb-2006)
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

#include "extras.h"

/****** dopus/main/AddListView ******************************************
*
*   NAME
*      AddListView -- Description
*
*   SYNOPSIS
*      int AddListView(struct DOpusListView * listview, int count);
*
*   FUNCTION
*
*   INPUTS
*       listview - 
*       count - 
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

#define LIST_IDBASE 32767

int _DOpus_AddListView(struct DOpusIFace *Self, struct DOpusListView *view, int count)
{
	struct RastPort *rp;
	int a, addcount = 0;

	while (view && (addcount < count))
	{
		view->key = NULL;
		view->selected = NULL;
		view->gadadd = 0;
		view->chk = view->itemselected;
		if(view->items)
			for(view->count = 0; view->items[view->count] && (!(view->flags & DLVF_ENDNL) || view->items[view->count][0]); view->count++);
		else
			view->count = 0;

		if(!(view->listgads = (struct Gadget *)Self->LAllocRemember(&view->key, sizeof(struct Gadget) * 3, MEMF_CLEAR)) || !(view->listimage = (struct Image *)Self->LAllocRemember(&view->key, sizeof(struct Image), MEMF_CLEAR)) || !(view->listprop = (struct PropInfo *)Self->LAllocRemember(&view->key, sizeof(struct PropInfo), MEMF_CLEAR)) || !(view->selected = (char *)Self->LAllocRemember(&view->key, view->count, MEMF_CLEAR)))
		{
			Self->LFreeRemember(&view->key);
			break;
		}
		if(view->selectarray)
			IExec->CopyMem(view->selectarray, view->selected, view->count);
		if(view->slidercol == -1)
			view->slidercol = 1;
		if(view->sliderbgcol == -1)
			view->slidercol = 0;
		if(view->textcol == -1)
			view->textcol = 1;
		if(view->boxhi == -1)
			view->boxhi = 1;
		if(view->boxlo == -1)
			view->boxlo = 2;
		if(view->arrowfg == -1)
			view->arrowfg = 1;
		if(view->arrowbg == -1)
			view->arrowbg = 0;
		if(view->itemfg == -1)
			view->itemfg = 1;
		if(view->itembg == -1)
			view->itembg = 0;
		rp = view->window->RPort;
		view->fw = rp->Font->tf_XSize;
		view->fh = rp->Font->tf_YSize;
		if(view->fh < 8)
			view->fh = 8;
		view->fb = rp->Font->tf_Baseline;
		view->ty = (view->fh - 8) / 2;
		view->lines = view->h / view->fh;
		view->columns = view->w / view->fw;
		if(view->columns > 128)
			view->columns = 128;
		view->yo = view->y + ((view->h - (view->lines * view->fh)) / 2);
		view->xo = view->x + ((view->w - (view->columns * view->fw)) / 2);
		if(view->flags & DLVF_CHECK)
		{
			view->xo += 22;
			view->columns = (view->w - 22) / view->fw;
		}
		if(view->topitem > (view->count - view->lines))
			view->topitem = view->count - view->lines;
		if(view->topitem < 0)
			view->topitem = 0;

		savepens(view);
		IGraphics->SetDrMd(rp, JAM2);
		if(view->flags & DLVF_DUMB)
			Self->Do3DBox(rp, view->x, view->y, view->w, view->h, view->boxlo, view->boxhi);
		else
			Self->Do3DBox(rp, view->x, view->y, view->w, view->h, view->boxhi, view->boxlo);
		Self->Do3DBox(rp, view->x + view->w + 4, view->y, view->sliderwidth + 4, view->h - 16, view->boxhi, view->boxlo);
		Self->DoArrow(rp, view->x + view->w + 4, view->y + view->h - 14, view->sliderwidth + 4, 6, view->arrowfg, view->arrowbg, 0);
		Self->Do3DBox(rp, view->x + view->w + 4, view->y + view->h - 14, view->sliderwidth + 4, 6, view->boxhi, view->boxlo);
		Self->DoArrow(rp, view->x + view->w + 4, view->y + view->h - 6, view->sliderwidth + 4, 6, view->arrowfg, view->arrowbg, 1);
		Self->Do3DBox(rp, view->x + view->w + 4, view->y + view->h - 6, view->sliderwidth + 4, 6, view->boxhi, view->boxlo);
		if(view->title)
		{
			IGraphics->SetAPen(rp, view->textcol);
			a = strlen(view->title);
			if(view->flags & DLVF_TTOP)
				IGraphics->Move(rp, view->x, (view->y - view->fh - 2) + view->fb);
			else
				IGraphics->Move(rp, view->x - 6 - (a * view->fw), view->yo + view->fb);
			IGraphics->Text(rp, view->title, a);
		}
		view->listimage->Width = view->sliderwidth;
		view->listimage->Height = view->h - 18;
		view->listimage->Depth = 1;
		view->listimage->PlaneOnOff = view->slidercol;
		view->listprop->Flags = FREEVERT | PROPBORDERLESS;
		view->listprop->VertBody = 0xffff;
		view->listgads[0].NextGadget = &view->listgads[1];
		view->listgads[0].LeftEdge = view->x + view->w + 6;
		view->listgads[0].TopEdge = view->y + 1;
		view->listgads[0].Width = view->sliderwidth;
		view->listgads[0].Height = view->h - 18;
		view->listgads[0].Flags = GFLG_GADGHNONE;
		view->listgads[0].Activation = GACT_IMMEDIATE|GACT_RELVERIFY|GACT_FOLLOWMOUSE;
		view->listgads[0].GadgetType = GTYP_PROPGADGET;
		view->listgads[0].GadgetRender = (APTR) view->listimage;
		view->listgads[0].MutualExclude = view->sliderbgcol;
		view->listgads[0].SpecialInfo = (APTR) view->listprop;
		view->listgads[0].GadgetID = LIST_IDBASE + (view->listid * 3);
		view->listgads[1].LeftEdge = view->x + view->w + 2;
		view->listgads[1].TopEdge = view->y + view->h - 15;
		view->listgads[1].Width = view->sliderwidth + 8;
		view->listgads[1].Height = 8;
		view->listgads[1].Flags = GFLG_GADGHCOMP;
		view->listgads[1].Activation = GACT_IMMEDIATE|GACT_RELVERIFY;
		view->listgads[1].GadgetType = GTYP_BOOLGADGET;
		view->listgads[1].GadgetID = LIST_IDBASE + (view->listid * 3) + 1;
		IExec->CopyMem((char *)&view->listgads[1], (char *)&view->listgads[2], sizeof(struct Gadget));
		view->listgads[1].NextGadget = &view->listgads[2];
		view->listgads[2].TopEdge += 8;
		view->listgads[2].GadgetID++;
		if(view->flags & DLVF_HIREC)
			Self->AddGadgetBorders(&view->key, &view->listgads[1], 2, view->boxhi, view->boxlo);
		IIntuition->AddGList(view->window, view->listgads, -1, 3, NULL);
		view->gadadd = 3;
		view->mx = view->xo + (view->columns * view->fw) - 1;
		view->my = view->yo + (view->lines * view->fh) - 1;
		view->oldoffset = -1;
		restorepens(view);
		Self->FixSliderBody(view->window, &view->listgads[0], view->count, view->lines, 0);
		Self->FixSliderPot(view->window, &view->listgads[0], view->topitem, view->count, view->lines, 2);
		DisplayView(Self, view);
//		DisplayView(view);
		view = view->next;
		++addcount;
	}
	return (addcount);
}
