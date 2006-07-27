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

static struct NewBroker hotkey_broker =
{
	NB_VERSION,
	NULL,
	"Directory Opus © Jonathan Potter",
	"The most amazing program ever written",
	0, COF_SHOW_HIDE, 100, NULL, 0
};

static IX hotkey_ix = { IX_VERSION };
static APTR mui_gauge, mui_text;

void hotkeytaskcode()
{
	char cxname[20];
	int top, sig, waitbits, commodity = 0, command, x, run = 1;
	struct dopustaskmsg *hmsg;
	struct MsgPort *inputport;
	ULONG msgid, msgtype;
	struct dopushotkey *hotkey;
	CxObj *broker = NULL, *hotkey_filter = NULL, *mmb_filter = NULL;
	CxMsg *cxmsg;
	APTR muiapp, pwindow;

	hotkeymsg_port = CreateMsgPort();
	inputport = CreateMsgPort();

	pwindow = NULL;

	{
		strcpy(cxname, "Directory Opus");
		if(system_dopus_runcount)
		{
			char tmp[8];

			sprintf(tmp, " (%d)", system_dopus_runcount + 1);
			strcat(cxname, tmp);
		}
		hotkey_broker.nb_Name = cxname;
		hotkey_broker.nb_Port = inputport;
		if((broker = CxBroker(&hotkey_broker, NULL)))
		{

			/* Initialise main hotkey */
			if((hotkey_filter = set_dopus_filter(broker, inputport, NULL, config->hotkeycode, config->hotkeyqual, HOTKEY_UNICONIFY, 1)))
			{

				hotkey_ix.ix_Code = IECODE_LBUTTON;
				hotkey_ix.ix_Qualifier = IEQUALIFIER_RBUTTON;
				hotkey_ix.ix_QualMask = 0xffff & ~(IEQUALIFIER_LEFTBUTTON | IEQUALIFIER_RELATIVEMOUSE | IEQUALIFIER_CAPSLOCK);
				if(set_dopus_filter(broker, inputport, "rawmouse lbutton", 0, 0, HOTKEY_ABORT, 0))
				{
					hotkey_ix.ix_Code = IECODE_RBUTTON;
					hotkey_ix.ix_Qualifier = IEQUALIFIER_LEFTBUTTON;
					hotkey_ix.ix_QualMask = 0xffff & ~(IEQUALIFIER_RBUTTON | IEQUALIFIER_RELATIVEMOUSE | IEQUALIFIER_CAPSLOCK);
					if(set_dopus_filter(broker, inputport, "rawmouse rbutton", 0, 0, HOTKEY_ABORT, 0))
					{

						if(config->hotkeyflags & HOTKEY_USEMMB)
						{
							hotkey_ix.ix_Code = IECODE_MBUTTON;
							hotkey_ix.ix_Qualifier = IEQUALIFIER_MIDBUTTON;
							hotkey_ix.ix_QualMask = 0xffff & ~(IEQUALIFIER_RELATIVEMOUSE | IEQUALIFIER_CAPSLOCK);
							mmb_filter = set_dopus_filter(broker, inputport, "rawmouse midbutton", 0, 0, HOTKEY_MMB, 1);
						}

						add_hotkey_objects(broker, inputport, 1);
						set_hotkey(hotkey_filter, config->hotkeycode, config->hotkeyqual);

						ActivateCxObj(broker, 1);
						commodity = 1;
					}
				}
			}
		}
		if(broker && (!commodity))
		{
			add_hotkey_objects(broker, inputport, 0);
			DeleteCxObjAll(broker);
		}
	}

	muiapp = ApplicationObject,
		MUIA_Application_NoIconify, TRUE,
		MUIA_Application_UseRexx, FALSE,
		End;

	waitbits = 1 << hotkeymsg_port->mp_SigBit;
	if(commodity)
		waitbits |= 1 << inputport->mp_SigBit;

	NewRawDoFmt("DOPUS: muiapp is %08lx\n", (APTR)1, NULL, muiapp);

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
		command = 0;

		if(commodity)
		{
			while((cxmsg = (CxMsg *)GetMsg(inputport)))
			{
				msgid = CxMsgID(cxmsg);
				msgtype = CxMsgType(cxmsg);
				ReplyMsg((struct Message *)cxmsg);
				switch(msgtype)
				{
				case CXM_IEVENT:
					if((command = msgid) >= HOTKEY_HOTKEY)
					{
						x = command - HOTKEY_HOTKEY;
						command = HOTKEY_HOTKEY;
						hotkey = dopus_firsthotkey;
						while(x-- && hotkey)
							hotkey = hotkey->next;
						dopus_globalhotkey = hotkey;
					}
					break;

				case CXM_COMMAND:
					switch (msgid)
					{
					case CXCMD_KILL:
						command = HOTKEY_HOTKEY;
						dopus_globalhotkey = (struct dopushotkey *)-1;
						break;

					case CXCMD_DISABLE:
						ActivateCxObj(broker, 0);
						break;

					case CXCMD_ENABLE:
						ActivateCxObj(broker, 1);
						break;

					case CXCMD_APPEAR:
						command = HOTKEY_UNICONIFY;
						break;

					case CXCMD_DISAPPEAR:
						command = HOTKEY_HOTKEY;
						dopus_globalhotkey = (struct dopushotkey *)-2;
						break;
					}
					break;
				}
			}
		}

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
		case HOTKEY_UNICONIFY:
			if(status_configuring == -1)
				break;
			if(status_iconified == 1)
				Signal((struct Task *)main_proc, INPUTSIG_UNICONIFY);
			else if(status_iconified == 0)
			{
				if(MainScreen)
				{
					if(!status_configuring && config->screenflags & SCRFLAGS_HALFHEIGHT)
						top = main_scr.TopEdge;
					else
						top = 0;

					if(IntuitionBase->FirstScreen == MainScreen && MainScreen->TopEdge == top && (!(IntuitionBase->ActiveWindow || IntuitionBase->ActiveWindow->WScreen == MainScreen)))
					{
						ScreenToBack(MainScreen);
						if(Window->Parent && Window->Parent->WScreen == IntuitionBase->FirstScreen)
							ActivateWindow(Window->Parent);
						else
							ActivateWindow(IntuitionBase->FirstScreen->FirstWindow);
					}
					else
					{
						struct Window *window;

						ScreenToFront(MainScreen);
						if(MainScreen->FirstWindow == ansiread_window)
							window = Window;
						else
							window = MainScreen->FirstWindow;

						if(!(window->Flags & WFLG_BACKDROP))
							WindowToFront(window);
						ActivateWindow(window);
					}
					MoveScreen(MainScreen, 0, top - MainScreen->TopEdge);
				}
				else
				{
					if(IntuitionBase->ActiveWindow == Window && IntuitionBase->FirstScreen == Window->WScreen && Window->WScreen->LayerInfo.top_layer == Window->RPort->Layer)
					{
						WindowToBack(Window);
						if(Window->Parent)
							ActivateWindow(Window->Parent);
					}
					else
					{
						ScreenToFront(Window->WScreen);
						WindowToFront(Window);
						ActivateWindow(Window);
					}
				}
			}
			break;

		case HOTKEY_HOTKEY:
			if(!status_configuring)
				Signal((struct Task *)main_proc, INPUTSIG_HOTKEY);
			break;
		}

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

			case HOTKEY_HOTKEYCHANGE:
				if(commodity)
				{
					set_hotkey(hotkey_filter, config->hotkeycode, config->hotkeyqual);

					if(config->hotkeyflags & HOTKEY_USEMMB)
					{
						if(!mmb_filter)
						{
							hotkey_ix.ix_Code = IECODE_MBUTTON;
							hotkey_ix.ix_Qualifier = IEQUALIFIER_MIDBUTTON;
							hotkey_ix.ix_QualMask = 0xffff & ~(IEQUALIFIER_RELATIVEMOUSE | IEQUALIFIER_CAPSLOCK);
							mmb_filter = set_dopus_filter(broker, inputport, "rawmouse midbutton", 0, 0, HOTKEY_MMB, 1);
						}
					}
					else if(mmb_filter)
					{
						DeleteCxObjAll(mmb_filter);
						mmb_filter = NULL;
					}
				}
				break;

			case HOTKEY_KILLHOTKEYS:
				if(commodity)
					add_hotkey_objects(broker, inputport, 0);
				break;

			case HOTKEY_NEWHOTKEYS:
				if(commodity)
					add_hotkey_objects(broker, inputport, 1);
				break;
			}
			ReplyMsg((struct Message *)hmsg);
		}
	}

	if(commodity)
	{
		add_hotkey_objects(broker, inputport, 0);
		DeleteCxObjAll(broker);
		while((cxmsg = (CxMsg *)GetMsg(inputport)))
			ReplyMsg((struct Message *)cxmsg);
	}

	MUI_DisposeObject(muiapp);

	LFreeRemember(&prog_key);
	DeleteMsgPort(inputport);
	DeleteMsgPort(hotkeymsg_port);
//	Wait(0);
	Forbid();
	hotkeymsg_port = NULL;
}

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

static const char *Kstr = "K  ";

void clocktask()
{
	ULONG chipc, fast, wmes, h, m, s, cx, sig, cy, /*len,*/ ct, chipnum, fastnum, a, active = 1, usage;
	int len;
	USHORT clock_width, clock_height, scr_height;
	char buf[160], date[20], time[20], formstring[160], memstring[160], ampm;
	struct MsgPort *clock_time_port;
	struct timerequest ctimereq;
	struct DateTime datetime = { { 0, } };
	struct dopustaskmsg *cmsg;
	struct RastPort clock_rp;

	Forbid();
	CopyMem((STRPTR)main_rp, (STRPTR)&clock_rp, sizeof(struct RastPort));
	SetDrawModes(&clock_rp, config->clockfg, config->clockbg, JAM2);
	SetFont(&clock_rp, scr_font[FONT_CLOCK]);
	scr_height = scrdata_height + scrdata_yoffset;
	clock_width = scrdata_clock_width;
	clock_height = scrdata_clock_height;
	ct = scr_height - (clock_height - 1);
	cy = scrdata_clock_ypos + scr_font[FONT_CLOCK]->tf_Baseline - 1;
	Permit();

	clockmsg_port = CreateMsgPort();
	clock_time_port = CreateMsgPort();

	OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)&ctimereq, 0);
	ctimereq.tr_node.io_Message.mn_ReplyPort = clock_time_port;
	ctimereq.tr_node.io_Command = TR_ADDREQUEST;
	ctimereq.tr_node.io_Flags = 0;
	ctimereq.tr_time.tv_secs = 0;
	ctimereq.tr_time.tv_micro = 2;
	SendIO(&ctimereq.tr_node);

	chipnum = getmaxmem(MEMF_CHIP);
	fastnum = getmaxmem(MEMF_FAST);
	a = getmaxmem(MEMF_ANY);

	m = (config->scrclktype & SCRCLOCK_BYTES) ? 3 : 0;
	s = (config->scrclktype & SCRCLOCK_BYTES) ? 1 : 0;

	if(config->scrclktype & SCRCLOCK_C_AND_F)
	{
		sprintf(memstring, "%lc:%%-%ldld%s", globstring[STR_CLOCK_CHIP][0], chipnum + m, Kstr + s);
		if(fastnum > 1)
		{
			sprintf(memstring + strlen(memstring), "%lc:%%-%ldld%s", globstring[STR_CLOCK_FAST][0], fastnum + m, Kstr + s);
			sprintf(memstring + strlen(memstring), "%lc:%%-%ldld%s", globstring[STR_CLOCK_TOTAL][0], a + m, Kstr + s);
		}
	}
	else
	{
		sprintf(memstring, "%s%%-%ldld%s", globstring[STR_CLOCK_CHIP], chipnum + m, Kstr + s);
		if(fastnum > 1)
		{
			sprintf(memstring + strlen(memstring), "%s%%-%ldld%s", globstring[STR_CLOCK_FAST], fastnum + m, Kstr + s);
			sprintf(memstring + strlen(memstring), "%s%%-%ldld%s", globstring[STR_CLOCK_TOTAL], a + m, Kstr + s);
		}
	}

	if(!(config->scrclktype & (SCRCLOCK_MEMORY | SCRCLOCK_CPU | SCRCLOCK_DATE | SCRCLOCK_TIME)))
		sprintf(formstring, "Directory Opus  Version %s  Compiled %s  %s", str_version_string, comp_time, comp_date);

	sig = 1 << clock_time_port->mp_SigBit | 1 << clockmsg_port->mp_SigBit;

	FOREVER
	{
		wmes = Wait(sig);
		if(wmes & 1 << clockmsg_port->mp_SigBit)
		{
			while((cmsg = (struct dopustaskmsg *)GetMsg(clockmsg_port)))
			{
				switch (cmsg->command)
				{
				case TASK_QUIT:
					if(!(CheckIO(&ctimereq.tr_node)))
						AbortIO(&ctimereq.tr_node);
					WaitIO(&ctimereq.tr_node);
					CloseDevice((struct IORequest *)&ctimereq);
					DeleteMsgPort(clock_time_port);
					DeleteMsgPort(clockmsg_port);
					clockmsg_port = NULL;
					ReplyMsg((struct Message *)cmsg);
					Forbid();
					return;
				case CLOCK_ACTIVE:
					active = cmsg->value;
					break;
				}
				ReplyMsg((struct Message *)cmsg);
			}
		}
		if(wmes & 1 << clock_time_port->mp_SigBit)
		{
			if(active && !(Window->Flags & WFLG_MENUSTATE))
			{
				if(scr_height > ct + 1)
				{
					if(config->scrclktype & (SCRCLOCK_MEMORY | SCRCLOCK_CPU | SCRCLOCK_DATE | SCRCLOCK_TIME))
					{
						formstring[0] = 0;
						if(config->scrclktype & SCRCLOCK_MEMORY)
						{
							chipc = AvailMem(MEMF_CHIP);
							fast = AvailMem(MEMF_FAST);
							if(!(config->scrclktype & SCRCLOCK_BYTES))
							{
								chipc /= 1024;
								fast /= 1024;
							}
							sprintf(buf, memstring, chipc, fast, chipc + fast);
							strcat(formstring, buf);
						}
						if(config->scrclktype & SCRCLOCK_CPU)
						{
							usage = getusage();

							sprintf(buf, "CPU:%3ld%%  ", usage);
							strcat(formstring, buf);
						}
						if(config->scrclktype & (SCRCLOCK_DATE | SCRCLOCK_TIME))
						{
							DateStamp(&(datetime.dat_Stamp));
							initdatetime(&datetime, date, time, 0);

							if(config->scrclktype & SCRCLOCK_DATE)
							{
								sprintf(buf, "%-9s  ", date);
								strcat(formstring, buf);
							}
							if(config->scrclktype & SCRCLOCK_TIME)
							{
								if(config->dateformat & DATE_12HOUR)
								{
									h = datetime.dat_Stamp.ds_Minute / 60;
									m = datetime.dat_Stamp.ds_Minute % 60;
									s = datetime.dat_Stamp.ds_Tick / TICKS_PER_SECOND;
									if(h > 11)
									{
										ampm = 'P';
										h -= 12;
									}
									else
									{
										ampm = 'A';
									}
									if(h == 0)
									{
										h = 12;
									}
									sprintf(time, "%02ld:%02ld:%02ld%c", h, m, s, ampm);
								}
								strcat(formstring, time);
							}
						}
					}
					len = strlen(formstring);
					if(len > 1 && formstring[len - 2] == ' ')
						len -= 2;
					cx = (clock_width - dotextlength(&clock_rp, formstring, &len, clock_width - 4)) / 2;
					cx += scrdata_clock_xpos;
					if(cx < scrdata_clock_xpos)
						cx = scrdata_clock_xpos;

					SetAPen(&clock_rp, screen_pens[config->clockfg].pen);
					Move(&clock_rp, cx, cy);
					Text(&clock_rp, formstring, len);
					SetAPen(&clock_rp, screen_pens[config->clockbg].pen);

					if(cx > scrdata_clock_xpos)
						RectFill(&clock_rp, scrdata_clock_xpos, ct, cx - 1, scr_height - 2);
					if(clock_rp.cp_x < clock_width - 1)
						RectFill(&clock_rp, clock_rp.cp_x, ct, clock_width - 2, scr_height - 2);
				}
			}
			ctimereq.tr_time.tv_secs = 1;
			ctimereq.tr_time.tv_micro = 0;
			SendIO(&ctimereq.tr_node);
		}
	}
}
