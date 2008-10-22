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
#include <dos/dostags.h>

struct MsgPort *arbiter_reply_port;
struct MsgPort *arbiter_msg_port;
struct Process *arbiter_proc;
struct ProcessStart *arbiter_seglist;
struct Message *arbiter_startup;

int install_arbiter()
{
	if(!(arbiter_reply_port = IExec->AllocSysObjectTags(ASOT_PORT, TAG_END)))
		return 0;

	arbiter_startup = IExec->AllocSysObjectTags(ASOT_MESSAGE, ASOMSG_ReplyPort, arbiter_reply_port, TAG_END);

	if(!(arbiter_proc = (struct Process *)IDOS->CreateNewProcTags(NP_Entry, &arbiter_process, NP_Name, (Tag)"dopus_arbiter", NP_WindowPtr, (APTR)-1, NP_Priority, 0, TAG_END)))
		return 0;

	if(!(arbiter_msg_port = IDOS->GetProcMsgPort(arbiter_proc)))
		return 0;

	IExec->PutMsg(arbiter_msg_port, arbiter_startup);

	return 1;
}

void remove_arbiter()
{
	if(arbiter_msg_port)
	{
		arbiter_command(ARBITER_REMOVE, NULL, 0);
		IExec->WaitPort(arbiter_reply_port);
		IExec->GetMsg(arbiter_reply_port);
	}

	if(arbiter_startup)
		IExec->FreeSysObject(ASOT_MESSAGE, arbiter_startup);
	if(arbiter_reply_port)
		IExec->FreeSysObject(ASOT_PORT, arbiter_reply_port);

	return;
}

int arbiter_command(int command, APTR data, int flags)
{
	struct ArbiterMessage arbiter_msg;
	struct MsgPort command_port;

	IExec->CopyMem((char *)arbiter_reply_port, (char *)&command_port, sizeof(struct MsgPort));
	IExec->NewList(&command_port.mp_MsgList);

	arbiter_msg.msg.mn_Node.ln_Type = NT_MESSAGE;
	arbiter_msg.msg.mn_Node.ln_Pri = 0;
	arbiter_msg.msg.mn_ReplyPort = &command_port;
	arbiter_msg.msg.mn_Length = (UWORD) sizeof(struct ArbiterMessage);
	arbiter_msg.command = command;
	arbiter_msg.data = data;
	arbiter_msg.flags = flags;

	IExec->PutMsg(arbiter_msg_port, (struct Message *)&arbiter_msg);
	IExec->WaitPort(&command_port);
	IExec->GetMsg(&command_port);

	return (arbiter_msg.command);
}

struct LaunchList
{
	struct LaunchList *next;
	struct ProcessStart *seglist;
	struct ArbiterMessage launch_msg;
	struct ArbiterMessage *reply_msg;
	struct DOpusRemember *memory;
};

int arbiter_process(char *argstr, int32 arglen, struct ExecBase *sysbase2)
{
	struct MsgPort *my_process_port;
	struct Message *my_startup_message;
	struct ArbiterMessage *arb_msg, *remove_msg = NULL;
	struct LaunchList *first_launch = NULL, *launch, *launchpos;
	struct MsgPort *replyport;
	char ret, remove = 0;
	int wait_mask;

	my_process_port = IDOS->GetProcMsgPort(NULL);

	IExec->WaitPort(my_process_port);
	my_startup_message = IExec->GetMsg(my_process_port);

	wait_mask = 1 << my_process_port->mp_SigBit;

	if((replyport = IExec->AllocSysObjectTags(ASOT_PORT, TAG_END)))
	{
		wait_mask |= 1 << replyport->mp_SigBit;
	}

	for(;;)
	{
		if(replyport)
		{
			while ((arb_msg = (struct ArbiterMessage *)IExec->GetMsg(replyport)))
			{
				launch = first_launch;
				launchpos = NULL;
				while (launch)
				{
					if(arb_msg == &launch->launch_msg)
					{
						if(launchpos)
							launchpos->next = launch->next;
						else
							first_launch = launch->next;
						if(launch->reply_msg)
						{
							launch->reply_msg->command = arb_msg->command;
							IExec->ReplyMsg((struct Message *)launch->reply_msg);
						}
						IDOpus->LFreeRemember(&launch->memory);
						IExec->FreeVec(launch);
						break;
					}
					launchpos = launch;
					launch = launch->next;
				}
			}
			if(remove_msg && !first_launch)
			{
				IExec->ReplyMsg((struct Message *)remove_msg);
				break;
			}
		}
		while ((arb_msg = (struct ArbiterMessage *)IExec->GetMsg(my_process_port)))
		{
			ret = 0;
			switch (arb_msg->command)
			{
			case ARBITER_REMOVE:
				if(first_launch)
				{
					remove_msg = arb_msg;
				}
				else
				{
					remove = 1;
				}
				break;
			case ARBITER_LAUNCH:
//				if(replyport && (launch = IExec->AllocVecTags(sizeof(struct LaunchList), AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END)))
				if(replyport && (launch = IExec->AllocVec(sizeof(struct LaunchList), MEMF_SHARED | MEMF_CLEAR)))
				{
					struct ArbiterLaunch *arb_launch;
					struct MsgPort *port;

					if((launchpos = first_launch))
						for(; launchpos->next; launchpos = launchpos->next);

					if(launchpos)
						launchpos->next = launch;
					else
						first_launch = launch;

					arb_launch = (struct ArbiterLaunch *)arb_msg->data;

					launch->memory = arb_launch->launch_memory;
					if((launch->seglist = IDOpus->LAllocRemember(&launch->memory, sizeof(struct ProcessStart), MEMF_PUBLIC | MEMF_CLEAR)))
					{
						launch->launch_msg.msg.mn_Node.ln_Type = NT_MESSAGE;
						launch->launch_msg.msg.mn_ReplyPort = replyport;
						launch->launch_msg.msg.mn_Length = sizeof(struct ArbiterMessage);

						launch->launch_msg.command = arb_msg->command;
						launch->launch_msg.data = arb_launch->data;
						launch->launch_msg.flags = arb_msg->flags;


						if((port = (struct MsgPort *)&IDOS->CreateNewProcTags(NP_Name, arb_launch->launch_name, NP_Priority, 0, NP_Entry, arb_launch->launch_code, NP_StackSize, 8192, NP_FreeSeglist, FALSE, NP_CloseInput, FALSE, NP_CloseOutput, FALSE, NP_Child, TRUE, TAG_END)->pr_MsgPort))
						{
							IExec->PutMsg(port, (struct Message *)&launch->launch_msg);
							if(arb_msg->flags & ARB_WAIT)
							{
								launch->reply_msg = arb_msg;
								arb_msg = NULL;
							}
							else
								ret = 1;
						}
					}
					else
					{
						IDOpus->LFreeRemember(&launch->memory);
						IExec->FreeMem(launch, sizeof(struct ProcessStart));
					}
				}
				break;
			}
			if(arb_msg)
			{
				arb_msg->command = ret;
				IExec->ReplyMsg((struct Message *)arb_msg);
			}
		}
		if(remove)
			break;

		IExec->Wait(wait_mask);
	}

	if(replyport)
		IExec->FreeSysObject(ASOT_PORT, replyport);
	IExec->Forbid(); // what is this really?
	IExec->ReplyMsg(my_startup_message);

	return 0;
}

