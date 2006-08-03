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
#include <proto/amigaguide.h>
#include <proto/muimaster.h>

#include "dopus.h"
#include "mui.h"

static APTR setupfontdisplay(CONST_STRPTR name, struct TextFont *font, CONST_STRPTR text);
static void cleanup_fontdisplay(APTR win);

struct Library *AmigaGuideBase;

int showfont(STRPTR name, int size, int np)
{
	UBYTE viewstr[256+2];
	struct TextFont *font;
	struct TextAttr sfattr = { (STRPTR) name, size, 0, 0 };

	font = OpenDiskFont(&sfattr);

	if (font)
	{
		APTR win;
		LONG i, x, lo, hi;

		lo = font->tf_LoChar;
		hi = font->tf_HiChar;

		x = 2;

		viewstr[0] = 27;		// disable text engine
		viewstr[1] = '-';

		for (i = lo; i <= hi; i++)
		{
			viewstr[x] = i;
			x++;
		}

		viewstr[x] = 0;

		set(dopusapp, MUIA_Application_Sleep, TRUE);

		if ((win = setupfontdisplay(name, font, viewstr)))
		{
			ULONG sigs = 0;

			DoMethod(win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

			for (;;)
			{
				ULONG ret = DoMethod(dopusapp, MUIM_Application_NewInput, &sigs);

				if (ret == MUIV_Application_ReturnID_Quit)
				{
					break;
				}

				if (sigs)
					sigs = Wait(sigs);
			}

			cleanup_fontdisplay(win);
		}

		set(dopusapp, MUIA_Application_Sleep, FALSE);

		CloseFont(font);
	}

	return 1;
}

static APTR setupfontdisplay(CONST_STRPTR name, struct TextFont *font, CONST_STRPTR text)
{
	APTR win;

	win = WindowObject,
		MUIA_Window_ID, MAKE_ID('F','O','N','T'),
		MUIA_Window_Title, name,
		WindowContents, VGroup,
			Child, FloattextObject,
				MUIA_Font, font,
				MUIA_Floattext_Text, text,
				End,
		End,
	End;

	if (win)
	{
		ULONG ok;

		DoMethod(dopusapp, OM_ADDMEMBER, win);
		set(win, MUIA_Window_Open, TRUE);
		GetAttr(MUIA_Window_Open, win, &ok);

		if (!ok)
		{
			cleanup_fontdisplay(win);
			win = NULL;
		}
	}

	return win;
}

static void cleanup_fontdisplay(APTR win)
{
	DoMethod(dopusapp, OM_REMMEMBER, win);
	MUI_DisposeObject(win);
}

void readhelp(STRPTR file)
{
//D(bug("readhelp(%s)\n",file);Delay(50));
	if(file)
		doreadhelp(file);
	if(dopus_firsthelp)
		screen_gadgets[SCRGAD_TINYHELP].Flags &= ~GFLG_DISABLED;
	else
		screen_gadgets[SCRGAD_TINYHELP].Flags |= GFLG_DISABLED;
	if(Window && config->generalscreenflags & SCR_GENERAL_TINYGADS)
		dolittlegads(&screen_gadgets[SCRGAD_TINYHELP], &globstring[STR_TINY_BUTTONS][4], 1);
}

void doreadhelp(STRPTR file)
{
	int a, b = 0, /*helpsize, */inmsg;
	ULONG helpsize;
	STRPTR helpbuf, buf;
	struct Help *temph, *curhelp;

	freehelp();
	if(!file[0] || !(buf = AllocMem(10000, MEMF_CLEAR)))
		return;
	if(readfile(file, &helpbuf, &helpsize))
	{
		FreeMem(buf, 10000);
		return;
	}

	curhelp = NULL;
	inmsg = 0;
	for(a = 0; a < helpsize; a++)
	{
		if(helpbuf[a] == '*' && !inmsg)
		{
			inmsg = 1;
			b = 0;
			continue;
		}
		else
		{
			if(inmsg == 1)
			{
				if(b == 9999 || helpbuf[a] == '\n')
				{
					buf[b] = 0;
					if(!(temph = LAllocRemember(&help_key, sizeof(struct Help), MEMF_CLEAR)))
						break;
					if(!(temph->lookup = LAllocRemember(&help_key, b + 1, MEMF_CLEAR)))
						break;
					CopyMem(buf, temph->lookup, b);
					if(curhelp)
						curhelp->next = temph;
					else
						dopus_firsthelp = temph;
					curhelp = temph;
					inmsg = 2;
					b = 0;
					continue;
				}
				buf[b] = helpbuf[a];
				++b;
				continue;
			}
			else if(inmsg == 2)
			{
				if(b == 9999 || helpbuf[a] == '^')
				{
					if(curhelp)
					{
						buf[b] = 0;
						if(!(curhelp->message = LAllocRemember(&help_key, b + 1, MEMF_CLEAR)))
							break;
						CopyMem(buf, curhelp->message, b);
					}
					inmsg = 0;
					b = 0;
					continue;
				}
				buf[b] = helpbuf[a];
				++b;
				continue;
			}
		}
	}
	FreeMem(buf, 10000);
	FreeVec(helpbuf);
}

STATIC CONST CONST_STRPTR helpcontext[] =
{
	"Main",
	"About",
	"AddCustEntry",
	"AddCustHandler",
	"AddFile",
	"AddIcon",
	"Alarm",
	"All",
	"AnsiRead",
	"ARexx",
	"Assign",
	"Beep",
	"BufferList",
	"Busy",
	"ButtonIconify",
	"CD",
	"CheckAbort",
	"CheckFit",
	"ClearBuffers",
	"ClearSizes",
	"ClearWin",
	"Clone",
	"Comment",
	"Configure",
	"ContST",
	"Copy",
	"CopyAs",
	"CopyWindow",
	"DateStamp",
	"Defaults",
	"Delete",
	"DirTree",
	"DiskCopy",
	"DiskCopyBG",
	"DiskInfo",
	"DisplayDir",
	"DOpusToBack",
	"DOpusToFront",
	"Encrypt",
	"ErrorHelp",
	"Execute",
	"FileInfo",
	"FinishSection",
	"Format",
	"FormatBG",
	"GetAll",
	"GetDevices",
	"GetDirs",
	"GetEntry",
	"GetFileType",
	"GetFiles",
	"GetNextSelected",
	"GetSelectedAll",
	"GetSelectedDirs",
	"GetSelectedFiles",
	"GetSizes",
	"GetString",
	"HexRead",
	"Hunt",
	"Iconify",
	"IconInfo",
	"Install",
	"InstallBG",
	"LastSaved",
	"LoadConfig",
	"LoopPlay",
	"LoopPlay8svx",
	"MakeDir",
	"Modify",
	"Move",
	"MoveAs",
	"NewCLI",
	"NextDrives",
	"None",
	"notify",
	"OtherWindow",
	"Parent",
	"ParentList",
	"PatternMatch",
	"Play",
	"Play8svx",
	"PlayST",
	"Print",
	"PrintDir",
	"Protect",
	"Query",
	"Quit",
	"Read",
	"Redraw",
	"Relabel",
	"Remember",
	"RemoveEntry",
	"RemoveFile",
	"Rename",
	"Request",
	"Rescan",
	"Reselect",
	"Restore",
	"Root",
	"Run",
	"SaveConfig",
	"ScanDir",
	"ScrollH",
	"ScrollToShow",
	"ScrollV",
	"Search",
	"Select",
	"SelectEntry",
	"SelectFile",
	"SetVar",
	"SetWinTitle",
	"Show",
	"SmartRead",
	"Status",
	"StopST",
	"SwapWindow",
	"Toggle",
	"TopText",
	"Uniconify",
	"User1",
	"Verify",
	"Version",
	NULL
};

char *getAGnode(char *buf, char *func)
{
	int i;

	for(i = 0; func[i] && func[i] != ' '; i++)
		buf[i] = func[i];
	buf[i] = 0;

	for(i = 0; helpcontext[i]; i++)
		if(stricmp(buf, helpcontext[i]) == 0)
			break;

	if(helpcontext[i] == NULL)
		strcpy(buf, "Main");

	return buf;
}

void dohelp(STRPTR name, STRPTR func, int key, int qual, STRPTR defmsg)
{
	if(dopus_firsthelp)
	{
		char *msg = NULL, *buf = NULL, buf1[40], buf2[30];
		struct Help *help = dopus_firsthelp;
		int s = 0, s1;

		while(help)
		{
			if(help->lookup)
			{
				if(help->lookup[0] == '*' && func)
				{
					if(strcmp(help->lookup, func) == 0)
					{
						msg = help->message;
						break;
					}
				}
				else if(name)
				{
					if(strcmp(help->lookup, name) == 0)
					{
						msg = help->message;
						break;
					}
				}
			}
			help = help->next;
		}
		if(!msg)
			msg = (defmsg ? defmsg : globstring[STR_HELP_NOT_AVAILABLE]);

		if(key > 0)
		{
			RawkeyToStr(key, qual, buf2, NULL, 30);
			strcpy(buf1, globstring[STR_KEY]);
			strcat(buf1, buf2);
			strcat(buf1, "\n\n");
			s1 = strlen(buf1);
			s = strlen(msg) + 1;
			if((buf = AllocMem(s + s1, MEMF_CLEAR)))
			{
				CopyMem(buf1, buf, s1);
				CopyMem(msg, &buf[s1], s);
				s += s1;
				msg = buf;
			}
		}
		simplerequest(msg, globstring[STR_CONTINUE], NULL);
		if(buf)
			FreeMem(buf, s);
	}
	else if(AmigaGuideBase && func)
	{
		struct NewAmigaGuide nag = { 0 };
		AMIGAGUIDECONTEXT agc;
		char buf[32];

		nag.nag_Name = "DirectoryOpus.guide";
		nag.nag_Screen = Window->WScreen;
		nag.nag_Flags = 0 /*HTF_NOACTIVATE */ ;
		nag.nag_Context = helpcontext;
		nag.nag_Node = getAGnode(buf, func[0] == '*' ? func + 1 : func);

//		D(bug("Trying to display Amigaguide node \"%s\"\n", nag.nag_Node));
		if((agc = OpenAmigaGuide(&nag, NULL)))
		{
			CloseAmigaGuide(agc);
		}
		else
			simplerequest(defmsg ? defmsg : globstring[STR_HELP_NOT_AVAILABLE], globstring[STR_CONTINUE], NULL);
	}
	unbusy();
	return;
}

void checkstringgads(int a)
{
	--a;
	checkdir(str_pathbuffer[a], &path_strgadget[a]);
	strcpy(dopus_curwin[a]->directory, str_pathbuffer[a]);
}

void setdirsize(struct Directory *dir, long long byte, int win)
{
	if(status_iconified || win == -1)
		return;

	if(dir->size > 0)
	{
		dopus_curwin[win]->bytestot -= dir->size;
		if(dir->selected)
			dopus_curwin[win]->bytessel -= dir->size;
	}
	if(byte < -1)
		byte = -1;
	dir->size = byte;
	if(dir->size != -1)
	{
		dopus_curwin[win]->bytestot += byte;
		if(dir->selected)
			dopus_curwin[win]->bytessel += byte;
	}
}
