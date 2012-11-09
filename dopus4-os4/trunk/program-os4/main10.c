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

void initdatetime(struct DateTime *datetime, STRPTR datebuf, STRPTR timebuf, int foo)
{
	datetime->dat_Format = dateformat(config->dateformat);
	datetime->dat_StrDay = NULL;
	datetime->dat_StrDate = datebuf;
	datetime->dat_StrTime = timebuf;

	if(foo == -1 || (foo && config->dateformat & DATE_SUBST))
	{
		datetime->dat_Flags = DDTF_SUBST;
	}
	else
	{
		datetime->dat_Flags = 0;
	}
	if(foo > 0 && config->dateformat & DATE_12HOUR)
	{
		datetime->dat_Flags |= DDTF_12HOUR;
	}

	IDOS->DateToStr(datetime);
//	IDOpus->StampToStr(datetime);
}

void doarexx(int rexx)
{
	char buf[256];

	buf[0] = 0;
	if(rexx)
		strncpy(buf, rexx_args[0], 256);
	else
	{
		strncpy(buf, str_arexx_command, 256);
		if(!(whatsit(globstring[STR_ENTER_AREXX_COMMAND], 256, buf, (char *)-2)))
			return;
	}
	if(!buf[0])
		return;
	if(!(strstri(buf, "address")) && buf[0] != '\"')
	{
		strcpy(str_arexx_command, buf);
		strcpy(&buf[1], str_arexx_command);
		buf[0] = '*';
		if(getfunction(buf, NULL) != -1)
		{
			dofunctionstring(buf, NULL, NULL, NULL);
			return;
		}
		strncpy(buf, str_arexx_command, 256);
	}
	if(!(IExec->FindPort("REXX")))
	{
		simplerequest(TDRIMAGE_WARNING, globstring[STR_AREXX_NOT_RUNNING], str_okaystring, NULL);
		return;
	}
	strcpy(str_arexx_command, buf);
	status_flags |= STATUS_DONEREXX;
	rexx_command(buf, NULL);
}

void setcurdir(int rexx)
{
	static int time_through;
	char dir[256];
	BPTR lock;

	if(!rexx)
	{
		expand_path("", dir);
		if(!(whatsit(globstring[STR_ENTER_NEW_CD], 256, dir, (char *)-1)))
		{
			myabort();
			return;
		}
	}
	else
		strncpy(dir, rexx_args[0], 256);
	if(!(lock = IDOS->Lock(dir, ACCESS_READ)))
	{
		doerror(-1);
		return;
	}
	if((lock = IDOS->CurrentDir(lock)))
	{
		if(time_through)
			IDOS->UnLock(lock);
		++time_through;
	}
	okay();
}

void dodevicelist(int win)
{
	struct DosList *dl = NULL;
	char devname[32] = { 0, };
	struct Directory *addafter = NULL;

	if(status_iconified)
		return;

	makespecialdir(win, globstring[STR_DEVICE_LIST_TITLE]);

	dostatustext(globstring[STR_SCANNING_DEVICE_LIST]);
	busy();

	dl = IDOS->LockDosList(LDF_DEVICES | LDF_READ);
	while((dl = IDOS->NextDosEntry(dl, LDF_DEVICES)))
	{
		if(dl->dol_Type != DLT_DEVICE || dl->dol_Task)
		{
			IDOS->CopyStringBSTRToC(dl->dol_Name, devname, 32);
			strcat(devname, ":");
			if(!(addfile(dopus_curwin[win], win, devname, dl->dol_Type, 0, NULL, NULL, 0, 0, FALSE, NULL, addafter, 0, 0)))
				break;
		}
	}
	if((addafter = dopus_curwin[win]->firstentry))
		while(addafter->next)
			addafter = addafter->next;
	IDOS->UnLockDosList(LDF_DEVICES | LDF_READ);

	dl = IDOS->LockDosList(LDF_VOLUMES | LDF_READ);
	while((dl = IDOS->NextDosEntry(dl, LDF_VOLUMES)))
	{
		if(dl->dol_Type != DLT_DEVICE || dl->dol_Task)
		{
			IDOS->CopyStringBSTRToC(dl->dol_Name, devname, 32);
			strcat(devname, ":");
			if(!(addfile(dopus_curwin[win], win, devname, dl->dol_Type, 0, NULL, NULL, 0, 0, FALSE, NULL, addafter, 0, 0)))
				break;
		}
	}
	if((addafter = dopus_curwin[win]->firstentry))
		while(addafter->next)
			addafter = addafter->next;
	IDOS->UnLockDosList(LDF_VOLUMES | LDF_READ);

	dl = IDOS->LockDosList(LDF_ASSIGNS | LDF_READ);
	while((dl = IDOS->NextDosEntry(dl, LDF_ASSIGNS)))
	{
		if(dl->dol_Type != DLT_DEVICE || dl->dol_Task)
		{
			IDOS->CopyStringBSTRToC(dl->dol_Name, devname, 32);
			strcat(devname, ":");
			if(!(addfile(dopus_curwin[win], win, devname, dl->dol_Type, 0, NULL, NULL, 0, 0, FALSE, NULL, addafter, 0, 0)))
				break;
		}
	}
	if((addafter = dopus_curwin[win]->firstentry))
		while(addafter->next)
			addafter = addafter->next;
	IDOS->UnLockDosList(LDF_ASSIGNS | LDF_READ);

	refreshwindow(win, 3);
	unbusy();
	okay();
}

/*
int huntfile(char *name, char *completename, int *aa)
{
	char buf[FILEBUF_SIZE];
	int rec;

	IDOpus->StrToUpper(name, buf);
	if(IDOS->MatchPatternNoCase(str_hunt_name_parsed, buf))
	{
		char mesbuf[300], *ptr;

		if((ptr = IDOS->FilePart(completename)))
		{
			strncpy(buf, IDOS->FilePart(completename), 300);
			*ptr = 0;
		}
		else
		{
			buf[0] = 0;
		}

		*aa = 1;

		snprintf(mesbuf, 300, globstring[STR_FOUND_A_MATCH], buf, completename);
		if((rec = simplerequest(TDRIMAGE_QUESTION, mesbuf, globstring[STR_OKAY], globstring[STR_ABORT], globstring[STR_SKIP], NULL)) == 1)
		{
			if(!status_iconified)
			{
				unbusy();
				advancebuf(data_active_window, 1);
				strcpy(str_pathbuffer[data_active_window], completename);
				startgetdir(data_active_window, 3);
				busy();
			}
			return (-3);
		}
		else if(rec == 2)
		{
			return (0);
		}
		else if(rec == 0)
		{
			return (-1);
		}
	}
	else
	{
		*aa = 0;
	}

	return (0);
}
*/

void checksize(int win)
{
	BPTR mylock;

	IDOS->SetProcWindow((APTR)-1L);
	if((mylock = IDOS->Lock(str_pathbuffer[win], ACCESS_READ)))
	{
		IDOS->UnLock(mylock);
		seename(win);
	}
	else
		displayname(win, 1);
	if(config->errorflags & ERROR_ENABLE_DOS)
	{
		IDOS->SetProcWindow(Window);
	}
}

void centerwindow(struct NewWindow *wind)
{
	int h, w, pw, ph;

	if(!status_iconified && Window)
	{
		w = Window->WScreen->Width;
		h = Window->WScreen->Height;
		pw = Window->Width;
		ph = Window->Height;
	}
	else
	{
		struct Screen scrbuf;

		IDOpus->GetWBScreen(&scrbuf);
		pw = w = scrbuf.Width;
		ph = h = scrbuf.Height;
	}

	if(wind->Width > w)
		wind->Width = w;
	if(wind->Height > h)
		wind->Height = h;
	wind->LeftEdge = (pw - wind->Width) / 2;
	wind->TopEdge = (ph - wind->Height) / 2;
	wind->Type = CUSTOMSCREEN;

	if(!status_publicscreen)
		wind->Screen = MainScreen;
	else
	{
		if(Window)
			wind->Screen = Window->WScreen;
		else
			wind->Type = WBENCHSCREEN;
		if(!status_iconified && Window)
		{
			wind->LeftEdge += Window->LeftEdge;
			wind->TopEdge += Window->TopEdge;
		}
	}
}

char *parsedatetime(char *buf, char *dbuf, char *tbuf, int *dis)
{
	STRPTR datebuf, timebuf, ptr, temp;
	char dbuffer[80], mydtbuf[20], mytmbuf[20];
	struct DateTime datetime;
	int a, b, c, swap = 0;

	timebuf = datebuf = NULL;
	*dis = 0;
	strcpy(dbuffer, buf);
	b = strlen(dbuffer);
	for(a = 0; a < b; a++)
		if(!(isspace(dbuffer[a])))
			break;
	if(a == b)
		goto getout;
	if(dbuffer[a] == '>')
	{
		++a;
		*dis = 2;
		goto getout;
	}
	datebuf = &dbuffer[a];

	for(++a; a < b; a++)
	{
		if(isspace(dbuffer[a]))
		{
			dbuffer[a] = 0;
			break;
		}
		if(dbuffer[a] == ':')
			swap = 1;
	}
	if(a >= b)
		goto getout;
	for(++a; a < b; a++)
		if(!(isspace(dbuffer[a])))
			break;
	if(a >= b)
		goto getout;

	if(dbuffer[a] == '>')
	{
		++a;
		*dis = 1;
		goto getout;
	}

	timebuf = &dbuffer[a];
	for(++a; a < b; a++)
	{
		if(isspace(dbuffer[a]))
		{
			dbuffer[a] = 0;
			break;
		}
	}
	if(a == b)
		goto getout;
	c = a;
	for(++a; a < b; a++)
	{
		if(dbuffer[a] == '>')
		{
			c = a + 1;
			*dis = 1;
			break;
		}
	}
	a = c;
      getout:
	ptr = &buf[a];
	if(swap)
	{
		temp = datebuf;
		datebuf = timebuf;
		timebuf = temp;
	}
	IDOS->DateStamp(&(datetime.dat_Stamp));
	initdatetime(&datetime, mydtbuf, mytmbuf, -1);

	c = 0;
	if(datebuf && (a = strlen(datebuf)) < 16 && a > 2)
		strcpy(dbuf, datebuf);
	else
	{
		if((*dis) != 2)
		{
			c = 1;
			strcpy(dbuf, mydtbuf);
		}
		else
		{
			char *fptr;

			switch (dateformat(config->dateformat))
			{
			case FORMAT_INT:
				fptr = "78-01-01";
				break;
			default:
				fptr = "1-Jan-78";
				break;
			}
			strcpy(dbuf, fptr);
		}
	}
	if(timebuf && (a = strlen(timebuf)) < 9 && a > 2)
	{
		strcpy(tbuf, timebuf);
		if(!(*dis))
			*dis = -1;
	}
	else
		strcpy(tbuf, c ? (char *)mytmbuf : (char *)"00:00:00");

	return (ptr);
}

void copy_datestamp(struct DateStamp *source, struct DateStamp *dest)
{
	dest->ds_Days = source->ds_Days;
	dest->ds_Minute = source->ds_Minute;
	dest->ds_Tick = source->ds_Tick;
}

ULONG clone_screen(struct Screen *original, struct ExtNewScreen *clone)
{
	struct Screen scrbuf;
	struct Screen *screen = NULL;
	ULONG modes;

	if(original || (original = screen = IIntuition->LockPubScreen(NULL)))
	{
		clone->Width = original->Width;
		clone->Height = original->Height;
		if((clone->Depth = original->RastPort.BitMap->Depth) > config->scrdepth)
			clone->Depth = config->scrdepth;
		clone->ViewModes = original->ViewPort.Modes;
		modes = IGraphics->GetVPModeID(&original->ViewPort);
		if(screen)
			IIntuition->UnlockPubScreen(NULL, screen);
		return (modes);
	}

	IIntuition->GetScreenData(&scrbuf, sizeof(struct Screen), (original) ? CUSTOMSCREEN : WBENCHSCREEN, original);

	clone->Width = scrbuf.Width;
	clone->Height = scrbuf.Height;
	if((clone->Depth = scrbuf.RastPort.BitMap->Depth) > 8)
		clone->Depth = 8;
	clone->ViewModes = scrbuf.ViewPort.Modes;
	return ((ULONG) clone->ViewModes);
}

struct MsgPort *CreateUniquePort(CONST_STRPTR base, STRPTR buffer, int *count)
{
	int a;
	struct MsgPort *port;

	IExec->Forbid();
	for(a = 0;; a++)
	{
		snprintf(buffer, 30, "%s%d", config_replyport_basename, a);
		if(!(IExec->FindPort(buffer)))
		{
			sprintf(buffer, "%s.%d", base, a + 1);
			if(!(IExec->FindPort(buffer)))
			{
				port = IExec->AllocSysObjectTags(ASOT_PORT, ASOPORT_Name, buffer, ASOPORT_CopyName, TRUE, TAG_DONE);
				if(count)
					*count = a;
				break;
			}
		}
	}
	IExec->Permit();
	return (port);
}

