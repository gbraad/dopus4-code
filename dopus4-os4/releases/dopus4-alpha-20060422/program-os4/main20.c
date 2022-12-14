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

#include <proto/locale.h>
#define NO_INLINE_STDARG
#include <proto/pm.h>

void quit()
{
	int a;

	remclock();
	removehotkeys();
	endnotifies();

	if(input_req)
	{
		IExec->CloseDevice((struct IORequest *)input_req);
		IExec->DeleteIORequest((struct IORequest *)input_req);
	}

	close_rexx_port();

	main_proc->pr_WindowPtr = windowptr_save;
	closedisplay();

	remove_arbiter();

	allocdirbuffers(0);
	for(a = 0; a < 2; a++)
	{
		freedir(dopus_specialwin[a], -1);
		if(completion[a].memory)
			IDOpus->LFreeRemember(&completion[a].memory);
	}
	if(dir_memory_pool)
		IExec->FreeSysObject(ASOT_MEMPOOL, dir_memory_pool);
//		IExec->DeletePool(dir_memory_pool);

	for(a = 0; a < NUMFONTS; a++)
		freefont(a);
	freedynamiccfg();
	freehelp();
	IDOpus->LFreeRemember(&general_key);
	IDOpus->LFreeRemember(&border_key);
	IDOpus->LFreeRemember(&menu_key);
	IDOpus->FreeStringFile(&stringdata);

	if(configopus_segment)
		IDOS->UnLoadSeg(configopus_segment);
	for(a = 0; a < 3; a++)
	{
		if(external_mod_segment[a])
			IDOS->UnLoadSeg(external_mod_segment[a]);
	}

	if(user_appicon)
		IIcon->FreeDiskObject(user_appicon);

	if(nil_file_handle)
		IDOS->Close(nil_file_handle);
	IDOS->UnLock(ramdisk_lock);
	if(func_reselection.reselection_list)
		IExec->FreeMem(func_reselection.reselection_list, func_reselection.reselection_size);
	if(count_port)
		IExec->DeletePort(count_port);
	if(arexx_port)
		IExec->DeletePort(arexx_port);
	if(general_port)
		IExec->DeletePort(general_port);
	if(appmsg_port)
		IExec->DeletePort(appmsg_port);

	if(sortmenu)
		IPopupMenu->PM_FreePopupMenu(sortmenu);

	ILocale->CloseLocale(locale);

	if(IDOpus)
		IExec->DropInterface((struct Interface *)IDOpus);
	if(DOpusBase)
		IExec->CloseLibrary(DOpusBase);

	main_proc->pr_CIS = old_pr_cis;
	main_proc->pr_COS = old_pr_cos;
	main_proc->pr_ConsoleTask = old_pr_consoletask;

	exit(0);
}

void remclock()
{
	if(clockmsg_port)
	{
		dotaskmsg(clockmsg_port, TASK_QUIT, 0, 0, NULL, 0);
		/*DeleteTask(clock_task);*/
		clock_task = NULL;
	}
}

void removehotkeys()
{
	if(hotkeymsg_port)
	{
		dotaskmsg(hotkeymsg_port, TASK_QUIT, 0, 0, NULL, 0);
		IExec->DeleteTask(hotkey_task);
		hotkey_task = NULL;
	}
}

void freefont(num)
     int num;
{
	if(scr_font[num])
		IGraphics->CloseFont(scr_font[num]);
	scr_font[num] = NULL;
}

void freehelp()
{
	IDOpus->LFreeRemember(&help_key);
	dopus_firsthelp = NULL;
}

void freedynamiccfg()
{
	struct ConfigStuff cstuff;

	fixcstuff(&cstuff);
	IDOpus->FreeConfig(&cstuff);
	cstufffix(&cstuff);
}

int checkwindowquit()
{
	struct Window *wind, *temp;

	if(!MainScreen)
		return (1);

	if((IIntuition->PubScreenStatus(MainScreen, PSNF_PRIVATE) & PSNF_PRIVATE) == 0)
	{
		busy();
		if(!(simplerequest(globstring[STR_ALIEN_WINDOWS], globstring[STR_CLOSE], str_cancelstring, NULL)))
		{
			unbusy();
			IIntuition->PubScreenStatus(MainScreen, 0);
			return (0);
		}
		for(wind = MainScreen->FirstWindow; wind;)
		{
			temp = wind->NextWindow;
			if(wind != Window)
				IIntuition->CloseWindow(wind);
			wind = temp;
		}
		unbusy();
	}
	return (1);
}

void dotaskmsg(struct MsgPort *port, int command, int value, int total, char *data, int flag)
{
	if(port)
	{
		struct dopustaskmsg taskmsg;

		taskmsg.msg.mn_Node.ln_Type = NT_MESSAGE;
		taskmsg.msg.mn_ReplyPort = general_port;
		taskmsg.msg.mn_Length = (UWORD) sizeof(struct dopustaskmsg);
		taskmsg.command = command;
		taskmsg.value = value;
		taskmsg.total = total;
		taskmsg.data = data;
		taskmsg.flag = flag;
		IExec->Forbid();
		IExec->PutMsg(port, (struct Message *)&taskmsg);
		IExec->Permit();
		IExec->WaitPort(general_port);
/* cut by Jacek Rzeuski (31.03.2000) to avoid on-copy-exit crashes
        GetMsg(general_port);
*/
/* inserted by Jacek Rzeuski (31.03.2000) to avoid on-copy-exit crashes */
		while(IExec->GetMsg(general_port) != (struct Message *)&taskmsg);
/* end of */
	}
}

void closedisplay()
{
	if(status_iconified)
		cleanupiconify();
	else
	{
		if(MainScreen)
			IIntuition->ScreenToBack(MainScreen);
		freedragbuffers();
		IExec->Forbid();
		if(Window)
		{
			if(Window->MenuStrip)
				IIntuition->ClearMenuStrip(Window);
			if(IWorkbench && dopus_appwindow)
			{
				IWorkbench->RemoveAppWindow(dopus_appwindow);
				dopus_appwindow = NULL;
			}
			free_colour_table();
			IIntuition->CloseWindow(Window);
			Window = NULL;
		}
		if(MainScreen)
		{
			if(!IIntuition->CloseScreen(MainScreen))
			{
				simplerequest(globstring[STR_CANNOT_CLOSE_SCREEN], str_okaystring, NULL);
				while(!(IIntuition->CloseScreen(MainScreen)))
					IDOS->Delay(50);
			}
			MainScreen = NULL;
		}
		IExec->Permit();
	}
}
