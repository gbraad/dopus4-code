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

#include "dopus.h"
//#include <proto/asyncio.h>

int copyfile(STRPTR src, STRPTR dst, int *err, STRPTR password, int encryptstate)
{
	struct FileInfoBlock *cfinfo = IDOS->AllocDosObject(DOS_FIB, NULL);
	char *buffer;
	int out, length, suc, readsize, size_read, size_write, size_total, ret = 0, buffer_size = 0, size;
	int prog = (config->dynamicflags & UPDATE_PROGRESSIND_COPY);
	BPTR inhandle, outhandle;
	struct DateStamp ds, *dsp;

	buffer = NULL;

	inhandle = 0;
	outhandle = 0;

	if(password)
	{
		int a, encrypt = 1;

		for(a = 0; password[a]; a++)
		{
			encrypt *= password[a];
		}
		srand(encrypt);
	}

	suc = lockandexamine(src, cfinfo);

	if(!suc)
	{
		*err = IDOS->IoErr();
		return (0);
	}

	if(!(size = cfinfo->fib_Size))
	{
		if(!(out = IDOS->FOpen(dst, MODE_NEWFILE, 0)))
			goto failed;
		IDOS->FClose(out);
		if(config->copyflags & COPY_DATE)
			setdate(dst, &(cfinfo->fib_Date));
		if(config->copyflags & COPY_PROT)
			IDOS->SetProtection(dst, cfinfo->fib_Protection & ((config->copyflags & COPY_COPYARC) ? ~0 : ~FIBF_ARCHIVE));
		if(config->copyflags & COPY_NOTE)
			IDOS->SetComment(dst, cfinfo->fib_Comment);
		IDOS->SetOwner(dst, (cfinfo->fib_OwnerUID << 16) | cfinfo->fib_OwnerGID);
		return (1);
	}

	if(size <= COPY_BUF_SIZE)
	{
		dotaskmsg(hotkeymsg_port, PROGRESS_UPDATE, 100, 100, NULL, 1);
		prog = 0;
	}
	else
	{
		dotaskmsg(hotkeymsg_port, PROGRESS_UPDATE, 0, 100, NULL, 1);
	}

	if(!(inhandle = IDOS->FOpen(src, MODE_OLDFILE, 0)))
		goto failed;

	if(!(outhandle = IDOS->FOpen(dst, MODE_NEWFILE, 0)))
		goto failed;

	if(size > (64 * 1024))
	{
		buffer_size = size / 2;
	}
	else
	{
		buffer_size = size;
	}
	if(buffer_size > (128 * 1024))
	{
		buffer_size = 128 * 1024;
	}

	while(buffer_size > 0)
	{
		if((buffer = IExec->AllocMem(buffer_size, MEMF_ANY)))
			break;
		buffer_size /= 2;
	}
	if(!buffer)
	{
		goto failed;
	}

	size_read = size_write = 0;
	size_total = size * 2;

	while(size > 0)
	{
		readsize = (size > buffer_size) ? buffer_size : size;
		length = IDOS->FRead(inhandle, buffer, 1, readsize);

		size -= readsize;
		size_read += length;

		if(prog)
		{
			dotaskmsg(hotkeymsg_port, PROGRESS_UPDATE, size_read + size_write, size_total, NULL, 1);
		}

		if(status_haveaborted)
		{
			ret = -1;
			goto failed;
		}

		if(password)
		{
			int a;
			char enbyte;

			if(encryptstate)
			{
				for(a = 0; a < length; a++)
				{
					enbyte = rand();
					buffer[a] += enbyte;
				}
			}
			else
			{
				for(a = 0; a < length; a++)
				{
					enbyte = rand();
					buffer[a] -= enbyte;
				}
			}
		}

		if(length > 0)
		{
			if((IDOS->FWrite(outhandle, buffer, 1, length)) == -1)
				goto failed;
		}
		size_write += length;

		if(prog)
		{
			dotaskmsg(hotkeymsg_port, PROGRESS_UPDATE, size_read + size_write, size_total, NULL, 1);
		}

		if(status_haveaborted)
		{
			ret = -1;
			goto failed;
		}
	}

	IDOS->FClose(inhandle);
	IDOS->FClose(outhandle);

	IExec->FreeMem(buffer, buffer_size);

	if(config->copyflags & COPY_DATE)
	{
		setdate(dst, &(cfinfo->fib_Date));
		dsp = &cfinfo->fib_Date;
	}
	else
	{
		IDOS->DateStamp(&ds);
		dsp = &ds;
	}
	copy_datestamp(dsp, &dos_copy_date);

	if(config->copyflags & COPY_PROT)
		IDOS->SetProtection(dst, cfinfo->fib_Protection & ((config->copyflags & COPY_COPYARC) ? ~0 : ~FIBF_ARCHIVE));
	dos_copy_protection = cfinfo->fib_Protection;

	if(config->copyflags & COPY_NOTE)
	{
		IDOS->SetComment(dst, cfinfo->fib_Comment);
		strcpy(dos_copy_comment, cfinfo->fib_Comment);
	}
	else
	{
		dos_copy_comment[0] = 0;
	}

	return (1);

      failed:
	*err = IDOS->IoErr();
	if(buffer)
		IExec->FreeMem(buffer, buffer_size);
	if(inhandle)
		IDOS->FClose(inhandle);
	if(outhandle)
	{
		IDOS->FClose(outhandle);
		IDOS->DeleteFile(dst);
	}

	IDOS->FreeDosObject(DOS_FIB, cfinfo);

	return (ret);
}

struct Directory *checktot(struct DirectoryWindow *dir)
{
	struct Directory *first;

	if(!dir)
		return (NULL);
	first = dir->firstentry;
	while(first)
	{
		if(first->selected && first->type <= ENTRY_FILE)
			break;
		first = first->next;
	}
	return (first);
}

struct Directory *checkdirtot(struct DirectoryWindow *dir)
{
	struct Directory *first;

	if(!dir)
		return (NULL);
	first = dir->firstentry;
	while(first)
	{
		if(first->selected && ENTRYTYPE(first->type) == ENTRY_DIRECTORY)
			break;
		first = first->next;
	}
	return (first);
}

struct Directory *checkdevtot(struct DirectoryWindow *dir)
{
	struct Directory *first;

	if(!dir)
		return (NULL);
	first = dir->firstentry;
	while(first)
	{
		if(first->selected && first->type == ENTRY_DEVICE)
			break;
		first = first->next;
	}
	return (first);
}

struct Directory *checkalltot(struct DirectoryWindow *dir)
{
	struct Directory *first;

	if(!dir)
		return (NULL);
	first = dir->firstentry;
	while(first)
	{
		if(first->selected && first->type != ENTRY_DEVICE && first->type != ENTRY_CUSTOM)
			break;
		first = first->next;
	}
	return (first);
}

/* Enforcer HIT: with ARexx script sameselect.dopus */

struct Directory *findfile(struct DirectoryWindow *dir, STRPTR name, int *count)
{
	struct Directory *find;
/*
    char parsebuf[100];

    LParsePatternI(name,parsebuf);
*/

//	D(bug("findfile(%s)\n", name ? name : "<NULL>"));
	if(dir)
	{
		if(str_arcorgname[0])
			name = str_arcorgname;	/* required for double-click */

		find = dir->firstentry;
		if(count)
			*count = 0;
		while(find)
		{
/*
            if (find->name && (LMatchPatternI(parsebuf,find->name))) return(find);
*/
			if(find->name && !(strcmp(name, find->name)))
				return (find);

			find = find->next;
			if(count)
				++(*count);
		}
	}
	return (NULL);
}

int delfile(STRPTR name, STRPTR nam, STRPTR errs, int unprotect, int errcheck)
{
	int suc, a, err, try = 0, recplus = 0;
	char buf[300], buf2[100];

      loop:
	suc = IDOS->DeleteFile(name);
	if(!suc)
	{
		if((err = IDOS->IoErr()) == ERROR_OBJECT_NOT_FOUND)
			suc = 1;
		else
		{
			if(err == ERROR_DIRECTORY_NOT_EMPTY)
				return (-2);
			else if(err == ERROR_DELETE_PROTECTED && try == 0)
			{
				if(!(config->deleteflags & DELETE_SET))
				{
					if(!unprotect)
					{
						doerror(ERROR_DELETE_PROTECTED);
						geterrorstring(buf2, ERROR_DELETE_PROTECTED);
						sprintf(buf, globstring[STR_ERROR_OCCURED], globstring[STR_DELETING], nam, buf2);
						strcat(buf, globstring[STR_SELECT_UNPROTECT]);
						if(!(a = simplerequest(buf, globstring[STR_UNPROTECT], globstring[STR_ABORT], globstring[STR_UNPROTECT_ALL], globstring[STR_SKIP], NULL)))
							return (-1);
						if(a == 3)
							return (0);
						if(a == 2)
							recplus = 1;
					}
				}
				try = 1;
				IDOS->SetProtection(name, 0);
				goto loop;
			}
			else if(!errcheck)
				return (-2);
			doerror(err);
			if((a = checkerror(errs, nam, err)) == 3)
				return (-1);
			if(a == 1)
				goto loop;
		}
	}
	else
	{
		if(recplus)
			suc = 2;
		else
			suc = 1;
	}
	return (suc);
}

int getwildrename(STRPTR sname, STRPTR dname, STRPTR name, STRPTR newn)
{
//    char sfirst[FILEBUF_SIZE],slast[FILEBUF_SIZE],dfirst[FILEBUF_SIZE],dlast[FILEBUF_SIZE],foon[FILEBUF_SIZE];
	int a, b, c /*,flen,llen,d */ ;

	char *spat = sname, *dpat = dname, *sn = name, *dn = newn;
	char c1, c2;

//	D(bug("getwildrename(%s,%s,%s,)\n", sname, dname, name));
/* check if filename matches source pattern */
	a = 1;
	b = 0;
	while(*spat)
	{
		if(*spat == '*')
		{
			b++;
			spat++;
			if(*spat)
				for(c1 = tolower(*spat); c1 != tolower(*sn); sn++);
		}
		else
		{
			c1 = tolower(*spat);
			c2 = tolower(*sn);

			if(c1 == c2)
			{
				spat++;
				sn++;
			}
			else
			{
				a = 0;
				break;
			}
		}
	}

/* count asterisks in destination pattern */
	for(c = 0; *dpat; dpat++)
		if(*dpat == '*')
			c++;

	if(a && (b == c))	// try to build destination filename
	{
		spat = sname;
		dpat = dname;
		sn = name;

		while(*spat || *dpat || *sn)
		{
			// skip to wildcard part of filename
			if(*spat)
			{
				for(; *spat && (*spat != '*'); spat++, sn++);
				if(*spat)
					spat++;
			}
			// copy replacement text
			if(*dpat)
			{
				for(; *dpat && (*dpat != '*'); *dn++ = *dpat++);
				if(*dpat)
					dpat++;
			}
			// copy wildcard part of source filename
			if(*sn)
				for(c1 = tolower(*spat); c1 != tolower(*sn); sn++)
					if(*spat != '*')
						*dn++ = *sn;
			*dn = 0;
		}
		return 1;
	}
	return (0);
}

void filloutcopydata(struct Directory *dir)
{
	dos_copy_date.ds_Days = dir->date.ds_Days;
	dos_copy_date.ds_Minute = dir->date.ds_Minute;
	dos_copy_date.ds_Tick = dir->date.ds_Tick;
	dos_copy_protection = dir->protection;
	if(dir->comment)
		strcpy(dos_copy_comment, dir->comment);
	else
		dos_copy_comment[0] = 0;
}

void filloutcopydatafile(STRPTR fil)
{
	struct FileInfoBlock *fileinfo = IDOS->AllocDosObject(DOS_FIB, NULL);

	if(lockandexamine(fil, fileinfo))
	{
		dos_copy_date.ds_Days = fileinfo->fib_Date.ds_Days;
		dos_copy_date.ds_Minute = fileinfo->fib_Date.ds_Minute;
		dos_copy_date.ds_Tick = fileinfo->fib_Date.ds_Tick;
		dos_copy_protection = fileinfo->fib_Protection;
		strcpy(dos_copy_comment, fileinfo->fib_Comment);
	}

	IDOS->FreeDosObject(DOS_FIB, NULL);
}

void update_buffer_stamp(int win, int true)
{
	struct FileInfoBlock *fib = IDOS->AllocDosObject(DOS_FIB, NULL);
	char dirbuf[256];
	struct DirectoryWindow *dirwin;

	if(win == -1 || !(config->dirflags & DIRFLAGS_REREADOLD))
		return;

	dirwin = dopus_curwin[win];
	strcpy(dirbuf, str_pathbuffer[win]);
	FOREVER
	{
		if(true)
		{
			if(lockandexamine(dirbuf, fib))
				copy_datestamp(&fib->fib_Date, &dirwin->dirstamp);
			if(!(doparent(dirbuf)) || !(dirwin = findbuffer(dirbuf, win, 0, 1)))
				break;
		}
		else
		{
			if(!(dirwin = dirwin->next) || dirwin == dopus_curwin[win])
				break;
			if(strncmp(dirwin->directory, dirbuf, strlen(dirbuf)) == 0)
			{
				dirwin->dirstamp.ds_Days = 0;
				dirwin->dirstamp.ds_Minute = 0;
				dirwin->dirstamp.ds_Tick = 0;
			}
		}
	}

	IDOS->FreeDosObject(DOS_FIB, fib);
}

int check_key_press(struct dopusfunction *func, USHORT code, USHORT qual)
{
	if(!func->function || !func->function[0] || (func->key == 0xff && func->qual == 0) || func->qual != qual)
		return (0);
	if(func->key == 0xff || func->key == code)
		return (1);
	return (0);
}
