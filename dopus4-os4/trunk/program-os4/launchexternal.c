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
#include "view.h"
#include <dos/dostags.h>

int start_external(struct dopus_func_start *func)
{
	int arg;
	char path[256], *ptr;

	func->key = NULL;
	func->status = 0;
	func->resseg = NULL;

	if(!(func->replyport = IExec->AllocSysObjectTags(ASOT_PORT, TAG_END)))
		return (0);
	if(!(func->startup.wbstartup.sm_ArgList = IDOpus->LAllocRemember(&func->key, sizeof(struct WBArg) * (func->argcount + 1), MEMF_CLEAR | MEMF_SHARED)))
		return (0);

	func->startup.wbstartup.sm_Message.mn_Node.ln_Name = NULL;

	func->startup.wbstartup.sm_Message.mn_Node.ln_Type = NT_MESSAGE;
	func->startup.wbstartup.sm_Message.mn_ReplyPort = func->replyport;
	func->startup.wbstartup.sm_Message.mn_Length = (UWORD) sizeof(struct DOpusStartup);

	func->startup.wbstartup.sm_NumArgs = func->argcount;
	for(arg = 0; arg < func->argcount; arg++)
		func->startup.wbstartup.sm_ArgList[arg].wa_Name = func->args[arg];

	strlcpy(path, func->segname, sizeof(path));
	if((ptr = (char *)IDOS->FilePart(path)))
		*ptr = 0;

	if(!(func->startup.wbstartup.sm_ArgList[0].wa_Lock = IDOS->Lock(path, ACCESS_READ)))
		return (0);

	func->startup.retcode = 0;

	func->status = FS_CURDIR;
	func->olddir = IDOS->CurrentDir(func->startup.wbstartup.sm_ArgList[0].wa_Lock);

	if(!func->segment)
	{
		IExec->ObtainSemaphore(ss);
		if((func->resseg = IDOS->FindSegment(func->segname, NULL, 0)))
		{
			func->resseg->seg_UC++;
			func->segment = func->resseg->seg_Seg;
		}
		IExec->ReleaseSemaphore(ss);
		if(!func->segment && !(func->segment = (BPTR) IDOS->LoadSeg(func->segname)))
		{
			return (0);
		}
	}

	func->status = FS_SEGMENT;

	func->startup.wbstartup.sm_Segment = func->segment;

	if(!(func->startup.wbstartup.sm_Process = &IDOS->CreateNewProcTags(NP_Name, func->procname, NP_Seglist, func->startup.wbstartup.sm_Segment, NP_StackSize, func->stack, NP_FreeSeglist, FALSE, TAG_END)->pr_MsgPort))
		return (0);

	func->startup.wbstartup.sm_ToolWindow = NULL;
	IExec->PutMsg(func->startup.wbstartup.sm_Process, (struct Message *)&func->startup);

	func->status = FS_LAUNCHED;
	return (1);
}

int close_external(struct dopus_func_start *func, int wait)
{
	if(func->status >= FS_LAUNCHED && wait)
	{
		IExec->WaitPort(func->replyport);
		IExec->GetMsg(func->replyport);
	}
	if(func->status >= FS_SEGMENT)
	{
		IExec->Forbid();
		if(func->resseg)
		{
			func->resseg->seg_UC--;
			func->segment = 0;
		}
		else if(!func->flags & FF_SAVESEG && func->segment)
		{
			IDOS->UnLoadSeg(func->segment);
			func->segment = 0;
		}
		IExec->Permit();
	}
	if(func->status >= FS_CURDIR)
	{
		IDOS->CurrentDir(func->olddir);
		IDOS->UnLock(func->startup.wbstartup.sm_ArgList[0].wa_Lock);
	}
	if(func->replyport)
		IExec->FreeSysObject(ASOT_PORT, func->replyport);
	IDOpus->LFreeRemember(&func->key);
	return (func->startup.retcode);
}

void doconfig()
{
	char buf[100], buf1[20], replyname[50], portname[50], funcpath[80], old_language[30];
	int old_bufcount, a, confignotdone = 1;
	struct MsgPort *conport, *cmdport;
	struct DOpusMessage *replymsg;
	struct configconfig cfg, *rcfg;
	struct ConfigStuff cstuff;
	struct dopus_func_start config_func;
	char *func_args[2], *old_name;
	struct Config config_backup = {0};

	snprintf(replyname, sizeof(replyname), "%s%d", config_replyport_basename, system_dopus_runcount);
	if(!(conport = IExec->AllocSysObjectTags(ASOT_PORT, ASOPORT_Name, replyname, ASOPORT_Pri, 20, ASOPORT_Public, TRUE, TAG_DONE)))
		return;

	snprintf(buf, sizeof(buf), "%d", system_dopus_runcount);
	snprintf(buf1, sizeof(buf1), "dopus4_config%d", system_dopus_runcount);

	strlcpy(funcpath, "PROGDIR:Modules/ConfigOpus", sizeof(funcpath));

	if(!configopus_segment)
	{
		dostatustext(globstring[STR_LOADING_CONFIG]);
	}

	func_args[0] = funcpath;
	func_args[1] = buf;

	config_func.segment = configopus_segment;
	config_func.procname = "ConfigOpus";

	config_func.segname = funcpath;
	config_func.argcount = 2;
	config_func.args = func_args;
	config_func.stack = 8192;
	config_func.flags = (config->loadexternal & LOAD_CONFIG) ? FF_SAVESEG : 0;

	if(!(start_external(&config_func)))
	{
		close_external(&config_func, 0);
		dostatustext(globstring[STR_CONFIG_NOT_FOUND]);
		IExec->FreeSysObject(ASOT_PORT, conport);
		return;
	}

	dostatustext(globstring[STR_WAITING_FOR_PORT]);
	snprintf(portname, sizeof(portname), "dopus4_config_port%d", system_dopus_runcount);
	cmdport = NULL;

	for(a = 0; a < 1000; a++)
	{
		IExec->Forbid();
		cmdport = IExec->FindPort(portname);
		IExec->Permit();
		if(cmdport)
			break;
		IDOS->Delay(5);
	}

	if(!cmdport)
	{
		close_external(&config_func, 0);
		dostatustext(globstring[STR_CONFIG_NOT_FOUND]);
		IExec->FreeSysObject(ASOT_PORT, conport);
		return;
	}

	status_configuring = -1;
	shutthingsdown(1);
	old_bufcount = config->bufcount;
	strlcpy(old_language, config->language, sizeof(old_language));
	dotaskmsg(hotkeymsg_port, HOTKEY_KILLHOTKEYS, 0, 0, NULL, 0);

	endnotifies();

	IExec->Forbid();
	old_name = arexx_port->mp_Node.ln_Name;
	arexx_port->mp_Node.ln_Name = (STRPTR)"foo";
	IExec->Permit();

	while(confignotdone == 1)
	{
		while((replymsg = (struct DOpusMessage *)IExec->GetMsg(conport)))
		{
			switch (replymsg->command)
			{
			case CONFIG_ALL_DONE:
				confignotdone = 0;
				break;
			case CONFIG_GET_CONFIG:
				IExec->CopyMem(config, &config_backup, sizeof(struct Config));
				cfg.config = config;
				cfg.firsttype = dopus_firsttype;
				cfg.typekey = filetype_key;
				cfg.firstbank = dopus_firstgadbank;
				cfg.changed = config_changed;
				cfg.firsthotkey = dopus_firsthotkey;
				strlcpy(cfg.configname, str_config_basename, sizeof(cfg.configname));
				cfg.Screen = MainScreen;
				replymsg->data = &cfg;
				break;
			case CONFIG_HERES_CONFIG:
				rcfg = (struct configconfig *)replymsg->data;
				if (rcfg->changed)
					if (!checkwindowquit())
					{
						IExec->CopyMem(&config_backup, config, sizeof(struct Config));
						break;
					}
				dopus_firsttype = rcfg->firsttype;
				dopus_firstgadbank = rcfg->firstbank;
				dopus_firsthotkey = rcfg->firsthotkey;
				filetype_key = rcfg->typekey;
				config_changed = rcfg->changed;
				strlcpy(str_config_file, rcfg->configname, sizeof(str_config_file));
				strlcat(str_config_file, ".CFG", sizeof(str_config_file));
				strlcpy(str_config_basename, rcfg->configname, sizeof(str_config_basename));
				status_configuring = 1;
				break;
			case CONFIG_NEW_HOTKEY:
				dotaskmsg(hotkeymsg_port, HOTKEY_HOTKEYCHANGE, 0, 0, NULL, 0);
				break;
			}
			IExec->ReplyMsg((struct Message *)replymsg);
		}

		if(confignotdone == 1)
		{
			IExec->Wait(1 << conport->mp_SigBit);
		}
	}

	config_func.flags = (config->loadexternal & LOAD_CONFIG) ? FF_SAVESEG : 0;
	close_external(&config_func, 1);
	configopus_segment = config_func.segment;

	dopus_curgadbank = dopus_firstgadbank;
	data_gadgetrow_offset = data_drive_offset = 0;

	if(config_changed)
	{
		if(config_changed == 2)
			config_changed = 0;
		closedisplay();
		status_configuring = -1;
		dotaskmsg(hotkeymsg_port, HOTKEY_HOTKEYCHANGE, 0, 0, NULL, 0);
		dotaskmsg(hotkeymsg_port, HOTKEY_NEWHOTKEYS, 0, 0, NULL, 0);
		if(config->bufcount != old_bufcount)
		{
			allocdirbuffers(config->bufcount);
			for(a = 0; a < 2; a++)
				strlcpy(str_pathbuffer[a], dopus_curwin[a]->directory, sizeof(str_pathbuffer[0]));
		}
		if(strcmp(old_language, config->language))
			read_data_files(0);

		if(config->loadexternal & LOAD_CONFIG)
		{
			configopus_segment = IDOS->LoadSeg("PROGDIR:Modules/ConfigOpus");
		}
		else
		{
			configopus_segment = 0L;
		}

		if(config->loadexternal & LOAD_PRINT)
		{
			dopusprint_segment = IDOS->LoadSeg("PROGDIR:Modules/DOpus_Print");
		}
		else
		{
			dopusprint_segment = 0L;
		}

		fixcstuff(&cstuff);
		IDOpus->CheckConfig(&cstuff);
		SetUp(2);
	}
	else
	{
		initclock();
		fixcstuff(&cstuff);
		dostatustext(globstring[STR_WELCOME_BACK_TO_DOPUS]);
		dopustofront();
	}

	IExec->FreeSysObject(ASOT_PORT, conport);

	IExec->Forbid();
	arexx_port->mp_Node.ln_Name = old_name;
	IExec->Permit();

	rexx_command(config->configreturnscript, NULL);

	startnotifies();
	status_configuring = 0;

	return;
}

void dopus_print(int rexx, struct DOpusArgsList *arglist, int printdir, char *port, struct ViewData *vdata)
{
	char *args[16], portname[21], arglistbuf[20], funcpath[80];
	struct dopus_func_start print_func;
	int a, argcount, waitbits, abase = 3;

	if(!vdata)
		dostatustext(globstring[STR_STARTING_PRINT_MODULE]);

	print_func.segment = dopusprint_segment;
	print_func.procname = "DOpus_Print";

	strlcpy(funcpath, "PROGDIR:Modules/DOpus_Print", sizeof(funcpath)); //external_modules[SEG_PRINT]);

	print_func.segname = funcpath;

	args[0] = print_func.segname;

	strlcpy(portname, "&", sizeof(portname));
	strlcat(portname, port, sizeof(portname));
	args[1] = portname;

	if(printdir)
	{
		int win;

		if(!rexx || rexx_argcount < 1 || (win = atoi(rexx_args[0])) < 0 || win > 1)
			win = data_active_window;
		snprintf(arglistbuf, sizeof(arglistbuf), "@%d", win);
		abase = 2;
		--rexx_argcount;
	}
	else
	{
		snprintf(arglistbuf, sizeof(arglistbuf), "!%lu", (uint32)arglist);
	}

	args[2] = arglistbuf;
	argcount = 3;

	if(rexx && rexx_argcount > 0)
	{
		if((argcount += rexx_argcount) > 16)
		{
			argcount = 16;
		}
		for(a = 3; a < argcount; a++)
		{
			args[a] = rexx_args[a - abase];
		}
	}

	for(a = argcount; a < 16; a++)
	{
		args[a] = (char *)"";
	}

	print_func.argcount = argcount;
	print_func.args = args;
	print_func.stack = 8192;
	print_func.flags = (config->loadexternal & LOAD_PRINT) ? FF_SAVESEG : 0;

	if(!(start_external(&print_func)))
	{
		close_external(&print_func, 0);
		if(!vdata)
		{
			dostatustext(globstring[STR_UNABLE_TO_LOAD_MODULE]);
		}
		return;
	}

	waitbits = 1 << print_func.replyport->mp_SigBit;
	if(vdata)
	{
		waitbits |= 1 << vdata->view_port->mp_SigBit;
	}
	else
	{
		waitbits |= rexx_signalbit;
	}

	for(;;)
	{
		if((IExec->Wait(waitbits)) & rexx_signalbit && !vdata)
		{
			rexx_dispatch(0);
			continue;
		}
		if(vdata)
		{
			struct DOpusMessage *dmsg;
			struct DOpusArgsList *arg;

			while((dmsg = (struct DOpusMessage *)IExec->GetMsg(vdata->view_port)))
			{
				switch (dmsg->command)
				{
				case DOPUSMSG_GETVIS:
					IExec->CopyMem((char *)&vdata->view_vis_info, (char *)dmsg->data, sizeof(struct VisInfo));
					break;
				case DOPUSMSG_GETNEXTFILE:
					arg = (struct DOpusArgsList *)dmsg->data;
					if(arg->single_file)
					{ // Should be PATHBUF_SIZE instead of 256
						strlcpy(arg->file_data, arg->single_file, 256);
						arg->single_file = NULL;
					}
					else
						arg->file_data[0] = 0;
					break;
				}
				IExec->ReplyMsg((struct Message *)dmsg);
			}
		}
		if(IExec->GetMsg(print_func.replyport))
			break;
	}

	close_external(&print_func, 0);
	dopusprint_segment = print_func.segment;

	if(!vdata)
	{
		okay();
	}
}

int dopus_iconinfo(char *filename)
{
	struct Screen *infoscreen = NULL;
	BPTR plock, flock = IDOS->Lock(filename, ACCESS_READ);
	char buffer[FILEBUF_SIZE] = { 0, }, *a = NULL;
	int ret = 1;

	if(!flock)
	{
		return(-1);
	}

	plock = IDOS->ParentDir(flock);
	IDOS->UnLock(flock);

	if(!plock)
	{
		return(-1);
	}

	if(MainScreen)
	{
		infoscreen = MainScreen;
	}
	else
	{
		infoscreen = IIntuition->LockPubScreen(NULL);
		if (!infoscreen)
		{
			IDOS->UnLock(plock);
			return(-2);
		}
	}

	strlcpy(buffer, IDOS->FilePart(filename), sizeof(buffer));
	if((a = isicon(buffer)))
	{
		*a = '\0';
		dostatustext(globstring[STR_SHOWING_FILE]);
		if (!IWorkbench->WBInfo(plock, buffer, infoscreen))
		{
			ret = -4;
		}
	}
	else
	{
		ret = -3;
	}

	if(!MainScreen)
	{
		IIntuition->UnlockPubScreen(NULL, infoscreen);
	}

	IDOS->UnLock(plock);

	return(ret);
}

void setup_externals()
{
	APTR wsave;

	wsave = IDOS->SetProcWindow((APTR)-1L);

	if(config->loadexternal & LOAD_CONFIG)
	{
		configopus_segment = IDOS->LoadSeg("PROGDIR:Modules/ConfigOpus");
	}
	else
	{
		configopus_segment = 0L;
	}

	if(config->loadexternal & LOAD_PRINT)
	{
		dopusprint_segment = IDOS->LoadSeg("PROGDIR:Modules/DOpus_Print");
	}
	else
	{
		dopusprint_segment = 0L;
	}

	IDOS->SetProcWindow(wsave);
}

void fill_out_visinfo(struct VisInfo *vis, struct Screen *scr)
{
	if(scr && (!Window || Window->WScreen != scr))
	{
		vis->vi_flags &= ~VISF_WINDOW;
		vis->vi_screen = scr;

		vis->vi_fg = config->requestfg;
		vis->vi_bg = config->requestbg;
		vis->vi_shine = config->gadgettopcol;
		vis->vi_shadow = config->gadgetbotcol;
		vis->vi_stringcol[0] = config->stringfgcol;
		vis->vi_stringcol[1] = config->stringbgcol;
		vis->vi_activestringcol[0] = config->stringselfgcol;
		vis->vi_activestringcol[1] = config->stringselbgcol;
	}
	else
	{
		if(vis->vi_flags & VISF_WINDOW)
			vis->vi_screen = (struct Screen *)Window;
		else if(Window)
			vis->vi_screen = Window->WScreen;
		else
			vis->vi_screen = NULL;

		vis->vi_fg = screen_pens[config->requestfg].pen;
		vis->vi_bg = screen_pens[config->requestbg].pen;
		vis->vi_shine = screen_pens[config->gadgettopcol].pen;
		vis->vi_shadow = screen_pens[config->gadgetbotcol].pen;
		vis->vi_stringcol[0] = screen_pens[config->stringfgcol].pen;
		vis->vi_stringcol[1] = screen_pens[config->stringbgcol].pen;
		vis->vi_activestringcol[0] = screen_pens[config->stringselfgcol].pen;
		vis->vi_activestringcol[1] = screen_pens[config->stringselbgcol].pen;
	}

	if(vis->vi_flags & VISF_8POINTFONT)
		vis->vi_font = scr_font[FONT_GENERAL];
	else
		vis->vi_font = scr_font[FONT_REQUEST];

	if(config->generalscreenflags & SCR_GENERAL_REQDRAG)
		vis->vi_flags |= VISF_BORDERS;
	else
		vis->vi_flags &= ~VISF_BORDERS;

	vis->vi_language = config->language;
}
