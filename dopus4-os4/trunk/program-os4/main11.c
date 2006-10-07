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

#include "dopus.h"

#define BUTPROP 1
#define BUTEXIT 2

extern USHORT uniconifydata[2][20];

static struct AppIcon *appicon;
static struct AppMenuItem *appmenu;
static struct Image iconbutpropimage;
static struct PropInfo iconbutpropinfo =
{
	AUTOKNOB | FREEVERT | PROPNEWLOOK | PROPBORDERLESS, 0, 0, 0, 0x3000
};

struct Image uniconifyimage[2] =
{
	{ 0, 0, 11, 10, 2, uniconifydata[0], 3, 0, NULL },
	{ 0, 0, 11, 10, 2, uniconifydata[1], 3, 0, NULL }
};

struct Gadget dummybordergad =
{
	NULL, -18, 0, 17, 10, GFLG_GADGHNONE | GFLG_RELRIGHT, GACT_RIGHTBORDER, GTYP_BOOLGADGET, NULL, NULL, NULL, 0, NULL, 0, NULL
}
, iconbutpropgad =
{
	&dummybordergad, -14, 0, 11, 0, GFLG_GADGHNONE | GFLG_RELRIGHT, GACT_IMMEDIATE | GACT_RELVERIFY | GACT_FOLLOWMOUSE | GACT_RIGHTBORDER, GTYP_PROPGADGET, (APTR) & iconbutpropimage, NULL, NULL, 0, (APTR) & iconbutpropinfo, BUTPROP, NULL
}
, iconbutexitgad =
{
	&iconbutpropgad, 0, 0, 11, 10, GFLG_GADGIMAGE | GFLG_GADGHIMAGE | GFLG_RELRIGHT, GACT_RELVERIFY | GACT_RIGHTBORDER, GTYP_BOOLGADGET, (APTR) & uniconifyimage[0], (APTR) & uniconifyimage[1], NULL, 0, NULL, BUTEXIT, NULL
};

struct NewWindow icon_win =
{
	100, 0, 433, 10, 255, 255, ICON_IDCMP, WFLG_CLOSEGADGET | WFLG_RMBTRAP | WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_NEWLOOKMENUS, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, WBENCHSCREEN
}
, appicon_win =
{
	0, 0, 1, 1, 255, 255, ICON_IDCMP, WFLG_BACKDROP | WFLG_BORDERLESS | WFLG_SIMPLE_REFRESH | WFLG_NOCAREREFRESH, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, WBENCHSCREEN
};

static int icon_ty, icon_len;
static UBYTE icon_fpen, icon_bpen;
static int icon_type, icon_gotclock;
static struct TextFont *winfont;
static char icontitletext[200];
static struct DOpusRemember *icon_key;
static struct MsgPort *iconify_time_port;
static struct timerequest iconify_timereq;

static struct DiskObject iconify_appicon =
{
	0, 0, { NULL, 0, 0, 80, 41, GADGBACKFILL, 0, 0, (APTR)&appicon_image, NULL, NULL, 0, NULL, 0, NULL }, 0, NULL, NULL, NO_ICON_POSITION, NO_ICON_POSITION, NULL, NULL, 0
};

void iconify(int louise, int buttons, int banknum)
{
	ULONG class;
	USHORT code, gadgetid = 0;
	struct DateTime dt;
	struct Screen scrbuf, *sptr;
	struct DrawInfo *drinfo;
	int wmes, chipc, fast, h, m, s, waitbits, a = 0, b, nheight, nwidth, buttonrows = 0, oldrows, olddata_gadgetrow_offset, x, y, x1, y1, c, d, w, fastnum = 0, chipnum = 0, bankcount = 0, bankstep = 0, menunum, itemnum, num, cdelay, usage, oldusage = 100;
	char date[16], time[16], buf[50], buf1[50], buf2[50], /*ampm, */formstring[100], *old;
	struct dopusgadgetbanks *bank, *oldbank, **bankarray = { NULL, };
	struct dopushotkey *hotkey;
	struct dopusfuncpar par;
	struct AppMessage *amsg;
	struct MenuItem *item;

	endnotifies();
	shutthingsdown(0);

	status_iconified = 1;
	status_flags &= ~STATUS_ISINBUTTONS;
	cdelay = 0;
	IDOpus->GetWBScreen(&scrbuf);
	nwidth = scrbuf.Width;
	nheight = scrbuf.Height;
	winfont = scrbuf.RastPort.Font;
	appicon = NULL;
	icon_key = NULL;
	icon_type = config->icontype;
	icon_gotclock = 0;

	if((sptr = IIntuition->LockPubScreen(NULL)))
	{
		drinfo = IIntuition->GetScreenDrawInfo(sptr);
		icon_fpen = drinfo->dri_Pens[FILLTEXTPEN];
		icon_bpen = drinfo->dri_Pens[FILLPEN];
		IIntuition->FreeScreenDrawInfo(sptr, drinfo);
		IIntuition->UnlockPubScreen(NULL, sptr);
	}
	else
	{
		icon_fpen = 1;
		icon_bpen = 0;
	}

	oldrows = config->gadgetrows;
	oldbank = dopus_curgadbank;
	olddata_gadgetrow_offset = data_gadgetrow_offset;

	if(buttons)
	{
		bank = dopus_firstgadbank;
		bankcount = 0;
		while(bank)
		{
			++bankcount;
			bank = bank->next;
		}
		if((bankarray = IDOpus->LAllocRemember(&icon_key, bankcount * 4, MEMF_CLEAR)))
		{
			bank = dopus_firstgadbank;
			for(a = 0; a < bankcount; a++)
			{
				bankarray[a] = bank;
				bank = bank->next;
			}
		}
		else
		{
			bankcount = 0;
		}
		icon_win.LeftEdge = config->iconbutx;
		icon_win.TopEdge = config->iconbuty;
		icon_type &= ~(ICON_NOWINDOW | ICON_APPICON);
		if(buttons > 1)
		{
			buttonrows = buttons - 1;
			if(buttonrows != 2 && buttonrows != 3 && buttonrows != 6)
				buttonrows = config->gadgetrows;
		}
		else
		{
			buttonrows = config->gadgetrows;
		}
		if(buttonrows < 2)
		{
			buttonrows = 2;
		}
		config->gadgetrows = buttonrows;
		scr_gadget_rows = buttonrows;
		bankstep = 6 / buttonrows;
	}
	else
	{
		icon_win.LeftEdge = config->iconx;
		icon_win.TopEdge = config->icony;

		if((icon_type & ICON_APPICON) && !docky)
		{
			if(!IWorkbench || !(appmenu = IWorkbench->AddAppMenuItemA(0, 0, str_arexx_portname, appmsg_port, 0)) || !(appicon = IWorkbench->AddAppIconA(0, 0, "Directory Opus", appmsg_port, 0, user_appicon == NULL ? &iconify_appicon : user_appicon, NULL)))
			{
				icon_type &= ~(ICON_APPICON);
				if(appicon)
				{
					IWorkbench->RemoveAppIcon(appicon);
					appicon = NULL;
				}
				if(appmenu)
				{
					IWorkbench->RemoveAppMenuItem(appmenu);
					appmenu = NULL;
				}
			}
		}
		else if(docky)
		{
			IApplication->SetApplicationAttrs(appID, REGAPP_Hidden, TRUE, TAG_DONE);
		}
	}

	if(!(icon_type & (ICON_NOWINDOW | ICON_APPICON)))
	{
		icon_win.Title = (STRPTR) str_space_string;
		if(!(icon_type & (ICON_MEMORY | ICON_DATE | ICON_TIME | ICON_CPU)))
		{
			if(buttons)
				strcpy(buf, "WW");
			else
				strcpy(buf, "DirOpusWW");
		}
		else
		{
			strcpy(buf, "W");
			icon_gotclock = 1;
			if(icon_type & ICON_MEMORY)
			{
				chipnum = getmaxmem(MEMF_CHIP);
				fastnum = getmaxmem(MEMF_FAST);
				if(icon_type & ICON_BYTES)
				{
					chipnum += 3;
					fastnum += 3;
				}
				if(icon_type & ICON_C_AND_F)
				{
					if(fastnum < 2)
						sprintf(buf2, "%lc:", globstring[STR_CLOCK_MEM][0]);
					else
						sprintf(buf2, "%lc:", globstring[STR_CLOCK_CHIP][0]);
					strcat(buf, buf2);
				}
				else
				{
					if(fastnum < 2)
						strcat(buf, globstring[STR_CLOCK_MEM]);
					else
						strcat(buf, globstring[STR_CLOCK_CHIP]);
				}
				for(a = 0; a < chipnum; a++)
					buf1[a] = '8';
				buf1[a] = 0;
				strcat(buf, buf1);
				if(fastnum > 1)
				{
					if(icon_type & ICON_C_AND_F)
						sprintf(buf2, " %lc:", globstring[STR_CLOCK_FAST][0]);
					else
						sprintf(buf2, " %s", globstring[STR_CLOCK_FAST]);
					strcat(buf, buf2);
					for(a = 0; a < fastnum; a++)
						buf1[a] = '8';
					buf1[a] = 0;
					strcat(buf, buf1);
				}
				strcat(buf, " ");
			}
			if(icon_type & ICON_CPU)
				strcat(buf, "CPU:100% ");
			if(icon_type & ICON_DATE)
				strcat(buf, "88-WWW-88 ");
			if(icon_type & ICON_TIME)
			{
				strcat(buf, "88:88:88");
				if(config->dateformat & DATE_12HOUR)
					strcat(buf, "P");
				strcat(buf, " ");
			}
			if((icon_len = strlen(buf)) && buf[icon_len - 1] != ' ')
				strcat(buf, " ");

			strcat(buf, "W");

		}
		icon_len = 0;

		icon_win.Height = scrbuf.WBorTop + winfont->tf_YSize + (1);
		icon_win.Width = dotextlength(&scrbuf.RastPort, buf, &icon_len, 0);
		icon_ty = ((icon_win.Height - winfont->tf_YSize) / 2) + winfont->tf_Baseline;
		if(buttons)
		{
			icon_win.Height += buttonrows * (scr_font[FONT_ICONIFY]->tf_YSize + 2) + scrbuf.WBorBottom;
			icon_win.Width = scrbuf.Width;
			iconbutexitgad.LeftEdge = -14;

			iconbutexitgad.TopEdge = scrbuf.WBorTop + winfont->tf_YSize + 3;
			dummybordergad.TopEdge = iconbutexitgad.TopEdge;
			iconbutpropgad.TopEdge = iconbutexitgad.TopEdge + 12;
			iconbutpropgad.Height = icon_win.Height - iconbutpropgad.TopEdge - scrbuf.WBorBottom - 1;
			dummybordergad.NextGadget = NULL;

			icon_win.FirstGadget = &iconbutexitgad;
		}
		else
		{
			icon_win.Width += 50;
			icon_win.FirstGadget = NULL;
		}

		if(icon_win.LeftEdge + icon_win.Width > nwidth)
			icon_win.LeftEdge = nwidth - icon_win.Width;
		if(icon_win.TopEdge + icon_win.Height > nheight)
			icon_win.TopEdge = nheight - icon_win.Height - 1;
		if(icon_win.LeftEdge < 0)
			icon_win.LeftEdge = 0;
		if(icon_win.TopEdge < 0)
			icon_win.TopEdge = 0;

		if(!(Window = (struct Window *)IIntuition->OpenWindow(&icon_win)))
			goto uniconify;
		main_rp = Window->RPort;

		if(IWorkbench)
			dopus_appwindow = IWorkbench->AddAppWindowA(APPWINID, 0, Window, appmsg_port, NULL);

		if(buttons)
		{
			get_colour_table();
			IDOpus->FixSliderBody(Window, &iconbutpropgad, (bankcount * 6) / buttonrows, 1, 1);
			IGraphics->SetFont(main_rp, scr_font[FONT_ICONIFY]);
			scrdata_drive_width = 0;
			data_gadgetrow_offset = 0;
			if((dopus_curgadbank = dopus_firstgadbank))
			{
				for(a = 0; a < banknum; a++)
				{
					if(!dopus_curgadbank->next)
						break;
					dopus_curgadbank = dopus_curgadbank->next;
				}
			}
			IDOpus->FixSliderPot(Window, &iconbutpropgad, a * bankstep, (bankcount * 6) / buttonrows, 1, 1);
			w = Window->Width - Window->BorderLeft - Window->BorderRight;
			scrdata_gadget_width = w / 7;
			scrdata_gadget_xpos = ((w - (scrdata_gadget_width * 7)) / 2) + Window->BorderLeft;
			if(scrdata_gadget_xpos < Window->BorderLeft)
				scrdata_gadget_xpos = Window->BorderLeft;
			scrdata_gadget_offset = 0;
			scrdata_gadget_height = scr_font[FONT_ICONIFY]->tf_YSize + 2;
			scrdata_gadget_ypos = Window->BorderTop + 1;
			drawgadgets(1, 0);
			layout_menus();
			status_flags |= STATUS_ISINBUTTONS;
		}
		else
			IGraphics->SetFont(main_rp, winfont);

		IGraphics->SetDrMd(main_rp, JAM2);
		if(icon_gotclock)
		{
			if((iconify_time_port = IExec->CreatePort(0, 0)))
			{
				IExec->OpenDevice(TIMERNAME, UNIT_VBLANK, &iconify_timereq.tr_node, 0);
				iconify_timereq.tr_node.io_Message.mn_ReplyPort = iconify_time_port;
				iconify_timereq.tr_node.io_Command = TR_ADDREQUEST;
				iconify_timereq.tr_node.io_Flags = 0;
				iconify_timereq.tr_time.tv_secs = 0;
				iconify_timereq.tr_time.tv_micro = 2;
				IExec->SendIO(&iconify_timereq.tr_node);
			}
			else
				icon_gotclock = 0;
		}
		else
		{
			if(buttons)
				IIntuition->SetWindowTitles(Window, globstring[STR_BUTTON_STRIP], (char *)-1);
			else
				IIntuition->SetWindowTitles(Window, "DirOpus", (char *)-1);
		}
	}
	else if(icon_type & ICON_APPICON)
	{
		Window = IIntuition->OpenWindowTags(&appicon_win, WA_PubScreenName, (Tag) "Workbench", WA_PubScreenFallBack, TRUE, TAG_END);
	}

	if(!(status_flags & STATUS_DONEREXX))
	{
		status_flags |= STATUS_DONEREXX;
		rexx_command(config->startupscript, NULL);
	}

	for(;;)
	{
		waitbits = 0;
		if(IWorkbench)
		{
			if(icon_type & ICON_APPICON || dopus_appwindow)
			{
				waitbits |= 1 << appmsg_port->mp_SigBit;
			}
		}
		if(IApplication && appID)
		{
			if(docky)
			{
				waitbits |= 1 << applibport->mp_SigBit;
			}
		}
		if(icon_gotclock)
		{
			formstring[0] = 0;
			if(icon_type & ICON_MEMORY)
			{
				chipc = IExec->AvailMem(MEMF_CHIP);
				fast = IExec->AvailMem(MEMF_ANY) - chipc;
				if(!(icon_type & ICON_BYTES))
				{
					chipc /= 1024;
					fast /= 1024;
				}
				if(fastnum > 1)
				{
					if(icon_type & ICON_C_AND_F)
					{
						sprintf(buf1, "%lc:%%%dld %lc:%%%dld ", globstring[STR_CLOCK_CHIP][0], chipnum, globstring[STR_CLOCK_FAST][0], fastnum);
					}
					else
					{
						sprintf(buf1, "%s%%%dld %s%%%dld ", globstring[STR_CLOCK_CHIP], chipnum, globstring[STR_CLOCK_FAST], fastnum);
					}
					sprintf(buf, buf1, chipc, fast);
				}
				else
				{
					if(icon_type & ICON_C_AND_F)
					{
						sprintf(buf1, "%lc:%%%dld ", globstring[STR_CLOCK_MEM][0], chipnum);
					}
					else
					{
						sprintf(buf1, "%s%%%dld ", globstring[STR_CLOCK_MEM], chipnum);
					}
					sprintf(buf, buf1, chipc);
				}
				strcat(formstring, buf);
			}
			if(icon_type & ICON_CPU)
			{
				usage = getusage();

				if(!usage)
					usage = oldusage;
				sprintf(buf, "CPU:%3d%% ", usage);
				strcat(formstring, buf);
				oldusage = usage;
			}
			IDOS->DateStamp(&(dt.dat_Stamp));
			initdatetime(&dt, date, time, 0);
			if(icon_type & ICON_DATE)
			{
				strcat(formstring, date);
				strcat(formstring, " ");
			}
			if(icon_type & ICON_TIME)
			{
				if(config->dateformat & DATE_12HOUR)
				{
					h = dt.dat_Stamp.ds_Minute / 60;
					m = dt.dat_Stamp.ds_Minute % 60;
					s = dt.dat_Stamp.ds_Tick / TICKS_PER_SECOND;
					sprintf(time, "%02d:%02d:%02d", h, m, s);
				}
				strcat(formstring, time);
				strcat(formstring, " ");
			}
			if(!cdelay)
			{
				formstring[icon_len] = 0;
				iconstatustext(formstring, buttons);
			}
			waitbits |= 1 << iconify_time_port->mp_SigBit;
		}
		if(Window)
			waitbits |= 1 << Window->UserPort->mp_SigBit;
		waitbits |= INPUTSIG_UNICONIFY | rexx_signalbit | INPUTSIG_HOTKEY;
		if((wmes = IExec->Wait(waitbits)) & INPUTSIG_HOTKEY)
		{
			if(dopus_globalhotkey == (struct dopushotkey *)-1)
			{
				cleanupiconify();
				status_iconified = 0;
				quit();
			}
			else if(dopus_globalhotkey == (struct dopushotkey *)-2)
			{
				dopus_globalhotkey = NULL;
				continue;
			}
			else if(dopus_globalhotkey)
			{
				hotkey = dopus_globalhotkey;
				dopus_globalhotkey = NULL;
				IExec->CopyMem((char *)&hotkey->func.which, (char *)&par, sizeof(struct dopusfuncpar));
				par.which &= FLAG_OUTWIND | FLAG_NOQUOTE | FLAG_SHELLUP | FLAG_DOPUSUP;
				par.which |= FLAG_ASYNC;
				if(par.which & FLAG_OUTWIND)
					par.which |= FLAG_WB2F;
				status_flags |= STATUS_FROMHOTKEY;
				dofunctionstring(hotkey->func.function, hotkey->name, NULL, &par);
				status_flags &= ~STATUS_FROMHOTKEY;
			}
		}

		if(IApplication && appID && (wmes & (1 << applibport->mp_SigBit)))
		{
			struct ApplicationMsg *applibmsg;
			while((applibmsg = (struct ApplicationMsg *)IExec->GetMsg(applibport)))
			{
				ULONG applibmsgtype = applibmsg->type;
				switch(applibmsgtype)
				{
				case APPLIBMT_Unhide:
					IApplication->SetApplicationAttrs(appID, REGAPP_Hidden, FALSE, TAG_DONE);
					IExec->ReplyMsg((struct Message *)applibmsg);
					goto deiconify;
					break;
				case APPLIBMT_ToFront:
					IApplication->SetApplicationAttrs(appID, REGAPP_Hidden, FALSE, TAG_DONE);
					IExec->ReplyMsg((struct Message *)applibmsg);
					goto deiconify;
					break;
				case APPLIBMT_OpenDoc:
					{
						char pathbuf[768], filebuf[256];
						struct ApplicationOpenPrintDocMsg *aopdm = (struct ApplicationOpenPrintDocMsg *)&applibmsg->msg;

						IUtility->Strlcpy(pathbuf, aopdm->fileName, 768);
						IUtility->Strlcpy(filebuf, IDOS->FilePart(aopdm->fileName), 256);
						*IDOS->PathPart(pathbuf) = 0;
						strcpy(func_external_file, aopdm->fileName);
						ftype_doubleclick(pathbuf, filebuf, 0);
						unbusy();
					}
					break;
				case APPLIBMT_Quit:
					IExec->ReplyMsg((struct Message *)applibmsg);
					cleanupiconify();
					status_iconified = 0;
					quit();
					break;
				}
				IExec->ReplyMsg((struct Message *)applibmsg);
			}
		}

		if(IWorkbench)
		{
			if((((icon_type & ICON_APPICON) && !docky) || dopus_appwindow) && wmes & 1 << appmsg_port->mp_SigBit)
			{
				if(dopus_appwindow)
				{
					IIntuition->ActivateWindow(Window);
				}
				while((amsg = (struct AppMessage *)IExec->GetMsg(appmsg_port)))
				{
					if((amsg->am_Type == AMTYPE_APPICON && amsg->am_NumArgs == 0) || (amsg->am_Type == AMTYPE_APPMENUITEM))
					{
						IExec->ReplyMsg((struct Message *)amsg);
						goto endiconify;
					}
					else if((amsg->am_Type == AMTYPE_APPICON && Window) || (amsg->am_Type == AMTYPE_APPWINDOW && amsg->am_ID == APPWINID))
					{
						for(a = 0; a < amsg->am_NumArgs; a++)
						{
							if(amsg->am_ArgList[a].wa_Lock)
							{
								if(status_flags & STATUS_ISINBUTTONS && amsg->am_MouseY > Window->BorderTop)
								{
									if(dopus_curgadbank && (b = gadgetfrompos(amsg->am_MouseX, amsg->am_MouseY)) != -1)
									{
										b += data_gadgetrow_offset * 7;
										if(isvalidgad(&dopus_curgadbank->gadgets[b]))
										{
											IDOS->NameFromLock(amsg->am_ArgList[a].wa_Lock, func_external_file, 256);
											if(func_external_file[0] && func_external_file[(strlen(func_external_file) - 1)] == ':' && !amsg->am_ArgList[a].wa_Name[0])
												IDOS->AddPart(func_external_file, "Disk.info", 256);
											else
												IDOS->AddPart(func_external_file, amsg->am_ArgList[a].wa_Name, 256);
											if(!(IDOpus->CheckExist(func_external_file, NULL)))
											{
												IDOpus->StrConcat(func_external_file, ".info", 256);
												if(!isicon(func_external_file))
													func_external_file[0] = 0;
											}
											dofunctionstring(dopus_curgadbank->gadgets[b].function, dopus_curgadbank->gadgets[b].name, NULL, (struct dopusfuncpar *)&dopus_curgadbank->gadgets[b].which);
											if(!status_iconified)
												break;
											cdelay = 3;
											IIntuition->ModifyIDCMP(Window, ICON_IDCMP);
										}
									}
								}
								else
								{
									if(amsg->am_ArgList[a].wa_Name[0])
									{
										char pathbuf[256];

										IDOS->NameFromLock(amsg->am_ArgList[a].wa_Lock, pathbuf, 256);
										strcpy(func_external_file, pathbuf);
										IDOS->AddPart(func_external_file, amsg->am_ArgList[a].wa_Name, 256);
										ftype_doubleclick(pathbuf, amsg->am_ArgList[a].wa_Name, 0);
										IIntuition->ModifyIDCMP(Window, ICON_IDCMP);
										unbusy();
									}
								}
							}
						}
						func_external_file[0] = 0;
					}
					IExec->ReplyMsg((struct Message *)amsg);
					if(!status_iconified)
						return;
				}
			}
		}
		if(wmes & rexx_signalbit)
		{
			func_global_function = 0;
			rexx_dispatch(0);
			if(func_global_function == FUNC_UNICONIFY)
				wmes = INPUTSIG_UNICONIFY;
			else if(func_global_function == FUNC_QUIT)
			{
				cleanupiconify();
				status_iconified = 0;
				quit();
			}
		}
		if(wmes & INPUTSIG_UNICONIFY)
		{
		      endiconify:
			status_iconified = -1;
			cleanupiconify();
			goto uniconify;
		}
		if(Window && wmes & 1 << Window->UserPort->mp_SigBit)
		{
			while(getintuimsg())
			{
				class = IMsg->Class;
				code = IMsg->Code;
				x = IMsg->MouseX;
				y = IMsg->MouseY;
				if(class == IDCMP_GADGETUP || class == IDCMP_GADGETDOWN)
					gadgetid = ((struct Gadget *)IMsg->IAddress)->GadgetID;
				IExec->ReplyMsg((struct Message *)IMsg);
				switch (class)
				{
				case IDCMP_MENUPICK:
					if(!buttons)
						break;
					Window->Flags |= WFLG_RMBTRAP;
					if(Window->MenuStrip && code != MENUNULL)
					{
						menunum = menu_real_number[MENUNUM(code)];
						num = ITEMNUM(code);
						item = menu_menus[menunum].FirstItem;
						for(a = 0, itemnum = 0; a < num && item; a++)
						{
							if(item->Flags & ITEMTEXT)
								++itemnum;
							item = item->NextItem;
						}
						if(item)
						{
							a = (menunum * 20) + itemnum;
							if(!isvalidgad(&config->menu[a]))
								break;
							dofunctionstring(config->menu[a].function, config->menu[a].name, NULL, (struct dopusfuncpar *)&config->menu[a].which);
							if(!status_iconified)
								return;
							cdelay = 3;
							IIntuition->ModifyIDCMP(Window, ICON_IDCMP);
						}
					}
					break;
				case IDCMP_GADGETDOWN:
					if(!buttons || gadgetid != BUTPROP || !bankcount)
						break;
					FOREVER
					{
						bank = dopus_curgadbank;
						b = data_gadgetrow_offset;
						data_gadgetrow_offset = IDOpus->GetSliderPos(&iconbutpropgad, (bankcount * 6) / buttonrows, 1);
						a = data_gadgetrow_offset / bankstep;
						data_gadgetrow_offset %= bankstep;
						data_gadgetrow_offset *= buttonrows;
						dopus_curgadbank = bankarray[a];
						if(bank != dopus_curgadbank || data_gadgetrow_offset != b)
							drawgadgets(0, 0);
						if(getintuimsg())
						{
							class = IMsg->Class;
							IExec->ReplyMsg((struct Message *)IMsg);
							if(class == IDCMP_MOUSEMOVE)
								continue;
							if(class == IDCMP_GADGETUP)
								break;
						}
						IExec->Wait(1 << Window->UserPort->mp_SigBit);
					}
					IDOpus->FixSliderPot(Window, &iconbutpropgad, (a * bankstep) + (data_gadgetrow_offset / buttonrows), (bankcount * 6) / buttonrows, 1, 1);
					break;
				case IDCMP_GADGETUP:
					if(!buttons)
						break;
					if(gadgetid >= MAIN_GAD_BASE && gadgetid < MAIN_GAD_BASE + GADCOUNT)
					{
						a = (gadgetid - MAIN_GAD_BASE) + (data_gadgetrow_offset * 7);
						if(!dopus_curgadbank || !(isvalidgad(&dopus_curgadbank->gadgets[a])))
							break;
						dofunctionstring(dopus_curgadbank->gadgets[a].function, dopus_curgadbank->gadgets[a].name, NULL, (struct dopusfuncpar *)&dopus_curgadbank->gadgets[a].which);
						if(!status_iconified)
							return;
						cdelay = 3;
						IIntuition->ModifyIDCMP(Window, ICON_IDCMP);
					}
					else if(gadgetid == BUTEXIT)
						goto deiconify;
					break;
				case IDCMP_DISKREMOVED:
				case IDCMP_DISKINSERTED:
					setupchangestate();
					break;
				case IDCMP_CLOSEWINDOW:
					if(!(a = simplerequest(globstring[STR_REALLY_QUIT], globstring[STR_QUIT], str_cancelstring, globstring[STR_UNICONIFY], NULL)))
						break;
					if(a == 2)
						goto endiconify;
					cleanupiconify();
					status_iconified = 0;
					quit();
					break;
				case IDCMP_MOUSEBUTTONS:
					if(code != MENUDOWN)
					{
						if(!(icon_type & (ICON_MEMORY | ICON_DATE | ICON_TIME | ICON_CPU)))
							iconstatustext("Dir Opus", buttons);
						break;
					}
					if(buttons)
					{
						if(x < 0 || y < Window->BorderTop || x > Window->Width || y > Window->Height)
						{
							quickfixmenus();
							break;
						}
						for(c = 0; c < buttonrows; c++)
						{
							a = 42 + ((data_gadgetrow_offset + c) * 7);
							for(d = 0; d < 7; d++, a++)
							{
								if(isvalidgad(&dopus_curgadbank->gadgets[a]))
								{
									y1 = (c * scrdata_gadget_height) + scrdata_gadget_ypos - 1;
									x1 = scrdata_gadget_xpos + (d * scrdata_gadget_width);
									if(x >= x1 && x < x1 + scrdata_gadget_width && y >= y1 && y < y1 + scrdata_gadget_height)
									{
										if(!(dormbgadget(x1, y1, &dopus_curgadbank->gadgets[a - 42], &dopus_curgadbank->gadgets[a])))
										{
											dofunctionstring(dopus_curgadbank->gadgets[a].function, dopus_curgadbank->gadgets[a].name, NULL, (struct dopusfuncpar *)&dopus_curgadbank->gadgets[a].which);
											if(!status_iconified)
												return;
											cdelay = 3;
											IIntuition->ModifyIDCMP(Window, ICON_IDCMP);
											c = buttonrows;
											break;
										}
									}
								}
							}
						}
						break;
					}
				      deiconify:
					status_iconified = -1;
					cleanupiconify();
				      uniconify:
					config->gadgetrows = oldrows;
					dopus_curgadbank = oldbank;
					data_gadgetrow_offset = olddata_gadgetrow_offset;
					SetUp(louise);
					if(Window && status_publicscreen)
						IIntuition->ScreenToFront(Window->WScreen);
					rexx_command(config->uniconscript, NULL);
					startnotifies();
					IDOpus->LFreeRemember(&icon_key);
					return;
				}
			}
		}
		if(icon_gotclock && IExec->CheckIO(&iconify_timereq.tr_node))
		{
			IExec->WaitIO(&iconify_timereq.tr_node);
			iconify_timereq.tr_time.tv_secs = 1;
			iconify_timereq.tr_time.tv_micro = 0;
			IExec->SendIO(&iconify_timereq.tr_node);
			if(cdelay)
			{
				if(!(--cdelay) && !system_version2)
				{
					old = Window->Title;
					IIntuition->SetWindowTitles(Window, str_space_string, (char *)-1);
					IIntuition->SetWindowTitles(Window, old, (char *)-1);
				}
			}
		}
	}
}

void remiclock()
{
	if(!(IExec->CheckIO(&iconify_timereq.tr_node)))
		IExec->AbortIO(&iconify_timereq.tr_node);
	IExec->WaitIO(&iconify_timereq.tr_node);
	IExec->CloseDevice(&iconify_timereq.tr_node);
	IExec->DeletePort(iconify_time_port);
}

int getmaxmem(ULONG type)
{
	ULONG size = 0;
	int a;

	size = IExec->AvailMem(type | MEMF_TOTAL);
	size = (size + 1023) / 1024;
	for(a = 1;; a++)
		if(!(size /= 10))
			break;
	return (a);
}

void iconstatustext(char *buf, int buttons)
{
	strcpy(icontitletext, buf);
	IIntuition->SetWindowTitles(Window, icontitletext, (char *)-1);
}

void cleanupiconify()
{
	if(icon_type & ICON_APPICON)
	{
		if(appicon)
		{
			IWorkbench->RemoveAppIcon(appicon);
			appicon = NULL;
		}
		if(appmenu)
		{
			IWorkbench->RemoveAppMenuItem(appmenu);
			appmenu = NULL;
		}
		if(Window)
		{
			IIntuition->CloseWindow(Window);
			Window = NULL;
		}
	}
	else
	{
		if(Window)
		{
			if(status_flags & STATUS_ISINBUTTONS)
			{
				free_colour_table();
				if(IWorkbench && dopus_appwindow)
				{
					IWorkbench->RemoveAppWindow(dopus_appwindow);
					dopus_appwindow = NULL;
				}
				config->iconbutx = Window->LeftEdge;
				config->iconbuty = Window->TopEdge;
			}
			else
			{
				config->iconx = Window->LeftEdge;
				config->icony = Window->TopEdge;
			}
		}
		if(icon_gotclock)
			remiclock();
		if(Window)
		{
			if(Window->MenuStrip)
				IIntuition->ClearMenuStrip(Window);
			IIntuition->CloseWindow(Window);
			Window = NULL;
		}
	}
	IDOpus->LFreeRemember(&icon_key);
}
