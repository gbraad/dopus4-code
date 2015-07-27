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

void seedate(struct DateStamp *ds, char *date, int pad, int datesize)
{
	char timebuf[16], datebuf[16];
	struct DateTime dt;

	copy_datestamp(ds, &dt.dat_Stamp);
	initdatetime(&dt, datebuf, timebuf, 1);

	snprintf(date, datesize, "%s %s", datebuf, timebuf);
}

int setdate(char *name, struct DateStamp *date)
{
	int rc = IDOS->SetFileDate(name, date);
	return (rc ? 1 : IDOS->IoErr());
}

void seename(int win)
{
	long long tot;
	int bl, a = 0;
	static char buf[256];

	if(win < 0 || status_iconified)
		return;
	if(!dopus_curwin[win]->firstentry || (dopus_curwin[win]->firstentry->type != ENTRY_DEVICE && dopus_curwin[win]->firstentry->type != ENTRY_CUSTOM))
	{
		if(str_pathbuffer[win][0] == 0)
		{
			if(win == data_active_window)
				IGraphics->SetAPen(main_rp, screen_pens[config->disknameselbg].pen);
			else
				IGraphics->SetAPen(main_rp, screen_pens[config->disknamebg].pen);
			rectfill(main_rp, scrdata_diskname_xpos[win] + 2, scrdata_diskname_ypos, scrdata_diskname_width[win], scrdata_diskname_height - 2);
			IGraphics->SetAPen(main_rp, screen_pens[1].pen);
			return;
		}
		else
		{
			if(dopus_curwin[win]->flags & DWF_ARCHIVE)
			{
				if(dopus_curwin[win]->arcname)
				{
					char *c;

					c = strstr(dopus_curwin[win]->directory, IDOS->FilePart(dopus_curwin[win]->arcname));
					if(c)
						for(a = 0; c && (*c != '/') && (a < 31); c++)
							dopus_curwin[win]->diskname[a++] = *c;
					dopus_curwin[win]->diskname[a] = 0;
					displayname(win, 1);
					return;
				}
			}
			strlcpy(buf, str_pathbuffer[win], sizeof(buf));
			dopus_curwin[win]->diskname[0] = 0;
			IDOS->SetProcWindow((APTR)-1L);
			if(!(a = getroot(buf, NULL)))
			{
				strlcpy(dopus_curwin[win]->diskname, globstring[STR_DIR_NOT_AVAILABLE_TITLE], sizeof(dopus_curwin[win]->diskname));
				dopus_curwin[win]->disktot = dopus_curwin[win]->diskfree = dopus_curwin[win]->diskblock = -1;
				displayname(win, 1);
				if(config->errorflags & ERROR_ENABLE_DOS)
					IDOS->SetProcWindow(Window);
				return;
			}
			strlcpy(dopus_curwin[win]->diskname, buf, sizeof(dopus_curwin[win]->diskname));
			strlcpy(dopus_curwin[win]->volumename, buf, sizeof(dopus_curwin[win]->volumename));

			dopus_curwin[win]->diskfree = bytes(str_pathbuffer[win], &tot, &bl);
			dopus_curwin[win]->disktot = tot;
			dopus_curwin[win]->diskblock = bl;
			if(a == ID_WRITE_PROTECTED)
				dopus_curwin[win]->flags |= DWF_READONLY;
			else
				dopus_curwin[win]->flags &= ~DWF_READONLY;
			if(config->errorflags & ERROR_ENABLE_DOS)
				IDOS->SetProcWindow(Window);
		}
	}
	displayname(win, 0);
}

void displayname(int win, int clear)
{
	long long free, tot;
	int b, x = 0, nn = 0, len, len2, len3, x1, cx1, cx2;
	static char buf[30], buf2[80], buf3[20];
	double pct;

	if(status_iconified || win < 0)
		return;
	if(str_pathbuffer[win][0] == 0)
	{
		if(dopus_curwin[win]->disktot != -1 && (!dopus_curwin[win]->firstentry || dopus_curwin[win]->firstentry->type != ENTRY_DEVICE))
			nn = 2;
		else
			nn = 1;
	}
	if(nn != 2)
	{
		free = dopus_curwin[win]->diskfree;
		tot = dopus_curwin[win]->disktot;
		IGraphics->SetDrMd(main_rp, JAM2);

		strlcpy(buf2, dopus_curwin[win]->diskname, sizeof(buf2));
		strlcat(buf2, str_space_string, sizeof(buf2));
		IGraphics->SetFont(main_rp, scr_font[FONT_NAMES]);

		len = 30;
		x1 = dotextlength(main_rp, buf2, &len, scrdata_diskname_width[win] - 4);

		if(!nn && tot > -1)
		{
			if(config->showfree & SHOWFREE_BYTES || config->showfree == 0)
			{
				if(dopus_curwin[win]->flags & DWF_READONLY)
					snprintf(buf, sizeof(buf), " (%qd)", free);
				else
					snprintf(buf, sizeof(buf), " %qd", free);
			}
			else if(config->showfree & SHOWFREE_KILO)
			{
				getsizestring(buf, free, sizeof(buf));
				if(dopus_curwin[win]->flags & DWF_READONLY)
				{
					char buf1[30];

					snprintf(buf1, sizeof(buf1), "(%s)", buf);
					strlcpy(buf, buf1, sizeof(buf));
				}
			}
			else if(config->showfree & SHOWFREE_BLOCKS)
			{
				if(dopus_curwin[win]->flags & DWF_READONLY)
					snprintf(buf, sizeof(buf), " (%d)", dopus_curwin[win]->diskblock);
				else
					snprintf(buf, sizeof(buf), " %d", dopus_curwin[win]->diskblock);
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
					snprintf(buf, sizeof(buf), " (%d%%)", b);
				else
					snprintf(buf, sizeof(buf), " %d%%", b);
			}
			strlcpy(buf3, buf, sizeof(buf3));
			strlcat(buf3, str_space_string, sizeof(buf3));
			len2 = 12;
			len3 = strlen(buf);
			FOREVER
			{
				x = dotextlength(main_rp, buf3, &len2, scrdata_diskname_width[win] - x1 - 4);
				if(len2 >= len3 || (--len) == 0)
					break;
				len2 = 12;
				x1 = dotextlength(main_rp, buf2, &len, scrdata_diskname_width[win] - 4);
			}
			strlcpy(buf3, str_space_string, sizeof(buf3));
			strlcpy(&buf3[len2 - len3], buf, sizeof(buf3) - (len2 - len3));
		}
		else
			x = 0;
	}

	if(clear)
	{
		if(win == data_active_window)
			IGraphics->SetAPen(main_rp, screen_pens[config->disknameselbg].pen);
		else
			IGraphics->SetAPen(main_rp, screen_pens[config->disknamebg].pen);
		rectfill(main_rp, scrdata_diskname_xpos[win] + 2, scrdata_diskname_ypos, scrdata_diskname_width[win], scrdata_diskname_height - 2);
	}

	if(nn != 2)
	{
		if(win == data_active_window)
		{
			IGraphics->SetAPen(main_rp, screen_pens[config->disknameselfg].pen);
			IGraphics->SetBPen(main_rp, screen_pens[config->disknameselbg].pen);
		}
		else
		{
			IGraphics->SetAPen(main_rp, screen_pens[config->disknamefg].pen);
			IGraphics->SetBPen(main_rp, screen_pens[config->disknamebg].pen);
		}

		IGraphics->Move(main_rp, scrdata_diskname_xpos[win] + 4, scrdata_diskname_ypos + scr_font[FONT_NAMES]->tf_Baseline);
		IGraphics->Text(main_rp, buf2, len);

		cx1 = main_rp->cp_x;
		if(x)
		{
			x1 = (scrdata_diskname_xpos[win] + scrdata_diskname_width[win]) - x;
			if(x1 < scrdata_diskname_xpos[win] + 2)
				x1 = scrdata_diskname_xpos[win] + 2;
			cx2 = x1 - 1;
			IGraphics->Move(main_rp, x1, scrdata_diskname_ypos + scr_font[FONT_NAMES]->tf_Baseline);
			IGraphics->Text(main_rp, buf3, len2);
		}
		else
			cx2 = scrdata_diskname_xpos[win] + scrdata_diskname_width[win] + 1;
		if(!clear && cx1 <= cx2)
		{
			IGraphics->SetAPen(main_rp, main_rp->BgPen);
			IGraphics->RectFill(main_rp, cx1, scrdata_diskname_ypos, cx2, scrdata_diskname_height + scrdata_diskname_ypos - 3);
		}
		IGraphics->SetFont(main_rp, scr_font[FONT_GENERAL]);
	}
}

void relabel_disk(int rexx, char *path)
{
	char oldname[36], name[36];
	char buf[256];

	strlcpy(buf, rexx ? rexx_args[0] : path, sizeof(buf));
	if(!(getroot(buf, NULL)))
	{
		doerror(-1);
		return;
	}
	strlcat(buf, ":", sizeof(buf));
	getroot(buf, NULL);

	strlcpy(name, buf, sizeof(name));
	strlcpy(oldname, name, sizeof(oldname));
	if(!rexx)
	{
		if(!(whatsit(globstring[STR_ENTER_NEW_DISK_NAME], 30, name, NULL)))
			return;
	}
	else
		strlcpy(name, rexx_args[1], sizeof(name));

	if(name[strlen(name) - 1] == ':')
		name[strlen(name) - 1] = 0;

	strlcat(buf, ":", sizeof(buf));
	if(!IDOS->Relabel(buf, name))
	{
		doerror(-1);
	}
	else if((!status_iconified) && (dopus_curwin[data_active_window] != dopus_specialwin[data_active_window]))
	{
		if((strncmp(str_pathbuffer[data_active_window], oldname, strlen(oldname))) == 0 && str_pathbuffer[data_active_window][strlen(oldname)] == ':')
		{
			strlcpy(buf, name, sizeof(buf));
			strlcat(buf, &str_pathbuffer[data_active_window][strlen(oldname)], sizeof(buf));
			strlcpy(str_pathbuffer[data_active_window], buf, sizeof(str_pathbuffer[0]));
			checkdir(str_pathbuffer[data_active_window], &path_strgadget[data_active_window]);
			strlcpy(dopus_curwin[data_active_window]->directory, str_pathbuffer[data_active_window], sizeof(dopus_curwin[0]->directory));
		}
		seename(data_active_window);
	}
}

int getroot(char *name, struct DateStamp *ds)
{
	struct InfoData *info = IDOS->AllocDosObject(DOS_INFODATA, NULL);
	BPTR lock1;
	struct FileLock *lock2;
	struct DeviceList *dl;
	int a;

	if(!(lock1 = IDOS->Lock(name, ACCESS_READ)))
	{
		IDOS->FreeDosObject(DOS_INFODATA, info);
		return (0);
	}
	lock2 = (struct FileLock *)BADDR(lock1);
	for(a = 0; a < FILEBUF_SIZE; a++)
		name[a] = 0;
	dl = (struct DeviceList *)BADDR(lock2->fl_Volume);
	if (dl->dl_Name)
		a = IDOS->CopyStringBSTRToC(dl->dl_Name, name, FILEBUF_SIZE);
	if(ds)
		IExec->CopyMem((char *)&dl->dl_VolumeDate, (char *)ds, sizeof(struct DateStamp));
	IDOS->Info(lock1, info);
	a = info->id_DiskState;
	IDOS->UnLock(lock1);
	IDOS->FreeDosObject(DOS_INFODATA, info);
	return a; //(info->id_DiskState);
}

BPTR getrootlock(BPTR lock1)
{
	BPTR lock2;

	while((lock2 = IDOS->ParentDir(lock1)))
	{
		IDOS->UnLock(lock1);
		lock1 = lock2;
	}
	return(lock1);
}

void strtostamp(char *date, char *time, struct DateStamp *ds)
{
	struct DateTime datetime;

	datetime.dat_Format = dateformat(config->dateformat);
	datetime.dat_Flags = 0;
	datetime.dat_StrDate = date;
	datetime.dat_StrTime = time;
	IDOS->StrToDate((struct DateTime *)&datetime);
	copy_datestamp(&datetime.dat_Stamp, ds);
}
