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

#define LIB_VER 22
#define LIB_REV 25

int onworkbench = 0;

int main(int argc, char **argv)
{
	uint32 class;
	uint16 gadgetid = 0, code;
	int32 num, a;
	char portname[50], rportname[50], oldconfigname[256];
	struct WBStartup *wbmsg;
	struct ConfigUndo *undo;

	/* Attempt to open the DOPUS.LIBRARY. Look first in default search path, and then look for it on the distribution disk. If we can't find it exit */
	if(!(DOpusBase = IExec->OpenLibrary("dopus.library", LIB_VER)))
	{
		if(!(DOpusBase = IExec->OpenLibrary("PROGDIR:/libs/dopus.library", LIB_VER)))
		{
			DOpusBase = NULL;
		}
	}
	if(DOpusBase && !(IDOpus = (struct DOpusIFace *)IExec->GetInterface(DOpusBase, "main", 1, NULL)))
	{
		IDOpus = NULL;
	}
	if(!DOpusBase || !IDOpus)
	{
		IDOS->Printf("Can't Open dopus.library and get IDOpus interface\n");
		IExec->DropInterface((struct Interface *)IDOpus);
		IExec->CloseLibrary(DOpusBase);
		return 5;
	}

	network = 0;

	free_colour_table(NULL);

	help_ok = 1;

	currenthelpname[0] = 0;

	if(!(IDOpus->FindSystemFile("DirectoryOpus.CFG", configname, 256, SYSFILE_DATA)))
	{
		if(IDOpus->CheckExist("PROGDIR:/S", NULL))
		{
			IUtility->Strlcpy(configname, "PROGDIR:/S/DirectoryOpus.CFG", 256);
		}
		else
		{
			IUtility->Strlcpy(configname, "s:DirectoryOpus.CFG", 256);
		}
	}

	if(!(IDOpus->FindSystemFile("DirectoryOpus.CLA", classname, 256, SYSFILE_DATA)))
	{
		classname[0] = 0;
	}

	if(argc)
	{
		dropboxicon = IIcon->GetDiskObject(argv[0]);
	}
	else
	{
		wbmsg = (struct WBStartup *)argv;

		dropboxicon = IIcon->GetDiskObject(wbmsg->sm_ArgList->wa_Name);

		if(wbmsg->sm_NumArgs > 1)
		{
			IDOS->StrToLong(wbmsg->sm_ArgList[1].wa_Name, &num);
			sprintf(portname, "dopus4_config_port%ld", num);
			if(!(conport = IExec->CreatePort(portname, 20)))
			{
				quit();
			}
			sprintf(rportname, "dopus4_config_reply%ld", num);
			IExec->Forbid();
			if(!(cmdport = IExec->FindPort(rportname)))
			{
				IExec->DeletePort(conport);
				conport = NULL;
			}
			IExec->Permit();
		}
	}
	if(!dropboxicon)
	{
		dropboxicon = &dropboxobj;
	}

	myproc = (struct Process *)IExec->FindTask(0);
	wsave = IDOS->SetProcWindow((APTR)-1L);

	for(a = 0; a < 13; a++)
	{
		if(!(functypelist[a] = IDOpus->LAllocRemember(&mainkey, 40, MEMF_CLEAR)))
		{
			quit();
		}
	}
	functypelist[13] = NULL;

	if(!cmdport && !(config = (struct Config *)IExec->AllocMem(sizeof(struct Config), MEMF_CLEAR)))
	{
		quit();
	}
	getconfig();
	IUtility->Strlcpy(oldconfigname, configname, 256);

	appport = IExec->CreatePort(NULL, 0);

	read_strings();
	load_clips();
	readhelp();

/*	if(config->scrdepth < 2)
	{
		config->scrdepth += 2;
	}*/

	open_screen();

	undo = makeundo(UNDO_ALL);
	showconfigscreen(CFG_MAINMENU);
	giveconfig();

	for(;;)
	{
		IExec->Wait(1 << Window->UserPort->mp_SigBit);
		while((IMsg = getintuimsg()))
		{
			class = IMsg->Class;
			code = IMsg->Code;
			if(class == IDCMP_GADGETUP || class == IDCMP_GADGETDOWN)
			{
				gadgetid = ((struct Gadget *)IMsg->IAddress)->GadgetID;
			}
			IExec->ReplyMsg((struct Message *)IMsg);
			switch (class)
			{
			case IDCMP_MENUPICK:
				if(code == MENUNULL)
					break;
				switch ((num = ITEMNUM(code)))
				{
				case 0:
				case 1:
					if(lchanged && !request(cfg_string[STR_REALLY_DISCARD_CONFIG]))
						break;
					if(doload(CFG_MAINMENU, num))
					{
						busy();
						readhelp();
						unbusy();
						close_screen();
						open_screen();
						configmsg(CONFIG_NEW_HOTKEY);
						showconfigscreen(CFG_MAINMENU);
					}
					break;
				}
				break;
			case IDCMP_GADGETUP:
				curoperation = gadgetid;
				switch (gadgetid)
				{
				case CFG_DRIVE:
					if(dodriveconfig())
						lchanged = 1;
					showconfigscreen(CFG_MAINMENU);
					break;
				case CFG_FILETYPE:
					if(dofiletypeconfig())
						lchanged = 1;
					showconfigscreen(CFG_MAINMENU);
					break;
				case CFG_GADGET:
					if(dogadgetconfig())
						lchanged = 1;
					showconfigscreen(CFG_MAINMENU);
					break;
				case CFG_HOTKEYS:
					if(dohotkeysconfig())
						lchanged = 1;
					showconfigscreen(CFG_MAINMENU);
					break;
				case CFG_MENU:
					if(domenuconfig())
						lchanged = 1;
					showconfigscreen(CFG_MAINMENU);
					break;
				case CFG_OPERATION:
					if(dooperationconfig())
						lchanged = 1;
					showconfigscreen(CFG_MAINMENU);
					break;
				case CFG_SCREEN:
					if(doscreenconfig())
						lchanged = 1;
					showconfigscreen(CFG_MAINMENU);
					break;
				case CFG_SYSTEM:
					if(dosystemconfig())
						lchanged = 1;
					showconfigscreen(CFG_MAINMENU);
					break;
				case CFG_SAVE:
					dosave(0);
					lchanged = 2;
					quit();
					break;
				case CFG_OKAY:
				case CFG_CANCEL:
					if((gadgetid == CFG_CANCEL || !cmdport) && lchanged && !do_request(cfg_string[STR_EXIT_WITHOUT_SAVING], cfg_string[STR_OKAY_TO_QUIT], cfg_string[STR_CANCEL]))
						break;
					if(gadgetid == CFG_CANCEL)
					{
						doundo(undo, UNDO_ALL);
						strcpy(configname, oldconfigname);
						lchanged = changed = 0;
					}
					doundo(undo, 0);
					quit();
				}
				curoperation = -1;
			}
		}
	}
	return 0;
}

void quit()
{
	if(Window)
	{
		IDOpus->SetBusyPointer(Window);
	}
	if(cmdport)
	{
		giveconfig();
		configmsg(CONFIG_ALL_DONE);
	}
	else
	{
		fixcstuff(&cstuff);
		IDOpus->FreeConfig(&cstuff);
		if(config)
		{
			IExec->FreeMem(config, sizeof(struct Config));
		}
	}
	if(myproc)
	{
		IDOS->SetProcWindow(wsave);
	}
	if(firstclip)
	{
		save_clips();
	}
	if(Screen && !onworkbench)
	{
		IIntuition->ScreenToBack(Screen);
	}
	close_screen();
	IDOpus->FreeStringFile(&stringdata);
	if(conport)
	{
		IExec->DeletePort(conport);
	}
	if(appport)
	{
		IExec->DeletePort(appport);
	}
	if(tfont)
	{
		IGraphics->CloseFont(tfont);
	}
	if(dropboxicon != &dropboxobj)
	{
		IIcon->FreeDiskObject(dropboxicon);
	}

	IDOpus->LFreeRemember(&mainkey);
	IDOpus->LFreeRemember(&clipkey);
	IDOpus->LFreeRemember(&gadgetkey);
	IDOpus->LFreeRemember(&tickkey);
	IDOpus->LFreeRemember(&fontkey);
	IDOpus->LFreeRemember(&helpkey);
	IDOpus->LFreeRemember(&buttonkey);

	IExec->DropInterface((struct Interface *)IDOpus);
	IExec->CloseLibrary(DOpusBase);
	exit(0);
}

void getconfig()
{
	struct configconfig *cfg;

	if(cmdport)
	{
		configmsg(CONFIG_GET_CONFIG);
		cfg = (struct configconfig *)msg.buffer;
		config = cfg->config;
		firsttype = cfg->firsttype;
		firstbank = cfg->firstbank;
		firsthotkey = cfg->firsthotkey;
		typekey = cfg->typekey;
		changed = cfg->changed;
		IUtility->Strlcpy(configname, cfg->configname, 256);
		IUtility->Strlcat(configname, ".CFG", 256);
		IUtility->Strlcpy(loadnamebuf, configname, 256);
		Screen = cfg->Screen;
	}
	else
	{
		readconfig();
	}
}

void giveconfig()
{
	struct configconfig cfg;
	char *ptr;

	if(cmdport)
	{
		cfg.config = config;
		cfg.firsttype = firsttype;
		cfg.firstbank = firstbank;
		cfg.firsthotkey = firsthotkey;
		cfg.typekey = typekey;
		cfg.changed = ((lchanged || changed) ? 1 : 0);
		if(lchanged == 2)
			cfg.changed = 2;
		strcpy(cfg.configname, configname);
		if((ptr = strstri(cfg.configname, ".CFG")))
			*ptr = 0;
		cfg.Window = Window;
		cfg.Screen = Screen;
		msg.buffer = (char *)&cfg;
		configmsg(CONFIG_HERES_CONFIG);
	}
}

void readconfig()
{
	fixcstuff(&cstuff);
	if((IDOpus->ReadConfig(configname, &cstuff)) != 1)
		IDOpus->DefaultConfig(&cstuff);
	strcpy(loadnamebuf, configname);
	cstufffix(&cstuff);
}

void configmsg(int command)
{
	if(cmdport)
	{
		msg.msg.mn_Node.ln_Type = NT_MESSAGE;
		msg.msg.mn_ReplyPort = conport;
		msg.msg.mn_Length = (UWORD) sizeof(struct dopusconfigmsg);
		msg.command = command;
		IExec->PutMsg(cmdport, (struct Message *)&msg);
		IExec->WaitPort(conport);
		IExec->GetMsg(conport);
	}
}

void fixcstuff(struct ConfigStuff *cstuff)
{
	cstuff->config = config;
	cstuff->typekey = typekey;
	cstuff->firsttype = firsttype;
	cstuff->firstbank = firstbank;
	cstuff->curbank = curbank;
	cstuff->firsthotkey = firsthotkey;
	if(!Screen && Window)
	{
		config->config_x = Window->LeftEdge;
		config->config_y = Window->TopEdge;
	}
}

void cstufffix(struct ConfigStuff *cstuff)
{
	config = cstuff->config;
	typekey = cstuff->typekey;
	firsttype = cstuff->firsttype;
	firstbank = cstuff->firstbank;
	curbank = cstuff->curbank;
	firsthotkey = cstuff->firsthotkey;
}

void cleanconfigscreen()
{
	struct Gadget *gad;
	struct Message *msg;

	for(;;)
	{
		gad = Window->FirstGadget;
		while(gad)
		{
			if(!(gad->GadgetType & GTYP_SYSGADGET))
			{
				IIntuition->RemoveGList(Window, gad, 1);
				break;
			}
			gad = gad->NextGadget;
		}
		if(!gad)
			break;
	}

	IGraphics->SetDrMd(rp, JAM2);
	IGraphics->SetAPen(rp, screen_pens[0].pen);
	IGraphics->RectFill(rp, x_off, y_off, x_bot, y_bot);

	IGraphics->SetBPen(rp, screen_pens[1].pen);
	IGraphics->SetBPen(rp, screen_pens[0].pen);

	IDOpus->LFreeRemember(&gadgetkey);
	removetickgads();

	while((msg = IExec->GetMsg(Window->UserPort)))
		IExec->ReplyMsg(msg);
}

void showconfigscreen(int scr)
{
	cleanconfigscreen();

	switch (scr)
	{
	case CFG_MAINMENU:
		doscreentitle(cfg_string[STR_CONFIGURATION_MAIN_SCREEN]);
		initsidegads(mainmenugads, 0, 1);
		break;

	case CFG_OPERATION:
		initsidegads(operationgads, 1, 1);
		doscreentitle(cfg_string[STR_OPERATION_SCREEN]);
		break;

	case CFG_GADGET:
		if(gadgads)
		{
			IDOpus->AddGadgets(Window, gadgads, NULL, 84, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen, 1);
		}
		initsidegads(gadgetgads, 0, 0);
		maingad[GAD_COPYGADGET].Activation |= GACT_TOGGLESELECT;
		maingad[GAD_SWAPGADGET].Activation |= GACT_TOGGLESELECT;
		maingad[GAD_DELETEGADGET].Activation |= GACT_TOGGLESELECT;
		IGraphics->SetAPen(rp, screen_pens[1].pen);
		IDOpus->UScoreText(rp, cfg_string[STR_LEFT_MOUSE_BUTTON], x_off + 38, y_off + 71, -1);
		IGraphics->BltTemplate((UBYTE *)uparrow_data, 0, 2, rp, x_off + 179, y_off + 66, 12, 6);
		IDOpus->UScoreText(rp, cfg_string[STR_RIGHT_MOUSE_BUTTON], x_off + 454, y_off + 75, -1);
		IGraphics->BltTemplate((UBYTE *)downarrow_data, 0, 2, rp, x_off + 437, y_off + 70, 12, 6);

	case CFG_MENU:
		if(scr == CFG_MENU)
		{
			if(menugads)
				IIntuition->AddGList(Window, menugads, -1, MENUCOUNT + 5 - (HIDDENLINES * 5), NULL);
			initsidegads(menugadgets, 1, 0);
			maingad[MENU_OKAY].Activation &= ~GACT_TOGGLESELECT;
			maingad[MENU_CANCEL].Activation &= ~GACT_TOGGLESELECT;
			IIntuition->AddGList(Window, menuslidergads, -1, 3, NULL);
			IDOpus->AddGadgets(Window, &menuslidergads[1], NULL, 2, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen, 0);
			do3dbox(x_off + 612, y_off + 21, 12, 144 - HIDDENLINES * 8);
			IDOpus->DoArrow(rp, x_off + 612, y_off + 119, 12, 6, screen_pens[config->arrowfg].pen, screen_pens[config->arrowbg].pen, 0);
			IDOpus->DoArrow(rp, x_off + 612, y_off + 127, 12, 6, screen_pens[config->arrowfg].pen, screen_pens[config->arrowbg].pen, 1);
			menusliderimage.PlaneOnOff = config->slidercol;
			IDOpus->FixSliderBody(Window, &menuslidergads[0], 20, 20 - HIDDENLINES, 0);
			IDOpus->FixSliderPot(Window, &menuslidergads[0], 0, 20, 20 - HIDDENLINES, 2);
			do_menuscreen_title(STR_MENU_SCREEN);
		}

	case CFG_DRIVE:
		if(scr == CFG_DRIVE)
		{
			if(drivegads)
				IIntuition->AddGList(Window, drivegads, -1, DRIVECOUNT, NULL);
			initsidegads(drivegadgets, 1, 0);
			maingad[DRIVE_GETDRIVES].Activation &= ~GACT_TOGGLESELECT;
			maingad[DRIVE_OKAY].Activation &= ~GACT_TOGGLESELECT;
			maingad[DRIVE_CANCEL].Activation &= ~GACT_TOGGLESELECT;
			do_menuscreen_title(STR_DRIVE_SCREEN);
		}
		dogadgetinfo(NULL);
		break;

	case CFG_FILETYPE:
		initsidegads(filetypeactiongadgets, 1, 0);
		maingad[FILETYPE_NEW].Activation &= ~GACT_TOGGLESELECT;
		maingad[FILETYPE_OKAY].Activation &= ~GACT_TOGGLESELECT;
		maingad[FILETYPE_CANCEL].Activation &= ~GACT_TOGGLESELECT;
		doscreentitle(cfg_string[STR_FILETYPE_SCREEN]);
		setuplist(&filetypeactionlist, 12, 14);
		filetypeactionlist.itemselected = -1;
		IDOpus->AddListView(&filetypeactionlist, 1);
		dogadgetinfo(NULL);
		break;

	case CFG_SCREEN:
		initsidegads(screengadgets, 1, 1);
		doscreentitle(cfg_string[STR_SCREEN_SCREEN]);
		break;

	case CFG_SYSTEM:
		initsidegads(systemgads, 1, 1);
		doscreentitle(cfg_string[STR_SYSTEM_SCREEN]);
		break;

	case CFG_HOTKEYS:
		initsidegads(hotkeysgadgets, 1, 0);
		maingad[HOTKEYS_NEWHOTKEY].Activation &= ~GACT_TOGGLESELECT;
		maingad[HOTKEYS_OKAY].Activation &= ~GACT_TOGGLESELECT;
		maingad[HOTKEYS_CANCEL].Activation &= ~GACT_TOGGLESELECT;
		doscreentitle(cfg_string[STR_HOTKEYS_SCREEN]);
		setuplist(&hotkeyslist, 12, 14);
		hotkeyslist.itemselected = -1;
		hotkeyslist.items = NULL;
		hotkeyslist.topitem = 0;
		IDOpus->AddListView(&hotkeyslist, 1);
		dogadgetinfo(NULL);
		break;
	}
	if(scr == CFG_MAINMENU)
		strcpy(currenthelpname, cfg_string[STR_MAIN_MENU]);
	else
		strcpy(currenthelpname, mainmenugads[scr]);
}

void initsidegads(STRPTR *gads, int toggle, int vert)
{
	int num, a, x, y, x1 = 0, ac, dn, gnum, rnum, w, h;
	struct Gadget *gadbuf;

	if(!gads)
		return;

	for(num = 0, rnum = 0;; rnum++)
	{
		if(!gads[rnum])
			break;
		if(gads[rnum][0])
			++num;
	}

	if(!(gadbuf = IDOpus->LAllocRemember(&gadgetkey, sizeof(struct Gadget) * num, MEMF_CLEAR)))
		quit();

	h = 12;
	if(vert)
	{
		x = x_off + 8;
		y = y_off + 4;
		w = 112;
	}
	else
	{
		dn = rnum / 5;
		if(dn * 5 == rnum)
			--dn;
		ac = rnum - dn * 5;
		w = 104;
		x1 = x = ((Window->Width - (ac * (w + 12))) / 2) - 2;
		y = y_off + 172 - (dn * (h + 2));
	}
	gnum = 0;
	for(a = 0; a < rnum; a++)
	{
		if(gads[a][0])
		{
			gadbuf[gnum].NextGadget = (gnum < num - 1) ? &gadbuf[gnum + 1] : NULL;
			gadbuf[gnum].LeftEdge = x;
			gadbuf[gnum].TopEdge = y;
			gadbuf[gnum].Width = w;
			gadbuf[gnum].Height = h;
			gadbuf[gnum].Flags = GFLG_GADGHCOMP;
			gadbuf[gnum].Activation = GACT_RELVERIFY;
			if(toggle)
				gadbuf[gnum].Activation |= GACT_TOGGLESELECT;
			gadbuf[gnum].GadgetType = GTYP_BOOLGADGET;
			gadbuf[gnum].GadgetID = gnum;
			++gnum;
		}
		if(vert)
			y += (h + 2);
		else
		{
			x += (w + 16);
			if(x > (Window->Width - 10 - w))
			{
				x = x1;
				y += (h + 2);
			}
		}
	}
	IGraphics->SetAPen(rp, screen_pens[1].pen);
	IDOpus->AddGadgetBorders(&gadgetkey, gadbuf, gnum, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);
	IDOpus->AddGadgets(Window, gadbuf, gads, num, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen, 1);
	maingad = gadbuf;
}

void inittickgads(struct ConfigGadget *gads, int flag, int flag2)
{
	int32 num, a, y, gad, last = 0, lasty, dy, xp, yp, fl = 0, b;
	char **namearray;
	struct StringInfo *sinfo;

	removetickgads();
	if(!gads)
		return;

	num = 0;
	for(a = 0;; a++)
	{
		if(!gads[a].type)
			break;
		if(gads[a].type != CTYPE_TEXT && gads[a].type != CTYPE_SKIP)
			++num;
	}

	if(!num)
		return;

	if(!(tickgad = IDOpus->LAllocRemember(&tickkey, sizeof(struct Gadget) * num, MEMF_CLEAR)) || !(namearray = IDOpus->LAllocRemember(&tickkey, num * 4, MEMF_CLEAR)))
		return;

	y = y_off + 46;
	lasty = y;
	gad = 0;
	dy = 0;

	IGraphics->SetAPen(rp, screen_pens[1].pen);

	for(a = 0;; a++)
	{
		if(!gads[a].type)
			break;
		if(gads[a].type == CTYPE_SKIP)
			continue;
		switch (gads[a].type)
		{
		case CTYPE_MOVE:
			y = y_off + (gads[a].x - 12);
			dy = 0;
			break;
		case CTYPE_MOVEREL:
			dy = gads[a].x;
			break;
		case CTYPE_TEXT:
			if(gads[a].x < 0)
				xp = x_off + 158;
			else
				xp = x_off + (gads[a].x - 2);
			if(gads[a].y < 0)
			{
				yp = y + 8;
				dy = 13;
			}
			else
			{
				yp = y_off + (gads[a].y - 12);
				if(gads[a].value == 1)
				{
					y = yp - 8;
					dy = 13;
				}
				else
					dy = 0;
			}
			if(gads[a].nameentry > -1)
				IDOpus->UScoreText(rp, cfg_string[gads[a].nameentry], xp, yp, -1);
			break;
		case CTYPE_GAD:
		case CTYPE_RAD:
		case CTYPE_MRAD:
			if(gads[a].nameentry > -1)
				namearray[gad] = cfg_string[gads[a].nameentry];
			else
				namearray[gad] = (char *)-1;
			tickgad[gad].NextGadget = (gad < num - 1) ? &tickgad[gad + 1] : NULL;
			if(gads[a].type == CTYPE_GAD)
			{
				if(gads[a].w < 0)
					tickgad[gad].Width = 26;
				else
					tickgad[gad].Width = gads[a].w;
				if(gads[a].h < 0)
					tickgad[gad].Height = 11;
				else
					tickgad[gad].Height = gads[a].h;
				tickgad[gad].GadgetRender = checkoffimage;
				tickgad[gad].SelectRender = checkonimage;
				tickgad[gad].MutualExclude = GAD_CHECK;
				if(gads[a].x < 0)
					xp = x_off + 156;
				else
					xp = x_off + (gads[a].x - 2);
				if(gads[a].y < 0)
				{
					yp = y;
					dy = 13;
				}
				else
				{
					yp = y_off + (gads[a].y - 12);
					dy = 0;
				}
				tickgad[gad].Activation = GACT_RELVERIFY | GACT_TOGGLESELECT;
			}
			else
			{
				if(gads[a].w < 0)
					tickgad[gad].Width = 15;
				else
					tickgad[gad].Width = gads[a].w;
				if(gads[a].h < 0)
					tickgad[gad].Height = 7;
				else
					tickgad[gad].Height = gads[a].h;
				tickgad[gad].GadgetRender = (APTR) buttonoffimage;
				tickgad[gad].SelectRender = (APTR) buttononimage;
				tickgad[gad].MutualExclude = GAD_RADIO;
				if(gads[a].x < 0)
					xp = x_off + 158;
				else
					xp = x_off + (gads[a].x - 2);
				if(gads[a].y < 0)
				{
					yp = y;
					dy = 10;
				}
				else
				{
					yp = y_off + (gads[a].y - 12);
					dy = 0;
				}
				tickgad[gad].Activation = GACT_IMMEDIATE | GACT_TOGGLESELECT;
			}
			tickgad[gad].LeftEdge = xp;
			tickgad[gad].TopEdge = yp;
			tickgad[gad].Flags = GFLG_GADGIMAGE | GFLG_GADGHIMAGE;
			if(gads[a].value == 0)
				fl = flag;
			else if(gads[a].value == 1)
				fl = flag2;
			if(gads[a].type == CTYPE_MRAD)
			{
				b = gads[a].mutualex & (~gads[a].bit);
				if(!(fl & b))
					tickgad[gad].Flags |= GFLG_SELECTED;
			}
			else if(fl && fl & gads[a].bit)
				tickgad[gad].Flags |= GFLG_SELECTED;
			tickgad[gad].GadgetType = GTYP_BOOLGADGET;
			tickgad[gad].GadgetID = GAD_BASE + (gads[a].value * 4000) + gads[a].bit;
			tickgad[gad].UserData = (APTR) & gads[a];
			gads[a].gad = &tickgad[gad];
			last = gads[a].type;
			++gad;
			break;
		case CTYPE_STR:
			if(gads[a].y == -2)
				y = lasty;
			else
			{
				if(last == CTYPE_GAD || gads[a].type == CTYPE_RAD || gads[a].type == CTYPE_MRAD)
					++y;
				++y;
			}
			if(gads[a].bit & 4 || gads[a].nameentry == -1)
				namearray[gad] = (char *)-1;
			else
				namearray[gad] = cfg_string[gads[a].nameentry];
			tickgad[gad].NextGadget = (gad < num - 1) ? &tickgad[gad + 1] : NULL;
			if(gads[a].bit & 4)
			{
				if(gads[a].x < 0)
					xp = x_off + 338;
				else
					xp = x_off + (gads[a].x - 2);
				if(gads[a].y < 0)
				{
					yp = y;
					dy = 13;
				}
				else
				{
					yp = y_off + (gads[a].y - 12);
					dy = 0;
				}
			}
			else
			{
				if(gads[a].x < 0)
					xp = x_off + 306;
				else
					xp = x_off + (gads[a].x - 2);
				if(gads[a].y < 0)
				{
					yp = y;
					dy = 13;
				}
				else
				{
					yp = y_off + (gads[a].y - 12);
					dy = 0;
				}
			}
			if(gads[a].w < 0)
			{
				tickgad[gad].Width = Window->Width - xp - 30;
				if(gads[a].bit & 4)
					tickgad[gad].Width -= 2;
			}
			else
				tickgad[gad].Width = gads[a].w;
			if(gads[a].h < 0)
				tickgad[gad].Height = 8;
			else
				tickgad[gad].Height = gads[a].h;
			tickgad[gad].LeftEdge = xp;
			tickgad[gad].TopEdge = yp;
			tickgad[gad].Flags = GFLG_GADGHCOMP | GFLG_STRINGEXTEND | GFLG_TABCYCLE;
			tickgad[gad].Activation = GACT_RELVERIFY | GACT_TOGGLESELECT;
			if(gads[a].bit & 1)
				tickgad[gad].Activation |= GACT_LONGINT;
			if(gads[a].bit & 4)
			{
				tickgad[gad].MutualExclude = 1;
				++y;
			}
			tickgad[gad].GadgetType = GTYP_STRGADGET;
			if((sinfo = (struct StringInfo *)IDOpus->LAllocRemember(&tickkey, sizeof(struct StringInfo), MEMF_CLEAR)))
			{
				sinfo->Buffer = (STRPTR)IDOpus->LAllocRemember(&tickkey, gads[a].mutualex, MEMF_CLEAR);
				sinfo->MaxChars = gads[a].mutualex;
				sinfo->Extension = &stringex;
			}
			tickgad[gad].SpecialInfo = (APTR) sinfo;
			tickgad[gad].GadgetID = STRING_BASE + gad;
			tickgad[gad].UserData = (APTR) & gads[a];
			gads[a].gad = &tickgad[gad];
			last = gads[a].type;
			++gad;
			break;
		case CTYPE_REQ:
			y = lasty;
			if(gads[a].x < 0)
				xp = x_off + 302;
			else
				xp = x_off + (gads[a].x - 2);
			if(gads[a].y < 0)
			{
				yp = y - 4;
				dy = 13;
			}
			else
			{
				yp = y_off + (gads[a].y - 12);
				dy = 0;
			}
			if(gads[a].nameentry > -1)
				namearray[gad] = cfg_string[gads[a].nameentry];
			else
				namearray[gad] = (char *)-1;
			tickgad[gad].NextGadget = (gad < num - 1) ? &tickgad[gad + 1] : NULL;
			tickgad[gad].LeftEdge = xp;
			tickgad[gad].TopEdge = yp;
			if(gads[a].w < 0)
				tickgad[gad].Width = 28;
			else
				tickgad[gad].Width = gads[a].w;
			if(gads[a].h < 0)
				tickgad[gad].Height = 14;
			else
				tickgad[gad].Height = gads[a].h;
			tickgad[gad].Flags = GFLG_GADGHCOMP;
			tickgad[gad].Activation = GACT_RELVERIFY;
			tickgad[gad].GadgetType = GTYP_BOOLGADGET;
			tickgad[gad].GadgetID = REQ_BASE + gad;
			tickgad[gad].MutualExclude = GAD_GLASS;
			tickgad[gad].UserData = (APTR) & gads[a];
			IDOpus->AddGadgetBorders(&tickkey, &tickgad[gad], 1, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);
			doglassimage(&tickgad[gad]);
			gads[a].gad = &tickgad[gad - 1];
			last = gads[a].type;
			++gad;
			break;
		}
		lasty = y;
		y += dy;
	}
	IDOpus->AddGadgets(Window, tickgad, namearray, num, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen, 1);
	tickcount = num;
}

void removetickgads()
{
	if(tickgad && tickcount)
	{
		IIntuition->RemoveGList(Window, tickgad, tickcount);
		tickgad = NULL;
		tickcount = 0;
	}
	IDOpus->LFreeRemember(&tickkey);
	IGraphics->SetAPen(rp, screen_pens[0].pen);
	IGraphics->RectFill(rp, x_off + 122, y_off + 1, x_bot, y_bot);
}

int processtickgad(struct ConfigGadget *gads, int flag, int sel, int num)
{
	int a, ref = 0, hitgad = -1;
	struct Gadget *gad;

	for(a = 0;; a++)
	{
		if(!gads[a].type)
			break;
		if(num == -1 || gads[a].value == num)
		{
			if(gads[a].type == CTYPE_GAD || gads[a].type == CTYPE_RAD || gads[a].type == CTYPE_MRAD)
			{
				if(gads[a].bit == sel)
				{
					hitgad = a;
					break;
				}
			}
		}
	}

	flag = 0;
	for(a = 0;; a++)
	{
		if(!gads[a].type)
			break;
		if(num == -1 || gads[a].value == num)
		{
			gad = gads[a].gad;
			if(gads[a].type == CTYPE_GAD || gads[a].type == CTYPE_RAD || gads[a].type == CTYPE_MRAD)
			{
				if(a == hitgad)
				{
					if(!(gad->Flags & GFLG_SELECTED) && gads[a].mutualex & sel)
					{
						gad->Flags |= GFLG_SELECTED;
						ref = 1;
					}
				}
				else
				{
					if(gad->Flags & GFLG_SELECTED)
					{
						if(gads[a].type == CTYPE_MRAD)
						{
							if(gads[a].mutualex & sel)
							{
								gad->Flags &= ~GFLG_SELECTED;
								ref = 1;
							}
						}
						else if(gads[hitgad].mutualex & gads[a].bit)
						{
							gad->Flags &= ~GFLG_SELECTED;
							ref = 1;
						}
					}
				}
				if(gads[a].type != CTYPE_MRAD && gad->Flags & GFLG_SELECTED)
					flag |= gads[a].bit;
			}
			else if(gads[a].type == CTYPE_STR)
			{
				if(gads[a].buffer)
				{
					if(gads[a].bit & 2)
						IDOpus->CheckHexGad(gad, Window, 0, 0xff);
					strcpy((char *)gads[a].buffer, ((struct StringInfo *)gad->SpecialInfo)->Buffer);
				}
			}
		}
	}
	if(ref)
		IIntuition->RefreshGList(tickgad, Window, NULL, tickcount);
	return (flag);
}

struct ConfigUndo *makeundo(int type)
{
	struct ConfigUndo *undo;
	struct DOpusRemember *key;
	struct dopusgadgetbanks *bank, *newbank, *curbank;
	struct dopushotkey *hotkey, *newhotkey, *curhotkey;
	int a;

	key = NULL;
	if(!(undo = IDOpus->LAllocRemember(&key, sizeof(struct ConfigUndo), MEMF_CLEAR)))
		return (0);
	if(type & UNDO_MAIN)
		undo->config = (struct Config *)getcopy((char *)config, sizeof(struct Config), &key);
	if(type & UNDO_GADGET)
	{
		bank = firstbank;
		curbank = NULL;
		while(bank)
		{
			if((newbank = (struct dopusgadgetbanks *)getcopy((char *)bank, sizeof(struct dopusgadgetbanks), &key)))
			{
				newbank->next = NULL;
				copygadgets(bank, newbank, &key);
				if(curbank)
					curbank->next = newbank;
				else
					undo->firstbank = newbank;
				curbank = newbank;
			}
			bank = bank->next;
		}
	}
	if(type & UNDO_MENU && (undo->menu = (struct newdopusfunction *)getcopy((char *)config->menu, sizeof(struct newdopusfunction) * MENUCOUNT, &key)))
	{
		for(a = 0; a < MENUCOUNT; a++)
		{
			undo->menu[a].name = getcopy(config->menu[a].name, -1, &key);
			undo->menu[a].function = getcopy(config->menu[a].function, -1, &key);
		}
		for(a = 0; a < 5; a++)
			strcpy(undo->menutit[a], config->menutit[a]);
	}
	if(type & UNDO_DRIVE && (undo->drive = (struct dopusfunction *)getcopy((char *)config->drive, sizeof(struct dopusfunction) * DRIVECOUNT, &key)))
	{
		for(a = 0; a < DRIVECOUNT; a++)
			undo->drive[a].function = getcopy(config->drive[a].function, -1, &key);
	}
	if(type & UNDO_FILETYPE)
		copyfiletypes(firsttype, &undo->firsttype, &key);
	if(type & UNDO_HOTKEYS)
	{
		hotkey = firsthotkey;
		curhotkey = NULL;
		while(hotkey)
		{
			if((newhotkey = (struct dopushotkey *)getcopy((char *)hotkey, sizeof(struct dopushotkey), &key)))
			{
				newhotkey->func.function = getcopy(hotkey->func.function, -1, &key);
				newhotkey->next = NULL;
				if(curhotkey)
					curhotkey->next = newhotkey;
				else
					undo->firsthotkey = newhotkey;
				curhotkey = newhotkey;
			}
			hotkey = hotkey->next;
		}
	}
	undo->key = key;
	return (undo);
}

void copygadgets(struct dopusgadgetbanks *bank1, struct dopusgadgetbanks *bank2, struct DOpusRemember **key)
{
	int a;

	for(a = 0; a < 84; a++)
	{
		bank2->gadgets[a].name = getcopy(bank1->gadgets[a].name, -1, key);
		bank2->gadgets[a].function = getcopy(bank1->gadgets[a].function, -1, key);
	}
}

void copyfiletypes(struct dopusfiletype *oldfirst, struct dopusfiletype **newfirst, struct DOpusRemember **key)
{
	struct dopusfiletype *type, *newtype, *curtype = NULL;
	int a;

	type = oldfirst;
	*newfirst = NULL;
	while(type)
	{
		if((newtype = (struct dopusfiletype *)getcopy((char *)type, sizeof(struct dopusfiletype), key)))
		{
			for(a = 0; a < FILETYPE_FUNCNUM; a++)
				newtype->function[a] = getcopy(type->function[a], -1, key);
			newtype->recognition = getcopy(type->recognition, -1, key);
			newtype->iconpath = getcopy(type->iconpath, -1, key);
			if(curtype)
				curtype->next = newtype;
			else
				*newfirst = newtype;
			curtype = newtype;
			curtype->next = NULL;
		}
		type = type->next;
	}
}

void doundo(struct ConfigUndo *undo, int type)
{
	struct DOpusRemember *key;
	struct dopusgadgetbanks *bank, *curbank, *newbank;
	struct dopushotkey *hotkey, *curhotkey, *newhotkey;
	int a;

	if(!undo)
		return;
	if(!type)
	{
		key = undo->key;
		IDOpus->LFreeRemember(&key);
	}
	else
	{
		if(type & UNDO_MENU)
		{
			for(a = 0; a < MENUCOUNT; a++)
				freenewfunction(&config->menu[a]);
		}
		if(type & UNDO_DRIVE)
			for(a = 0; a < DRIVECOUNT; a++)
				freefunction(&config->drive[a]);
		if(type & UNDO_MAIN && undo->config)
			copyconfigonly(undo->config, config);
		if(type & UNDO_GADGET)
		{
			freegadgets(firstbank);
			firstbank = NULL;
			bank = undo->firstbank;
			curbank = NULL;
			while(bank)
			{
				if((newbank = (struct dopusgadgetbanks *)getcopy((char *)bank, sizeof(struct dopusgadgetbanks), NULL)))
				{
					newbank->next = NULL;
					copygadgets(bank, newbank, NULL);
					if(curbank)
						curbank->next = newbank;
					else
						firstbank = newbank;
					curbank = newbank;
				}
				bank = bank->next;
			}
		}
		if(type & UNDO_MENU && undo->menu)
		{
			IExec->CopyMem((char *)undo->menu, (char *)config->menu, sizeof(struct newdopusfunction) * MENUCOUNT);
			for(a = 0; a < MENUCOUNT; a++)
			{
				config->menu[a].name = getcopy(undo->menu[a].name, -1, NULL);
				config->menu[a].function = getcopy(undo->menu[a].function, -1, NULL);
			}
			for(a = 0; a < 5; a++)
				strcpy(config->menutit[a], undo->menutit[a]);
		}
		if(type & UNDO_DRIVE && undo->drive)
		{
			IExec->CopyMem((char *)undo->drive, (char *)config->drive, sizeof(struct dopusfunction) * DRIVECOUNT);
			for(a = 0; a < DRIVECOUNT; a++)
				config->drive[a].function = getcopy(undo->drive[a].function, -1, NULL);
		}
		if(type & UNDO_FILETYPE)
		{
			IDOpus->LFreeRemember(&typekey);
			firsttype = NULL;
			copyfiletypes(undo->firsttype, &firsttype, &typekey);
		}
		if(type & UNDO_HOTKEYS)
		{
			hotkey = firsthotkey;
			while(hotkey)
			{
				curhotkey = hotkey->next;
				freestring(hotkey->func.function);
				IExec->FreeMem(hotkey, sizeof(struct dopushotkey));
				hotkey = curhotkey;
			}
			firsthotkey = NULL;
			curhotkey = NULL;
			hotkey = undo->firsthotkey;
			while(hotkey)
			{
				if((newhotkey = (struct dopushotkey *)getcopy((char *)hotkey, sizeof(struct dopushotkey), NULL)))
				{
					newhotkey->func.function = getcopy(hotkey->func.function, -1, NULL);
					newhotkey->next = NULL;
					if(curhotkey)
						curhotkey->next = newhotkey;
					else
						firsthotkey = newhotkey;
					curhotkey = newhotkey;
				}
				hotkey = hotkey->next;
			}
		}
	}
}

void freegadgets(struct dopusgadgetbanks *first)
{
	struct dopusgadgetbanks *temp;

	while(first)
	{
		temp = first->next;
		freegadgetbank(first, 1);
		first = temp;
	}
}

void freegadgetbank(struct dopusgadgetbanks *bank, int free)
{
	int a;

	if(bank)
	{
		for(a = 0; a < GADCOUNT; a++)
		{
			freestring(bank->gadgets[a].name);
			freestring(bank->gadgets[a].function);
		}
		if(free)
			IExec->FreeMem(bank, sizeof(struct dopusgadgetbanks));
	}
}

void freefunction(struct dopusfunction *func)
{
	freestring(func->function);
	func->function = NULL;
}

void freenewfunction(struct newdopusfunction *func)
{
	freestring(func->name);
	func->name = NULL;
	freestring(func->function);
	func->function = NULL;
}

char *getcopy(STRPTR object, int size, struct DOpusRemember **key)
{
	char *newobject;

	if(!object)
		return (NULL);
	if(size == -1)
		size = strlen(object) + 1;
	if(key)
		newobject = (char *)IDOpus->LAllocRemember(key, size, MEMF_CLEAR);
	else
		newobject = IExec->AllocMem(size, MEMF_CLEAR);
	if(newobject)
		IExec->CopyMem((char *)object, (char *)newobject, size);
	return (newobject);
}

void makestring(int8 *buf, ...)
{
	va_list ap;
	char *ptr;
	struct Gadget *gad, *first = NULL;
	struct ConfigGadget *cgad;

	if(!tickgad)
		return;
	gad = tickgad;

	va_start(ap, buf);
	while(buf && gad)
	{
		if(gad->GadgetType == GTYP_STRGADGET)
		{
			cgad = (struct ConfigGadget *)gad->UserData;
			cgad->buffer = buf;
			ptr = (char *)((struct StringInfo *)gad->SpecialInfo)->Buffer;
			strcpy(ptr, (char *)buf);
			IDOpus->RefreshStrGad(gad, Window);
			if(!first)
				first = gad;
			buf = va_arg(ap, int8 *);
		}
		gad = gad->NextGadget;
	}
	if(first)
		IDOpus->ActivateStrGad(first, Window);
}

int getstring(STRPTR text, STRPTR buf, int len, int num)
{
	struct DOpusSimpleRequest req;
	char *gads[3];
	static int rets[2] = { 1, 0 };
	int a;

	gads[0] = cfg_string[STR_OKAY];
	gads[1] = cfg_string[STR_CANCEL];
	gads[2] = NULL;

	req.text = text;
	req.gads = gads;
	req.rets = rets;
	req.hi = screen_pens[config->gadgettopcol].pen;
	req.lo = screen_pens[config->gadgetbotcol].pen;
	req.fg = screen_pens[config->requestfg].pen;
	req.bg = screen_pens[config->requestbg].pen;
	req.strbuf = buf;
	req.strlen = len;
	req.flags = num | SRF_BORDERS | SRF_RECESSHI | SRF_EXTEND;
	req.value = (int)&stringex;
	req.font = NULL;
	req.title = "ConfigOpus";
	busy();
	a = IDOpus->DoSimpleRequest(Window, &req);
	unbusy();
	return (a);
}

int request(STRPTR text)
{
	return (do_request(text, cfg_string[STR_OKAY], cfg_string[STR_CANCEL]));
}

int do_request(STRPTR text, STRPTR pos, STRPTR neg)
{
	char buf[100] = { 0, };
	int32 a;

	sprintf(buf, "%s|%s", pos, neg);
/*	struct DOpusSimpleRequest req;
	char *gads[3];
	static int rets[2] = { 1, 0 };
	int a;

	gads[0] = pos;
	gads[1] = neg;
	gads[2] = NULL;

	req.text = text;
	req.gads = gads;
	req.rets = rets;
	req.hi = screen_pens[config->gadgettopcol].pen;
	req.lo = screen_pens[config->gadgetbotcol].pen;
	req.fg = screen_pens[config->requestfg].pen;
	req.bg = screen_pens[config->requestbg].pen;
	req.strbuf = NULL;
	req.flags = SRF_BORDERS | SRF_RECESSHI;
	req.font = NULL;
	req.title = "ConfigOpus";
	busy();
	a = IDOpus->DoSimpleRequest(Window, &req);
	unbusy();*/

//	IDOS->SetProcWindow(wsave);
	busy();
	a = IDOS->TimedDosRequesterTags(TDR_Timeout, 0, TDR_Window, Window, TDR_ImageType, TDRIMAGE_WARNING, TDR_FormatString, text, TDR_TitleString, "ConfigOpus", TDR_GadgetString, buf);
	unbusy();
//	wsave = IDOS->SetProcWindow((APTR)-1L);
	return (a);
}

void doglassimage(struct Gadget *gad)
{
	int a, o;

	o = rp->FgPen;
	IGraphics->SetDrMd(rp, JAM1);
	IGraphics->SetAPen(rp, screen_pens[config->gadgetbotcol].pen);
	for(a = 0; a < 2; a++)
	{
		IGraphics->BltTemplate((UBYTE *)glass_data[a], 0, 4, rp, gad->LeftEdge + 4, gad->TopEdge + 2, 20, 9);
		IGraphics->SetAPen(rp, screen_pens[config->gadgettopcol].pen);
	}
	IGraphics->SetDrMd(rp, JAM2);
	IGraphics->SetAPen(rp, o);
}

void freestring(STRPTR str)
{
	if(str)
		IExec->FreeMem(str, strlen(str) + 1);
}

static char lasttitle[80];

void doscreentitle(STRPTR str)
{
	if(str && str != (char *)-1)
		strcpy(lasttitle, str);
	IIntuition->SetWindowTitles(Window, lasttitle, (char *)-1);
}

struct IntuiMessage *getintuimsg()
{
	struct IntuiMessage *msg;
	struct MenuItem *item;
	int a;
	char buf[10], c;

	if((msg = (struct IntuiMessage *)IExec->GetMsg(Window->UserPort)))
	{
		if(msg->Class == IDCMP_RAWKEY)
		{
			if(msg->Code == 0x5f && help_ok)
			{
				doconfighelp();
				IExec->ReplyMsg((struct Message *)msg);
				msg = NULL;
			}
			else if(msg->Qualifier & IEQUALIFIER_RCOMMAND && Window->MenuStrip)
			{
				if(!(msg->Code & 0x80))
				{
					IDOpus->RawkeyToStr(msg->Code, 0, NULL, buf, 10);
					c = IUtility->ToUpper(buf[0]);
					item = Window->MenuStrip->FirstItem;
					a = 0;
					while(item)
					{
						if(item->Command && c == item->Command)
						{
							msg->Class = IDCMP_MENUPICK;
							msg->Code = a << 5;
							break;
						}
						item = item->NextItem;
						++a;
					}
				}
			}
		}
		else if(msg->Class == IDCMP_MENUPICK)
		{
			doscreentitle((char *)-1);
			if(Window->MenuStrip == &projectmenu && MENUNUM(msg->Code) == 0)
			{
				a = 0;
				switch (ITEMNUM(msg->Code))
				{
				case 3:
					dosave(0);
					a = 1;
					break;
				case 4:
					dosave(1);
					a = 1;
					break;
				}
				if(a)
				{
					IExec->ReplyMsg((struct Message *)msg);
					msg = NULL;
				}
			}
		}
		else if(msg->Class == IDCMP_MOUSEMOVE && Window->MenuStrip)
		{
			if(msg->MouseY > Window->BorderTop && msg->MouseY < Window->Height && msg->MouseX >= 0 && msg->MouseX < Window->Width)
				Window->Flags |= WFLG_RMBTRAP;
			else
				Window->Flags &= ~WFLG_RMBTRAP;
		}
	}
	return (msg);
}

struct TextFont *getfont(STRPTR font, int *size, int flags)
{
	BPTR lock;
	struct TextFont *tf;
	static struct TextAttr sfont = { NULL, 0, 0, 0 };

	sfont.ta_Name = (STRPTR)font;
	sfont.ta_YSize = *size;
	if((tf = IGraphics->OpenFont(&sfont)) && tf->tf_YSize == sfont.ta_YSize && (!(tf->tf_Flags & FPF_PROPORTIONAL) || flags != FFLAG_NOPROP))
		return (tf);
	if(tf)
		IGraphics->CloseFont(tf);
	if(!(lock = IDOS->Lock("FONTS:", ACCESS_READ)))
	{
		strcpy(font, "topaz.font");
		*size = 8;
		sfont.ta_YSize = 8;
		return ((IGraphics->OpenFont(&sfont)));
	}
	IDOS->UnLock(lock);
	if(!DiskfontBase || !(tf = IDiskfont->OpenDiskFont(&sfont)))
	{
		if((tf = IGraphics->OpenFont(&sfont)))
			*size = tf->tf_YSize;
	}
	if(!tf || (!tf && flags != FFLAG_8ONLY) || (tf && flags == FFLAG_NOPROP && tf->tf_Flags & FPF_PROPORTIONAL))
	{
		if(tf)
			IGraphics->CloseFont(tf);
		strcpy(font, "topaz.font");
		*size = 8;
		sfont.ta_YSize = 8;
		tf = IGraphics->OpenFont(&sfont);
	}
	return (tf);
}

void loadrgb4(struct Screen *scr, USHORT *pal, int num)
{
	int a, b;
	ULONG *data;

	if((data = IExec->AllocMem(num * 3 * sizeof(ULONG), 0)))
	{
		for(a = 0, b = 0; a < num; a++)
		{
			data[b++] = (((pal[a] >> 8) & 0xf) << 28) | 0x0fffffff;
			data[b++] = (((pal[a] >> 4) & 0xf) << 28) | 0x0fffffff;
			data[b++] = ((pal[a] & 0xf) << 28) | 0x0fffffff;
		}
		load_palette(scr, data, num);
		IExec->FreeMem(data, num * 3 * sizeof(ULONG));
	}
}

void load_palette(struct Screen *screen, uint32 *palette, int numcols)
{
	int a = 0, b = 0;
	if(!screen)
	{
		for(a = 0, b = 0; a < numcols; a++)
		{
			IGraphics->SetRGB32(&Window->WScreen->ViewPort, screen_pens[a].pen, palette[b], palette[b + 1], palette[b + 2]);
			b++; b++; b++;
		}
	}
	else
	{
		uint32 *backup_palette;

		if((backup_palette = IExec->AllocMem(((numcols * 3) + 2) * sizeof(ULONG), MEMF_CLEAR)))
		{
			IExec->CopyMem((char *)palette, (char *)&backup_palette[1], (numcols * 3) * sizeof(ULONG));
			backup_palette[0] = numcols << 16;
			backup_palette[(numcols * 3) + 1] = 0;
			IGraphics->LoadRGB32(&screen->ViewPort, backup_palette);
			IExec->FreeMem(backup_palette, ((numcols * 3) + 2) * sizeof(ULONG));
		}
	}
}

void get_palette(struct Screen *screen, uint32 *palette, int numcols)
{
	int a, b;
	if(!screen)
	{
		for(a = 0, b = 0; a < numcols; a++, b += 3)
		{
			IGraphics->GetRGB32(Window->WScreen->ViewPort.ColorMap, screen_pens[a].pen, 1, &palette[b]);
		}
	}
	else
	{
		IGraphics->GetRGB32(screen->ViewPort.ColorMap, 0, numcols, palette);
	}
}

void open_screen()
{
	if(!Window)
	{
		int a;
		struct Screen *wbscreen = NULL, *usescreen;
		char *psname = NULL;

		onworkbench = 0;
		if(tfont)
		{
			IGraphics->CloseFont(tfont);
		}
		a = 8;
		tfont = getfont(config->fontbufs[0], &a, FFLAG_8ONLY);
		strcpy(sfont.ta_Name, tfont->tf_Message.mn_Node.ln_Name);

		if(Screen)
		{
			struct List *psl;
			struct PubScreenNode *psn;

			if((psl = IIntuition->LockPubScreenList()))
			{
				for(psn = (struct PubScreenNode *)psl->lh_Head; psn->psn_Node.ln_Succ; psn = (struct PubScreenNode *)psn->psn_Node.ln_Succ)
				{
					if(psn->psn_Screen == Screen)
					{
						psname = psn->psn_Node.ln_Name;
						break;
					}
				}
				IIntuition->UnlockPubScreenList();
			}
		}
		if((wbscreen = IIntuition->LockPubScreen(psname)))
		{
			int pen, num;
			struct ColorMap *cm;

			cm = wbscreen->ViewPort.ColorMap;
			num = 1 << ((config->scrdepth > 4) ? 4 : config->scrdepth);

			for(pen = 0; pen < num; pen++)
			{
				screen_pens[pen].red = config->new_palette[(pen * 3)];
				screen_pens[pen].green = config->new_palette[(pen * 3) + 1];
				screen_pens[pen].blue = config->new_palette[(pen * 3) + 2];
				if((screen_pens[pen].pen = IGraphics->ObtainPen(cm, -1, screen_pens[pen].red, screen_pens[pen].green, screen_pens[pen].blue, PEN_EXCLUSIVE)) == -1)
//				if((screen_pens[pen].pen = IGraphics->ObtainBestPenA(cm, screen_pens[pen].red, screen_pens[pen].green, screen_pens[pen].blue, NULL)) == -1)
				{
					break;
				}
				screen_pens[pen].alloc = 1;
			}

			if(pen < num)
			{
				free_colour_table(cm);
			}
			else
			{
				onworkbench = 1;
			}
		}
		else
		{
//			IExec->DebugPrintF("Problem!\n");
		}

		configscr.Depth = config->scrdepth;
		if(!onworkbench)
		{
			scr_taglist[1].ti_Data = IGraphics->BestModeID(BIDTAG_DesiredWidth, configscr.Width, BIDTAG_DesiredHeight, configscr.Height, BIDTAG_Depth, configscr.Depth, wbscreen ? BIDTAG_MonitorID : TAG_IGNORE, wbscreen ? IGraphics->GetVPModeID(&wbscreen->ViewPort) & MONITOR_ID_MASK : 0, TAG_END);
			if(scr_taglist[1].ti_Data == INVALID_ID)
			{
				scr_taglist[1].ti_Data = HIRES_KEY;
			}

			while(!(Screen = IIntuition->OpenScreen((struct NewScreen *)&configscr)))
			{
				if(configscr.Depth <= 2)
					quit();
				--configscr.Depth;
			}
			IIntuition->ShowTitle(Screen, FALSE);
			usescreen = Screen;
		}
		else
		{
			usescreen = wbscreen;
		}

		vp = &usescreen->ViewPort;
		config->scrdepth = configscr.Depth;
		screen_depth = configscr.Depth;

		configwin.Screen = usescreen;
		configwin.Height = usescreen->WBorTop + usescreen->Font->ta_YSize + 200; //189;

		configwin.LeftEdge = (usescreen->Width - configwin.Width) >> 1;
		configwin.TopEdge = ((usescreen->Height - configwin.Height - (usescreen->Font->ta_YSize + 1)) >> 1) + usescreen->Font->ta_YSize + 1;

		if(!Screen)
		{
			if(config->config_x > -1)
			{
				configwin.LeftEdge = config->config_x;
			}
			if(config->config_y > -1)
			{
				configwin.TopEdge = config->config_y;
			}
		}

		if(configwin.Width + configwin.LeftEdge > usescreen->Width)
			configwin.LeftEdge = usescreen->Width - configwin.Width;

		if(configwin.Height + configwin.TopEdge > usescreen->Height)
			configwin.TopEdge = usescreen->Height - configwin.Height;

		if(!(Window = IIntuition->OpenWindow(&configwin)))
		{
			quit();
		}

		if(!Screen)
		{
			IIntuition->SetWindowTitles(Window, (char *)-1, configscr.DefaultTitle);
		}

		rp = Window->RPort;
		IDOpus->FSSetMenuStrip(Window, &projectmenu);

		x_off = Window->BorderLeft;
		y_off = Window->BorderTop;
		x_bot = Window->Width - Window->BorderRight - 1;
		y_bot = Window->Height - Window->BorderBottom - 1;

		if(tfont)
		{
			IGraphics->SetFont(rp, tfont);
		}
		if(wbscreen)
		{
			IIntuition->UnlockPubScreen(NULL, wbscreen);
		}
	}

	load_palette(Screen, config->new_palette, 1 << ((screen_depth > 4) ? 4 : screen_depth));

	doradiobuttons();
	fix_gadget_positions();
	if(Screen || cmdport)
	{
		IIntuition->ScreenToFront(Window->WScreen);
	}
}

void close_screen()
{
	free_colour_table((Window) ? Window->WScreen->ViewPort.ColorMap : NULL);
	if(Window)
	{
		if(Window->MenuStrip)
		{
			IIntuition->ClearMenuStrip(Window);
		}
		IIntuition->CloseWindow(Window);
		Window = NULL;
	}
	if(Screen)
	{
		IIntuition->CloseScreen(Screen);
		Screen = NULL;
	}
	onworkbench = 0;
}

void free_colour_table(struct ColorMap *cm)
{
	int a;

	if(cm)
	{
		for(a = 0; a < 16; a++)
		{
			if(screen_pens[a].alloc)
				IGraphics->ReleasePen(cm, screen_pens[a].pen);
		}
	}

	for(a = 0; a < 16; a++)
	{
		screen_pens[a].pen = a;
		screen_pens[a].alloc = 0;
	}
}

unsigned char getkeyshortcut(const char *str)
{
	char *c;

	if((c = strchr(str, '_')))
		return IUtility->ToLower(c[1]);
	else
		return 0;
}
