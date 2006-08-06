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

#include <proto/alib.h>
#include <proto/locale.h>
#include <proto/muimaster.h>

#include "dopus.h"
#include "mui.h"

void quit()
{
	int a;

	removehotkeys();
	endnotifies();

	close_rexx_port();

	main_proc->pr_WindowPtr = windowptr_save;
	closedisplay();

	remove_arbiter();

	allocdirbuffers(0);
	for(a = 0; a < 2; a++)
	{
		freedir(dopus_specialwin[a], -1);
		if(completion[a].memory)
			LFreeRemember(&completion[a].memory);
	}
	if(dir_memory_pool)
		DeletePool(dir_memory_pool);

	for(a = 0; a < NUMFONTS; a++)
		freefont(a);
	freedynamiccfg();
	freehelp();
	LFreeRemember(&general_key);
	LFreeRemember(&border_key);
	LFreeRemember(&menu_key);
	FreeStringFile(&stringdata);

	#if 0
	if(configopus_segment)
		UnLoadSeg(configopus_segment);
	for(a = 0; a < 3; a++)
	{
		if(external_mod_segment[a])
			UnLoadSeg(external_mod_segment[a]);
	}
	#endif

	UnLock(ramdisk_lock);

	if(func_reselection.reselection_list)
		FreeMem(func_reselection.reselection_list, func_reselection.reselection_size);
	DeleteMsgPort(count_port);
	if(arexx_port)
	{
		RemPort(arexx_port);
		DeleteMsgPort(arexx_port);
	}
	DeleteMsgPort(general_port);

	MUI_DisposeObject(dopusapp);

	delete_classes();

	if(user_appicon)
		FreeDiskObject(user_appicon);

	CloseLocale(locale);
	CloseLibrary(DOpusBase);

	close_libs();

	main_proc->pr_CIS = old_pr_cis;
	main_proc->pr_COS = old_pr_cos;
	main_proc->pr_ConsoleTask = old_pr_consoletask;

	if(nil_file_handle)
		Close(nil_file_handle);

	exit(0);
}

void removehotkeys()
{
	if(hotkeymsg_port)
	{
		dotaskmsg(hotkeymsg_port, TASK_QUIT, 0, 0, NULL, 0);

		while (hotkeymsg_port)
		{
			Delay(2);
		}
//		DeleteTask(hotkey_task);
		hotkey_task = NULL;
	}
}

void freefont(num)
     int num;
{
	if(scr_font[num])
		CloseFont(scr_font[num]);
	scr_font[num] = NULL;
}

void freehelp()
{
	LFreeRemember(&help_key);
	dopus_firsthelp = NULL;
}

void freedynamiccfg()
{
	struct ConfigStuff cstuff;

	fixcstuff(&cstuff);
	FreeConfig(&cstuff);
	cstufffix(&cstuff);
}

int checkwindowquit()
{
	struct Window *wind, *temp;

	if(!MainScreen)
		return (1);

	if((PubScreenStatus(MainScreen, PSNF_PRIVATE) & PSNF_PRIVATE) == 0)
	{
		busy();
		if(!(simplerequest(globstring[STR_ALIEN_WINDOWS], globstring[STR_CLOSE], str_cancelstring, NULL)))
		{
			unbusy();
			PubScreenStatus(MainScreen, 0);
			return (0);
		}
		for(wind = MainScreen->FirstWindow; wind;)
		{
			temp = wind->NextWindow;
			if(wind != Window)
				CloseWindow(wind);
			wind = temp;
		}
		unbusy();
	}
	return (1);
}

void dotaskmsg(struct MsgPort *port, int command, int value, int total, STRPTR data, int flag)
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
		Forbid();
		PutMsg(port, (struct Message *)&taskmsg);
		Permit();
		WaitPort(general_port);
		while(GetMsg(general_port) != (struct Message *)&taskmsg);
	}
}

void closedisplay()
{
	{
		if(MainScreen)
			ScreenToBack(MainScreen);
		freedragbuffers();
		Forbid();
		if(Window)
		{
			if(Window->MenuStrip)
			{
				ClearMenuStrip(Window);
			}
			free_colour_table();

			CloseWindow(Window);
			Window = NULL;
		}
		if(MainScreen)
		{
			if(!CloseScreen(MainScreen))
			{
				simplerequest(globstring[STR_CANNOT_CLOSE_SCREEN], str_okaystring, NULL);
				while(!(CloseScreen(MainScreen)))
					Delay(50);
			}
			MainScreen = NULL;
		}
		Permit();
	}
}
