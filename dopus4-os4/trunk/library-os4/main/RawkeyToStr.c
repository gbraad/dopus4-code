/*
 *  $VER: RawkeyToStr.c $Revision$ (01-Feb-2006)
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

#include <stdio.h>
#include <string.h>

#include <exec/exec.h>
#include <proto/exec.h>
#include <proto/console.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/RawkeyToStr ******************************************
*
*   NAME
*      RawkeyToStr -- Description
*
*   SYNOPSIS
*      int RawkeyToStr(uint16 code, uint16 qual, char * buf, char * kbuf, 
*          int len);
*
*   FUNCTION
*
*   INPUTS
*       code - 
*       qual - 
*       buf - 
*       kbuf - 
*       len - 
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

int _DOpus_RawkeyToStr(struct DOpusIFace *Self, uint16 code, uint16 qual, char *buf, char *kbuf, int len)
{
	struct Device *ConsoleDevice;
	struct ConsoleIFace *IConsole;
	struct InputEvent inev;
	struct MsgPort *port;
	struct IOStdReq *req;
	struct Process *myproc;
	char ocbuf[20], cbuf[20];
	const char *foo;

	if(buf)
		buf[0] = 0;
	ocbuf[0] = cbuf[0] = 0;
	if(code != (uint16) ~ 0 && code != 0xff)
	{
		if(code & IECODE_UP_PREFIX)
			code -= 0x80;
		if(code >= 0x50 && code <= 0x59)
			snprintf(cbuf, sizeof(cbuf), "F%d", code - 0x4f);
		else if(code != (uint16)~0 && code != 0xff)
		{
			foo = NULL;
			switch (code)
			{
			case 0x45:
				foo = "ESCAPE";
				break;
			case 0x46:
				foo = "DELETE";
				break;
			case 0x41:
				foo = "BACKSPACE";
				break;
			case 0x42:
				foo = "TAB";
				break;
			case 0x44:
			case 0x2b:
				foo = "RETURN";
				break;
			case 0x5f:
				foo = "HELP";
				break;
			case 0x60:
			case 0x4c:
				foo = "CURSOR-UP";
				break;
			case 0x4d:
				foo = "CURSOR-DOWN";
				break;
			case 0x4f:
				foo = "CURSOR-LEFT";
				break;
			case 0x4e:
				foo = "CURSOR-RIGHT";
				break;
			case 0x43:
				foo = "ENTER";
				break;
			case 0x40:
				foo = "SPACE";
				break;
			}
			if(foo)
				strlcpy(cbuf, foo, sizeof(cbuf));
			if(!(myproc = (struct Process *)IExec->FindTask(NULL)))
				return (0);
			port = &myproc->pr_MsgPort;
//			if(!(req = (struct IOStdReq *)IExec->CreateIORequest(port, sizeof(struct IOStdReq))))
			if (!(req = IExec->AllocSysObjectTags(ASOT_IOREQUEST,
			                           ASOIOR_ReplyPort, port,
			                           ASOIOR_Size, sizeof(struct IOStdReq),
			                           TAG_END)))
			{
				return (0);
			}
			if(IExec->OpenDevice("console.device", -1, (struct IORequest *)req, 0))
			{
//				IExec->DeleteIORequest((struct IORequest *)req);
				IExec->FreeSysObject(ASOT_IOREQUEST, req);
				return (0);
			}
			ConsoleDevice = (struct Device *)req->io_Device;
			if(!(IConsole = (struct ConsoleIFace *)IExec->GetInterface((struct Library *)ConsoleDevice, "main", 1, NULL)))
			{
//				IExec->DeleteIORequest((struct IORequest *)req);
				IExec->FreeSysObject(ASOT_IOREQUEST, req);
				return (0);
			}
			inev.ie_NextEvent = NULL;
			inev.ie_Class = IECLASS_RAWKEY;
			inev.ie_SubClass = 0L;
			inev.ie_Code = code;
			inev.ie_Qualifier = qual & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT);
			inev.ie_EventAddress = NULL;
			ocbuf[0] = 0;
			IConsole->RawKeyConvert(&inev, ocbuf, 2, NULL);
			ocbuf[1] = 0;
			IExec->CloseDevice((struct IORequest *)req);
//			IExec->DeleteIORequest((struct IORequest *)req);
			IExec->FreeSysObject(ASOT_IOREQUEST, req);
			if(kbuf)
				kbuf[0] = ocbuf[0];
			if(!foo)
				strlcpy(cbuf, ocbuf, sizeof(cbuf));
		}
	}
	if(buf)
	{
		if(qual & IEQUALIFIER_LCOMMAND)
			strlcat(buf, "LAMIGA + ", len);
		if(qual & IEQUALIFIER_RCOMMAND)
			strlcat(buf, "RAMIGA + ", len);
		if(qual & IEQUALIFIER_CONTROL)
			strlcat(buf, "CONTROL + ", len);
		if(qual & IEQUALIFIER_LSHIFT)
			strlcat(buf, "LSHIFT + ", len);
		if(qual & IEQUALIFIER_RSHIFT)
			strlcat(buf, "RSHIFT + ", len);
		if(qual & IEQUALIFIER_LALT)
			strlcat(buf, "LALT + ", len);
		if(qual & IEQUALIFIER_RALT)
			strlcat(buf, "RALT + ", len);
		if((code == (uint16) ~ 0) || (code == 0xff && buf[0]))
			buf[strlen(buf) - 3] = 0;
		else if(cbuf[0])
		{
			Self->StrToUpper(cbuf, ocbuf);
			strlcat(buf, "'", len);
			strlcat(buf, ocbuf, len);
			strlcat(buf, "'", len);
		}
	}

	return (1);
}
