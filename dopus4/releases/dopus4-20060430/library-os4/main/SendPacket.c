/*
 *  $VER: SendPacket.c $Revision$ (01-Feb-2006)
 *
 *  This file is part of dopus.
 *
 *  (C) Copyright 2006 Hyperion Entertainment
 *      All rights reserved
 *
 * $Id$
 *
 * $Log$
 *
 *
 */


#include <exec/exec.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/SendPacket ******************************************
*
*   NAME
*      SendPacket -- Description
*
*   SYNOPSIS
*      int SendPacket(struct MsgPort * port, int action, ULONG * args, 
*          int nargs);
*
*   FUNCTION
*
*   INPUTS
*       port - 
*       action - 
*       args - 
*       nargs - 
*
*   RESULT
*       The result ...
*
*   EXAMPLE
*
*   NOTES
*
*   BUGS
*
*   SEE ALSO
*
*****************************************************************************
*
*/

int _DOpus_SendPacket(struct DOpusIFace *Self, struct MsgPort *port, int action, ULONG *args, int nargs)
{
	struct StandardPacket *packet;
	struct MsgPort *repport;
	struct Process *myproc;
	int count, res1 = 0;
	ULONG *pargs;

	if ((packet = IExec->AllocMem(sizeof(struct StandardPacket), MEMF_PUBLIC|MEMF_CLEAR)))
	{
		if((repport = IExec->CreatePort(NULL,0)))
		{
			packet->sp_Msg.mn_Node.ln_Name = (char *)&(packet->sp_Pkt);
			packet->sp_Pkt.dp_Link = &(packet->sp_Msg);
			packet->sp_Pkt.dp_Port = repport;
			packet->sp_Pkt.dp_Type = action;

			pargs = (ULONG *)&(packet->sp_Pkt.dp_Arg1);
			if(nargs>7)
				nargs=7;
			for(count = 0; count < nargs; count++)
				pargs[count] = args[count];
			
			IExec->PutMsg(port, (struct Message *)packet);
			IExec->WaitPort(repport);
			IExec->GetMsg(repport);

			res1 = packet->sp_Pkt.dp_Res1;

			if((myproc = (struct Process *)IExec->FindTask(NULL)))
				myproc->pr_Result2 = packet->sp_Pkt.dp_Res2;

			IExec->DeletePort(repport);
		}
		IExec->FreeMem(packet, sizeof(struct StandardPacket));
	}
	return(res1);
}
