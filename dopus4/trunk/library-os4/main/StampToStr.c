/*
 *  $VER: StampToStr.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/StampToStr ******************************************
*
*   NAME
*      StampToStr -- Description
*
*   SYNOPSIS
*      void StampToStr(struct DateTime * datetime);
*
*   FUNCTION
*
*   INPUTS
*       datetime - 
*
*   RESULT
*       This function does not return a result
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

void _DOpus_StampToStr(struct DOpusIFace *Self, struct DateTime *datetime)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *DOSBase = IExec->OpenLibrary("dos.library", 50L);
	struct DOSIFace *IDOS = (struct DOSIFace *)IExec->GetInterface(DOSBase, "main", 1, NULL);
	int hr, min, sec;
	ULONG ds_minute, ds_tick;
	STRPTR time;

	time = datetime->dat_StrTime;
	datetime->dat_StrTime = NULL;
	IDOS->DateToStr(datetime);
	datetime->dat_StrTime = time;

	if(datetime->dat_StrTime)
	{
		ds_minute = datetime->dat_Stamp.ds_Minute;
		ds_tick = datetime->dat_Stamp.ds_Tick;

		if(ds_minute > 1440)
			ds_minute %= 1440;
		if(ds_tick > 3000)
			ds_tick %= 3000;

		hr = ds_minute / 60;
		min = ds_minute % 60;
		sec = ds_tick / 50;

		LSprintf(datetime->dat_StrTime, "%02d:%02d:%02d", hr, min, sec);
	}

	if(datetime->dat_StrDate)
	{
		if((time = strchr(datetime->dat_StrDate, ' ')))
			*time = 0;
	}

	IExec->DropInterface((struct Interface *)IDOS);
	IExec->CloseLibrary(DOSBase);
	return;
}

