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
#include <proto/amigaguide.h>

int showfont(char *name, int size, int np)
{
	int base, y, fred, t, len;
	char a;
	struct TextFont *font;
	char fontbuf[256], *fontptr;
	struct TextAttr sfattr = { (STRPTR) name, size, 0, 0 };
	struct RastPort *font_rp;

	if (!(font = IDiskfont->OpenDiskFont(&sfattr)))
	{
		doerror(-1);
		return (0);
	}

	if(MainScreen)
	{
		fontscreen = MainScreen;
	}
	else
	{
		fontscreen = IIntuition->LockPubScreen(NULL);
	}

//	font = IDiskfont->OpenDiskFont(&sfattr);

	fontwindow = IIntuition->OpenWindowTags(NULL, WA_CustomScreen, fontscreen, WA_Left, 128, WA_Top, 128, WA_Width, 1024, WA_Height, 768, WA_Flags, WFLG_GIMMEZEROZERO | WFLG_ACTIVATE | WFLG_RMBTRAP, WA_IDCMP, IDCMP_MOUSEBUTTONS | IDCMP_VANILLAKEY, TAG_END);
	if(!fontwindow)
	{
		IGraphics->CloseFont(font);
		if(fontscreen != MainScreen)
			IIntuition->UnlockPubScreen(NULL, fontscreen);
		doerror(-1);
		return (0);
	}

//	if(!font || !fontwindow)
//	{
//		doerror(-1);
//		return (0);
//	}


	base = font->tf_Baseline;

	IGraphics->SetRPAttrs(font_rp = fontwindow->RPort, RPTAG_Font, font, RPTAG_APen, 1, RPTAG_DrMd, JAM1, TAG_END);

	y = base;
	a = font->tf_LoChar;
	len = t = 0;

	IGraphics->Move(font_rp, 0, y);
	fontptr = fontbuf;

	for(;;)
	{
		len += IGraphics->TextLength(font_rp, &a, 1);
		if(len > fontwindow->GZZWidth || t > 254)
		{
			IGraphics->Text(font_rp, fontbuf, t);
			y += size;
			IGraphics->Move(font_rp, 0, y);
			len = t = 0;
			fontptr = fontbuf;
			if(y - base > fontwindow->GZZHeight)
				break;
		}
		*(fontptr++) = a;
		++t;
		if((++a) > font->tf_HiChar)
			a = font->tf_LoChar;
	}

	IIntuition->ActivateWindow(fontwindow);

	fred = WaitForMouseClick(fontwindow);

	cleanup_fontdisplay();
	IGraphics->CloseFont(font);

	if(fontscreen != MainScreen)
		IIntuition->UnlockPubScreen(NULL, fontscreen);

	return ((fred == 0) ? 1 : -1);
}

/*
int setupfontdisplay(int depth, UWORD *coltab)
{
	struct DimensionInfo dims;
	struct RastPort *font_rp;

	font_scr.Width = ((struct GfxBase *)(IGraphics->Data.LibBase))->NormalDisplayColumns;
	font_scr.Height = STDSCREENHEIGHT;
	font_scr.Depth = depth;
	{
		font_scr.Extension[0].ti_Data = IGraphics->BestModeID(BIDTAG_DesiredWidth, font_scr.Width, BIDTAG_DesiredHeight, font_scr.Height, BIDTAG_Depth, font_scr.Depth, BIDTAG_MonitorID, IGraphics->GetVPModeID(&Window->WScreen->ViewPort) & MONITOR_ID_MASK, TAG_END);
		if(font_scr.Extension[0].ti_Data == INVALID_ID)
			font_scr.Extension[0].ti_Data = HIRES_KEY;
		if(IGraphics->GetDisplayInfoData(NULL, (UBYTE *) & dims, sizeof(struct DimensionInfo), DTAG_DIMS, font_scr.Extension[0].ti_Data))
		{
			font_scr.Width = dims.Nominal.MaxX - dims.Nominal.MinX + 1;
			font_scr.Height = dims.Nominal.MaxY - dims.Nominal.MinY + 1;
		}
	}
	if(!(fontscreen = IIntuition->OpenScreenTags(NULL, SA_LikeWorkbench, TRUE, TAG_DONE)))
		return (0);
	font_win.Width = fontscreen->Width;
	font_win.Height = fontscreen->Height;
	font_win.Screen = fontscreen;
	if(!(fontwindow = IIntuition->OpenWindow(&font_win)))
	{
		IIntuition->CloseScreen(fontscreen);
		return (0);
	}
	font_rp = fontwindow->RPort;
	if(coltab)
	{
		int a, num;

		num = 1 << depth;
		for(a = 0; a < num; a++)
		{
			coltab[a] = IGraphics->GetRGB4(fontscreen->ViewPort.ColorMap, a);
		}
	}
	IGraphics->LoadRGB4(&fontscreen->ViewPort, nullpalette, 1 << depth);
	return (1);
}
*/

void cleanup_fontdisplay()
{
	IIntuition->ScreenToFront(Window->WScreen);
	if(fontwindow)
	{
		IIntuition->CloseWindow(fontwindow);
		fontwindow = NULL;
	}
	IIntuition->ActivateWindow(Window);
}

void readhelp(char *file)
{
	if(file)
		doreadhelp(file);
	if(dopus_firsthelp)
		screen_gadgets[SCRGAD_TINYHELP].Flags &= ~GFLG_DISABLED;
	else
		screen_gadgets[SCRGAD_TINYHELP].Flags |= GFLG_DISABLED;
	if(Window && config->generalscreenflags & SCR_GENERAL_TINYGADS)
		dolittlegads(&screen_gadgets[SCRGAD_TINYHELP], &globstring[STR_TINY_BUTTONS][4], 1);
}

void doreadhelp(char *file)
{
	int a, b = 0, inmsg;
	int64 helpsize;
	STRPTR helpbuf, buf;
	struct Help *temph, *curhelp;

	freehelp();
	if(!file[0] || !(buf = IExec->AllocVec(10000, MEMF_CLEAR)))
		return;
	if(readfile(file, &helpbuf, &helpsize))
	{
		IExec->FreeVec(buf);
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
					if(!(temph = IExec->AllocPooled(help_memory_pool, sizeof(struct Help))))
						break;
					if(!(temph->lookup = IExec->AllocPooled(help_memory_pool, b + 1)))
						break;
					IExec->CopyMem(buf, temph->lookup, b);
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
						if(!(curhelp->message = IExec->AllocPooled(help_memory_pool, b + 1)))
							break;
						IExec->CopyMem(buf, curhelp->message, b);
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
	IExec->FreeVec(buf);
	IExec->FreeVec(helpbuf);
}

CONST_STRPTR helpcontext[] =
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
	"Copy",
	"CopyAs",
	"CopyWindow",
	"DateStamp",
	"Defaults",
	"Delete",
	"DirTree",
	"DiskInfo",
	"DisplayDir",
	"DOpusToBack",
	"DOpusToFront",
	"Encrypt",
	"ErrorHelp",
	"Execute",
	"Extract",
	"FileInfo",
	"FinishSection",
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

void dohelp(char *name, char *func, int key, int qual, char *defmsg)
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
			IDOpus->RawkeyToStr(key, qual, buf2, NULL, 30);
			strcpy(buf1, globstring[STR_KEY]);
			strcat(buf1, buf2);
			strcat(buf1, "\n\n");
			s1 = strlen(buf1);
			s = strlen(msg) + 1;
			if((buf = IExec->AllocVec(s + s1, MEMF_CLEAR)))
			{
				IExec->CopyMem(buf1, buf, s1);
				IExec->CopyMem(msg, &buf[s1], s);
				s += s1;
				msg = buf;
			}
		}
		simplerequest(TDRIMAGE_INFO, msg, globstring[STR_CONTINUE], NULL);
		if(buf)
			IExec->FreeVec(buf);
	}
	else if(IAmigaGuide && func)
	{
		struct NewAmigaGuide nag = { 0 };
		AMIGAGUIDECONTEXT agc;
		char buf[32];

		nag.nag_Name = "DirectoryOpus.guide";
		nag.nag_Screen = Window->WScreen;
		nag.nag_Flags = 0;
		nag.nag_Context = helpcontext;
		nag.nag_Node = getAGnode(buf, func[0] == '*' ? func + 1 : func);

		if((agc = IAmigaGuide->OpenAmigaGuide(&nag, NULL)))
		{
			IAmigaGuide->CloseAmigaGuide(agc);
		}
		else
			simplerequest(TDRIMAGE_INFO, defmsg ? defmsg : globstring[STR_HELP_NOT_AVAILABLE], globstring[STR_CONTINUE], NULL);
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
