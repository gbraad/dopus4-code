/*
 *  $VER: FindSystemFile.c $Revision$ (01-Feb-2006)
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
#include <stdio.h>
#include <string.h>

#include "extras.h"

/****** dopus/main/FindSystemFile ******************************************
*
*   NAME
*      FindSystemFile -- Description
*
*   SYNOPSIS
*      int FindSystemFile(char * name, char * buffer, int size, int type);
*
*   FUNCTION
*
*   INPUTS
*       name - 
*       buffer - 
*       size - 
*       type - 
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

const static char *look_dirs[] =
{
	"PROGDIR:C/",

	"PROGDIR:S/",

	"PROGDIR:Libs/",

	"PROGDIR:Rexx/",

	"PROGDIR:Modules/",

	"PROGDIR:Requesters/",
};


int _DOpus_FindSystemFile(struct DOpusIFace *Self, const char *name, char *buf, int size, int type)
{
	char temp[256];

      tryloop:
	strlcpy(temp, name, 256);
	if(!(IDOS->FindSegment(temp, NULL, 0)))
	{
		if(Self->CheckExist(temp, NULL) >= 0)
		{
			int a;
			BPTR lock;

			for(a = 0; a < 1; a++)
			{
				snprintf(temp, 256, "%s%s", look_dirs[type + a], name);

				if(Self->CheckExist(temp, NULL) < 0)
					break;
			}
			if(a == 1)
				temp[0] = 0;
			else if((lock = IDOS->Lock(temp, ACCESS_READ)))
			{
				IDOS->NameFromLock(lock, temp, 256);
				IDOS->UnLock(lock);
			}
		}
		else
		{
			BPTR lock;

			if((lock = IDOS->Lock("", ACCESS_READ)))
			{
				IDOS->NameFromLock(lock, temp, 256);
				IDOS->UnLock(lock);
				IDOS->AddPart(temp, name, 256);
			}
		}
	}
	if(temp[0])
	{
		strlcpy(buf, temp, size);
		return (1);
	}
	else if(type == SYSFILE_MODULE)
	{
		struct DOpusSimpleRequest req;
		char reqbuf[300];
		static char *req_gads[3];
		static int req_rets[2] = { 1, 0 };

		req_gads[0] = string_table[STR_TRY_AGAIN];
		req_gads[1] = string_table[STR_CANCEL];
		req_gads[2] = NULL;

		snprintf(reqbuf, sizeof(reqbuf), string_table[STR_CANNOT_FIND_FILE_REQ], name, req_gads[0], req_gads[1]);

		req.text = reqbuf;
		req.gads = req_gads;
		req.rets = req_rets;
		req.hi = -1;
		req.lo = -1;
		req.fg = -1;
		req.bg = -1;
		req.strbuf = NULL;
		req.flags = 0;
		req.font = NULL;
		if(Self->DoSimpleRequest(NULL, &req))
			goto tryloop;
	}
	strlcpy(buf, name, size);

	return (0);
}

