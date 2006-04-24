/*
 *  $VER: ListViewIDCMP.c $Revision$ (01-Feb-2006)
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
#include <proto/graphics.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

#include "extras.h"

/****** dopus/main/ListViewIDCMP ******************************************
*
*   NAME
*      ListViewIDCMP -- Description
*
*   SYNOPSIS
*      struct DOpusListView * ListViewIDCMP(struct DOpusListView * listview, 
*          struct IntuiMessage * imsg);
*
*   FUNCTION
*
*   INPUTS
*       listview - 
*       imsg - 
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

#define GAD_SLIDER 0
#define GAD_UP     1
#define GAD_DOWN   2

struct DOpusListView * _DOpus_ListViewIDCMP(struct DOpusIFace *Self, struct DOpusListView *view, struct IntuiMessage *imsg)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *IntuitionBase = IExec->OpenLibrary("intuition.library", 50L);
	struct IntuitionIFace *IIntuition = (struct IntuitionIFace *)IExec->GetInterface(IntuitionBase, "main", 1, NULL);
	struct Library *GfxBase = IExec->OpenLibrary("graphics.library", 50L);
	struct GraphicsIFace *IGraphics = (struct GraphicsIFace *)IExec->GetInterface(GfxBase, "main", 1, NULL);
	struct Library *DOSBase = IExec->OpenLibrary("dos.library", 50L);
	struct DOSIFace *IDOS = (struct DOSIFace *)IExec->GetInterface(DOSBase, "main", 1, NULL);
	int gadgetid, list, x, y, rep, xo, histate = 0, offset, itemnum, newoffset, lastout = 0, temp;
	ULONG class, idcmpflags;
	USHORT code;

	class = imsg->Class;
	code = imsg->Code;
	idcmpflags = view->window->IDCMPFlags;

	if(class == IDCMP_GADGETUP || class == IDCMP_GADGETDOWN)
	{
		gadgetid = ((struct Gadget *)imsg->IAddress)->GadgetID;
		if(gadgetid < LIST_IDBASE)
			return ((struct DOpusListView *)-1);
		gadgetid -= LIST_IDBASE;
		list = gadgetid / 3;
		gadgetid %= 3;
		while (view)
		{
			if(view->listid == list && view->window == imsg->IDCMPWindow)
				break;
			view = view->next;
		}
		if(!view)
			return ((struct DOpusListView *)-1);
		IExec->ReplyMsg((struct Message *)imsg);

		if(!(idcmpflags & IDCMP_MOUSEMOVE))
			IIntuition->ModifyIDCMP(view->window, idcmpflags|IDCMP_MOUSEMOVE);

		switch (gadgetid)
		{
		case GAD_SLIDER:
			view->topitem = Self->GetSliderPos(&view->listgads[0], view->count, view->lines);
			DisplayView(view);
			if(view->sliderbgcol)
				Self->ShowSlider(view->window, &view->listgads[0]);
			if(class == IDCMP_GADGETDOWN)
			{
				FOREVER
				{
					IExec->Wait(1 << view->window->UserPort->mp_SigBit);
					while((imsg = (struct IntuiMessage *)IExec->GetMsg(view->window->UserPort)))
					{
						class = imsg->Class;
						IExec->ReplyMsg((struct Message *)imsg);
						if(class == IDCMP_MOUSEMOVE)
						{
							view->topitem = Self->GetSliderPos(&view->listgads[0], view->count, view->lines);
							if(view->sliderbgcol)
								Self->ShowSlider(view->window, &view->listgads[0]);
							DisplayView(view);
						}
						else if(class == IDCMP_GADGETUP)
							break;
					}
					if(class == IDCMP_GADGETUP)
						break;
				}
				view->topitem = Self->GetSliderPos(&view->listgads[0], view->count, view->lines);
				if(view->sliderbgcol)
					Self->ShowSlider(view->window, &view->listgads[0]);
				DisplayView(view);
			}
			break;

		case GAD_UP:
			if(view->topitem == 0)
				break;
			--view->topitem;
		case GAD_DOWN:
			if(gadgetid == GAD_DOWN)
			{
				if(view->topitem > view->count - (view->lines + 1))
					break;
				++view->topitem;
			}
			Self->FixSliderPot(view->window, &view->listgads[0], view->topitem, view->count, view->lines, 1);
			DisplayView(view);
			IDOS->Delay(5);
			FOREVER
			{
				if((imsg = (struct IntuiMessage *)IExec->GetMsg(view->window->UserPort)))
				{
					class = imsg->Class;
					code = imsg->Code;
					IExec->ReplyMsg((struct Message *)imsg);
					if(class == IDCMP_GADGETUP || (class == IDCMP_MOUSEBUTTONS && code == SELECTUP))
						break;
				}
				if(gadgetid == GAD_UP)
				{
					if(view->topitem == 0)
						break;
					--view->topitem;
				}
				else
				{
					if(view->topitem > view->count - (view->lines + 1))
						break;
					++view->topitem;
				}
				Self->FixSliderPot(view->window, &view->listgads[0], view->topitem, view->count, view->lines, 1);
				DisplayView(view);
				if(view->flags & DLVF_SLOW)
					IDOS->Delay(1);
				else
					IGraphics->WaitTOF();
			}
			break;
		}
		IIntuition->ModifyIDCMP(view->window, idcmpflags);
		return (NULL);
	}
	else if(class == IDCMP_EXTENDEDMOUSE)
	{
		struct IntuiWheelData *iwd = imsg->IAddress;
		code = imsg->Code;
		IExec->ReplyMsg((struct Message *)imsg);
		if(code & IMSGCODE_INTUIWHEELDATA)
		{
			if(iwd->WheelY < 0)
			{
				if(view->topitem != 0)
				--view->topitem;
			}
			else if(iwd->WheelY > 0)
			{
				if(view->topitem <= view->count - (view->lines + 1))
				++view->topitem;
			}
		}
		Self->FixSliderPot(view->window, &view->listgads[0], view->topitem, view->count, view->lines, 1);
		DisplayView(view);
		if(view->flags & DLVF_SLOW)
			IDOS->Delay(1);
		else
			IGraphics->WaitTOF();
		IIntuition->ModifyIDCMP(view->window, idcmpflags);
		return (NULL);
	}
	else if(class == IDCMP_MOUSEBUTTONS && code == SELECTDOWN)
	{
		x = imsg->MouseX;
		y = imsg->MouseY;

		while (view)
		{
			if(view->window == imsg->IDCMPWindow && !(view->flags & DLVF_DUMB))
			{
				if(view->flags & DLVF_CHECK)
					xo = view->xo - 22;
				else
					xo = view->xo;
				if(x >= xo && x < view->mx && y >= view->yo && y < view->my)
					break;
			}
			view = view->next;
		}
		if(!view)
			return ((struct DOpusListView *)-1);
		IExec->ReplyMsg((struct Message *)imsg);

		offset = (y - view->yo) / view->fh;
		itemnum = offset + view->topitem;
		if(itemnum >= view->count)
		{
			itemnum = view->count - 1;
			offset = itemnum - view->topitem;
			if(offset < 0)
				return (NULL);
		}

		IIntuition->ModifyIDCMP(view->window, idcmpflags|IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE|IDCMP_INTUITICKS);
		if(!(rep = view->window->Flags & WFLG_REPORTMOUSE))
			view->window->Flags |= WFLG_REPORTMOUSE;

		savepens(view);
		if(view_valid(view, itemnum))
		{
			dohilite(view, offset);
			histate = 1;
		}

		FOREVER
		{
			while((imsg = (struct IntuiMessage *)IExec->GetMsg(view->window->UserPort)))
			{
				class = imsg->Class;
				code = imsg->Code;
				x = imsg->MouseX;
				y = imsg->MouseY;
				IExec->ReplyMsg((struct Message *)imsg);

				if(class == IDCMP_MOUSEBUTTONS)
				{
					if(code == SELECTUP)
						break;
					if(code == MENUUP)
					{
						if(histate)
						{
							histate = 0;
							dohilite(view, offset);
						}
						break;
					}
				}
				else if(class == IDCMP_INTUITICKS)
				{
					if((newoffset = ((y - view->yo) / view->fh)) != offset)
					{
						newoffset = scroll_view(view, newoffset, &histate, offset);
						if(newoffset >= 0 && newoffset < view->lines)
						{
							offset = newoffset;
							temp = itemnum;
							itemnum = view->topitem + offset;
							if(itemnum != temp && view_valid(view, itemnum))
							{
								histate = 1;
								dohilite(view, offset);
							}
						}
					}
				}
				else if(class == IDCMP_MOUSEMOVE)
				{
					if((newoffset = ((y - view->yo) / view->fh)) != offset || x < xo || x >= view->mx || y < view->yo)
					{
						if(y < view->yo || y > view->yo + view->h)
						{
							if(lastout)
								continue;
							lastout = 1;
						}
						else
							lastout = 0;
						if(view->count < view->lines && newoffset >= view->count && offset >= view->count - 1 && histate)
							continue;
						if(histate)
						{
							histate = 0;
							dohilite(view, offset);
						}
						if(x < xo || x >= view->mx)
							continue;
						newoffset = scroll_view(view, newoffset, &histate, 0);
						if(newoffset >= 0 && newoffset < view->lines)
						{
							offset = newoffset;
							itemnum = view->topitem + offset;
							if(view_valid(view, itemnum))
							{
								histate = 1;
								dohilite(view, offset);
							}
						}
					}
					else if(!histate)
					{
						if(view_valid(view, itemnum))
						{
							histate = 1;
							dohilite(view, offset);
						}
					}
				}
			}
			if(class == IDCMP_MOUSEBUTTONS && (code == SELECTUP || code == MENUUP))
				break;
			IExec->Wait(1 << view->window->UserPort->mp_SigBit);
		}

		if(histate)
		{
			if(view->flags & DLVF_MULTI)
			{
				view->selected[itemnum] ^= LVARRAY_SELECTED;
				if(view->flags & DLVF_CHECK)
				{
					dohilite(view, offset);
					IGraphics->SetAPen(view->window->RPort, view->itemfg);
					Self->DrawCheckMark(view->window->RPort, view->xo - 18, view->yo + (view->fh * offset) + view->ty, (view->selected[itemnum] & LVARRAY_SELECTED));
				}
			}
			else if(view->flags & DLVF_LEAVE)
			{
				dohilite(view, offset);
				IGraphics->SetAPen(view->window->RPort, view->itemfg);
				if(view->itemselected == itemnum)
				{
					if(view->flags & DLVF_TOGGLE)
					{
						if(view->chk == view->itemselected)
						{
							if(view->flags & DLVF_CHECK)
							{
								Self->DrawCheckMark(view->window->RPort, view->xo - 18, view->yo + (view->fh * offset) + view->ty, 0);
							}
							else
								dohilite(view, offset);
							view->chk = -1;
						}
						else
						{
							if(view->flags & DLVF_CHECK)
							{
								Self->DrawCheckMark(view->window->RPort, view->xo - 18, view->yo + (view->fh * offset) + view->ty, 1);
							}
							else
								dohilite(view, offset);
							view->chk = view->itemselected;
						}
					}
				}
				else
				{
					if(view->flags & DLVF_CHECK)
					{
						Self->DrawCheckMark(view->window->RPort, view->xo - 18, view->yo + (view->fh * offset) + view->ty, 1);
					}
					else
						dohilite(view, offset);
					if(view->chk > -1)
					{
						y = view->chk - view->topitem;
						if(y >= 0 && y < view->lines)
						{
							if(view->flags & DLVF_CHECK)
							{
								Self->DrawCheckMark(view->window->RPort, view->xo - 18, view->yo + (view->fh * y) + view->ty, 0);
							}
							else
								dohilite(view, y);
						}
					}
					view->chk = itemnum;
				}
			}
			else
				dohilite(view, offset);
		}
		if(!rep)
			view->window->Flags &= ~WFLG_REPORTMOUSE;
		IIntuition->ModifyIDCMP(view->window, idcmpflags);
		restorepens(view);
		if(histate)
		{
			view->itemselected = itemnum;
			return (view);
		}
		return (NULL);
	}
	return ((struct DOpusListView *)-1);
}
