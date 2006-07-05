/*
 *  $VER: RawkeyToStr.c $Revision$ (01-Feb-2006)
 *
 *  This file is part of dopus.
 *
 * $Id$
 *
 * $Log$
 *
 *
 */


#include <string.h>
#include <exec/exec.h>
#include <proto/exec.h>
#include <proto/console.h>
#include <proto/utility.h>
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

int _DOpus_RawkeyToStr(UWORD code, UWORD qual, char *buf, char *kbuf, int len)
{
	struct Device *ConsoleDevice;
	struct InputEvent inev;
	struct MsgPort *port;
	struct IOStdReq *req;
	struct Process *myproc;
	char ocbuf[20], cbuf[20], *foo;

	if(buf)
		buf[0] = 0;
	ocbuf[0] = cbuf[0] = 0;
	if(code != (UWORD) ~ 0 && code != 0xff)
	{
		if(code & IECODE_UP_PREFIX)
			code -= 0x80;
		if(code >= 0x50 && code <= 0x59)
			NewRawDoFmt("F%ld", NULL, cbuf, code - 0x4f);
		else if(code != (UWORD)~0 && code != 0xff)
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
				strcpy(cbuf, foo);
			if(!(myproc = (struct Process *)FindTask(NULL)))
				return (0);
			port = &myproc->pr_MsgPort;
			if(!(req = (struct IOStdReq *)CreateIORequest(port, sizeof(struct IOStdReq))))
				return (0);
			if(OpenDevice("console.device", -1, (struct IORequest *)req, 0))
			{
				DeleteIORequest((struct IORequest *)req);
				return (0);
			}
			ConsoleDevice = (struct Device *)req->io_Device;
			inev.ie_NextEvent = NULL;
			inev.ie_Class = IECLASS_RAWKEY;
			inev.ie_SubClass = 0L;
			inev.ie_Code = code;
			inev.ie_Qualifier = qual & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT);
			inev.ie_EventAddress = NULL;
			ocbuf[0] = 0;
			RawKeyConvert(&inev, ocbuf, 2, NULL);
			ocbuf[1] = 0;
			CloseDevice((struct IORequest *)req);
			DeleteIORequest((struct IORequest *)req);
			if(kbuf)
				kbuf[0] = ocbuf[0];
			if(!foo)
				strcpy(cbuf, ocbuf);
		}
	}
	if(buf)
	{
		if(qual & IEQUALIFIER_LCOMMAND)
			StrConcat(buf, "LCOMMAND + ", len);
		if(qual & IEQUALIFIER_RCOMMAND)
			StrConcat(buf, "RCOMMAND + ", len);
		if(qual & IEQUALIFIER_CONTROL)
			StrConcat(buf, "CONTROL + ", len);
		if(qual & IEQUALIFIER_LSHIFT)
			StrConcat(buf, "LSHIFT + ", len);
		if(qual & IEQUALIFIER_RSHIFT)
			StrConcat(buf, "RSHIFT + ", len);
		if(qual & IEQUALIFIER_LALT)
			StrConcat(buf, "LALT + ", len);
		if(qual & IEQUALIFIER_RALT)
			StrConcat(buf, "RALT + ", len);
		if((code == (UWORD) ~ 0) || (code == 0xff && buf[0]))
			buf[strlen(buf) - 3] = 0;
		else if(cbuf[0])
		{
			StrToUpper(cbuf, ocbuf);
			StrConcat(buf, "'", len);
			StrConcat(buf, ocbuf, len);
			StrConcat(buf, "'", len);
		}
	}

	return (1);
}
