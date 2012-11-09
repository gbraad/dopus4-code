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

void doassign(int rexx)
{
	int a = 0, blank = 0, tot = 0;
	char buf[FILEBUF_SIZE], buf1[256];
	struct Directory *dev, *next;
	BPTR lock;

	if(status_iconified)
		return;

	buf[0] = 0;
	if(checkdevtot(dopus_curwin[data_active_window]))
	{
		dev = dopus_curwin[data_active_window]->firstentry;
		while(dev)
		{
			next = dev->next;
			if((dev->selected) && (dev->type == ENTRY_DEVICE) && ((dev->size == DLT_DIRECTORY) || (dev->size == DLT_LATE) || (dev->size == DLT_NONBINDING)))
			{
				tot = 0;
				if(!blank)
				{
					expand_path(dev->name, buf1);
					if((!whatsit(globstring[STR_ENTER_ASSIGN_NAME], 256, buf1, (char *)-1)))
					{
						myabort();
						return;
					}
					if(buf1[0] == 0)
						blank = 1;
				}
				strcpy(buf, dev->name);
				buf[(strlen(buf) - 1)] = 0;
				if(blank)
				{
					a = IDOS->AssignLock(buf, 0);
				}
				else
				{
					switch (dev->size)
					{
					case DLT_DIRECTORY:
						if((lock = IDOS->Lock(buf1, ACCESS_READ)))
						{
							if(!(a = IDOS->AssignLock(buf, lock)))
								IDOS->UnLock(lock);
						}
						else
							a = 0;
						break;
					case DLT_LATE:
						a = IDOS->AssignLate(buf, buf1);
						break;
					case DLT_NONBINDING:
						a = IDOS->AssignPath(buf, buf1);
						break;
					}
				}
				if(!a)
					dostatustext(globstring[STR_CANT_CANCEL_ASSIGN]);
				else
				{
					tot = 1;
					if(blank)
						removefile(dev, dopus_curwin[data_active_window], data_active_window, TRUE);
					else
						unselect(data_active_window, dev);
				}
			}
			dev = next;
		}
		if(tot)
			okay();
		return;
	}
	if(str_pathbuffer[data_active_window][0] == 0)
		return;
	if(!rexx)
	{
		if((!whatsit(globstring[STR_ENTER_DEVICE_NAME], 31, buf, NULL)))
		{
			myabort();
			return;
		}
	}
	else
	{
		strcpy(buf, rexx_args[0]);
	}
	for(a = 0; a < strlen(buf); a++)
	{
		if(buf[a] == ':' || buf[a] == '/')
		{
			buf[a] = 0;
		}
	}
	if((IDOS->AssignPath(buf, str_pathbuffer[data_active_window])))
	{
		okay();
	}
	else
	{
		dostatustext(globstring[STR_ASSIGN_FAILED]);
	}
}

int checkdest(int w)
{
	if(!status_iconified && w > -1 && str_pathbuffer[w][0] == 0)
	{
		dostatustext(globstring[STR_NO_DESTINATION_SELECTED]);
		simplerequest(TDRIMAGE_INFO, globstring[STR_NO_DESTINATION_SELECTED], globstring[STR_CONTINUE], NULL);
		return (0);
	}
	return (1);
}

int checksame(char *src, char *dst, int type)
{
	BPTR lk1, lk2;
	int ret;

	lk1 = IDOS->Lock(src, ACCESS_READ);
	lk2 = IDOS->Lock(dst, ACCESS_READ);
	if(!(lk1) || !(lk2))
	{
		IDOS->UnLock(lk1);
		IDOS->UnLock(lk2);
		return (LOCK_DIFFERENT);

	}
	ret = IDOS->SameLock(lk1, lk2);
	if((ret == LOCK_SAME) && (type < 2))
	{
		int a = (type ? STR_CANT_OVERCOPY_FILES : STR_CANT_COPY_DIR_TO_ITSELF);
		dostatustext(globstring[a]);
		simplerequest(TDRIMAGE_WARNING, globstring[a], globstring[STR_CONTINUE], NULL);
	}
	IDOS->UnLock(lk1);
	IDOS->UnLock(lk2);
	return (ret);
}

BOOL getreal(char *name, STRPTR realname, int size)
{ //Returns complete path of file in primary assignment of multi-assigns
	APTR wsave;
	STRPTR file = NULL;
	struct DevProc *devproc = NULL;

	if (!name) return FALSE;
	wsave = IDOS->SetProcWindow((APTR)-1L);
	if ((devproc = IDOS->GetDeviceProc(name, devproc)))
	{
		if (devproc->dvp_Flags & DVPF_ASSIGN)
		{
			if (IDOS->DevNameFromLock(devproc->dvp_Lock, realname, size, DN_FULLPATH))
//			if (IDOS->NameFromLock(devproc->dvp_Lock, realname, size))
			{
				file = strchr(name,':'); file++;
				IDOS->AddPart(realname, file, size);
				IDOS->FreeDeviceProc(devproc);
				IDOS->SetProcWindow(wsave);
				return TRUE;
			}
		}
		IDOS->FreeDeviceProc(devproc);
	}
	IDOS->SetProcWindow(wsave);
	return FALSE;
}

int checkexist(char *name, int *size)
{ //Checks existence in primary assignment of multi-assign directories
	static char buffer[FILEBUF_SIZE] = {0};

	if (!name) return 0;
	if (getreal(name, buffer, FILEBUF_SIZE))
		return IDOpus->CheckExist(buffer, size);
	return IDOpus->CheckExist(name, size);
}

struct ExamineData *examineobject(char *name)
{ //Returns object in primary assignment of multi-assign directories
	char buffer[FILEBUF_SIZE] = {0};

	if (!name) {IDOS->SetIoErr(ERROR_REQUIRED_ARG_MISSING); return NULL;}
	if (getreal(name, buffer, FILEBUF_SIZE))
		return IDOS->ExamineObjectTags(EX_StringName, buffer, TAG_END);
	return IDOS->ExamineObjectTags(EX_StringName, name, TAG_END);
}

int32 expand_path(CONST_STRPTR path, STRPTR buffer)
{
	BPTR lock;
	APTR save;
	int suc;

	save = IDOS->SetProcWindow((APTR)-1L);
	buffer[0] = 0;
	if((lock = IDOS->Lock(path, ACCESS_READ)))
	{
		IDOS->NameFromLock(lock, buffer, 256);
		IDOS->UnLock(lock);
		suc = 1;
	}
	else
		suc = 0;
	IDOS->SetProcWindow(save);
	return (suc);
}
