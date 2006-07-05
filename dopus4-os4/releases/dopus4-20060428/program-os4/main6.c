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

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include "dopus.h"

void get_printdir_data(struct PrintDirData *pddata)
{
	char buf[256];
	int win;

	if(pddata->win < 0 || pddata->win > 1)
		win = data_active_window;
	else
		win = pddata->win;

	pddata->namelen = config->displaylength[win][DISPLAY_NAME] / scrdata_font_xsize;
	pddata->entry = dopus_curwin[win]->firstentry;
	pddata->titlebuf[0] = 0;
	if((dopus_curwin[win]->filesel + dopus_curwin[win]->dirsel) > 0)
		pddata->onlysel = 1;
	else
		pddata->onlysel = 0;

	if(pddata->entry == NULL)
		goto printdiskname;
	else
		switch (ENTRYTYPE(pddata->entry->type))
		{
		case ENTRY_DEVICE:
			strcpy(pddata->titlebuf, globstring[STR_DEVICE_LIST]);
			break;
		case ENTRY_CUSTOM:
			switch (pddata->entry->subtype)
			{
			case CUSTOMENTRY_DIRTREE:
				strcpy(pddata->titlebuf, globstring[STR_DIR_TREE]);
				strcat(pddata->titlebuf, "\n");
				goto printdiskname;
			default:
				strcpy(pddata->titlebuf, globstring[STR_CUSTOM_LIST]);
				break;
			}
			break;
		default:
		      printdiskname:
			strcat(pddata->titlebuf, globstring[STR_DIRECTORY]);
			if(expand_path(str_pathbuffer[win], buf))
				strcat(pddata->titlebuf, buf);
			else
				strcat(pddata->titlebuf, str_pathbuffer[win]);
			break;
		}
	strcat(pddata->titlebuf, "\n");
}

void startnotify(int win)
{
	if(!dos_notify_req[win])
		return;
	main_proc->pr_WindowPtr = (APTR) - 1;
	endnotify(win);
	if(config->dynamicflags & UPDATE_NOTIFY && str_pathbuffer[win][0])
	{
		if(dopus_curwin[win]->disktot > (2 * (1 << 20)))
		{
			strcpy(dos_notify_names[win], str_pathbuffer[win]);
			dos_notify_req[win]->nr_UserData = win;
			dos_notify_req[win]->nr_Flags = NRF_SEND_MESSAGE;
			dos_notify_req[win]->nr_stuff.nr_Msg.nr_Port = count_port;
			if(!(IDOS->StartNotify(dos_notify_req[win])))
				dos_notify_names[win][0] = 0;
		}
	}
	if(config->errorflags & ERROR_ENABLE_DOS)
		main_proc->pr_WindowPtr = (APTR) Window;
}

void endnotify(int win)
{
	if(dos_notify_req[win] && dos_notify_names[win][0])
	{
		IDOS->EndNotify(dos_notify_req[win]);
		dos_notify_names[win][0] = 0;
	}
}

void startnotifies()
{
	int a;

	for(a = 0; a < 2; a++)
	{
		if(IUtility->Stricmp(dos_notify_names[a], str_pathbuffer[a]))
			startnotify(a);
	}
}

void endnotifies()
{
	int a;

	for(a = 0; a < 2; a++)
		endnotify(a);
}

int dormbgadget(int x1, int y1, struct newdopusfunction *gad1, struct newdopusfunction *gad2)
{
	struct RMBGadget rmbgad;
	int a;

	if(config->generalscreenflags & SCR_GENERAL_INDICATERMB)
		do3dbox(main_rp, x1 + 2, y1 + 1, scrdata_gadget_width - 4, scrdata_gadget_height - 2);
	rmbgad.flags = RGF_ALTTEXT;
	rmbgad.id = 0;

	if(gadget_unsel_border)
	{
		rmbgad.flags |= RGF_ALTBORD;
		rmbgad.high_bord[0] = gadget_unsel_border;
		rmbgad.high_bord[1] = gadget_sel_border;
	}
	for(a = 0; a < 2; a++)
	{
		rmbgad.txt[a].x = 2;
		rmbgad.txt[a].y = 1;
		rmbgad.txt[a].w = scrdata_gadget_width - 4;
		rmbgad.txt[a].h = scrdata_gadget_height - 2;
	}
	rmbgad.x = x1;
	rmbgad.y = y1;
	rmbgad.w = scrdata_gadget_width;
	rmbgad.h = scrdata_gadget_height;
	rmbgad.txt[0].fg = screen_pens[gad1->fpen].pen;
	rmbgad.txt[0].bg = screen_pens[gad1->bpen].pen;
	rmbgad.txt[0].text = gad1->name;
	rmbgad.txt[1].fg = screen_pens[gad2->fpen].pen;
	rmbgad.txt[1].bg = screen_pens[gad2->bpen].pen;
	rmbgad.txt[1].text = gad2->name;
	if(status_iconified < 1)
		IGraphics->SetFont(main_rp, scr_font[FONT_GADGETS]);
	a = IDOpus->DoRMBGadget(&rmbgad, Window);
	if(status_iconified < 1)
		IGraphics->SetFont(main_rp, scr_font[FONT_GENERAL]);
	if(config->generalscreenflags & SCR_GENERAL_INDICATERMB)
		drawgadgetcorner(main_rp, x1 + 2, y1 + 1);
	return (a);
}

int gadgetfrompos(int x, int y)
{
	int xg, yg, gad;

	if(y < scrdata_gadget_ypos - 1 || x < scrdata_gadget_xpos)
		return (-1);
	x -= scrdata_gadget_xpos;
	y -= (scrdata_gadget_ypos - 1);
	xg = x / scrdata_gadget_width;
	yg = y / scrdata_gadget_height;
	gad = (yg * 7) + xg;
	if(gad < 0 || gad >= (scr_gadget_rows * 7))
		return (-1);
	xg = (gad % 7) * scrdata_gadget_width;
	yg = (gad / 7) * scrdata_gadget_height;
	if(x >= xg && x < xg + scrdata_gadget_width && y >= yg && y < yg + scrdata_gadget_height)
		return (gad);
	return (-1);
}

int isvalidgad(struct newdopusfunction *gad)
{
	return ((gad->name && gad->name[0] && gad->function && gad->function[0]));
}

int getpal()
{
	int p;
	struct Screen *screen;
	ULONG modeid = LORES_KEY;
	struct DisplayInfo displayinfo;

	p = (((struct GfxBase *)(IGraphics->Data.LibBase))->DisplayFlags & PAL) ? 1 : 0;
	if((screen = IIntuition->LockPubScreen(NULL)))
	{
		if((modeid = IGraphics->GetVPModeID(&(screen->ViewPort))) != INVALID_ID)
		{
			if(!((modeid & MONITOR_ID_MASK) == NTSC_MONITOR_ID || (modeid & MONITOR_ID_MASK) == PAL_MONITOR_ID))
				modeid = LORES_KEY;
		}
		IIntuition->UnlockPubScreen(NULL, screen);
	}
	if(IGraphics->GetDisplayInfoData(NULL, (UBYTE *) & displayinfo, sizeof(struct DisplayInfo), DTAG_DISP, modeid))
	{
		if(displayinfo.PropertyFlags & DIPF_IS_PAL)
			p = 1;
		else
			p = 0;
	}
	return (p);
}

void sendmouseevent(UBYTE class, UWORD code)
{
	struct InputEvent inputev;

	inputev.ie_NextEvent = NULL;
	inputev.ie_Class = class;
	inputev.ie_Code = code;
	inputev.ie_Qualifier = 0;
	inputev.ie_X = 0;
	inputev.ie_Y = 0;

	if(CxBase && ICommodities)
	{
		ICommodities->AddIEvents(&inputev);
	}
	else
	{
		input_req->io_Command = IND_WRITEEVENT;
		input_req->io_Flags = 0;
		input_req->io_Length = sizeof(struct InputEvent);
		input_req->io_Data = (APTR) & inputev;
		IExec->DoIO((struct IORequest *)input_req);
	}
}

void quickfixmenus()
{
	int a;
	ULONG class;
	USHORT code;

	if(Window->MenuStrip)
	{
		IExec->Forbid();
		a = 0;
		while(getintuimsg())
		{
			class = IMsg->Class;
			code = IMsg->Code;
			IExec->ReplyMsg((struct Message *)IMsg);
			if(class == IDCMP_MOUSEBUTTONS && code == MENUUP)
			{
				a = 1;
				break;
			}
		}
		if(!a)
		{
//			D(bug("Simulating RMB click\n"));
			Window->Flags &= ~WFLG_RMBTRAP;
			sendmouseevent(IECLASS_RAWMOUSE, IECODE_RBUTTON | IECODE_UP_PREFIX /*,0,0 */ );
			sendmouseevent(IECLASS_RAWMOUSE, IECODE_RBUTTON /*,0,0 */ );
		}
		IExec->Permit();
	}
}

char *getfiledescription(STRPTR name, int win)
{
	int a;
	char buf[256];
	struct dopusfiletype *type;

	for(a = 0; a < DISPLAY_LAST + 1; a++)
	{
		if(config->displaypos[win][a] == DISPLAY_FILETYPE)
			break;
	}
	if(a > DISPLAY_LAST || !name[0])
		return (NULL);
	IDOpus->StrCombine(buf, str_pathbuffer[win], name, 256);
	if(!(type = checkfiletype(buf, -1, 0)))
		return (NULL);
	if((strcmp(type->type, "Default") == 0) || (strcmp(type->type, globstring[STR_FTYPE_DEFAULT]) == 0))
		return (globstring[STR_FTYPE_UNKNOWN]);
	return (type->type);
}

void fixhlen(int win)
{
	int a;

	if(dopus_curwin[win]->total > 0 && (dopus_curwin[win]->firstentry->type == ENTRY_CUSTOM || dopus_curwin[win]->firstentry->type == ENTRY_DEVICE))
	{
		if(dopus_curwin[win]->firstentry->type == ENTRY_DEVICE)
			dopus_curwin[win]->hlen = scrdata_dispwin_nchars[win];
	}
	else
	{
		dopus_curwin[win]->hlen = 0;
		if(dopus_curwin[win]->total > 0 || str_pathbuffer[win][0])
		{
			for(a = 0; a <= DISPLAY_LAST; a++)
			{
				if(config->displaypos[win][a] == DISPLAY_NAME)
					dopus_curwin[win]->hlen += config->displaylength[win][DISPLAY_NAME];
				else if(config->displaypos[win][a] == DISPLAY_SIZE)
					dopus_curwin[win]->hlen += config->displaylength[win][DISPLAY_SIZE];
				else if(config->displaypos[win][a] == DISPLAY_PROTECT)
					dopus_curwin[win]->hlen += config->displaylength[win][DISPLAY_PROTECT] + scrdata_font_xsize;
				else if(config->displaypos[win][a] == DISPLAY_DATE)
				{
					dopus_curwin[win]->hlen += config->displaylength[win][DISPLAY_DATE] + scrdata_font_xsize;
				}
				else if(config->displaypos[win][a] == DISPLAY_COMMENT)
					dopus_curwin[win]->hlen += config->displaylength[win][DISPLAY_COMMENT];
				else if(config->displaypos[win][a] == DISPLAY_FILETYPE)
					dopus_curwin[win]->hlen += config->displaylength[win][DISPLAY_FILETYPE];
			}
		}
		if(dopus_curwin[win]->hlen < scrdata_dispwin_nchars[win])
			dopus_curwin[win]->hlen = scrdata_dispwin_nchars[win];
		if(dopus_curwin[win]->hoffset >= (dopus_curwin[win]->hlen - scrdata_dispwin_nchars[win]))
			dopus_curwin[win]->hoffset = dopus_curwin[win]->hlen - scrdata_dispwin_nchars[win];
	}
}


/* Get the name of the screen we are currently on */
char *get_our_pubscreen()
{
	char *name = NULL;

	if(Window)
	{
		struct List *pubscreenlist;
		struct PubScreenNode *node;

		if((pubscreenlist = IIntuition->LockPubScreenList()))
		{
			for(node = (struct PubScreenNode *)pubscreenlist->lh_Head; node->psn_Node.ln_Succ; node = (struct PubScreenNode *)node->psn_Node.ln_Succ)
			{
				/* See if this node is our screen */
				if(node->psn_Screen == Window->WScreen)
				{
					/* Get name pointer */
					name = node->psn_Node.ln_Name;
					break;
				}
			}
			IIntuition->UnlockPubScreenList();
		}

		/* Otherwise use default title */
		if(!name)
		{
			name = Window->WScreen->DefaultTitle;
		}
	}

	/* If no window open, use port name */
	else
	{
		name = str_arexx_portname;
	}

	return (name);
}

/* Change name of arexx port */

void change_port_name(STRPTR name)
{
	IExec->Forbid();
	IExec->RemPort(arexx_port);
	strncpy(str_arexx_portname, name, 29);
	str_arexx_portname[29] = 0;
	IExec->AddPort(arexx_port);
	IExec->Permit();
}
