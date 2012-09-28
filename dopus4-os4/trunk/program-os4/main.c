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
#include <graphics/rpattr.h>

static const char* stackcookie USED = "$STACK: 524288";

static BOOL staybehindWB = FALSE;

int main(int argc, char **argv)
{
	uint32 a, iconstart, sup, checkrunning, nsee;
	struct WBStartup *WBmsg = NULL;
	struct WBArg *p = NULL;
	char **toolarray, *s, *startdir = NULL;
	char buf[1024];

	signal(SIGINT, SIG_IGN); // disable C library break handling.
	/* Get pointer to our Process structure and set our WindowPtr for errors to -1 (no errors appear). */
	main_proc = (struct Process *)IExec->FindTask(NULL);
//	IDOS->SetProcWindow((APTR)-1L);

	/* Attempt to open the DOPUS.LIBRARY. Look first in default search path, and then look for it on the distribution disk. If we can't find it exit */
	if(!(DOpusBase = IExec->OpenLibrary("dopus.library", 22)))
	{
		if(!(DOpusBase = IExec->OpenLibrary("PROGDIR:libs/dopus.library", 22)))
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
		IDOS->Printf("Can't open dopus.library and get IDOpus interface!\n");
		IExec->DropInterface((struct Interface *)IDOpus);
		IExec->CloseLibrary(DOpusBase);
		return 5;
	}

	AmigaGuideBase = IExec->OpenLibrary("amigaguide.library", 0L);
	IAmigaGuide = (struct AmigaGuideIFace *)IExec->GetInterface(AmigaGuideBase, "main", 1, NULL);
	ApplicationBase = IExec->OpenLibrary("application.library", 0L);
	IApplication = (struct ApplicationIFace *)IExec->GetInterface(ApplicationBase, "application", 1, NULL);
	CxBase = IExec->OpenLibrary("commodities.library", 0L);
	ICommodities = (struct CommoditiesIFace *)IExec->GetInterface(CxBase, "main", 1, NULL);
	GadToolsBase = IExec->OpenLibrary("gadtools.library", 0L);
	IGadTools = (struct GadToolsIFace *)IExec->GetInterface(GadToolsBase, "main", 1, NULL);
	LayersBase = IExec->OpenLibrary("layers.library", 0L);
	ILayers = (struct LayersIFace *)IExec->GetInterface(LayersBase, "main", 1, NULL);
	PopupMenuBase = IExec->OpenLibrary("popupmenu.library", 0L);
	IPopupMenu = (struct PopupMenuIFace *)IExec->GetInterface(PopupMenuBase, "main", 1, NULL);
	RexxSysBase = IExec->OpenLibrary("rexxsyslib.library", 0L);
	IRexxSys = (struct RexxSysIFace *)IExec->GetInterface(RexxSysBase, "main", 1, NULL);
	xadMasterBase = IExec->OpenLibrary("xadmaster.library", 0L);
	IxadMaster = (struct xadMasterIFace *)IExec->GetInterface(xadMasterBase, "main", 1, NULL);

	if(!IAmigaGuide || !IApplication || !ICommodities || !IGadTools || !ILayers || !IPopupMenu || !IRexxSys || !IxadMaster)
	{
		IDOS->Printf("Can't open required libraries and interfaces!\n");
		quit();
		return 5;
	}

	/* status_flags contains various flags; initialise it to 0 to start with */

	/* Initialise various data */
	if((str_last_rexx_result = IExec->AllocVec(256, MEMF_ANY)))
	{
		str_last_rexx_result[0] = 0;
	}
	else
	{
		quit();
	}

	scrdata_is_pal = getpal();

	iconstart = 0;
	checkrunning = 0;

	strcpy(str_config_basename, "DirectoryOpus");

	if(argc >= 1)
	{
		if(IconBase && IIcon)
		{
			user_appicon = IIcon->GetDiskObject(argv[0]);
		}

		for(a = 1; a < argc; a++)
		{
			if(argv[a][0] == '-')
			{
				switch (argv[a][1])
				{
				case 'i':
					iconstart = 1;
					break;
				case 'b':
					iconstart = 2;
					break;
				case 'c':
					strcpy(str_config_basename, &argv[a][2]);
					break;
				case 'C':
					checkrunning = 1;
					break;
				case 'B':
					staybehindWB = TRUE;
				case 'd':
					docky = TRUE;
				}
			}
			else
			{
				startdir = argv[a];
			}
		}
	}
	else if(argc == 0 && IconBase && IIcon)
	{
		WBmsg = (struct WBStartup *)argv;
		p = WBmsg->sm_ArgList;
		if((user_appicon = IIcon->GetDiskObject(p->wa_Name)))
		{
			toolarray = user_appicon->do_ToolTypes;
			if((IIcon->FindToolType(toolarray, "ICONSTART")))
				iconstart = 1;
			if((IIcon->FindToolType(toolarray, "BUTTONSTART")))
				iconstart = 2;
			if((s = (STRPTR)IIcon->FindToolType(toolarray, "CONFIGFILE")))
				strcpy(str_config_basename, s);
			if((IIcon->FindToolType(toolarray, "CHECK")))
				checkrunning = 1;
			if(IIcon->FindToolType(toolarray,"BEHINDWB"))
				staybehindWB = TRUE;
			if(IIcon->FindToolType(toolarray,"DOCKY"))
				docky = TRUE;
		}
		if(WBmsg->sm_NumArgs > 1)
		{
			for(a = 1; a < WBmsg->sm_NumArgs; a++)
			{
				p = &(WBmsg->sm_ArgList[a]);
				IDOS->NameFromLock(p->wa_Lock, buf, 1024);
				startdir = buf;
			}
		}
	}

	if(user_appicon)
	{
		user_appicon->do_CurrentX = NO_ICON_POSITION;
		user_appicon->do_CurrentY = NO_ICON_POSITION;
	}

	old_pr_cis = main_proc->pr_CIS;
	old_pr_cos = main_proc->pr_COS;
	old_pr_consoletask = main_proc->pr_ConsoleTask;

	nil_file_handle = IDOS->Open("NIL:", MODE_NEWFILE);
	main_proc->pr_CIS = nil_file_handle;
	main_proc->pr_COS = nil_file_handle;
	main_proc->pr_ConsoleTask = (APTR)((struct FileHandle *)BADDR(nil_file_handle))->fh_Type;

	if((s = (STRPTR)strstri(IDOS->FilePart(str_config_basename), ".CFG")))
	{
		*s = 0;
	}

	/* Memory Pools used in different places */
	dir_memory_pool = IExec->AllocSysObjectTags(ASOT_MEMPOOL, ASOPOOL_MFlags, MEMF_CLEAR, ASOPOOL_Puddle, 64 * 1024, ASOPOOL_Threshold, 16 * 1024, TAG_DONE);
	general_memory_pool = IExec->AllocSysObjectTags(ASOT_MEMPOOL, ASOPOOL_MFlags, MEMF_CLEAR, ASOPOOL_Puddle, 64 * 1024, ASOPOOL_Threshold, 16 * 1024, TAG_DONE);
	help_memory_pool = IExec->AllocSysObjectTags(ASOT_MEMPOOL, ASOPOOL_MFlags, MEMF_CLEAR, ASOPOOL_Puddle, 64 * 1024, ASOPOOL_Threshold, 16 * 1024, TAG_DONE);
	filetype_memory_pool = IExec->AllocSysObjectTags(ASOT_MEMPOOL, ASOPOOL_MFlags, MEMF_CLEAR, ASOPOOL_Puddle, 64 * 1024, ASOPOOL_Threshold, 16 * 1024, TAG_DONE);
	menu_memory_pool = IExec->AllocSysObjectTags(ASOT_MEMPOOL, ASOPOOL_MFlags, MEMF_CLEAR, ASOPOOL_Puddle, 64 * 1024, ASOPOOL_Threshold, 16 * 1024, TAG_DONE);
	if(!dir_memory_pool && !general_memory_pool && !help_memory_pool && !filetype_memory_pool && !menu_memory_pool)
	{
		quit();
	}

	allocstrings();

	nullpalette = (UWORD *)astring(sizeof(UWORD) * 256);

	scrdata_norm_width = ((struct GfxBase *)(IGraphics->Data.LibBase))->NormalDisplayColumns;
	scrdata_norm_height = ((struct GfxBase *)(IGraphics->Data.LibBase))->NormalDisplayRows;

	IDOpus->FindSystemFile("DOpusRT", str_dopusrt_path, 256, SYSFILE_COMMAND);

	config = (struct Config *)astring(sizeof(struct Config));

	read_configuration(0);

	if(startdir)
	{
		strcpy((char *)config->autodirs[0], startdir);
	}

	initlistermenu();

	if(!(install_arbiter()) || !(count_port = IExec->AllocSysObject(ASOT_PORT, NULL)) || !(general_port = IExec->AllocSysObject(ASOT_PORT, NULL)) || !(arexx_port = CreateUniquePort("DOPUS", str_arexx_portname, &system_dopus_runcount)))
	{
		quit();
	}

	rexx_signalbit = 1 << arexx_port->mp_SigBit;

	if(WorkbenchBase && IWorkbench)
	{
		if(!(appmsg_port = IExec->AllocSysObject(ASOT_PORT, NULL)))
		{
			quit();
		}
	}

	if(!CxBase && ICommodities)
	{
		if(!(input_req = (struct IOStdReq *)IExec->AllocSysObjectTags(ASOT_IOREQUEST, ASOIOR_ReplyPort, general_port, TAG_DONE)))
		{
			quit();
		}
		if((IExec->OpenDevice("input.device", 0, (struct IORequest *)input_req, 0)) != 0)
		{
			IExec->FreeSysObject(ASOT_IOREQUEST, (struct IORequest *)input_req);
			input_req = NULL;
			quit();
		}
	}
	ramdisk_lock = IDOS->Lock("RAM:", ACCESS_READ);

	strcpy(str_select_pattern[0], "#?");
	strcpy(str_select_pattern[3], "#?");

	{
		struct DateTime dt;
		IDOS->DateStamp(&dt.dat_Stamp);
		initdatetime(&dt, str_select_pattern[1], NULL, 1);
	}

	init_menus();

	if(checkrunning && (IExec->FindTask("dopus_hotkeez")))
	{
		status_iconified = 1;
		switch (simplerequest(globstring[STR_DOPUS_ALREADY_RUNNING], globstring[STR_RUN], str_cancelstring, globstring[STR_BRING_TO_FRONT], NULL))
		{
		case 2:
			{
				if(CxBase && ICommodities)
				{
					struct InputEvent ie;

					ie.ie_NextEvent = NULL;
					ie.ie_Class = IECLASS_RAWKEY;
					ie.ie_Code = config->hotkeycode;
					ie.ie_Qualifier = config->hotkeyqual;
					ICommodities->AddIEvents(&ie);
				}
			}
		case 0:
			quit();
			break;
		default:
			status_iconified = 0;
		}
	}
	hotkey_task = NULL;

	data_colorclock = (scrdata_is_pal ? 3546895 : 3579545);

	setup_externals();

	do_remember_config(remember_data);

	allocdirbuffers(config->bufcount);

	for(a = 0; a < 2; a++)
	{
		horiz_propimage[a].Width = 65;
	}

	hotkey_task = (struct Task *)IExec->CreateTask("dopus_hotkeez", config->priority + 1, hotkeytaskcode, 8192, NULL);

	/* Semaphore locking for external launch */
	IExec->Forbid();
	ss = IExec->FindSemaphore("DosResident");
	IExec->Permit();
	/* END Semaphore */

	/* BEGIN: application.library code */
	if((config->icontype & ICON_APPICON) && docky)
	{
		appID = 0;
		if(WBmsg)
		{
			appID = IApplication->RegisterApplication("DOpus", REGAPP_URLIdentifier, "zerohero.se", REGAPP_WBStartup, WBmsg, TAG_DONE);
		}
		else
		{
			BPTR applock = IDOS->Lock(argv[0], ACCESS_READ);
			if(applock)
			{
				appID = IApplication->RegisterApplication("DOpus", REGAPP_URLIdentifier, "zerohero.se", REGAPP_FileLock, applock, TAG_DONE);
				IDOS->UnLock(applock);
			}
		}
		if(appID)
		{
			IApplication->GetApplicationAttrs(appID, APPATTR_Port, (uint32)&applibport, TAG_DONE);
		}
	}
	/* END: application.library code */

	if(iconstart)
	{
		SetUp(-1);
	}

	sup = nsee = 0;
	if(config->autodirs[0][0] || config->autodirs[1][0])
	{
		if(!iconstart)
		{
			sup = 1;
			SetUp(1);
			busy();
		}
		nsee = 1;
		for(a = 0; a < 2; a++)
		{
			if(config->autodirs[a][0])
			{
				strncpy(str_pathbuffer[a], (char *)config->autodirs[a], 108);
				checkdir(str_pathbuffer[a], (sup) ? &path_strgadget[a] : NULL);
				strncpy(dopus_curwin[a]->directory, str_pathbuffer[a], 108);
				getdir(dopus_curwin[a], a, 0);
			}
		}
	}
	if(iconstart)
	{
		status_configuring = 0;
		iconify(sup + 1, iconstart - 1, 0);
	}
	else if(!sup)
	{
		SetUp(1);
	}
	if(nsee)
	{
		for(a = 0; a < 2; a++)
		{
			seename(a);
			checkdir(str_pathbuffer[a], &path_strgadget[a]);
			fixhorizprop(a);
		}
	}
	if(!iconstart)
	{
		unbusy();
	}

	doidcmp();

	quit();

	return 0;
}

int SetUp(int tit)
{
	int y, a, b, c, count = 0, lim = 48, num, other_offset;
	struct Screen scrbuf, *pubscr = NULL;
	struct DimensionInfo dims;
	DisplayInfoHandle *handle;
	struct dopusgadgetbanks *bank;
	struct RastPort testrastport;

	removewindowgadgets(Window);

	IExec->SetTaskPri((struct Task *)main_proc, config->priority);
	if(hotkey_task)
	{
		IExec->SetTaskPri(hotkey_task, config->priority + 1);
	}
	status_configuring = -1;
	status_iconified = 0;

	main_scr.ViewModes = 0; //HIRES;
	config->screenmode = checkscreenmode(config->screenmode);

	main_win.Flags = WFLG_NW_EXTENDED | WFLG_NEWLOOKMENUS;
	mainwindow_tags[0].ti_Tag = TAG_SKIP;

	if(config->screenmode == MODE_PUBLICSCREEN && (pubscr = IIntuition->LockPubScreen(config->pubscreen_name)))
	{
		IExec->CopyMem((char *)pubscr, (char *)&scrbuf, sizeof(struct Screen));
	}
	else
	{
		IDOpus->GetWBScreen(&scrbuf);
		if(config->screenmode == MODE_PUBLICSCREEN)
		{
			config->screenmode = MODE_WORKBENCHUSE;
		}
	}

//	setup_draw_info();

	if(config->screenmode == MODE_WORKBENCHUSE || config->screenmode == MODE_PUBLICSCREEN)
	{
		status_publicscreen = 1;
	}
	else
	{
		status_publicscreen = 0;
		if(config->screenmode == MODE_WORKBENCHCLONE)
		{
			mainscreen_tags[0].ti_Data = clone_screen(NULL, &main_scr);
		}
		else
		{
			if((handle = IGraphics->FindDisplayInfo(config->screenmode)) && (IGraphics->GetDisplayInfoData(handle, (UBYTE *)&dims, sizeof(struct DimensionInfo), DTAG_DIMS, 0)))
			{
				mainscreen_tags[0].ti_Data = (ULONG) config->screenmode;
				if(config->screenflags & SCRFLAGS_DEFWIDTH)
					main_scr.Width = (dims.TxtOScan.MaxX - dims.TxtOScan.MinX) + 1;
				else
					main_scr.Width = config->scrw;
				if(config->screenflags & SCRFLAGS_DEFHEIGHT)
					main_scr.Height = (dims.TxtOScan.MaxY - dims.TxtOScan.MinY) + 1;
				else
					main_scr.Height = config->scrh;
			}
			else
			{
				main_scr.Width = scrbuf.Width;
				main_scr.Height = scrbuf.Height;
				main_scr.ViewModes = scrbuf.ViewPort.Modes;
			}
		}
	}

	if(config->scrdepth < 2)
	{
		config->scrdepth = 2;
	}
	main_scr.Depth = config->scrdepth;

	if(config->gadgetrows < 0 || config->gadgetrows > 6)
	{
		config->gadgetrows = 6;
	}
	scr_gadget_rows = config->gadgetrows;
	if(data_gadgetrow_offset + scr_gadget_rows > 6)
	{
		data_gadgetrow_offset = 0;
	}

      tryfonts:
	if(count == 5)
	{
		status_iconified = 1;
		simplerequest(globstring[STR_UNABLE_TO_OPEN_WINDOW], globstring[STR_CONTINUE], NULL);
		status_iconified = 0;
		quit();
	}
	else if(count == 4)
	{
		for(a = 0; a < NUMFONTS; a++)
		{
			if(config->fontsizes[a] > 8)
			{
				strcpy(config->fontbufs[a], "topaz.font");
				config->fontsizes[a] = 8;
			}
		}
	}
	else if(count)
	{
		if(Window)
		{
			busy();
		}
		for(a = 0; a < NUMFONTS; a++)
		{
			if(config->fontsizes[a] > lim)
				config->fontsizes[a] /= 2;
		}
		lim /= 2;
	}

	config->fontsizes[0] = 8;
	for(a = 0; a < FONT_LAST; a++)
	{
		freefont(a);
		b = config->fontsizes[a];
		scr_font[a] = getfont(config->fontbufs[a], &b, (a == FONT_GENERAL) || (a == FONT_TEXT));
		config->fontsizes[a] = b;
	}

	if(Window && count)
	{
		unbusy();
	}

	scrdata_font_xsize = scr_font[FONT_DIRS]->tf_XSize;
	scrdata_font_ysize = scr_font[FONT_DIRS]->tf_YSize;
	scrdata_font_baseline = scr_font[FONT_DIRS]->tf_Baseline;

	screen_attr.ta_Name = (STRPTR) config->fontbufs[FONT_GENERAL];
	menu_attr.ta_Name = (STRPTR) config->fontbufs[FONT_MENUS];
	menu_attr.ta_YSize = config->fontsizes[FONT_MENUS];
	main_screen_attr.ta_Name = (STRPTR) config->fontbufs[FONT_SCREEN];
	main_screen_attr.ta_YSize = config->fontsizes[FONT_SCREEN];

	if(tit > -1)
	{
		if(config->screenmode == MODE_WORKBENCHUSE)
		{
			main_win.Type = WBENCHSCREEN;
		}
		else if(pubscr)
		{
			main_win.Type = PUBLICSCREEN;
			mainwindow_tags[0].ti_Tag = WA_PubScreen;
			mainwindow_tags[0].ti_Data = (uint32)pubscr;
		}
		else
		{
			main_win.Type = CUSTOMSCREEN;


			if(!MainScreen)
			{
				if(config->screenmode == MODE_WORKBENCHCLONE)
				{
					MainScreen = IIntuition->OpenScreenTags(NULL, SA_Type, PUBLICSCREEN, SA_PubName, str_arexx_portname, SA_Depth, config->scrdepth, SA_LikeWorkbench, TRUE, SA_Behind, staybehindWB, TAG_DONE);
				}
				else
				{
					MainScreen = IIntuition->OpenScreenTags(NULL, SA_Type, PUBLICSCREEN, SA_PubName, str_arexx_portname, SA_DisplayID, config->screenmode, SA_Depth, config->scrdepth, SA_LikeWorkbench, TRUE, SA_Behind, staybehindWB, TAG_DONE);
				}

				if(!(MainScreen))
				{
					status_iconified = 1;
					simplerequest(globstring[STR_UNABLE_TO_OPEN_WINDOW], globstring[STR_CONTINUE], NULL);
					status_iconified = 0;
					if(config->screenmode == HIRES_KEY && config->scrw == 640 && config->scrh == 200 + (scrdata_is_pal * 56) && config->screenflags == 0 && config->scrdepth == 2)
						quit();
					config->screenmode = HIRES_KEY;
					config->scrw = 640;
					config->scrh = 200 + (scrdata_is_pal * 56);
					config->scrdepth = 2;
					config->screenflags = 0;
					if(pubscr)
					{
						IIntuition->UnlockPubScreen(NULL, pubscr);
					}
					return (SetUp(tit));
				}
//				load_palette(MainScreen, config->new_palette);  //Removed, it causes wrong colors for system stuff

				main_win.Screen = MainScreen;

				IIntuition->PubScreenStatus(MainScreen, 0);
				IExec->CopyMem((char *)MainScreen, (char *)&scrbuf, sizeof(struct Screen));
			}
		}
	}

	if(config->generalscreenflags & SCR_GENERAL_WINBORDERS && MainScreen)
		status_publicscreen = 1;

	/* Screen clock bar */
	scrdata_clock_height = scr_font[FONT_CLOCK]->tf_YSize + 2;
	if(scrdata_clock_height < 10)
		scrdata_clock_height = 10;

	/* Status bar */
	if(config->generalscreenflags & SCR_GENERAL_TITLESTATUS)
		scrdata_status_height = 0;
	else
		scrdata_status_height = scr_font[FONT_STATUS]->tf_YSize + 3;

	/* Disk name bars */
	scrdata_diskname_height = scr_font[FONT_NAMES]->tf_YSize + 2;

	/* String gadgets */
	scrdata_string_height = config->stringheight;
	if(scrdata_string_height < scr_font[FONT_STRING]->tf_YSize)
		scrdata_string_height = scr_font[FONT_STRING]->tf_YSize;

	/* Calculate window borders */
	if(status_publicscreen)
	{
		if(MainScreen)
		{
			scrdata_xoffset = MainScreen->WBorLeft;
//			scrdata_yoffset = MainScreen->WBorTop + scr_font[FONT_SCREEN]->tf_YSize + 1;
			scrdata_yoffset = MainScreen->WBorTop + MainScreen->Font->ta_YSize + 1; // Fixes some corruption on own screen on OS4.x it seems
		}
		else
		{
			scrdata_xoffset = scrbuf.WBorLeft;
			scrdata_yoffset = scrbuf.WBorTop + scrbuf.Font->ta_YSize + 1;
		}
	}
	else
	{
		scrdata_xoffset = 0;

		if(MainScreen)
			scrdata_yoffset = MainScreen->BarHeight + 1;
		else
			scrdata_yoffset = ((struct GfxBase *)(IGraphics->Data.LibBase))->DefaultFont->tf_YSize + 3;
	}

	/* Calculcate minimum window width and height */
	scr_min_width = (config->sliderwidth + 8) * 2 + (config->arrowsize[1] + config->arrowsize[2]) * 4;
	if(scr_gadget_rows)
		scr_min_width += 8 * (scr_font[FONT_GADGETS]->tf_XSize * 2);
	scr_min_width += scrdata_font_xsize * 8;

	scr_min_height = ((status_publicscreen) ? scrdata_yoffset : 0) + scrdata_status_height + scrdata_diskname_height + (config->arrowsize[0] * 3) + config->sliderheight + scrdata_string_height + scrdata_clock_height + (scr_font[FONT_GADGETS]->tf_YSize * 2) + 20;

	if(main_scr.Width < scr_min_width)
		main_scr.Width = scr_min_width;
	if(main_scr.Height < scr_min_height)
		main_scr.Height = scr_min_height;
	if(Window)
		IIntuition->WindowLimits(Window, scr_min_width, scr_min_height, -1, -1);
	else
	{
		main_win.MinWidth = scr_min_width;
		main_win.MinHeight = scr_min_height;
	}

	/* Get window size */
	if(status_publicscreen)
		main_win.Width = config->scr_winw;
	else
		main_win.Width = config->scrw;
	if(config->screenflags & SCRFLAGS_DEFWIDTH || main_win.Width < 0)
		main_win.Width = scrbuf.Width;
	if(main_win.Width > scrbuf.Width)
		main_win.Width = scrbuf.Width;

	if(status_publicscreen)
		main_win.Height = config->scr_winh;
	else
		main_win.Height = main_scr.Height;
	if(config->screenflags & SCRFLAGS_DEFHEIGHT || main_win.Height < 0)
	{
		main_win.Height = scrbuf.Height;
		if(status_publicscreen)
		{
			config->scr_winy = scrbuf.BarHeight + 1;
			main_win.Height -= config->scr_winy;
		}
	}
	if(main_win.Height > scrbuf.Height)
		main_win.Height = scrbuf.Height;

	/* If window is already open use it's dimensions (this is a mess actually) */
	if(Window)
	{
		main_win.Width = Window->Width;
		main_win.Height = Window->Height;
	}

	/* Adjust for window borders */
	if(status_publicscreen)
	{
		scrdata_width = main_win.Width - scrdata_xoffset - scrbuf.WBorRight;
		scrdata_height = main_win.Height - scrdata_yoffset - scrbuf.WBorBottom;
	}
	else
	{
		scrdata_width = main_scr.Width - scrdata_xoffset;
		scrdata_height = main_scr.Height - scrdata_yoffset;
	}

	/* Screen clock bar */
	scrdata_clock_width = scrdata_width - ((config->generalscreenflags & SCR_GENERAL_TINYGADS) ? 115 : 1);
	scrdata_clock_xpos = scrdata_xoffset + 2;
	scrdata_clock_ypos = (scrdata_yoffset + scrdata_height + 1) - scrdata_clock_height + 1;

	/* Disk name bars */
	scrdata_diskname_ypos = scrdata_status_height + 1 + scrdata_yoffset;

	/* Fuck knows */
	scrdata_scr_height = scrdata_height;
	scrdata_gadget_height = scr_font[FONT_GADGETS]->tf_YSize + 2;

	for(;;)
	{
		other_offset = (scrdata_diskname_ypos - scrdata_yoffset) + scrdata_diskname_height + scrdata_string_height + config->sliderheight + (scr_gadget_rows * scrdata_gadget_height) + scrdata_clock_height + 9;

		scrdata_dispwin_ypos = scrdata_diskname_ypos + scrdata_diskname_height;

		scrdata_dispwin_height = scrdata_scr_height - other_offset;

		if(((config->arrowsize[0] * 2) + 8 + scrdata_font_ysize) > scrdata_dispwin_height || (other_offset > scrdata_scr_height - (config->sliderheight + 32)))
		{

			if(scr_gadget_rows == 6)
				scr_gadget_rows = 3;
			else if(scr_gadget_rows > 0)
				--scr_gadget_rows;
			else
			{
				++count;
				goto tryfonts;
			}
			continue;
		}
		else
		{
			break;
		}
	}

	scrdata_dispwin_lines = scrdata_dispwin_height / scrdata_font_ysize;
	scrdata_dirwin_height = scrdata_dispwin_lines * scrdata_font_ysize;
	scrdata_dirwin_bottom = scrdata_yoffset + (scrdata_scr_height - scrdata_string_height - config->sliderheight - (scr_gadget_rows * scrdata_gadget_height) - scrdata_clock_height - 32);

	scrdata_gadget_ypos = scrdata_dirwin_bottom + 33 + scrdata_string_height + config->sliderheight;

	scrdata_prop_height = scrdata_dispwin_height - ((config->arrowsize[0] + 2) * 2);

	scrdata_gadget_width = scrdata_width / 8;
	scrdata_drive_width = 0;
	IGraphics->InitRastPort(&testrastport);
	IGraphics->SetFont(&testrastport, scr_font[FONT_GADGETS]);
	for(a = 0; a < USEDRIVECOUNT; a++)
	{
		if((b = IGraphics->TextLength(&testrastport, config->drive[a].name, strlen(config->drive[a].name))) > scrdata_drive_width)
			scrdata_drive_width = b;
	}
	if(scrdata_drive_width > 0)
	{
		scrdata_drive_width += 4;
		if(scrdata_drive_width <= scrdata_gadget_width)
			scrdata_gadget_width = ((scrdata_width - scrdata_drive_width) / 7);
		scrdata_drive_width = scrdata_width - (scrdata_gadget_width * 7);
	}
	else
	{
		scrdata_drive_width = 0;
		scrdata_gadget_width = scrdata_width / 7;
	}

	IGraphics->SetFont(&testrastport, scr_font[FONT_DIRS]);
	getprotdatelengths(&testrastport);

	string_extend.Font = scr_font[FONT_STRING];
	if(config->stringfgcol == config->stringbgcol)
		++config->stringfgcol;
	if(config->stringselfgcol == config->stringselbgcol)
		++config->stringselfgcol;
	path_stringinfo[0].Extension = &string_extend;
	path_stringinfo[1].Extension = &string_extend;

	if(scr_gadget_rows)
	{
		bank = dopus_firstgadbank;
		for(num = 0; bank; num++)
			bank = bank->next;
		scr_gadget_bank_count = (num * 6) / scr_gadget_rows;
	}
	else
		scr_gadget_bank_count = 0;

	if(config->generalscreenflags & SCR_GENERAL_GADSLIDERS && scr_gadget_rows)
	{
		int slider_width = 0;

		if(scrdata_drive_width)
			slider_width += 16;
		if(scr_gadget_bank_count > 1)
			slider_width += 16;

		scrdata_gadget_width = ((scrdata_width - slider_width - scrdata_drive_width) / 7);
		a = scrdata_width - ((scrdata_gadget_width * 7) + scrdata_drive_width);

		if(scrdata_drive_width)
		{
			if(scr_gadget_bank_count > 1)
			{
				drive_propgad.Width = gadget_propgad.Width = (a / 2) - 8;
				if(a % 2)
					++gadget_propgad.Width;
			}
			else
			{
				drive_propgad.Width = a - 8;
				if(drive_propgad.Width > 8)
				{
					scrdata_drive_width += drive_propgad.Width - 8;
					drive_propgad.Width = 8;
				}
			}
		}
		else
			gadget_propgad.Width = a - 8;
	}

	scrdata_gadget_offset = (scrdata_width - ((scrdata_gadget_width * 7) + scrdata_drive_width)) / 2;
	if(config->generalscreenflags & SCR_GENERAL_GADSLIDERS && scr_gadget_rows)
	{
		if(scr_gadget_bank_count > 1)
			scrdata_gadget_offset -= gadget_propgad.Width + 8;
		else if(scrdata_drive_width)
			scrdata_gadget_offset -= drive_propgad.Width + 8;
	}
	if(scrdata_gadget_offset > scrdata_width)
		scrdata_gadget_offset = 0;

	scrdata_gadget_offset += scrdata_xoffset;
	scrdata_gadget_xpos = scrdata_gadget_offset + scrdata_drive_width;

	if(config->generalscreenflags&SCR_GENERAL_NEWLOOKPROP)
	{
		status_flags |= STATUS_NEWLOOK;
		for(a = 0; a < 2; a++)
		{
			vert_propinfo[a].Flags = AUTOKNOB | PROPNEWLOOK | FREEVERT | PROPBORDERLESS;
			horiz_propinfo[a].Flags = AUTOKNOB | PROPNEWLOOK | FREEHORIZ | PROPBORDERLESS;
		}
		drive_propinfo.Flags |= AUTOKNOB | PROPNEWLOOK | FREEVERT | PROPBORDERLESS;
		gadget_propinfo.Flags |= AUTOKNOB | PROPNEWLOOK | FREEVERT | PROPBORDERLESS;
	}
	else
	{
		status_flags &= ~STATUS_NEWLOOK;
		for(a = 0; a < 2; a++)
		{
			vert_propinfo[a].Flags=FREEVERT|PROPBORDERLESS;
			horiz_propinfo[a].Flags=FREEHORIZ|PROPBORDERLESS;
		}
		drive_propinfo.Flags=FREEVERT|PROPBORDERLESS;
		gadget_propinfo.Flags=FREEVERT|PROPBORDERLESS;
	}

	if(config->generalscreenflags & SCR_GENERAL_GADSLIDERS && scr_gadget_rows)
	{

		if(scrdata_drive_width > 0)
		{
			scrdata_gadget_xpos += drive_propgad.Width + 8;

			drive_propgad.LeftEdge = scrdata_gadget_offset + scrdata_drive_width + 4;
			drive_propgad.TopEdge = scrdata_gadget_ypos + 1;
			drive_propgad.Height = scr_gadget_rows * scrdata_gadget_height - 4;
			drive_propimage.Width = drive_propgad.Width;
			drive_propimage.Height = drive_propgad.Height;
			IDOpus->FixSliderBody(NULL, &drive_propgad, USEDRIVECOUNT, scr_gadget_rows, 0);
		}

		if(scr_gadget_bank_count > 1)
		{
			gadget_propgad.LeftEdge = scrdata_gadget_xpos + (scrdata_gadget_width * 7) + 4;
			gadget_propgad.TopEdge = scrdata_gadget_ypos + 1;
			gadget_propgad.Height = scr_gadget_rows * scrdata_gadget_height - 4;
			gadget_propimage.Width = gadget_propgad.Width;
			gadget_propimage.Height = gadget_propgad.Height;
			IDOpus->FixSliderBody(NULL, &gadget_propgad, scr_gadget_bank_count, 1, 0);
		}

		if(status_flags & STATUS_NEWLOOK)
			gadget_propimage.Height = 0;
	}

	if(tit > -1)
	{
		layoutcenter(config->windowdelta);

		if(config->generalscreenflags & SCR_GENERAL_TINYGADS)
		{
			for(a = 0; a < 6; a++)
			{
				screen_gadgets[SCRGAD_TINYHELP + a].LeftEdge = scrdata_clock_xpos + scrdata_clock_width + (a * 19) - 1;
				screen_gadgets[SCRGAD_TINYHELP + a].TopEdge = scrdata_clock_ypos - 2;
				screen_gadgets[SCRGAD_TINYHELP + a].Height = scrdata_clock_height;
			}
			screen_gadgets[SCRGAD_TINYHELP].Flags &= ~GFLG_SELECTED;
			screen_gadgets[SCRGAD_TINYAREXX].NextGadget = &screen_gadgets[SCRGAD_TINYHELP];
		}
		else
		{
			screen_gadgets[SCRGAD_TINYAREXX].NextGadget = &screen_gadgets[SCRGAD_MOVELEFT1];
		}

		for(y = 0; y < 2; y++)
		{
			fixhlen(y);
			vert_propimage[y].LeftEdge = horiz_propimage[y].LeftEdge = 0;
			vert_propimage[y].TopEdge = horiz_propimage[y].TopEdge = 0;
			vert_propimage[y].Width = config->sliderwidth;
			vert_propimage[y].Height = 0;
			horiz_propimage[y].Height = config->sliderheight;
			vert_propimage[y].ImageData = horiz_propimage[y].ImageData = NULL;
			vert_propimage[y].PlanePick = horiz_propimage[y].PlanePick = 0;
			vert_propimage[y].NextImage = horiz_propimage[y].NextImage = NULL;
			IDOpus->FixSliderBody(NULL, &horiz_propgad[y], dopus_curwin[y]->hlen, scrdata_dispwin_nchars[y], 0);
		}

		if(status_publicscreen)
		{
			if(MainScreen)
			{
				main_win.LeftEdge = config->scr_winx;
				main_win.TopEdge = config->scr_winy;
			}
			else
			{
				main_win.LeftEdge = config->wbwinx;
				main_win.TopEdge = config->wbwiny;
			}
			if(main_win.LeftEdge + main_win.Width > scrbuf.Width)
				main_win.LeftEdge = scrbuf.Width - main_win.Width;
			if(main_win.TopEdge + main_win.Height > scrbuf.Height)
				main_win.TopEdge = scrbuf.Height - main_win.Height;
			if(main_win.LeftEdge < 0)
				main_win.LeftEdge = 0;
			if(main_win.TopEdge < 0)
				main_win.TopEdge = 0;
			main_win.Flags |= WFLG_ACTIVATE | WFLG_RMBTRAP | WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET;
			main_win.Title = str_arexx_portname;
		}
		else
		{
			main_win.LeftEdge = 0;
			main_win.TopEdge = 0;
			main_win.Flags |= WFLG_ACTIVATE | WFLG_BACKDROP | WFLG_RMBTRAP | WFLG_BORDERLESS;
			main_win.Title = NULL;
		}

		if(!Window)
		{
			if(!(Window = IIntuition->OpenWindow((struct NewWindow *)&main_win)))
			{
				if(MainScreen)
				{
					IIntuition->ScreenToFront(MainScreen);
				}
				simplerequest(globstring[STR_UNABLE_TO_OPEN_WINDOW], globstring[STR_CONTINUE], NULL);
				if(config->screenmode == HIRES_KEY && config->scr_winw == 640 && config->scr_winh == 200 + (scrdata_is_pal * 56) && config->scrdepth == 2 && config->screenflags == 0)
					quit();
				config->screenmode = HIRES_KEY;
				config->scr_winw = 640;
				config->scr_winh = 200 + (scrdata_is_pal * 56);
				config->scrdepth = 2;
				config->screenflags = 0;
				if(MainScreen)
					IIntuition->CloseScreen(MainScreen);
				MainScreen = NULL;
				if(pubscr)
					IIntuition->UnlockPubScreen(NULL, pubscr);
				return (SetUp(tit));
			}

			/* Iconify Gadget Code */
			{
				DRI = IIntuition->GetScreenDrawInfo(Window->WScreen);
				iconifyimage = (struct Image *)IIntuition->NewObject(NULL, "sysiclass", SYSIA_DrawInfo, DRI, SYSIA_Which, ICONIFYIMAGE, TAG_END);
				if(iconifyimage)
				{
					iconifygadget = (struct Gadget *)IIntuition->NewObject(NULL, "buttongclass", GA_ID, FUNC_ICONIFY, GA_RelVerify, TRUE, GA_Image, iconifyimage, GA_TopBorder, TRUE, GA_RelRight, 0, GA_Titlebar, TRUE, TAG_END);
				}
				IIntuition->FreeScreenDrawInfo(Window->WScreen, DRI);
				
			}
			/* Iconify Gadget Code */
			
			main_win.Width = Window->Width;
			main_win.Height = Window->Height;
			if(pubscr)
				IIntuition->ScreenToFront(pubscr);
			get_colour_table();
		}
		else
		{
			IIntuition->RefreshWindowFrame(Window);
			IIntuition->RefreshGadgets(iconifygadget, Window, NULL);
			IGraphics->SetAPen(main_rp, 0);
			rectfill(main_rp, scrdata_xoffset, scrdata_yoffset, scrdata_width, scrdata_height);
		}

		main_rp = Window->RPort;
		main_rp->GelsInfo = &drag_gelsinfo;
		IGraphics->InitGels(&drag_dummy_sprite1, &drag_dummy_sprite2, &drag_gelsinfo);

		string_extend.Pens[0] = screen_pens[config->stringfgcol].pen;
		string_extend.Pens[1] = screen_pens[config->stringbgcol].pen;
		string_extend.ActivePens[0] = screen_pens[config->stringselfgcol].pen;
		string_extend.ActivePens[1] = screen_pens[config->stringselbgcol].pen;

		drive_propimage.PlaneOnOff = screen_pens[config->slidercol].pen;
		gadget_propimage.PlaneOnOff = screen_pens[config->slidercol].pen;
		drive_propgad.MutualExclude = screen_pens[config->sliderbgcol].pen;
		gadget_propgad.MutualExclude = screen_pens[config->sliderbgcol].pen;

		for(y = 0; y < 2; y++)
		{
			vert_propimage[y].PlaneOnOff = screen_pens[config->slidercol].pen;
			horiz_propimage[y].PlaneOnOff = screen_pens[config->slidercol].pen;
			vert_propimage[y].NextImage = horiz_propimage[y].NextImage = NULL;
			vert_propgad[y].MutualExclude = screen_pens[config->sliderbgcol].pen;
			horiz_propgad[y].MutualExclude = screen_pens[config->sliderbgcol].pen;
		}

		if(WorkbenchBase && IWorkbench && !dopus_appwindow) // Fixes AppWindow problem
			dopus_appwindow = IWorkbench->AddAppWindowA(APPWINID, 0, Window, appmsg_port, NULL);

		if(status_publicscreen)
		{
//			if(WorkbenchBase && IWorkbench && !dopus_appwindow)
//				dopus_appwindow = IWorkbench->AddAppWindowA(APPWINID, 0, Window, appmsg_port, NULL);

			IIntuition->SetWindowTitles(Window, (char *)-1, str_arexx_portname);
			screen_gadgets[SCRGAD_LEFTPARENT].Width = Window->BorderLeft + 2;
			screen_gadgets[SCRGAD_RIGHTPARENT].Width = Window->BorderRight + 2;

			size_gadgets[0].LeftEdge = Window->Width - Window->BorderRight - 1;
			size_gadgets[0].TopEdge = Window->Height - 16;
			size_gadgets[0].Width = Window->BorderRight + 1;
			size_gadgets[0].Height = 17;

			size_gadgets[1].LeftEdge = Window->Width - 32;
			size_gadgets[1].TopEdge = Window->Height - Window->BorderBottom - 1;
			size_gadgets[1].Width = 33;
			size_gadgets[1].Height = Window->BorderBottom + 1;
		}
		else
		{
			screen_gadgets[SCRGAD_LEFTPARENT].Width = 2;
			screen_gadgets[SCRGAD_RIGHTPARENT].Width = 2;
		}

		if((Window->WScreen->Width / Window->WScreen->Height) == 1)
			status_flags |= STATUS_SQUAREPIXEL;
		else
			status_flags &= ~STATUS_SQUAREPIXEL;

		if(config->generalflags & GENERAL_DRAG)
			allocdragbuffers();

		b = (scrdata_dispwin_height - scrdata_dirwin_height) / 2;
		if(b < 0)
			b = 0;

		if(scrdata_dispwin_lines < 5)
			c = scrdata_font_ysize / 2;
		else if(scrdata_dispwin_lines < 9)
			c = scrdata_font_ysize * 2;
		else
			c = scrdata_font_ysize * 3;

		for(a = 0; a < 2; a++)
		{
			scrdata_dirwin_ypos[a] = scrdata_dispwin_ypos + b;
			IExec->CopyMem((char *)main_rp, (char *)&dir_rp[a], sizeof(struct RastPort));
			IGraphics->SetFont(&dir_rp[a], scr_font[FONT_DIRS]);
			IGraphics->SetDrMd(&dir_rp[a], JAM2);

			scr_scroll_borders[a].MinX = scrdata_dirwin_xpos[a] + (((scrdata_dispwin_nchars[a] / 2) - 6));
			scr_scroll_borders[a].MaxX = scrdata_dirwin_xpos[a] + (((scrdata_dispwin_nchars[a] / 2) + 6));
			scr_scroll_borders[a].MinY = scrdata_dirwin_ypos[a] + (((scrdata_dispwin_lines / 2) * scrdata_font_ysize) - c);
			scr_scroll_borders[a].MaxY = scrdata_dirwin_ypos[a] + (((scrdata_dispwin_lines / 2) * scrdata_font_ysize) + c);
		}

		if(scrdata_status_height > 0)
		{
			scrdata_status_width = scrdata_width - 4;
		}
		else
		{
			struct Gadget *gad;
			int min = 0, max = 0;

			if(status_publicscreen)
			{
				gad = Window->FirstGadget;

				while(gad)
				{
					if(gad->GadgetType & GTYP_SYSGADGET && gad->TopEdge == 0)
					{
						if(gad->LeftEdge < 0)
						{
							if(gad->LeftEdge < min)
								min = gad->LeftEdge;
						}
						else if(gad->LeftEdge + gad->Width > max)
							max = gad->LeftEdge + gad->Width;
					}
					gad = gad->NextGadget;
				}

				scrdata_status_width = Window->Width - max + min - 8 - Window->WScreen->BarHBorder;
			}
			else
			{
				gad = MainScreen->FirstGadget;

				while(gad)
				{
					if(gad->LeftEdge < min)
						min = gad->LeftEdge;
					gad = gad->NextGadget;
				}

				scrdata_status_width = MainScreen->Width + min - 8 - MainScreen->BarHBorder;
			}
		}

		scrdata_status_xpos = 2 + scrdata_xoffset;
		scrdata_status_ypos = 1 + scrdata_yoffset;

		if(MainScreen && !staybehindWB)
			IIntuition->ScreenToFront(MainScreen);
		IDOS->SetProcWindow((config->errorflags & ERROR_ENABLE_DOS) ? Window : (APTR) - 1L);

		layout_menus();

		setupdisplay(1);

		initclock();
	}

	status_iconified = 0;
	if(tit == 1)
		dostatustext(globstring[STR_WELCOME_TO_DOPUS]);
	else if(tit == 2)
		dostatustext(globstring[STR_WELCOME_BACK_TO_DOPUS]);
	if(tit > -1 && !(status_flags & STATUS_DONEREXX))
	{
		status_flags |= STATUS_DONEREXX;
		rexx_command(config->startupscript, NULL);
	}
	status_configuring = 0;

	if(pubscr)
		IIntuition->UnlockPubScreen(NULL, pubscr);
	return (1);
}

void setupdisplay(int all)
{
	int a;

	drawscreen();
	for(a = 0; a < 2; a++)
	{
		if(config->generalscreenflags & SCR_GENERAL_NEWLOOKPROP)
		{
			IIntuition->RefreshGList(horiz_propgad, Window, NULL, 2);
		}
		fixhorizprop(a);
		if(!all)
		{
			doposhprop(a);
		}
	}
	setupgadgets();
	drawgadgets((all) ? 1 : -1, 0);
	fix_rmb_gadgets();
	if(config->generalscreenflags & SCR_GENERAL_TINYGADS)
	{
		dolittlegads(&screen_gadgets[SCRGAD_TINYFILTER], &globstring[STR_TINY_BUTTONS][6], 1);
	}

	if(scr_gadget_rows)
	{
		if(config->generalscreenflags & SCR_GENERAL_GADSLIDERS)
		{
			if(scrdata_drive_width)
			{
				IDOpus->FixSliderPot(NULL, &drive_propgad, data_drive_offset, USEDRIVECOUNT, scr_gadget_rows, 0);
				IDOpus->ShowSlider(Window, &drive_propgad);
			}
			if(scr_gadget_bank_count > 1)
			{
				fixgadgetprop();
				IDOpus->ShowSlider(Window, &gadget_propgad);
			}
		}
		data_drive_offset -= scr_gadget_rows;
		nextdrives();
	}

	for(a = 0; a < 2; a++)
	{
		refreshwindow(a, 3);
		doposprop(a);
	}
}

void drawscreen()
{
	int a;

	IGraphics->SetDrMd(main_rp, JAM1);

	dolittlegads(&screen_gadgets[SCRGAD_TINYBUFFERLIST], globstring[STR_TINY_BUTTONS], config->generalscreenflags & SCR_GENERAL_TINYGADS ? 10 : 4);

	/* Directory Windows */
	for(a = 0; a < 2; a++)
	{
		IGraphics->SetAPen(main_rp, screen_pens[config->filesbg].pen);
		rectfill(main_rp, scrdata_dispwin_xpos[a] + 2, scrdata_dispwin_ypos, scrdata_dispwin_width[a], scrdata_dispwin_height);
		do3dbox(main_rp, scrdata_dispwin_xpos[a] + 2, scrdata_dispwin_ypos, scrdata_dispwin_width[a], scrdata_dispwin_height);
		do3dbox(main_rp, scrdata_diskname_xpos[a] + 2, scrdata_diskname_ypos, scrdata_diskname_width[a], scrdata_diskname_height - 2);
	}

	/* Sliders/String gadgets */
	for(a = 0; a < 2; a++)
	{
		do3dbox(main_rp, vert_propgad[a].LeftEdge - 2, vert_propgad[a].TopEdge - 1, config->sliderwidth + 4, scrdata_prop_height + 2);
		do3dbox(main_rp, horiz_propgad[a].LeftEdge - 2, horiz_propgad[a].TopEdge - 1, horiz_propgad[a].Width + 4, config->sliderheight + 2);
		IDOpus->Do3DStringBox(main_rp, path_strgadget[a].LeftEdge, path_strgadget[a].TopEdge, path_strgadget[a].Width, scrdata_string_height, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);
	}

	/* Arrows */
	doarrowgadgets(&screen_gadgets[SCRGAD_MOVEUP1], 0);
	doarrowgadgets(&screen_gadgets[SCRGAD_MOVEDOWN1], 1);
	doarrowgadgets(&screen_gadgets[SCRGAD_MOVELEFT1], 3);
	doarrowgadgets(&screen_gadgets[SCRGAD_MOVERIGHT1], 2);
	doarrowgadgets(&screen_gadgets[SCRGAD_BUFFERLEFT1], 3);
	doarrowgadgets(&screen_gadgets[SCRGAD_BUFFERRIGHT1], 2);

	/* Status bar */
	if(scrdata_status_height > 0)
	{
		do3dbox(main_rp, scrdata_status_xpos, scrdata_status_ypos, scrdata_status_width, scrdata_status_height - 2);
		IGraphics->SetAPen(main_rp, screen_pens[config->statusbg].pen);
		rectfill(main_rp, scrdata_status_xpos, scrdata_status_ypos, scrdata_status_width, scrdata_status_height - 2);
	}

	/* Clock bar */
	if(!clock_task)
	{
		IGraphics->SetAPen(main_rp, screen_pens[config->clockbg].pen);
		rectfill(main_rp, scrdata_clock_xpos, scrdata_clock_ypos, scrdata_clock_width - 3, scrdata_clock_height - 2);
		do3dbox(main_rp, scrdata_clock_xpos, scrdata_clock_ypos - 1, scrdata_clock_width - 3, scrdata_clock_height - 2);
	}

	doactive(0, 0);

	IGraphics->SetAPen(main_rp, screen_pens[1].pen);
}

struct TextFont *getfont(STRPTR font, int *size, int noprop)
{
	struct TextFont *tf = NULL;
	static struct TextAttr sfont = { NULL, 0, 0, 0 };

	sfont.ta_Name = (STRPTR)font;
	sfont.ta_YSize = *size;
	if((tf = IGraphics->OpenFont(&sfont)) && tf->tf_YSize == sfont.ta_YSize && (!(tf->tf_Flags & FPF_PROPORTIONAL) || !noprop))
	{
		return (tf);
	}

	if(!DiskfontBase || !(tf = IDiskfont->OpenDiskFont(&sfont)))
	{
		if((tf = IGraphics->OpenFont(&sfont)))
		{
			*size = tf->tf_YSize;
		}
	}
	if(!tf || (tf->tf_Flags & FPF_PROPORTIONAL && noprop))
	{
		if(tf)
		{
			IGraphics->CloseFont(tf);
		}
		strcpy(font, "topaz.font");
		*size = 8;
		sfont.ta_YSize = 8;
		tf = IDiskfont->OpenDiskFont(&sfont);
	}
	return (tf);
}

void allocstrings()
{
	int a;
	for(a = 0; a < 16; a++)
		rexx_args[a] = astring(256);
	str_arexx_command = astring(256);
	for(a = 0; a < 4; a++)
		str_select_pattern[a] = astring(80);
	for(a = 0; a < 2; a++)
	{
		rexx_pathbuffer[a] = astring(256);
		dopus_specialwin[a] = (struct DirectoryWindow *)astring(sizeof(struct DirectoryWindow));
	}
	menu_menuitem = (struct MenuItem *)astring(MENUCOUNT * sizeof(struct MenuItem));
	menu_itemnames = astring(MENUCOUNT * MAXMENULENGTH);
	menu_intuitext = (struct IntuiText *)astring(MENUCOUNT * 2 * sizeof(struct IntuiText));
	main_gadgets = (struct Gadget *)astring((GADCOUNT / 2) * sizeof(struct Gadget));
	drive_gadgets = (struct Gadget *)astring(6 * sizeof(struct Gadget));
	str_space_string = astring(MAXDISPLAYLENGTH);
	for(a = 0; a < MAXDISPLAYLENGTH - 1; a++)
	{
		str_space_string[a] = ' ';
	}
	str_last_statustext = astring(512);
	remember_data = (struct RememberData *)astring(sizeof(struct RememberData));
}

char *astring(int len)
{
	STRPTR foo;
	if(!(foo = (STRPTR)IExec->AllocPooled(general_memory_pool, len)))
	{
		quit();
	}
	return (foo);
}

void allocdragbuffers()
{
	int a, w;

	freedragbuffers();
	drag_sprite.Depth = Window->WScreen->RastPort.BitMap->Depth;

	a = 35;
	w = (gettextlength(scr_font[FONT_DIRS], str_space_string, &a, 0) + 15);
	a = (scrdata_dispwin_width[0] > scrdata_dispwin_width[1]) ? scrdata_dispwin_width[0] : scrdata_dispwin_width[1];
	if(w > a)
		w = a;
	drag_sprite.Width = w / 16;
	drag_sprite.Height = scr_font[FONT_DIRS]->tf_YSize;

	if(!(drag_bob_buffer = IGraphics->AllocRaster(drag_sprite.Width * 16, drag_sprite.Height * drag_sprite.Depth)))
		return;
	if(!(drag_bob_savebuffer = IGraphics->AllocRaster(drag_sprite.Width * 16, drag_sprite.Height * drag_sprite.Depth)))
	{
		freedragbuffers();
		return;
	}

	IGraphics->InitBitMap(&drag_bob_bitmap, drag_sprite.Depth, drag_sprite.Width * 16, drag_sprite.Height);
	for(a = 0; a < drag_sprite.Depth; a++)
	{
		drag_bob_bitmap.Planes[a] = drag_bob_buffer + (RASSIZE(drag_sprite.Width * 16, drag_sprite.Height) * a);
	}
	for(; a < 8; a++)
		drag_bob_bitmap.Planes[a] = NULL;

	IGraphics->InitRastPort(&drag_bob_rastport);
	drag_bob_rastport.BitMap = &drag_bob_bitmap;
	IGraphics->SetFont(&drag_bob_rastport, scr_font[FONT_DIRS]);
	IGraphics->SetDrMd(&drag_bob_rastport, JAM1);
	drag_sprite.ImageData = (WORD *) drag_bob_buffer;
	drag_sprite.PlanePick = (1 << drag_sprite.Depth) - 1;
	drag_bob.SaveBuffer = (WORD *) drag_bob_savebuffer;
}

void freedragbuffers()
{
	if(drag_bob_buffer)
		IGraphics->FreeRaster(drag_bob_buffer, drag_sprite.Width * 16, drag_sprite.Height * drag_sprite.Depth);
	if(drag_bob_savebuffer)
		IGraphics->FreeRaster(drag_bob_savebuffer, drag_sprite.Width * 16, drag_sprite.Height * drag_sprite.Depth);
	drag_bob_buffer = drag_bob_savebuffer = NULL;
}

void load_palette(struct Screen *screen, uint32 *palette)
{
	ULONG backup_palette[98];
	int numcols;

	numcols = 1 << ((screen->RastPort.BitMap->Depth > 4) ? 4 : screen->RastPort.BitMap->Depth);
	if(numcols > 32)
	{
		numcols = 32;
	}

	IExec->CopyMem((char *)palette, (char *)&backup_palette[1], numcols * 3 * sizeof(ULONG));
	backup_palette[0] = numcols << 16;
	backup_palette[(numcols * 3) + 1] = 0;

	IGraphics->LoadRGB32(&screen->ViewPort, backup_palette);
}

void get_palette(struct Screen *screen, uint32 *palette)
{
	int numcols;

	numcols = 1 << ((screen->RastPort.BitMap->Depth > 4) ? 4 : screen->RastPort.BitMap->Depth);

	IGraphics->GetRGB32(screen->ViewPort.ColorMap, 0, numcols, palette);
}

void read_configuration(int def)
{
	char buf[256];

	freedynamiccfg();
	get_config_file(buf, ".CFG");
	if(def || !(readsetup(buf)))
	{
		getdefaultconfig();
	}
	if(LocaleBase && ILocale)
	{
		int i;

		if((locale = ILocale->OpenLocale(NULL)))
		{
			for(i = 0; locale->loc_LanguageName && (locale->loc_LanguageName[i] != '.'); i++)
				config->language[i] = locale->loc_LanguageName[i];
			config->language[i] = 0;
		}
	}
	read_data_files(1);
}

void read_data_files(int fb)
{
	char buf[256] = { 0, };

	if(get_data_file(buf, "HLP", fb))
	{
		readhelp(buf);
	}
	else if(get_data_file(buf, "guide", fb))
	{
		readhelp(NULL);
	}

	readstrings("dopus4.catalog");
}

int get_data_file(STRPTR buf, STRPTR suff, int fb)
{
	char temp[40];

	sprintf(temp, "DO_%s.%s", config->language, suff);

	if(!config->language[0] || !(IDOpus->FindSystemFile(temp, buf, 256, SYSFILE_DATA)))
	{
		if(!fb)
		{
			buf[0] = 0;
			return (0);
		}
		sprintf(temp, "DirectoryOpus.%s", suff);
		IDOpus->FindSystemFile(temp, buf, 256, SYSFILE_DATA);
	}
	return (1);
}

void get_config_file(STRPTR buf, STRPTR suff)
{
	char temp[256];

	strncpy(temp, str_config_basename, 256);
	strncat(temp, suff, 256);
//	IDOpus->StrCombine(temp, str_config_basename, suff, 256);
	if(IDOpus->CheckExist(temp, NULL) < 0)
	{
		strcpy(buf, temp);
		return;
	}
	if(IDOS->FilePart(str_config_basename))
	{
		strncpy(temp, IDOS->FilePart(str_config_basename), 256);
		strncat(temp, suff, 256);
//		IDOpus->StrCombine(temp, IDOS->FilePart(str_config_basename), suff, 256);
	}
	if(!(IDOpus->FindSystemFile(temp, buf, 256, SYSFILE_DATA)))
	{
		strncpy(temp, "DirectoryOpus", 256);
		strncat(temp, suff, 256);
//		IDOpus->StrCombine(temp, "DirectoryOpus", suff, 256);
		if(!(IDOpus->FindSystemFile(temp, buf, 256, SYSFILE_DATA)))
		{
			if(IDOpus->CheckExist("PROGDIR:S", NULL))
			{
				strncpy(buf, "PROGDIR:S/DirectoryOpus", 256);
				strncat(buf, suff, 256);
//				IDOpus->StrCombine(buf, "PROGDIR:S/DirectoryOpus", suff, 256);
			}
		}
	}
}

void setup_draw_info()
{
/*	int a, b;
	struct DrawInfo *drinfo;
	struct Screen *wbscreen;

	mainscreen_tags[0].ti_Data = 0;
	if((wbscreen = IIntuition->LockPubScreen(NULL)))
	{
		drinfo = IIntuition->GetScreenDrawInfo(wbscreen);
		b = drinfo->dri_NumPens;
		if(b > NUMDRIPENS)
			b = NUMDRIPENS;
		for(a = 0; a < b; a++)
			scr_drawinfo[a] = drinfo->dri_Pens[a];
		IIntuition->FreeScreenDrawInfo(wbscreen, drinfo);
		IIntuition->UnlockPubScreen(NULL, wbscreen);
	}

	scr_drawinfo[SHINEPEN] = config->gadgettopcol;
	scr_drawinfo[SHADOWPEN] = config->gadgetbotcol;
	scr_drawinfo[NUMDRIPENS] = (USHORT) ~ 0;*/
}
