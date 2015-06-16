/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 distributed under the GPL may be entitled
to upgrade to the latest versions of Directory Opus at a reduced price.
Please see http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include "config.h"

static struct DOpusRemember *hkey;
static char **hotkeylist;

int dohotkeysconfig()
{
	ULONG class;
	USHORT code, gadgetid = 0;
	struct ConfigUndo *undo;
	struct DOpusListView *view;
	struct dopushotkey hotkey, *newhot = NULL, *hot, *swapfirst = NULL;
	int a, mode = -1;

	showconfigscreen(CFG_HOTKEYS);
	makehotkeylist();
	undo = makeundo(UNDO_HOTKEYS);

	for(;;)
	{
		IExec->Wait(1 << Window->UserPort->mp_SigBit);
		while((IMsg = getintuimsg()))
		{
			if((view = IDOpus->ListViewIDCMP(&hotkeyslist, IMsg)) == (struct DOpusListView *)-1)
			{
				class = IMsg->Class;
				code = IMsg->Code;
				if(class == IDCMP_GADGETUP || class == IDCMP_GADGETDOWN)
					gadgetid = ((struct Gadget *)IMsg->IAddress)->GadgetID;
				IExec->ReplyMsg((struct Message *)IMsg);
				switch (class)
				{
				case IDCMP_MENUPICK:
					if(code == MENUNULL)
						break;
					if(mode > -1)
					{
						select_gadget(&maingad[mode], 0);
						mode = -1;
					}
					dogadgetinfo(NULL);
					switch ((a = ITEMNUM(code)))
					{
					case 0:
					case 1:
						if(doload(CFG_HOTKEYS, a))
						{
							makehotkeylist();
							if(a)
								dogadgetinfo(cfg_string[STR_NEW_HOTKEYS_MERGED]);
							else
								dogadgetinfo(cfg_string[STR_DEFAULT_HOTKEYS_MERGED]);
						}
						break;
					}
					break;
				case IDCMP_GADGETUP:
					if(gadgetid != HOTKEYS_OKAY && gadgetid != HOTKEYS_CANCEL)
					{
						if(mode == HOTKEYS_SWAP && swapfirst)
						{
							swapfirst = NULL;
							view->itemselected = -1;
							makehotkeylist();
						}
						if(mode == gadgetid)
						{
							mode = -1;
							dogadgetinfo(NULL);
							break;
						}
						if(mode > -1)
						{
							select_gadget(&maingad[mode], 0);
							mode = -1;
						}
					}
					switch (gadgetid)
					{
					case HOTKEYS_OKAY:
					case HOTKEYS_CANCEL:
						if(gadgetid == HOTKEYS_CANCEL)
							doundo(undo, UNDO_HOTKEYS);
						doundo(undo, 0);
						IDOpus->RemoveListView(&hotkeyslist, 1);
						IDOpus->LFreeRemember(&hkey);
						return ((gadgetid == HOTKEYS_OKAY));
					case HOTKEYS_NEWHOTKEY:
						hotkey.name[0] = 0;
						hotkey.func.function = NULL;
						hotkey.code = hotkey.qualifier = 0;
						hotkey.func.which = 0;
						hotkey.func.stack = 8192;
						hotkey.func.type = 1;
						hotkey.func.pri = 0;
						hotkey.func.key = hotkey.func.qual = 0;
						hotkey.func.delay = 2;
					      editnewhotkey:
						if(doedithotkey(&hotkey))
						{
							if((newhot = (struct dopushotkey *)getcopy((char *)&hotkey, sizeof(struct dopushotkey), NULL)))
							{
								newhot->func.function = getcopy(hotkey.func.function, -1, NULL);
								hot = firsthotkey;
								while(hot)
								{
									if(!hot->next)
										break;
									hot = hot->next;
								}
								if(hot)
									hot->next = newhot;
								else
									firsthotkey = newhot;
								newhot->next = NULL;
							}
						}
						freefunction(&hotkey.func);
						makehotkeylist();
						break;
					case HOTKEYS_DELETE:
						mode = HOTKEYS_DELETE;
						dogadgetinfo(cfg_string[STR_SELECT_HOTKEY_TO_DELETE]);
						break;
					case HOTKEYS_DUPLICATE:
						mode = HOTKEYS_DUPLICATE;
						dogadgetinfo(cfg_string[STR_SELECT_HOTKEY_TO_DUPLICATE]);
						break;
					case HOTKEYS_SWAP:
						mode = HOTKEYS_SWAP;
						dogadgetinfo(cfg_string[STR_SELECT_HOTKEY_TO_SWAP]);
						break;
					}
					break;
				}
			}
			else if(view)
			{
				if(view->itemselected > -1)
				{
					hot = firsthotkey;
					for(a = 0; a < view->itemselected; a++, hot = hot->next)
					{
						if(!hot)
							break;
						newhot = hot;
					}
					if(hot)
					{
						switch (mode)
						{
						case -1:
							view->itemselected = -1;
							doedithotkey(hot);
							makehotkeylist();
							break;
						case HOTKEYS_DELETE:
							if(request(cfg_string[STR_REALLY_DELETE_HOTKEY]))
							{
								dogadgetinfo(cfg_string[STR_HOTKEY_DELETED]);
								if(hot == firsthotkey)
									firsthotkey = firsthotkey->next;
								else
									newhot->next = hot->next;
								freefunction(&hot->func);
								IExec->FreeVec(hot);
							}
							else
								dogadgetinfo(cfg_string[STR_SELECT_HOTKEY_TO_DELETE]);
							makehotkeylist();
							break;
						case HOTKEYS_DUPLICATE:
							IExec->CopyMem((char *)hot, (char *)&hotkey, sizeof(struct dopushotkey));
							hotkey.func.function = getcopy(hot->func.function, -1, NULL);
							mode = -1;
							goto editnewhotkey;
						case HOTKEYS_SWAP:
							if(swapfirst)
							{
								if(swapfirst != hot)
									SwapMem((char *)&swapfirst->code, (char *)&hot->code, sizeof(struct dopushotkey) - sizeof(struct dopushotkey *));
								view->itemselected = -1;
								makehotkeylist();
								dogadgetinfo(cfg_string[STR_SELECT_HOTKEY_TO_SWAP]);
								swapfirst = NULL;
							}
							else
							{
								dogadgetinfo(cfg_string[STR_SELECT_SECOND_HOTKEY_TO_SWAP]);
								swapfirst = hot;
							}
							break;
						}
					}
				}
			}
		}
	}
}

void makehotkeylist()
{
	struct dopushotkey *hotkey;
	char *buf, str[40], name[40];
	int a, count, hotkeysize = 80;

	IDOpus->LFreeRemember(&hkey);
	hotkeylist = NULL;
	hotkey = firsthotkey;
	for(count = 0;; count++)
	{
		if(!hotkey)
			break;
		hotkey = hotkey->next;
	}
	if(count && (hotkeylist = IDOpus->LAllocRemember(&hkey, (count + 1) * 4, MEMF_CLEAR)) && (buf = IDOpus->LAllocRemember(&hkey, count * hotkeysize, MEMF_CLEAR)))
	{
		hotkey = firsthotkey;
		for(a = 0; a < count; a++)
		{
			hotkeylist[a] = &buf[a * hotkeysize];
			IDOpus->RawkeyToStr(hotkey->code, hotkey->qualifier, str, NULL, 39);
			str[39] = 0;
			strlcpy(name, hotkey->name, sizeof(name));
			name[36] = 0;
			snprintf(hotkeylist[a], hotkeysize, "%-36s %s", name, str);
			hotkey = hotkey->next;
		}
	}
	else
		hotkeylist = NULL;
	hotkeyslist.items = hotkeylist;
	hotkeyslist.itemselected = -1;
	IDOpus->RefreshListView(&hotkeyslist, 1);
}

int doedithotkey(struct dopushotkey *hotkey)
{
	int a;

	IDOpus->RemoveListView(&hotkeyslist, 1);
	cleanconfigscreen();
	doscreentitle(cfg_string[STR_HOTKEY_EDIT_SCREEN]);
	a = editfunction(&hotkey->func, CFG_HOTKEYS, hotkey);
	showconfigscreen(CFG_HOTKEYS);
	return (a);
}

void drawcornerimage(struct RastPort *r, int x1, int y1, int bc, int tc)
{
	IGraphics->SetAPen(r, screen_pens[0].pen);
	IGraphics->RectFill(r, x1 - 5, y1 - 1, x1, y1 + 3);
	IGraphics->SetDrMd(r, JAM1);
	IGraphics->SetAPen(r, screen_pens[bc].pen);
	IGraphics->BltTemplate((CONST PLANEPTR)pageflip_data1, 0, 2, r, x1 - 6, y1 - 1, 6, 5);
	IGraphics->SetAPen(r, screen_pens[tc].pen);
	IGraphics->BltTemplate((CONST PLANEPTR)pageflip_data2, 0, 2, r, x1 - 6, y1, 6, 3);
	IGraphics->SetDrMd(r, JAM2);
}

int fixrmbimage(struct RMBGadget *rmb, struct Gadget *gad, int on, int x, int y)
{
	int op, ret = 0, a;
	struct RMBGadget *rmbgad;

	op = rp->FgPen;
	rmbgad = rmb;
	while(rmbgad)
	{
		rmbgad->high_bord[0] = gadget_unsel_border;
		rmbgad->high_bord[1] = gadget_sel_border;
		rmbgad->flags |= RGF_ALTBORD;
		if(rmbgad->flags & RGF_ALTTEXT)
		{
			for(a = 0; a < 2; a++)
			{
				rmbgad->txt[a].fg = screen_pens[1].pen;
				rmbgad->txt[a].bg = screen_pens[0].pen;
			}
		}
		if(gad && on == 2)
		{
			rmbgad->x = gad->LeftEdge;
			rmbgad->y = gad->TopEdge;
		}
		if(x == -1 || (x >= rmbgad->x && x < rmbgad->x + rmbgad->w && y >= rmbgad->y && y < rmbgad->y + rmbgad->h))
		{
			if(on)
				drawcornerimage(rp, rmbgad->x + rmbgad->w, rmbgad->y + 1, config->gadgetbotcol, config->gadgettopcol);
			else
				do3dbox(rmbgad->x + 2, rmbgad->y + 1, rmbgad->w - 4, rmbgad->h - 2);
			ret = 1;
		}
		rmbgad = rmbgad->next;
		if(gad)
		{
			gad->GadgetRender = gadget_dog_unsel_border;
			gad->SelectRender = gadget_dog_sel_border;
			gad->Flags |= GFLG_GADGHIMAGE;
			gad->Flags &= ~GFLG_GADGHCOMP;
			if(rmbgad)
			{
				gad = gad->NextGadget;
				while(gad)
				{
					if(gad->LeftEdge == rmbgad->x && gad->TopEdge == rmbgad->y)
						break;
					gad = gad->NextGadget;
				}
			}
		}
	}
	IGraphics->SetAPen(rp, op);
	return (ret);
}

void doradiobuttons()
{
	int a;

	IDOpus->LFreeRemember(&buttonkey);

	buttonoffimage = IDOpus->GetButtonImage(15, 9, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen, screen_pens[0].pen, screen_pens[0].pen, &buttonkey);

	buttononimage = IDOpus->GetButtonImage(15, 9, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen, screen_pens[3].pen, screen_pens[0].pen, &buttonkey);

	if((checkoffimage = IDOpus->GetCheckImage(screen_pens[1].pen, screen_pens[0].pen, 0, &buttonkey)))
		IExec->CopyMem((char *)checkoffimage, (char *)&copy_checkoffimage, sizeof(struct Image));
	else
		copy_checkoffimage.ImageData = NULL;

	if((checkonimage = IDOpus->GetCheckImage(screen_pens[1].pen, screen_pens[0].pen, 1, &buttonkey)))
		IExec->CopyMem((char *)checkonimage, (char *)&copy_checkonimage, sizeof(struct Image));
	else
		copy_checkonimage.ImageData = NULL;

	stringex.Font = tfont;
	stringex.Pens[0] = screen_pens[config->stringfgcol].pen;
	stringex.Pens[1] = screen_pens[config->stringbgcol].pen;
	stringex.ActivePens[0] = screen_pens[config->stringselfgcol].pen;
	stringex.ActivePens[1] = screen_pens[config->stringselbgcol].pen;

	IDOpus->CreateGadgetBorders(&buttonkey, 104, 12, &gadget_dog_sel_border, &gadget_dog_unsel_border, 1, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);

	IDOpus->CreateGadgetBorders(&buttonkey, 104, 12, &gadget_sel_border, &gadget_unsel_border, 0, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);

	fix_gadget_border(&cmdcancelgad);
	fix_gadget_border(&helpcancelgad);
	for(a = 0; a < 3; a++)
		fix_gadget_border(&listokaygad[a]);
}

void fix_gadget_border(struct Gadget *gad)
{
	gad->Flags = (gadget_sel_border) ? GFLG_GADGHIMAGE : GFLG_GADGHCOMP;
	gad->GadgetRender = gadget_unsel_border;
	gad->SelectRender = gadget_sel_border;
}

void select_gadget(struct Gadget *gad, int state)
{
	if((state && !gad->SelectRender) || (!state && !gad->GadgetRender))
		IDOpus->HiliteGad(gad, rp);
	else
	{
		if(state)
			IIntuition->DrawBorder(rp, gad->SelectRender, gad->LeftEdge, gad->TopEdge);
		else
			IIntuition->DrawBorder(rp, gad->GadgetRender, gad->LeftEdge, gad->TopEdge);
	}
	if(gad->Activation & GACT_TOGGLESELECT)
	{
		if(state)
			gad->Flags |= GFLG_SELECTED;
		else
			gad->Flags &= ~GFLG_SELECTED;
	}
}

void fix_gadget_positions()
{
	int a;

	for(a = 0; a < 3; a++)
		fix_gad_pos(&menuslidergads[a], menuslidergads_xy[a]);
	for(a = 0; a < 4; a++)
	{
		fix_gad_pos(&editdrivegadgets[a], editdrivegadgets_xy[a]);
		fix_gad_pos(&icongads[a], icongads_xy[a]);
	}
	for(a = 0; a < 6; a++)
		fix_gad_pos(&palettegads[a], palettegads_xy[a]);
	for(a = 0; a < 7; a++)
		fix_gad_pos(&screenmodegads[a], screenmodegads_xy[a]);
	for(a = 0; a < 8; a++)
		fix_gad_pos(&arrowgadgets[a], arrowgadgets_xy[a]);
	for(a = 0; a < 12; a++)
		fix_gad_pos(&formatgads[a], formatgads_xy[a]);
	for(a = 0; a < 14; a++)
		fix_gad_pos(&editclassgadgets[a], editclassgadgets_xy[a]);
	for(a = 0; a < 15; a++)
		fix_gad_pos(&editfuncgadgets[a], editfuncgadgets_xy[a]);
	fix_gad_pos(&coloursgad, coloursgad_xy);
	fix_gad_pos(&hotkeymmbgad, hotkeymmbgad_xy);
	fix_gad_pos(&hotkeygad, hotkeygad_xy);
	fix_gad_pos(&fontsizegadget, fontsizegadget_xy);
	for(a = 0; a < 2; a++)
		fix_gad_pos(&screen_sliders_gadgets[a], screen_sliders_gadgets_xy[a]);
}

void fix_gad_pos(struct Gadget *gad, short *pos)
{
	gad->LeftEdge = x_off + pos[0];
	gad->TopEdge = y_off + pos[1];
}
