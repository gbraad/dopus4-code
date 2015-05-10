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
#include "DirectoryOpus_rev.h"

#define HOTKEY_UNICONIFY   1
#define HOTKEY_ABORT     3
#define HOTKEY_MMB     5
#define HOTKEY_HOTKEY    10

struct NewBroker hotkey_broker =
{
	NB_VERSION,
	NULL,
	"Directory Opus © Jonathan Potter",
	"The most amazing program ever written",
	0,
	COF_SHOW_HIDE,
	100,
	NULL,
	0
};

IX hotkey_ix = { IX_VERSION };

void hotkeytaskcode()
{
	char cxname[20];
	int top, sig, waitbits, commodity = 0, command, x, run = 1;
	struct dopustaskmsg *hmsg;
	struct MsgPort *inputport;
	uint32 msgid, msgtype;
	struct dopushotkey *hotkey;
	CxObj *broker = NULL, *hotkey_filter = NULL, *mmb_filter = NULL;
	CxMsg *cxmsg;

	hotkeymsg_port = IExec->CreatePort(NULL, 0);
	inputport = IExec->CreatePort(NULL, 0);

	if(CxBase && ICommodities)
	{
		strcpy(cxname, "Directory Opus");
		if(system_dopus_runcount)
		{
			char tmp[8];

			snprintf(tmp, 8, " (%d)", system_dopus_runcount + 1);
			strncat(cxname, tmp, 20);
		}
		hotkey_broker.nb_Name = cxname;
		hotkey_broker.nb_Port = inputport;
		if((broker = ICommodities->CxBroker(&hotkey_broker, NULL)))
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

						ICommodities->ActivateCxObj(broker, 1);
						commodity = 1;
					}
				}
			}
		}
		if(broker && (!commodity))
		{
			add_hotkey_objects(broker, inputport, 0);
			ICommodities->DeleteCxObjAll(broker);
		}
	}

	waitbits = 1 << hotkeymsg_port->mp_SigBit;
	if(commodity)
		waitbits |= 1 << inputport->mp_SigBit;

	while(run)
	{
		sig = IExec->Wait(waitbits);
		command = 0;

		if(commodity)
		{
			while((cxmsg = (CxMsg *)IExec->GetMsg(inputport)))
			{
				msgid = ICommodities->CxMsgID(cxmsg);
				msgtype = ICommodities->CxMsgType(cxmsg);
				IExec->ReplyMsg((struct Message *)cxmsg);
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
						ICommodities->ActivateCxObj(broker, 0);
						break;

					case CXCMD_ENABLE:
						ICommodities->ActivateCxObj(broker, 1);
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
			if(((struct IntuitionBase *)(IIntuition->Data.LibBase))->ActiveWindow == Window)
			{
				status_haveaborted = status_rexxabort = 1;
				IExec->Signal((struct Task *)main_proc, SIGBREAKF_CTRL_C);
			}
			break;

		case HOTKEY_MMB:
			if(!(config->hotkeyflags & HOTKEY_USEMMB))
				break;
		case HOTKEY_UNICONIFY:
			if(status_configuring == -1)
				break;
			if(status_iconified == 1)
				IExec->Signal((struct Task *)main_proc, INPUTSIG_UNICONIFY);
			else if(status_iconified == 0)
			{
				if(MainScreen)
				{
					if(!status_configuring && config->screenflags & SCRFLAGS_HALFHEIGHT)
						top = main_scr.TopEdge;
					else
						top = 0;

					if((((struct IntuitionBase *)(IIntuition->Data.LibBase))->FirstScreen == MainScreen) && (MainScreen->TopEdge == top) && (!((struct IntuitionBase *)(IIntuition->Data.LibBase))->ActiveWindow || (((struct IntuitionBase *)(IIntuition->Data.LibBase))->ActiveWindow->WScreen == MainScreen)))
					{
						IIntuition->ScreenToBack(MainScreen);
						if(Window->Parent && Window->Parent->WScreen == ((struct IntuitionBase *)(IIntuition->Data.LibBase))->FirstScreen)
							IIntuition->ActivateWindow(Window->Parent);
						else
							IIntuition->ActivateWindow(((struct IntuitionBase *)(IIntuition->Data.LibBase))->FirstScreen->FirstWindow);
					}
					else
					{
						struct Window *window;

						IIntuition->ScreenToFront(MainScreen);
						if(MainScreen->FirstWindow == ansiread_window)
							window = Window;
						else
							window = MainScreen->FirstWindow;

						if(!(window->Flags & WFLG_BACKDROP))
							IIntuition->WindowToFront(window);
						IIntuition->ActivateWindow(window);
					}
					IIntuition->MoveScreen(MainScreen, 0, top - MainScreen->TopEdge);
				}
				else
				{
					if(((struct IntuitionBase *)(IIntuition->Data.LibBase))->ActiveWindow == Window && ((struct IntuitionBase *)(IIntuition->Data.LibBase))->FirstScreen == Window->WScreen && Window->WScreen->LayerInfo.top_layer == Window->RPort->Layer)
					{
						IIntuition->WindowToBack(Window);
						if(Window->Parent)
							IIntuition->ActivateWindow(Window->Parent);
					}
					else
					{
						IIntuition->ScreenToFront(Window->WScreen);
						IIntuition->WindowToFront(Window);
						IIntuition->ActivateWindow(Window);
					}
				}
			}
			break;

		case HOTKEY_HOTKEY:
			if(!status_configuring)
				IExec->Signal((struct Task *)main_proc, INPUTSIG_HOTKEY);
			break;
		}

		while((hmsg = (struct dopustaskmsg *)IExec->GetMsg(hotkeymsg_port)))
		{
			switch (hmsg->command)
			{

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
						ICommodities->DeleteCxObjAll(mmb_filter);
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
			IExec->ReplyMsg((struct Message *)hmsg);
		}
	}

	if(commodity)
	{
		add_hotkey_objects(broker, inputport, 0);
		ICommodities->DeleteCxObjAll(broker);
		while((cxmsg = (CxMsg *)IExec->GetMsg(inputport)))
			IExec->ReplyMsg((struct Message *)cxmsg);
	}

	IExec->DeletePort(inputport);
	IExec->DeletePort(hotkeymsg_port);
	IExec->Wait(0);
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
			ICommodities->DeleteCxObjAll(filter_table[a]);
		IExec->FreeVec(filter_table);
		filter_table = NULL;
	}

	if(add == 0)
		return;

	for(hotkey = dopus_firsthotkey, hotkey_count = 0; hotkey; hotkey = hotkey->next)
		hotkey_count++;
	if(hotkey_count == 0)
		return;

	filter_table = doAllocVec(hotkey_count * sizeof(CxObj *), MEMF_CLEAR);
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
			ICommodities->SetFilterIX(filter, &hotkey_ix);
		}
		ICommodities->AttachCxObj(broker, filter);
		ICommodities->AttachCxObj(filter, CxSender(port, command));
		if(translate)
			ICommodities->AttachCxObj(filter, CxTranslate(NULL));

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
		ICommodities->SetFilterIX(filter, &hotkey_ix);
	}
}

const char *Kstr = "K  ";

void clocktask()
{
	uint32 wmes, h, m, s, cx, sig, cy, ct, active = 1;
	int len;
	uint16 clock_width, clock_height, scr_height;
	char buf[160], date[20], time[20], formstring[160], ampm;
	struct MsgPort *clock_time_port;
	struct TimeRequest ctimereq;
	struct DateTime datetime = { { 0, } };
	struct dopustaskmsg *cmsg;
	struct RastPort clock_rp;

	IExec->Forbid();
	IExec->CopyMem((STRPTR)main_rp, (STRPTR)&clock_rp, sizeof(struct RastPort));
	SetDrawModes(&clock_rp, config->clockfg, config->clockbg, JAM2);
	IGraphics->SetFont(&clock_rp, scr_font[FONT_CLOCK]);
	scr_height = scrdata_height + scrdata_yoffset;
	clock_width = scrdata_clock_width;
	clock_height = scrdata_clock_height;
	ct = scr_height - (clock_height - 1);
	cy = scrdata_clock_ypos + scr_font[FONT_CLOCK]->tf_Baseline - 1;
	IExec->Permit();

	clockmsg_port = IExec->CreatePort(NULL, 0);
	clock_time_port = IExec->CreatePort(0, 0);

	IExec->OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)&ctimereq.Request, 0);
	ctimereq.Request.io_Message.mn_ReplyPort = clock_time_port;
	ctimereq.Request.io_Command = TR_ADDREQUEST;
	ctimereq.Request.io_Flags = 0;
	ctimereq.Time.Seconds = 0;
	ctimereq.Time.Microseconds = 2;
	IExec->SendIO(&ctimereq.Request);

//	m = (config->scrclktype & SCRCLOCK_BYTES) ? 3 : 0;
//	s = (config->scrclktype & SCRCLOCK_BYTES) ? 1 : 0;

	if(!(config->scrclktype & (SCRCLOCK_DATE | SCRCLOCK_TIME)))
		snprintf(formstring, 160, "%s (%s)", NEW_VERS, DATE);

	sig = 1 << clock_time_port->mp_SigBit | 1 << clockmsg_port->mp_SigBit;

	for(;;)
	{
		wmes = IExec->Wait(sig);
		if(wmes & 1 << clockmsg_port->mp_SigBit)
		{
			while((cmsg = (struct dopustaskmsg *)IExec->GetMsg(clockmsg_port)))
			{
				switch (cmsg->command)
				{
				case TASK_QUIT:
					if(!(IExec->CheckIO(&ctimereq.Request)))
						IExec->AbortIO(&ctimereq.Request);
					IExec->WaitIO(&ctimereq.Request);
					IExec->CloseDevice((struct IORequest *)&ctimereq.Request);
					IExec->DeletePort(clock_time_port);
					IExec->DeletePort(clockmsg_port);
					clockmsg_port = NULL;
					IExec->ReplyMsg((struct Message *)cmsg);
					IExec->Forbid();
					return;
				case CLOCK_ACTIVE:
					active = cmsg->value;
					break;
				}
				IExec->ReplyMsg((struct Message *)cmsg);
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
						if(config->scrclktype & (SCRCLOCK_DATE | SCRCLOCK_TIME))
						{
							IDOS->DateStamp(&(datetime.dat_Stamp));
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

					IGraphics->SetAPen(&clock_rp, screen_pens[config->clockfg].pen);
					IGraphics->Move(&clock_rp, cx, cy);
					IGraphics->Text(&clock_rp, formstring, len);
					IGraphics->SetAPen(&clock_rp, screen_pens[config->clockbg].pen);

					if(cx > scrdata_clock_xpos)
						IGraphics->RectFill(&clock_rp, scrdata_clock_xpos, ct, cx - 1, scr_height - 2);
					if(clock_rp.cp_x < clock_width - 1)
						IGraphics->RectFill(&clock_rp, clock_rp.cp_x, ct, clock_width - 2, scr_height - 2);
				}
			}
			ctimereq.Time.Seconds = 1;
			ctimereq.Time.Microseconds = 0;
			IExec->SendIO(&ctimereq.Request);
		}
	}
}
