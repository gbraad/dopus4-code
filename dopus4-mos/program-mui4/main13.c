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

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include <libraries/mui.h>
#include <proto/alib.h>

#include "dopus.h"
#include "mui.h"

void seedate(struct DateStamp *ds, STRPTR date, int pad)
{
	char timebuf[16], datebuf[16];
	struct DateTime dt;

	copy_datestamp(ds, &dt.dat_Stamp);
	initdatetime(&dt, datebuf, timebuf, 1);

	sprintf(date, "%s %s", datebuf, timebuf);
}

int setdate(STRPTR name, struct DateStamp *date)
{
	int rc = SetFileDate(name, date);
	return (rc ? 1 : IoErr());
}

void seename(int win)
{
	long long tot;
	int bl, a = 0;
	static char buf[256];

	if(win < 0)
		return;

	if(!dopus_curwin[win]->firstentry || (dopus_curwin[win]->firstentry->type != ENTRY_DEVICE && dopus_curwin[win]->firstentry->type != ENTRY_CUSTOM))
	{
		if(str_pathbuffer[win][0] == 0)
		{
			DoMethod(dopusdirlist[win], MM_FileArea_SetDiskName, win == data_active_window ? &screen_pens[config->disknameselbg] : &screen_pens[config->disknamebg], NULL, NULL);
			return;
		}
		else
		{
			APTR oldwin;

			if(dopus_curwin[win]->flags & DWF_ARCHIVE)
			{
				if(dopus_curwin[win]->arcname)
				{
					char *c;

					c = strstr(dopus_curwin[win]->directory, FilePart(dopus_curwin[win]->arcname));
					if(c)
						for(a = 0; c && (*c != '/') && (a < 31); c++)
							dopus_curwin[win]->diskname[a++] = *c;
					dopus_curwin[win]->diskname[a] = 0;
					displayname(win, 1);
					return;
				}
			}
			strcpy(buf, str_pathbuffer[win]);
			dopus_curwin[win]->diskname[0] = 0;
			oldwin = main_proc->pr_WindowPtr;
			main_proc->pr_WindowPtr = (APTR) - 1;
			if(!(a = getroot(buf, NULL)))
			{
				strcpy(dopus_curwin[win]->diskname, globstring[STR_DIR_NOT_AVAILABLE_TITLE]);
				dopus_curwin[win]->disktot = dopus_curwin[win]->diskfree = dopus_curwin[win]->diskblock = -1;
				displayname(win, 1);
				if(config->errorflags & ERROR_ENABLE_DOS)
					main_proc->pr_WindowPtr = oldwin;
				return;
			}
			strcpy(dopus_curwin[win]->diskname, buf);
			strcpy(dopus_curwin[win]->volumename, buf);

			dopus_curwin[win]->diskfree = bytes(str_pathbuffer[win], &tot, &bl);
			dopus_curwin[win]->disktot = tot;
			dopus_curwin[win]->diskblock = bl;
			if(a == ID_WRITE_PROTECTED)
				dopus_curwin[win]->flags |= DWF_READONLY;
			else
				dopus_curwin[win]->flags &= ~DWF_READONLY;
			if(config->errorflags & ERROR_ENABLE_DOS)
				main_proc->pr_WindowPtr = oldwin;
		}
	}
	displayname(win, 0);
}

void displayname(int win, int clear)
{
	long long free, tot;
	int b, nn = 0;
	static char buf[32], buf2[32 + 12], buf3[20 + 12];
	double pct;

	if(win < 0)
		return;

	if(str_pathbuffer[win][0] == 0)
	{
		if(dopus_curwin[win]->disktot != -1 && (!dopus_curwin[win]->firstentry || dopus_curwin[win]->firstentry->type != ENTRY_DEVICE))
			nn = 2;
		else
			nn = 1;
	}

	if (nn != 2)
	{
		struct ColourTable *c;
		TEXT tcol[12];

		c = win == data_active_window ? &screen_pens[config->disknameselfg] : &screen_pens[config->disknamefg],

		NewRawDoFmt("\33P[%02x%02x%02x]", NULL, tcol, c->red & 0xff, c->green & 0xff, c->blue & 0xff);

		free = dopus_curwin[win]->diskfree;
		tot = dopus_curwin[win]->disktot;

		stccpy(buf, dopus_curwin[win]->diskname, sizeof(buf));
		NewRawDoFmt("%s%s", NULL, buf2, tcol, buf);

		if (!nn && tot > -1)
		{
			if(config->showfree & SHOWFREE_BYTES || config->showfree == 0)
			{
				if(dopus_curwin[win]->flags & DWF_READONLY)
					sprintf(buf3, "%s(%qd)", tcol, free);
				else
					sprintf(buf3, "%s%qd", tcol, free);
			}
			else if(config->showfree & SHOWFREE_KILO)
			{
				getsizestring(buf, free);

				if(dopus_curwin[win]->flags & DWF_READONLY)
				{
					sprintf(buf3, "%s(%s)", tcol, buf);
				}
				else
				{
					sprintf(buf3, "%s%s", tcol, buf);
				}
			}
			else if(config->showfree & SHOWFREE_BLOCKS)
			{
				if(dopus_curwin[win]->flags & DWF_READONLY)
					sprintf(buf3, "%s(%d)", tcol, dopus_curwin[win]->diskblock);
				else
					sprintf(buf3, "%s%d", tcol, dopus_curwin[win]->diskblock);
			}
			else if(config->showfree & SHOWFREE_PERCENT)
			{
				if(tot == 0)
					b = 100;
				else
				{
					pct = free;
					pct *= 100;
					b = (int)(pct / (double)tot);
				}
				if(b > 100)
					b = 100;
				if(dopus_curwin[win]->flags & DWF_READONLY)
					sprintf(buf3, "%s(%d%%)", tcol, b);
				else
					sprintf(buf3, "%s%d%%", tcol, b);
			}
		}
	}

	if (clear)
	{
		DoMethod(dopusdirlist[win], MM_FileArea_SetDiskName, win == data_active_window ? &screen_pens[config->disknameselbg] : &screen_pens[config->disknamebg], NULL, NULL);
	}

	if (nn != 2)
	{
		DoMethod(dopusdirlist[win], MM_FileArea_SetDiskName, win == data_active_window ? &screen_pens[config->disknameselbg] : &screen_pens[config->disknamebg], buf2, buf3);
	}
}

void relabel_disk(int rexx, STRPTR path)
{
	char oldname[36], name[36];
	TEXT buf[256];

	strcpy(buf, rexx ? rexx_args[0] : path);
	if(!(getroot(buf, NULL)))
	{
		doerror(-1);
		return;
	}
	strcat(buf, ":");
	getroot(buf, NULL);

	strcpy(name, buf);
	strcpy(oldname, name);
	if(!rexx)
	{
		if(!(whatsit(globstring[STR_ENTER_NEW_DISK_NAME], 30, name, NULL)))
			return;
	}
	else
		strcpy(name, rexx_args[1]);

	if(name[strlen(name) - 1] == ':')
		name[strlen(name) - 1] = 0;

	strcat(buf, ":");
	if(!Relabel(buf, name))
	{
		doerror(-1);
	}
	else if(dopus_curwin[data_active_window] != dopus_specialwin[data_active_window])
	{
		if((strncmp(str_pathbuffer[data_active_window], oldname, strlen(oldname))) == 0 && str_pathbuffer[data_active_window][strlen(oldname)] == ':')
		{
			strcpy(buf, name);
			StrConcat(buf, &str_pathbuffer[data_active_window][strlen(oldname)], 256);
			strcpy(str_pathbuffer[data_active_window], buf);
			checkdir(str_pathbuffer[data_active_window], &path_strgadget[data_active_window]);
			strcpy(dopus_curwin[data_active_window]->directory, str_pathbuffer[data_active_window]);
		}
		seename(data_active_window);
	}
}

int getroot(STRPTR name, struct DateStamp *ds)
{
	struct InfoData info;
	BPTR lock1;
	struct FileLock *lock2;
	char *p;
	struct DeviceList *dl;
	int a;

	if(!(lock1 = Lock(name, ACCESS_READ)))
		return (0);
	lock2 = (struct FileLock *)BADDR(lock1);
	for(a = 0; a < FILEBUF_SIZE; a++)
		name[a] = 0;
	dl = (struct DeviceList *)BADDR(lock2->fl_Volume);
	p = (char *)BADDR(dl->dl_Name);
	if(p)
		stccpy(name, p + 1, *p + 1);
	if(ds)
		CopyMem((char *)&dl->dl_VolumeDate, (char *)ds, sizeof(struct DateStamp));
	Info(lock1, &info);
	UnLock(lock1);
	return (info.id_DiskState);
}

BPTR getrootlock(BPTR lock1)
{
	BPTR lock2;

	while((lock2 = ParentDir(lock1)))
	{
		UnLock(lock1);
		lock1 = lock2;
	}
	return(lock1);
}

void strtostamp(STRPTR date, STRPTR time, struct DateStamp *ds)
{
	struct DateTime datetime;

	datetime.dat_Format = dateformat(config->dateformat);
	datetime.dat_Flags = 0;
	datetime.dat_StrDate = date;
	datetime.dat_StrTime = time;
	StrToDate((struct DateTime *)&datetime);
	copy_datestamp(&datetime.dat_Stamp, ds);
}
