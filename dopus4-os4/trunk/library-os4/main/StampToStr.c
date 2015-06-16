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

#include <stdio.h>
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
	uint32 hr, min, sec;
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

		if(datetime->dat_Flags & DDTF_12HOUR)
		{
			int ampm;

			if(hr >= 12)
			{
				if(hr > 12)
					hr -= 12;
				ampm = 1;
			}
			else
			{
				if(hr == 0)
					hr = 12;
				ampm = 0;
			}
			snprintf(datetime->dat_StrTime, LEN_DATSTRING, "%2ld:%02ld:%02ld%lc", hr, min, sec, (ampm) ? 'P' : 'A');
		}
		else
		{
			snprintf(datetime->dat_StrTime, LEN_DATSTRING, "%02ld:%02ld:%02ld", hr, min, sec);
		}
	}

	if(datetime->dat_StrDate)
	{
		if((time = strchr(datetime->dat_StrDate, ' ')))
			*time = 0;
	}

	return;
}
