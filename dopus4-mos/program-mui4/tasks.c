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
#include <proto/muimaster.h>

#include "dopus.h"
#include "mui.h"

#define HOTKEY_UNICONIFY   1
#define HOTKEY_ABORT     3
#define HOTKEY_MMB     5
#define HOTKEY_HOTKEY    10

struct ProgressBar
{
	int curr, max;
	BOOL incignore;
	BOOL hide;
};

#define ID_ABORT 0xc000

static APTR openprogresswindow(APTR app, CONST_STRPTR title, int value, int total, int flag);
static void progressbar(struct ProgressBar *bar);
static void progresstext(int val, int total, CONST_STRPTR text);

static struct DOpusRemember *prog_key;
static struct ProgressBar bar[2];

static APTR mui_gauge, mui_text;

void hotkeytaskcode()
{
	int sig, waitbits, run = 1;
	struct dopustaskmsg *hmsg;
	APTR muiapp, pwindow;

	hotkeymsg_port = CreateMsgPort();

	pwindow = NULL;

	muiapp = ApplicationObject,
		MUIA_Application_NoIconify, TRUE,
		MUIA_Application_UseRexx, FALSE,
		End;

	waitbits = 1 << hotkeymsg_port->mp_SigBit;

	sig = 0;

	while(run)
	{
		for (;;)
		{
			ULONG ret = DoMethod(muiapp, MUIM_Application_NewInput, &sig);

			if (ret == ID_ABORT)
			{
				status_haveaborted = status_rexxabort = 1;
				Signal((struct Task *)main_proc, INPUTSIG_ABORT);
			}

			if (sig)
				break;
		}

		sig = Wait(waitbits | sig);

		#if 0
		command = 0;

		switch (command)
		{
		case HOTKEY_ABORT:
			if(IntuitionBase->ActiveWindow == Window)
			{
				status_haveaborted = status_rexxabort = 1;
				Signal((struct Task *)main_proc, INPUTSIG_ABORT);
			}
			break;

		case HOTKEY_MMB:
			if(!(config->hotkeyflags & HOTKEY_USEMMB))
				break;
		case HOTKEY_HOTKEY:
			if(!status_configuring)
				Signal((struct Task *)main_proc, INPUTSIG_HOTKEY);
			break;
		}
		#endif

		while((hmsg = (struct dopustaskmsg *)GetMsg(hotkeymsg_port)))
		{

#define BAR_ID (hmsg->flag)

			switch (hmsg->command)
			{
			case PROGRESS_UPDATE:
				if(pwindow)
				{
					if(hmsg->value > -1)
					{
						bar[BAR_ID].curr = hmsg->value;
						bar[BAR_ID].max = hmsg->total;

						progressbar(&bar[BAR_ID]);
					}
					if(bar[BAR_ID].hide)
						break;
					if(BAR_ID == 0 || hmsg->data)
						progresstext(hmsg->value, hmsg->total, hmsg->data);
				}
				break;

			case PROGRESS_INCREASE:
				if(pwindow)
				{
					if(bar[BAR_ID].incignore)
						break;

					bar[BAR_ID].curr += hmsg->value;
					if(bar[BAR_ID].curr > bar[BAR_ID].max)
						bar[BAR_ID].curr = bar[BAR_ID].max;

					progressbar(&bar[BAR_ID]);
					if(bar[BAR_ID].hide)
						break;
					progresstext(bar[BAR_ID].curr, bar[BAR_ID].max, NULL);
				}
				break;

			case PROGRESS_OPEN:
				if(!pwindow)
				{
					pwindow = openprogresswindow(muiapp, hmsg->data, hmsg->value, hmsg->total, BAR_ID);
				}

				if (pwindow)
				{
					SetAttrs(mui_gauge, MUIA_Gauge_InfoText, "", MUIA_Gauge_Current, 0, TAG_DONE);
					SetAttrs(pwindow, MUIA_Window_Open, TRUE, TAG_DONE);
				}

				break;

			case PROGRESS_CLOSE:
				if(pwindow)
				{
					set(pwindow, MUIA_Window_Open, FALSE);
				}
				LFreeRemember(&prog_key);
				break;

			case TASK_QUIT:
				run = 0;
				break;
			}
			ReplyMsg((struct Message *)hmsg);
		}
	}

	MUI_DisposeObject(muiapp);

	LFreeRemember(&prog_key);
	DeleteMsgPort(hotkeymsg_port);
//	Wait(0);
	Forbid();
	hotkeymsg_port = NULL;
}

#if 0
void add_hotkey_objects(CxObj *broker, struct MsgPort *port, int add)
{
	static CxObj **filter_table;
	static int hotkey_count;
	struct dopushotkey *hotkey;
	int a;

	if(filter_table)
	{
		for(a = 0; a < hotkey_count; a++)
			DeleteCxObjAll(filter_table[a]);
		FreeMem(filter_table, hotkey_count * sizeof(CxObj *));
		filter_table = NULL;
	}

	if(add == 0)
		return;

	for(hotkey = dopus_firsthotkey, hotkey_count = 0; hotkey; hotkey = hotkey->next)
		hotkey_count++;
	if(hotkey_count == 0)
		return;

	filter_table = AllocMem(hotkey_count * sizeof(CxObj *), MEMF_CLEAR);
	if(filter_table == NULL)
		return;

	for(a = 0, hotkey = dopus_firsthotkey; hotkey; hotkey = hotkey->next, a++)
		filter_table[a] = set_dopus_filter(broker, port, NULL, hotkey->code, hotkey->qualifier, HOTKEY_HOTKEY + a, 1);
}

CxObj *set_dopus_filter(CxObj *broker, struct MsgPort *port, STRPTR string, USHORT code, USHORT qual, int command, int translate)
{
	CxObj *filter;

	if((filter = CxFilter(string)))
	{
		if(!string)
		{
			set_hotkey(filter, code, qual);
		}
		else
		{
			hotkey_ix.ix_Class = IECLASS_RAWMOUSE;
			hotkey_ix.ix_CodeMask = 0xff;
			hotkey_ix.ix_QualSame = 0;
			SetFilterIX(filter, &hotkey_ix);
		}
		AttachCxObj(broker, filter);
		AttachCxObj(filter, CxSender(port, command));
		if(translate)
			AttachCxObj(filter, CxTranslate(NULL));

		return (filter);
	}
	return (NULL);
}

void set_hotkey(CxObj *filter, USHORT code, USHORT qual)
{
	if(filter)
	{
		hotkey_ix.ix_Class = IECLASS_RAWKEY;
		if(qual == 0 && code == (USHORT) ~ 0)
		{
			hotkey_ix.ix_Code = 0xffff;
			hotkey_ix.ix_CodeMask = 0xffff;
			hotkey_ix.ix_Qualifier = 0xffff;
		}
		else
		{
			if(code == (USHORT) ~ 0)
			{
				hotkey_ix.ix_Code = 0;
				hotkey_ix.ix_CodeMask = 0;
			}
			else
			{
				hotkey_ix.ix_Code = code;
				hotkey_ix.ix_CodeMask = 0xff;
			}
			hotkey_ix.ix_Qualifier = qual & VALID_QUALIFIERS;
			hotkey_ix.ix_QualMask = VALID_QUALIFIERS;
		}
		SetFilterIX(filter, &hotkey_ix);
	}
}
#endif

static APTR openprogresswindow(APTR muiapp, CONST_STRPTR title, int value, int total, int flag)
{
	APTR win, abort;
	LONG reqwin, incignore = 0, a;

	if (flag & 0x80)
	{
		incignore = 1;
		flag ^= 0x80;
	}

	reqwin = config->generalscreenflags & SCR_GENERAL_REQDRAG;

	win = WindowObject,
			MUIA_Window_Width, MUIV_Window_Width_Visible(40),
			MUIA_Window_CloseGadget, FALSE,
			MUIA_Window_ShowAbout, FALSE,
			MUIA_Window_ShowIconify, FALSE,
			MUIA_Window_ShowJump, FALSE,
			MUIA_Window_ShowPopup, FALSE,
			MUIA_Window_ShowPrefs, FALSE,
			MUIA_Window_ShowSnapshot, FALSE,

			reqwin ? MUIA_Window_Title : TAG_IGNORE, title,
			!reqwin ? MUIA_Window_DragBar : TAG_IGNORE, FALSE,
			!reqwin ? MUIA_Window_DepthGadget : TAG_IGNORE, FALSE,

			WindowContents, VGroup,
				Child, mui_gauge = GaugeObject, MUIA_Gauge_Horiz, TRUE, End,
				Child, mui_text = TextObject, MUIA_Text_Copy, FALSE, End,
				Child, HCenter(abort = MakeButton(globstring[STR_ABORT])),
			End,
		End;

	if (win)
	{
		DoMethod(muiapp, OM_ADDMEMBER, win);
		DoMethod(abort, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Application, 2, MUIM_Application_ReturnID, ID_ABORT);
	}

	for(a = 0; a < 2; a++)
	{
		bar[a].curr = value;
		bar[a].max = total;
		bar[a].incignore = incignore;
		bar[a].hide = (a == 0) ? 0 : !flag;
	}

//	centerwindow(&progresswindow);
	return win;
}

static void progresstext(int val, int total, CONST_STRPTR text)
{
	static char buf[80], *ptr;

	if(val == -1)
		ptr = globstring[total ? STR_ABORTED : STR_COMPLETED];
	else
	{
		if(text)
			stccpy(buf, text, 80);
		else
			sprintf(buf, globstring[STR_REMAINING], val, total);
		ptr = buf;
	}

	SetAttrs(mui_text, MUIA_Text_Contents, ptr, TAG_DONE);
}

static void progressbar(struct ProgressBar *bar)
{
	LONG max, curr;

	if(bar->hide)
		return;

	max = bar->max;
	curr = bar->curr;

	SetAttrs(mui_gauge, MUIA_Gauge_Max, max, MUIA_Gauge_Current, curr, TAG_DONE);
}
