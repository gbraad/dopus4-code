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

struct MsgPort *arbiter_reply_port;
struct MsgPort *arbiter_msg_port;
struct Process *arbiter_proc;
struct DeathMessage *arbiter_dmsg;

int install_arbiter()
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
		arbiter_command(ARBITER_REMOVE, NULL, 0);
		IExec->WaitPort(arbiter_reply_port);
		IExec->GetMsg(arbiter_reply_port);
	}

	if(arbiter_dmsg)
		IExec->FreeSysObject(ASOT_MESSAGE, arbiter_dmsg);
	if(arbiter_reply_port)
		IExec->FreeSysObject(ASOT_PORT, arbiter_reply_port);

	return;
}

int arbiter_command(int command, APTR data, int flags)
{
	struct ArbiterMessage *arbiter_msg;

	arbiter_msg = IExec->AllocSysObjectTags(ASOT_MESSAGE, ASOMSG_Size, sizeof(struct ArbiterMessage), ASOMSG_ReplyPort, arbiter_reply_port, TAG_END);
	arbiter_msg->command = command;
	arbiter_msg->data = data;
	arbiter_msg->flags = flags;

	IExec->PutMsg(arbiter_msg_port, (struct Message *)arbiter_msg);
	IExec->WaitPort(arbiter_reply_port);
	IExec->GetMsg(arbiter_reply_port);

	return (arbiter_msg->command);
}

int arbiter_process(char *argstr, int32 arglen, struct ExecBase *sysbase2)
{
	struct MsgPort *my_process_port;
	struct ArbiterMessage *arb_msg;
	char ret, remove = 0;
	uint32 wait_mask;

	my_process_port = IDOS->GetProcMsgPort(NULL);

	wait_mask = 1 << my_process_port->mp_SigBit;

	while(remove == 0)
	{
		IExec->Wait(wait_mask);

		while ((arb_msg = (struct ArbiterMessage *)IExec->GetMsg(my_process_port)))
		{
			ret = 0;
			switch (arb_msg->command)
			{
			case ARBITER_REMOVE:
				remove = 1;
				break;
			}
			if(arb_msg)
			{
				arb_msg->command = ret;
				IExec->ReplyMsg((struct Message *)arb_msg);
			}
		}
	}

	return 0;
}

