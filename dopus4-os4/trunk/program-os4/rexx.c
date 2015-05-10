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

#define MAXRXOUTSTANDING 300
#define RXERRORIMGONE 100
#define RXERRORNOCMD 30

static struct RexxMsg *oldrxmsg;
static int rexx_closing;
static long replies_pending;

void rexx_dispatch(int allfuncs)
{
	struct RexxMsg *msg;
	int dontreply, a, cmd;
	char *command;

	while((msg = (struct RexxMsg *)IExec->GetMsg(arexx_port)))
	{
		if((!RexxSysBase || !(IRexxSys->IsRexxMsg((struct Message *)msg))) && msg->rm_Node.mn_Node.ln_Type != NT_REPLYMSG)
		{

			struct DOpusMessage *dopusmsg;
			struct DOpusArgsList *arglist;

			dopusmsg = (struct DOpusMessage *)msg;

			switch (dopusmsg->command)
			{
			case DOPUSMSG_GETVIS:
				fill_out_visinfo((struct VisInfo *)dopusmsg->data, NULL);
				break;
			case DOPUSMSG_GETNEXTFILE:
				arglist = (struct DOpusArgsList *)dopusmsg->data;
				if(arglist->single_file)
				{
					strcpy(arglist->file_data, arglist->single_file);
					arglist->single_file = NULL;
				}
				else if(arglist->file_list)
				{
					struct Directory *entry;

					entry = (struct Directory *)arglist->file_list;
					strcpy(arglist->file_data, dopus_curwin[arglist->file_window]->directory);
					IDOS->AddPart(arglist->file_data, entry->name, 256);
					arglist->last_select = arglist->file_list;

					entry = entry->next;
					while(entry)
					{
						if(entry->type < 0 && entry->selected)
							break;
						entry = entry->next;
					}

					arglist->file_list = (APTR) entry;
				}
				else
					arglist->file_data[0] = 0;
				break;
			case DOPUSMSG_UNSELECTFILE:
				arglist = (struct DOpusArgsList *)dopusmsg->data;
				if(arglist->last_select)
				{
					unselect(arglist->file_window, arglist->last_select);
					arglist->last_select = NULL;
				}
				break;
			case DOPUSMSG_GETPRINTDIR:
				get_printdir_data((struct PrintDirData *)dopusmsg->data);
				break;
			}
			IExec->ReplyMsg((struct Message *)dopusmsg);
		}
		else if(RexxSysBase && IRexxSys)
		{
			if(msg->rm_Node.mn_Node.ln_Type == NT_REPLYMSG)
			{
				if(msg->rm_Args[1])
					rexx_msg_reply(msg);
				IRexxSys->DeleteArgstring(msg->rm_Args[0]);
				IRexxSys->DeleteRexxMsg(msg);
				--replies_pending;
			}
			else
			{
				command = (char *)msg->rm_Args[0];
				while(*command > 0 && *command <= ' ')
					++command;
				msg->rm_Result1 = 0;
				msg->rm_Result2 = 0;
				dontreply = 0;
				if(rexx_closing)
					msg->rm_Result1 = RXERRORIMGONE;
				else
				{
					oldrxmsg = msg;
					for(cmd = 0; commandlist[cmd].name; cmd++)
					{
						a = strlen(commandlist[cmd].name);
//						if((strncmp(commandlist[cmd].name, command, a)) == 0 && (command[a] == 0 || (isspace(command[a]))))
						if((IUtility->Strnicmp(commandlist[cmd].name, command, a)) == 0 && (command[a] == 0 || (isspace(command[a]))))
						{
							func_global_function = 0;
							rexxdisp(msg, &commandlist[cmd], command + a);
							if(!allfuncs || func_global_function == FUNC_ICONIFY || func_global_function == FUNC_QUIT)
								break;
							if(func_global_function)
							{
								char buf[10];

								rexx_result_code = 0;
								rexx_return_value = 0;
								internal_function(func_global_function, rexx_global_flag, NULL, NULL);
								func_global_function = 0;
								sprintf(buf, "%ld", rexx_return_value);
								rexx_set_return(msg, rexx_result_code, buf);
							}
							break;
						}
					}
					if(!commandlist[cmd].name)
					{
						rexx_command(msg->rm_Args[0], msg);
						dontreply = 1;
					}
				}
				oldrxmsg = NULL;
				if(!dontreply)
					IExec->ReplyMsg((struct Message *)msg);
			}
		}
		else if(msg->rm_Node.mn_Node.ln_Type != NT_REPLYMSG)
			IExec->ReplyMsg((struct Message *)msg);
	}
}

struct RexxMsg *send_rexx_command(STRPTR command, int (*replyfunc) (), struct RexxMsg *msg)
{
	struct MsgPort *rxport;
	struct RexxMsg *RexxMsg;

	if(!RexxSysBase || replies_pending > MAXRXOUTSTANDING - 1 || !command || !command[0])
		return (NULL);

	if((RexxMsg = (struct RexxMsg *)IRexxSys->CreateRexxMsg(arexx_port, "dopus", str_arexx_portname)) && (RexxMsg->rm_Args[0] = (STRPTR)IRexxSys->CreateArgstring(command, (long)strlen(command))))
	{

		RexxMsg->rm_Action = RXCOMM;
		RexxMsg->rm_Args[1] = (STRPTR) replyfunc;
		RexxMsg->rm_Args[2] = (STRPTR) msg;
		RexxMsg->rm_Node.mn_Node.ln_Name = "REXX";

		IExec->Forbid();
		if((rxport = IExec->FindPort("REXX")))
			IExec->PutMsg(rxport, (struct Message *)RexxMsg);
		IExec->Permit();
		if(rxport)
		{
			++replies_pending;
			return (RexxMsg);
		}
		else
			IRexxSys->DeleteArgstring(RexxMsg->rm_Args[0]);
	}
	if(RexxMsg)
		IRexxSys->DeleteRexxMsg(RexxMsg);
	return (NULL);
}

void rexx_msg_reply(struct RexxMsg *msg)
{
	struct RexxMsg *oldmsg;

	if((oldmsg = (struct RexxMsg *)(msg->rm_Args[2])))
	{
		rexx_set_return(oldmsg, msg->rm_Result1, (char *)msg->rm_Result2);
		IExec->ReplyMsg((struct Message *)oldmsg);
	}
}

struct RexxMsg *rexx_command(STRPTR command, struct RexxMsg *syncmsg)
{
	struct RexxMsg *msg;

	if(!command || !command[0])
		msg = NULL;
	else if(syncmsg)
		msg = send_rexx_command(command, (APTR) & rexx_msg_reply, syncmsg);
	else
		msg = send_rexx_command(command, NULL, NULL);
	return (msg);
}

void rexx_copyresult(char *result)
{
	char *new_lrr;

	if(result)
		if((new_lrr = doAllocVec(strlen(result) + 1, MEMF_ANY)))
		{
			strcpy(new_lrr, result);
			IExec->FreeVec(str_last_rexx_result);
			str_last_rexx_result = new_lrr;
		}
}

void rexx_set_return(struct RexxMsg *msg, int rc, STRPTR result)
{
	str_last_rexx_result[0] = 0;

	if(msg && RexxSysBase)
	{
		if(rc || !result || !(msg->rm_Action & (1 << RXFB_RESULT)))
		{
			msg->rm_Result1 = rc;
			msg->rm_Result2 = 0;
		}
		else
		{
			msg->rm_Result1 = 0;
			msg->rm_Result2 = (long)IRexxSys->CreateArgstring(result, strlen(result));
			rexx_copyresult(result);
		}
	}
	else
		rexx_copyresult(result);
}

void close_rexx_port()
{
	if(RexxSysBase && IRexxSys)
	{
		rexx_closing = 1;
		if(oldrxmsg)
		{
			oldrxmsg->rm_Result1 = RXERRORIMGONE;
			IExec->ReplyMsg((struct Message *)oldrxmsg);
			oldrxmsg = NULL;
		}
		while(replies_pending)
		{
			if(!IExec->FindPort("REXX"))
				break;
			IExec->WaitPort(arexx_port);
			rexx_dispatch(0);
		}
	}
}
