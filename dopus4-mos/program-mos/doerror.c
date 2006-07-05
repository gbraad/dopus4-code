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

int doerror(int err)
{
	char buf[80];

	if(err == -1)
		err = IoErr();
	if(err)
	{
		geterrorstring(buf, err);
		dostatustext(buf);
		rexx_result_code = err;
		return (1);
	}
	return (0);
}

void geterrorstring(char *buf, int err)
{
	char buf2[80];

	Fault(err, NULL, buf2, 80);
	sprintf(buf, globstring[STR_DOS_ERROR_CODE], err);
	strcat(buf, " - ");
	strcat(buf, buf2);
}

void dostatustext(char *text)
{
	int x, len, l;
	struct RastPort *r;

	if(status_iconified)
	{
		if(status_flags & STATUS_ISINBUTTONS)
			iconstatustext(text, 1);
		return;
	}
	if(status_flags & STATUS_FROMHOTKEY)
		return;
	strcpy(str_last_statustext, text);
	if(scrdata_status_height > 0)
	{
		r = main_rp;
		SetDrawModes(r, config->statusfg, config->statusbg, JAM2);
		SetFont(r, scr_font[FONT_STATUS]);
		len = 0;
		l = dotextlength(r, text, &len, scrdata_status_width - 4);
		switch (scrdata_statustext_pos)
		{
		case TOPTEXT_CENTER:
			x = ((scrdata_status_width - l) / 2) + scrdata_status_xpos;
			break;
		case TOPTEXT_RIGHT:
			x = (scrdata_status_width - l) + scrdata_status_xpos;
			break;
		default /*case TOPTEXT_LEFT */ :
			x = scrdata_status_xpos + 2;
			break;
		}
		if(x < scrdata_status_xpos)
			x = scrdata_status_xpos;
		Move(r, x, scr_font[FONT_STATUS]->tf_Baseline + 2 + scrdata_yoffset);
		Text(r, text, len);
		SetFont(r, scr_font[FONT_GENERAL]);
		SetAPen(r, screen_pens[config->statusbg].pen);
		if(x > scrdata_status_xpos)
			RectFill(r, scrdata_status_xpos, scrdata_status_ypos + 1, x - 1, scrdata_yoffset + scrdata_status_height - 2);
		if(x + l < scrdata_status_xpos + scrdata_status_width)
			RectFill(r, x + l, scrdata_status_ypos + 1, scrdata_status_xpos + scrdata_status_width - 1, scrdata_yoffset + scrdata_status_height - 2);
	}
	else
	{
		if(status_publicscreen)
			SetWindowTitles(Window, str_last_statustext, (char *)-1);
		else
		{
			SetWindowTitles(Window, (char *)-1, str_last_statustext);
			if(IntuitionBase->ActiveScreen == MainScreen && IntuitionBase->ActiveWindow != Window)
				SetWindowTitles(IntuitionBase->ActiveWindow, (char *)-1, str_last_statustext);
			MainScreen->DefaultTitle = str_last_statustext;
		}
	}
}

void okay()
{
	dostatustext(globstring[STR_OKAY_TITLE]);
	rexx_result_code = 0;
}

void myabort()
{
	dostatustext(globstring[STR_ABORTED]);
	status_justabort = 1;
	status_haveaborted = 0;
	rexx_result_code = -1;
}

void dofilename(char *text)
{
	int a, len, pos, old;
	struct TextFont *font;

	old = scrdata_statustext_pos;
	len = a = strlen(text);
	if(scrdata_status_height > 0)
		font = scr_font[FONT_STATUS];
	else
		font = Window->WScreen->RastPort.Font;

	gettextlength(font, text, &len, scrdata_status_width);
	pos = a - len;
	if(config->dynamicflags & UPDATE_LEFTJUSTIFY)
	{
		scrdata_statustext_pos = TOPTEXT_LEFT;
		if(pos > 0)
		{
			for(a = pos; a < len; a++)
			{
				if(text[a] == '/' || text[a] == ':')
				{
					while(text[a] == '/' || text[a] == ':')
						++a;
					break;
				}
			}
			pos = a;
		}
	}
	else
		scrdata_statustext_pos = TOPTEXT_CENTER;
	dostatustext(&text[pos]);
	scrdata_statustext_pos = old;
}

void geterrorhelp(int st)
{
	char buf[80], buf2[30];
	int err;

	if(!st)
	{
		buf[0] = 0;
		if(!(whatsit(globstring[STR_ENTER_DOS_ERROR_CODE], 4, buf, NULL)) || buf[0] == 0)
			return;
		err = atoi(buf);
	}
	else
		err = atoi(rexx_args[0]);
	if(doerror(err))
	{
		sprintf(buf2, "!%d", err);
		dohelp(buf2, NULL, 0, 0, globstring[STR_NO_HELP_FOR_ERROR]);
	}
	else
		dostatustext(globstring[STR_NO_HELP_FOR_ERROR]);
}

int checkerror(char *action, char *name, int err)
{
	char buf[300], buf2[200];
	int a, skip = 1, erhelp = 1;

	if(err < 0)
	{
		err = -err;
		skip = 0;
	}
	if(!(config->errorflags & ERROR_ENABLE_OPUS))
		return ((skip) ? 2 : 3);

	geterrorstring(buf2, err);
	sprintf(buf, globstring[STR_ERROR_OCCURED], action, name, buf2);

	for(;;)
	{
		a = simplerequest(buf, globstring[STR_TRY_AGAIN], globstring[STR_ABORT], (skip) ? globstring[STR_SKIP] : NULL, (dopus_firsthelp && erhelp) ? globstring[STR_ERROR_ERROR_HELP] : NULL, NULL);

		if((skip && a == 3) || (!skip && a == 2))
		{
			char helpbuf[20];

			sprintf(helpbuf, "!%d", err);
			dohelp(helpbuf, NULL, 0, 0, globstring[STR_NO_HELP_FOR_ERROR]);
			busy();
		}
		else
			break;
	}

	if(a == 0)
		return (3);
	return (a);
}
