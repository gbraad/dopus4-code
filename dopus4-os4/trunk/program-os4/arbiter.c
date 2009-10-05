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
//#include "view.h"

struct MsgPort *arbiter_reply_port;
struct MsgPort *arbiter_msg_port;
struct Process *arbiter_proc;
struct DeathMessage *arbiter_dmsg;
struct ProgressData *progdata;

int32 install_arbiter()
{
	if((arbiter_reply_port = IExec->AllocSysObjectTags(ASOT_PORT, TAG_END)))
	{
		if((arbiter_dmsg = IExec->AllocSysObjectTags(ASOT_MESSAGE, ASOMSG_Size, sizeof(struct DeathMessage), ASOMSG_ReplyPort, arbiter_reply_port, TAG_END)))
		{
			if((arbiter_proc = (struct Process *)IDOS->CreateNewProcTags(NP_Entry, &arbiter_process, NP_Name, "dopus_arbiter", NP_NotifyOnDeathMessage, arbiter_dmsg, TAG_END)))
			{
				if((arbiter_msg_port = IDOS->GetProcMsgPort(arbiter_proc)))
				{
					return 1;
				}
			}
			IExec->FreeSysObject(ASOT_MESSAGE, arbiter_dmsg);
		}
		IExec->FreeSysObject(ASOT_PORT, arbiter_reply_port);
	}

	return 0;
}

void remove_arbiter()
{
	if(arbiter_msg_port)
	{
		arbiter_command(ARBITER_REMOVE, 0, 0, 0, 0, NULL, 0);
		IExec->WaitPort(arbiter_reply_port);
		IExec->GetMsg(arbiter_reply_port);
	}

	if(arbiter_dmsg)
		IExec->FreeSysObject(ASOT_MESSAGE, arbiter_dmsg);
	if(arbiter_reply_port)
		IExec->FreeSysObject(ASOT_PORT, arbiter_reply_port);

	return;
}

int32 arbiter_command(uint32 command, uint32 current, uint32 total, int64 file_current, int64 file_total, APTR data, char copy_file_indicator)
{
	int32 ret = 0;
	struct ArbiterMessage *arbiter_msg;

	if((arbiter_msg = IExec->AllocSysObjectTags(ASOT_MESSAGE, ASOMSG_Size, sizeof(struct ArbiterMessage), ASOMSG_ReplyPort, arbiter_reply_port, TAG_END)))
	{
		arbiter_msg->command = command;
		arbiter_msg->current = current;
		arbiter_msg->total = total;
		arbiter_msg->file_current = file_current;
		arbiter_msg->file_total = file_total;
		arbiter_msg->data = data;
		arbiter_msg->copy_file_indicator = copy_file_indicator;

		IExec->Forbid();
		IExec->PutMsg(arbiter_msg_port, (struct Message *)arbiter_msg);
		IExec->Permit();
		IExec->WaitPort(arbiter_reply_port);
		IExec->GetMsg(arbiter_reply_port);

		ret = arbiter_msg->command;

		IExec->FreeSysObject(ASOT_MESSAGE, arbiter_msg);
	}

	return ret;
}

uint32 window_mask;

int32 arbiter_process(char *argstr, int32 arglen, struct ExecBase *sysbase2)
{
	struct MsgPort *my_process_port;
	struct ArbiterMessage *arb_msg;
	char remove = 0;
	uint32 wait_mask;
	Object *progwin = NULL;
	struct Window *progwindow;

	my_process_port = IDOS->GetProcMsgPort(NULL);

	window_mask = 0;
	wait_mask = 1 << my_process_port->mp_SigBit;

	while(remove == 0)
	{
		IExec->Wait(wait_mask);

		if(wait_mask & (1 << my_process_port->mp_SigBit))
		{
			while ((arb_msg = (struct ArbiterMessage *)IExec->GetMsg(my_process_port)))
			{
				switch (arb_msg->command)
				{
				case ARBITER_REMOVE:
					remove = 1;
					break;
				case ARBITER_PROGRESS_OPEN:
					progwin = ra_progresswindow_build(arb_msg->data, arb_msg->total, arb_msg->copy_file_indicator);
					progwindow = ra_progresswindow_open(progwin);
					if(progwindow)
					{
						IIntuition->GetAttr(WINDOW_SigMask, progwin, &window_mask);
						wait_mask |= window_mask;
					}
					break;
				case ARBITER_PROGRESS_INCREASE:
					ra_progresswindow_update_two(arb_msg->current);
					break;
				case ARBITER_PROGRESS_UPDATE:
					ra_progresswindow_update_one(arb_msg->file_current, arb_msg->file_total, arb_msg->data);
					break;
				case ARBITER_PROGRESS_CLOSE:
					ra_progresswindow_close();
					wait_mask &= ~window_mask;
					break;
				}

				if(arb_msg)
				{
					arb_msg->command = 0;
					IExec->ReplyMsg((struct Message *)arb_msg);
				}
			}
		}
		if(wait_mask & window_mask)
		{
			uint16 code;
			uint32 result;

			while((result = RA_HandleInput(progwin, &code)) != WMHI_LASTMSG)
			{
				switch(result & WMHI_CLASSMASK)
				{
				case WMHI_GADGETUP:
					if((result & WMHI_GADGETMASK) == 5)
					{
						status_haveaborted = status_rexxabort = 1;
						IExec->Signal((struct Task *)main_proc, SIGBREAKF_CTRL_C);
					}
					break;
				}
			}
		}
	}

	return 0;
}

