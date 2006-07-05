/*
 *  $VER: FindSystemFile.c $Revision$ (01-Feb-2006)
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
#include <proto/dos.h>
#include <proto/utility.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

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

STATIC CONST CONST_STRPTR look_dirs[] =
{
	"PROGDIR:C/",

	"PROGDIR:S/",

	"PROGDIR:Libs/",

	"PROGDIR:Rexx/",

	"PROGDIR:Modules/",

	"PROGDIR:Requesters/",
};


int _DOpus_FindSystemFile(char *name, char *buf, int size, int type)
{
	TEXT temp[256*2];

      tryloop:
	stccpy(temp, name, 256);
	if(!(FindSegment(temp, NULL, 0)))
	{
		if(CheckExist(temp, NULL) >= 0)
		{
			int a;
			BPTR lock;

			for(a = 0; a < 1; a++)
			{
				NewRawDoFmt("%s%s", NULL, temp, look_dirs[type + a], name);

				if(CheckExist(temp, NULL) < 0)
					break;
			}
			if(a == 1)
				temp[0] = 0;
			else if((lock = Lock(temp, ACCESS_READ)))
			{
				NameFromLock(lock, temp, 256);
				UnLock(lock);
			}
		}
		else
		{
			BPTR lock;

			if((lock = Lock("", ACCESS_READ)))
			{
				PathName(lock, temp, 256);
				UnLock(lock);
				TackOn(temp, name, 256);
			}
		}
	}
	if(temp[0])
	{
		stccpy(buf, temp, size);
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

		NewRawDoFmt(string_table[STR_CANNOT_FIND_FILE_REQ], NULL, reqbuf, name, req_gads[0], req_gads[1]);

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
		if(DoSimpleRequest(NULL, &req))
			goto tryloop;
	}
	LStrnCpy(buf, name, size);

	return (0);
}

