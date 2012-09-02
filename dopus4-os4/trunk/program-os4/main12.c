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

#include <dos/filehandler.h>
#include <math.h>

#include "dopus.h"

void dodiskinfo(STRPTR path)
{
	BPTR lock;
	char buffer[256];
	struct Screen *pscreen;

	if(MainScreen)
	{
		pscreen = MainScreen;
	}
	else
	{
		pscreen = IIntuition->LockPubScreen(NULL);
	}


	if(pscreen && (lock = IDOS->Lock(path, ACCESS_READ)))
	{
		if((IDOS->DevNameFromLock(lock, buffer, 256, DN_DEVICEONLY)))
		{
			IDOS->UnLock(lock);
			if((lock = IDOS->Lock(buffer, ACCESS_READ)))
			{
				IWorkbench->WBInfo(lock, NULL, pscreen);
				IDOS->UnLock(lock);
			}
		}
	}

	if(!MainScreen)
	{
		IIntuition->UnlockPubScreen(NULL, pscreen);
	}

	return;
}

/*  The only call to this function was replaced by IDOS->DevNameFromLock()
	in getdir() function of main2.c. This function is no longer needed.
void get_device_task(BPTR lock, char *buffer, struct MsgPort *port)
{
	struct DosList *dl;
	struct DeviceList *devlist;
	struct FileLock *lock2;

	lock2 = (struct FileLock *)BADDR(lock);
	devlist = (struct DeviceList *)BADDR(lock2->fl_Volume);

	
	dl = IDOS->LockDosList(LDF_DEVICES | LDF_VOLUMES | LDF_ASSIGNS | LDF_READ);
	while((dl = IDOS->NextDosEntry(dl, LDF_DEVICES | LDF_VOLUMES | LDF_ASSIGNS)))
	{
		if(dl->dol_Type == DLT_DEVICE && dl->dol_Task == devlist->dl_Task)
			break;
	}

	if(dl)
	{
		IDOS->CopyStringBSTRToC(dl->dol_Name, buffer, 31);
	}
	else
	{
		strcpy(buffer, ((struct Task *)port->mp_SigTask)->tc_Node.ln_Name);
	}
	strcat(buffer, ":");

	IDOS->UnLockDosList(LDF_DEVICES | LDF_VOLUMES | LDF_ASSIGNS | LDF_READ);
}
*/

void getsizestring(char *buf, uint64 a)
{
	a /= 1024;
	if(a > 1073741824)
		sprintf(buf, "HUGE");
	else if(a > 1048576)
		sprintf(buf, "%.1f G", (double)((double)a / 1048576));
	else if(a > 1024)
		sprintf(buf, "%.1f M", (double)((double)a / 1024));
	else
		sprintf(buf, "%ld K", (long)a);
}
