/*
 *  $VER: GetDevices.c $Revision$ (01-Feb-2006)
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
#include <dos/dosextens.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

#include "extras.h"

/****** dopus/main/GetDevices ******************************************
*
*   NAME
*      GetDevices -- Description
*
*   SYNOPSIS
*      int GetDevices(struct ConfigStuff * cstuff);
*
*   FUNCTION
*
*   INPUTS
*       cstuff - 
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

int _DOpus_GetDevices(struct ConfigStuff *cstuff)
{
	struct DosList *dl;
	int a = 0;
	int gap, i, j, k, p, l, d;
	TEXT devname[16], pathname[260];
	struct Config *config;

	if(!(config = cstuff->config))
		return (0);

	dl = LockDosList(LDF_DEVICES | LDF_READ);
	while((dl = NextDosEntry(dl, LDF_DEVICES)))
	{
		if(dl->dol_Type == DLT_DEVICE && dl->dol_Task)
		{
			UBYTE *s = (UBYTE *)BADDR(dl->dol_Name);
			ULONG len = *s + 1;
			stccpy(pathname, s+1, len);
			strcat(pathname, ":");
			stccpy(devname, pathname, sizeof(devname));
			AssignDrive(cstuff, a, devname, pathname);
			++a;
			if(a == DRIVECOUNT)
				break;
		}
	}
	UnLockDosList(LDF_DEVICES | LDF_READ);
	for(gap = a / 2; gap > 0; gap /= 2)
	{
		for(i = gap; i < a; i++)
		{
			for(j = i - gap; j >= 0; j -= gap)
			{
				k = j + gap;
				if(Stricmp(config->drive[j].name, config->drive[k].name) <= 0)
					break;
				SwapMem((char *)&config->drive[j], (char *)&config->drive[k], sizeof(struct dopusfunction));
			}
		}
	}

	d = a;
	if(a < DRIVECOUNT)
	{
		dl = LockDosList(LDF_ASSIGNS | LDF_READ);
		while((dl = NextDosEntry(dl, LDF_ASSIGNS)))
		{
			UBYTE *s = (UBYTE *)BADDR(dl->dol_Name);
			ULONG len = *s + 1;
			stccpy(pathname, s+1, len);
			strcat(pathname, ":");
			stccpy(devname, pathname, 16);
			AssignDrive(cstuff, a, devname, pathname);
			++a;
			if(a == DRIVECOUNT)
				break;
		}
		UnLockDosList(LDF_DEVICES | LDF_READ);
	}
	p = a - d;
	for(gap = p / 2; gap > 0; gap /= 2)
	{
		for(i = gap; i < p; i++)
		{
			for(j = i - gap; j >= 0; j -= gap)
			{
				k = j + gap + d;
				l = j + d;
				if(Stricmp(config->drive[l].name, config->drive[k].name) <= 0)
					break;
				SwapMem((char *)&config->drive[l], (char *)&config->drive[k], sizeof(struct dopusfunction));
			}
		}
	}
	if(a < DRIVECOUNT)
	{
		for(i = a; i < DRIVECOUNT; i++)
			AssignDrive(cstuff, i, NULL, NULL);
	}

	return 1;
}

