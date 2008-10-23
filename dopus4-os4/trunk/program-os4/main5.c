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

int copyfile(STRPTR src, STRPTR dst, int *err, STRPTR password, int encryptstate)
{
	struct ExamineData *data = NULL;
	char *buffer = NULL;
	int ret = 0;
	int32 length;
	int64 readsize, size_read, size_write, size_total, size, buffer_size;
	int prog = (config->dynamicflags & UPDATE_PROGRESSIND_COPY);
	BPTR out, inhandle = 0, outhandle = 0;
	struct DateStamp ds, *dsp;

	if(password)
	{
		int a, encrypt = 1;

		for(a = 0; password[a]; a++)
		{
			encrypt *= password[a];
		}
		srand(encrypt);
	}

	if((data = IDOS->ExamineObjectTags(EX_StringName, src, TAG_END)) == NULL)
	{
		*err = IDOS->IoErr();
		return (0);
	}

	if(data->FileSize == 0)
	{
		if(!(out = IDOS->Open(dst, MODE_NEWFILE)))
			goto failed;
		IDOS->Close(out);
		if(config->copyflags & COPY_DATE)
			IDOS->SetFileDate(dst, &(data->Date));
		if(config->copyflags & COPY_PROT)
			IDOS->SetProtection(dst, data->Protection & ((config->copyflags & COPY_COPYARC) ? ~0 : ~FIBF_ARCHIVE));
		if(config->copyflags & COPY_NOTE)
			IDOS->SetComment(dst, data->Comment);
		IDOS->FreeDosObject(DOS_EXAMINEDATA, data);
		return (1);
	}

	dotaskmsg(hotkeymsg_port, PROGRESS_UPDATE, 0, 100, NULL, 1);

	if(!(inhandle = IDOS->Open(src, MODE_OLDFILE)))
		goto failed;

	if(!(outhandle = IDOS->Open(dst, MODE_NEWFILE)))
		goto failed;

	size = data->FileSize;
	buffer_size = 64 *1024;
	if(buffer_size > (10 * 1024 * 1024))
	{
		buffer_size = 10 * 1024 * 1024;
	}

	while(buffer_size > 0)
	{
		if((buffer = IExec->AllocVec(buffer_size, MEMF_ANY)))
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
		length = IDOS->Read(inhandle, buffer, readsize);

		size -= readsize;
//		size_read += length;
		size_read = size_read + (int64)length;

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
			if((IDOS->Write(outhandle, buffer, length)) == -1)
				goto failed;
		}
//		size_write += length;
		size_write = size_write + (int64)length;

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

	IDOS->Close(inhandle);
	IDOS->Close(outhandle);


	IExec->FreeVec(buffer);

	if(config->copyflags & COPY_DATE)
	{
		IDOS->SetFileDate(dst, &(data->Date));
		dsp = &data->Date;
	}
	else
	{
		IDOS->DateStamp(&ds);
		dsp = &ds;
	}
	copy_datestamp(dsp, &dos_copy_date);

	if(config->copyflags & COPY_PROT)
	{
		IDOS->SetProtection(dst, data->Protection & ((config->copyflags & COPY_COPYARC) ? ~0 : ~FIBF_ARCHIVE));
	}
	dos_copy_protection = data->Protection;

	if(config->copyflags & COPY_NOTE)
	{
		IDOS->SetComment(dst, data->Comment);
		strcpy(dos_copy_comment, data->Comment);
	}
	else
	{
		dos_copy_comment[0] = 0;
	}
	IDOS->FreeDosObject(DOS_EXAMINEDATA, data);

	return (1);

      failed:
	*err = IDOS->IoErr();
	if(buffer)
	{
		IExec->FreeVec(buffer);
	}
	if(inhandle)
	{
		IDOS->Close(inhandle);
	}
	if(outhandle)
	{
		IDOS->Close(outhandle);
		IDOS->DeleteFile(dst);
	}

	if(data)
	{
		IDOS->FreeDosObject(DOS_EXAMINEDATA, data);
	}

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

struct Directory *findfile(struct DirectoryWindow *dir, STRPTR name, int *count)
{
	struct Directory *find;

	if(dir)
	{
		if(str_arcorgname[0]) /* required for double-click */
		{
			name = str_arcorgname;
		}

		find = dir->firstentry;
		if(count)
			*count = 0;
		while(find)
		{
			if(find->name && !(IUtility->Stricmp(name, find->name)))
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

      loop:
	if((suc = IDOS->DeleteFile(name)) == 0)
	{
		if((err = IDOS->IoErr()) == ERROR_OBJECT_NOT_FOUND)
		{
			suc = 1;
		}
		else
		{
			if(err == ERROR_DIRECTORY_NOT_EMPTY)
			{
				return (-2);
			}
			else if(err == ERROR_DELETE_PROTECTED && try == 0)
			{
				if(!(config->deleteflags & DELETE_SET))
				{
					if(!unprotect)
					{
						char textformat[400], gadformat[100], errortext[100];

						doerror(ERROR_DELETE_PROTECTED);
						geterrorstring(errortext, ERROR_DELETE_PROTECTED);
						sprintf(textformat, globstring[STR_ERROR_OCCURED], globstring[STR_DELETING], nam, errortext);
						strcat(textformat, globstring[STR_SELECT_UNPROTECT]);
						sprintf(gadformat, "%s|%s|%s|%s", globstring[STR_UNPROTECT], globstring[STR_UNPROTECT_ALL], globstring[STR_SKIP], globstring[STR_ABORT]);
						if(!(a = ra_simplerequest(textformat, gadformat, REQIMAGE_WARNING)))
						{
							return (-1);
						}
						if(a == 3)
						{
							return (0);
						}
						if(a == 2)
						{
							glob_unprotect_all = 1;
							recplus = 1;
						}
					}
				}
				try = 1;
				IDOS->SetProtection(name, 0);
				goto loop;
			}
			else if(!errcheck)
			{
				return (-2);
			}
			doerror(err);
			if((a = checkerror(errs, nam, err)) == 3)
			{
				return (-1);
			}
			if(a == 1)
			{
				goto loop;
			}
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
	int a, b, c;

	char *spat = sname, *dpat = dname, *sn = name, *dn = newn;
	char c1, c2;

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
			{
				for(c1 = tolower(*spat); c1 != tolower(*sn); sn++);
			}
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
	{
		if(*dpat == '*')
		{
			c++;
		}
	}

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
				{
					spat++;
				}
			}
			// copy replacement text
			if(*dpat)
			{
				for(; *dpat && (*dpat != '*'); *dn++ = *dpat++);
				if(*dpat)
				{
					dpat++;
				}
			}
			// copy wildcard part of source filename
			if(*sn)
			{
				for(c1 = tolower(*spat); c1 != tolower(*sn); sn++)
				{
					if(*spat != '*')
					{
						*dn++ = *sn;
					}
				}
			}
			*dn = 0;
		}
		return 1;
	}
	return (0);
}

void update_buffer_stamp(int win, int true)
{
	char dirbuf[2048];
	struct DirectoryWindow *dirwin;

	if(win == -1 || !(config->dirflags & DIRFLAGS_REREADOLD))
	{
		return;
	}

	dirwin = dopus_curwin[win];
	strcpy(dirbuf, str_pathbuffer[win]);
	for(;;)
	{
		if(true)
		{
			struct ExamineData *data = NULL;

			if((data = IDOS->ExamineObjectTags(EX_StringName, dirbuf, TAG_END)))
			{
				copy_datestamp(&data->Date, &dirwin->dirstamp);
				IDOS->FreeDosObject(DOS_EXAMINEDATA, data);
			}
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
}

int check_key_press(struct dopusfunction *func, USHORT code, USHORT qual)
{
	if(!func->function || !func->function[0] || (func->key == 0xff && func->qual == 0) || func->qual != qual)
	{
		return (0);
	}
	if(func->key == 0xff || func->key == code)
	{
		return (1);
	}
	return (0);
}
