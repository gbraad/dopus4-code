/*
 *  $VER: DoRMBGadget.c $Revision$ (01-Feb-2006)
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
#include <proto/graphics.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

#include "extras.h"

/****** dopus/main/DoRMBGadget ******************************************
*
*   NAME
*      DoRMBGadget -- Description
*
*   SYNOPSIS
*      int DoRMBGadget(struct RMBGadget * rmbgadget, struct Window * window);
*
*   FUNCTION
*
*   INPUTS
*       rmbgadget - 
*       window - 
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

#define isinside(x,y,x1,y1,x2,y2) (x>=x1 && x<=x2 && y>=y1 && y<=y2)

int _DOpus_DoRMBGadget(struct RMBGadget *gad, struct Window *window)
{
	ULONG class;
	USHORT code;
	BOOL inside;
	struct RastPort *rp;
	int x, y, x1, y1, ret = -1;
	ULONG idcmpflags, windowflags;
	struct IntuiMessage *Msg;
	struct Gadget dummy_gadget;

	x = window->MouseX;
	y = window->MouseY;
	while (gad)
	{
		if(gad->w > 0 && gad->h > 0)
		{
			x1 = gad->x + gad->w - 1;
			y1 = gad->y + gad->h - 1;
			if(isinside(x, y, gad->x, gad->y, x1, y1))
				break;
		}
		gad = gad->next;
	}
	if(!gad)
		return (-1);
	rp = window->RPort;

	idcmpflags = window->IDCMPFlags;
	ModifyIDCMP(window, IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_INACTIVEWINDOW);

	windowflags = window->Flags;
	window->Flags |= WFLG_REPORTMOUSE;

	if(isinside(x, y, gad->x, gad->y, x1, y1))
	{
		if(gad->flags & RGF_ALTTEXT)
			ShowRMBGadName(rp, gad, 1);
		HighlightRMBGad(rp, gad, 1);
		inside = TRUE;
	}
	else
		inside = FALSE;

	dummy_gadget.LeftEdge = 0;
	dummy_gadget.TopEdge = 0;
	dummy_gadget.Width = window->Width;
	dummy_gadget.Height = window->Height;
	dummy_gadget.Flags = GFLG_GADGHNONE;
	dummy_gadget.Activation = GACT_IMMEDIATE;
	dummy_gadget.GadgetType = GTYP_BOOLGADGET;
	dummy_gadget.GadgetRender = NULL;
	dummy_gadget.SelectRender = NULL;
	dummy_gadget.GadgetText = NULL;
	AddGList(window, &dummy_gadget, 0, 1, NULL);

	FOREVER
	{
		while((Msg = (struct IntuiMessage *)GetMsg(window->UserPort)))
		{
			class = Msg->Class;
			code = Msg->Code;
			x = Msg->MouseX;
			y = Msg->MouseY;
			ReplyMsg((struct Message *)Msg);

			if(class == IDCMP_INACTIVEWINDOW)
				goto end_rmb;
			if(class == IDCMP_MOUSEBUTTONS && code == MENUUP)
			{
				if(inside)
					ret = gad->id;
				goto end_rmb;
			}
			if(class == IDCMP_MOUSEMOVE)
			{
				if(!(isinside(x, y, gad->x, gad->y, x1, y1)))
				{
					if(inside)
					{
						HighlightRMBGad(rp, gad, 0);
						if(gad->flags & RGF_ALTTEXT)
							ShowRMBGadName(rp, gad, 0);
						inside = FALSE;
					}
				}
				else
				{
					if(!inside)
					{
						if(gad->flags & RGF_ALTTEXT)
							ShowRMBGadName(rp, gad, 1);
						HighlightRMBGad(rp, gad, 1);
						inside = TRUE;
					}
				}
			}
		}
		Wait(1 << window->UserPort->mp_SigBit);
	}
      end_rmb:
	if(inside)
	{
		HighlightRMBGad(rp, gad, 0);
		if(gad->flags & RGF_ALTTEXT)
			ShowRMBGadName(rp, gad, 0);
	}
	RemoveGList(window, &dummy_gadget, 1);
	ModifyIDCMP(window, idcmpflags);
	window->Flags = windowflags;

	return (ret);
}
