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

#define USE_INLINE_STDARG

#include "dopus.h"
#include <proto/xadmaster.h>

#define EXALL_NUM 2

int readarchive(struct DirectoryWindow *dir, int win)
{
	BPTR lock;
	long len, i;
	struct xadFileInfo *xfi;
	struct DateStamp ds;
	char arcname[256] = { 0, }, arcdir[256] = { 0, }, buf[FILEBUF_SIZE] = { 0, }, *c = NULL;
	int type;

	strcpy(arcdir, "");
	strcpy(arcname, dir->directory);
	if((len = strlen(arcname)))
	{
		if(arcname[len - 1] == '/')
		{
			arcname[len - 1] = 0;
		}
	}
	while(!(lock = Lock(arcname, ACCESS_READ)))
	{
		c = FilePart(arcname);
		if(c == arcname)
		{
			return 0;
		}
		if(c > arcname)
		{
			*(c - 1) = 0;
		}
		strcpy(arcdir, dir->directory + (ULONG) c - (ULONG) arcname);
	}
	len = strlen(arcdir);
	UnLock(lock);
	if(!dir->xai)
	{
		dostatustext(globstring[STR_OPENING_ARCHIVE]);
		if((dir->arcname = AllocVec(strlen(arcname) + 1, MEMF_ANY)))
		{
			strcpy(dir->arcname, arcname);
			if((dir->xai = xadAllocObjectA(XADOBJ_ARCHIVEINFO, NULL)))
			{
				struct TagItem ti[2] = { { XAD_INFILENAME, (Tag)arcname} , { TAG_END, 0 } };

				if(!(xadGetInfoA(dir->xai, ti)))
				{
					if(dir->xai->xai_DiskInfo)
					{
						struct xadArchiveInfo *xai2;
						int err;

						if((xai2 = xadAllocObjectA(XADOBJ_ARCHIVEINFO, NULL)))
						{
							struct TagItem ti2[2];

							ti2[0].ti_Tag = XAD_ENTRYNUMBER;
							ti2[0].ti_Data = dir->xai->xai_DiskInfo->xdi_EntryNumber;
							ti2[1].ti_Tag = TAG_MORE;
							ti2[1].ti_Data = (ULONG) ti;

							if(!(err = xadGetDiskInfo(xai2, XAD_NOEMPTYERROR, 1, XAD_INDISKARCHIVE, (ULONG) ti2, TAG_DONE)))
							{
								if(xai2->xai_FileInfo)
								{
									xadFreeInfo(dir->xai);
									xadFreeObjectA(dir->xai, NULL);
									dir->xai = xai2;
								}
								else
									xadFreeInfo(xai2);
							}
							xadFreeObjectA(xai2, NULL);
						}
					}
					dir->flags |= DWF_ARCHIVE;
				}
				else
				{
					xadFreeObjectA(dir->xai, NULL);
					dir->xai = NULL;
					FreeVec(dir->arcname);
					dir->arcname = NULL;
					return (0);
				}
			}
			else
			{
				FreeVec(dir->arcname);
				dir->arcname = NULL;
				return (0);
			}
		}
		else
			return (0);
	}

	if((xfi = dir->xai->xai_FileInfo))
	{
		while(xfi)
		{
			UBYTE c1, c2;
			c = xfi->xfi_FileName;
			i = 0;
			do
			{
				c1 = tolower(arcdir[i]);
				c2 = tolower(xfi->xfi_FileName[i]);

				if(c1 == c2)
				{
					c++;
					i++;
				}
			}
			while(c1 == c2);

			if(i >= len)
			{
				for(i = 0; *c && (*c != '/') && (i < (FILEBUF_SIZE - 1)); i++, c++)
				{
					buf[i] = *c;
				}
				buf[i] = 0;

				if(!(findfile(dir, buf, NULL)))
				{
					if(*c == '/')
					{
						type = ST_USERDIR;
					}
					else
					{
						type = ST_FILE;
					}

					if((xfi->xfi_Flags & XADFIF_DIRECTORY) == XADFIF_DIRECTORY)
					{
						type = ST_USERDIR;
					}

					xadConvertDates(XAD_DATEXADDATE, (Tag) & xfi->xfi_Date, XAD_GETDATEDATESTAMP, (Tag) & ds, TAG_END);

					addfile(dir, win, buf, (type > 0) ? -1LL : xfi->xfi_Size, type, &ds, xfi->xfi_Comment, xfi->xfi_Protection, 0, FALSE, NULL, NULL, xfi->xfi_OwnerUID, xfi->xfi_OwnerGID);
				}
			}
			xfi = xfi->xfi_Next;
		}
		dostatustext(globstring[STR_OKAY_TITLE]);
		return (1);
	}
	return (0);
}

void freearchive(struct DirectoryWindow *dir)
{
	if(dir->xai)
	{
		xadFreeInfo(dir->xai);
		FreeVec(dir->arcname);
		dir->arcname = NULL;
		xadFreeObject(dir->xai, TAG_END);
		dir->xai = NULL;
		dir->arcpassword[0] = 0;
	}
}

BOOL getsourcefromarc(struct DirectoryWindow *dir, char *buf, char *file)
{
	if(dir && (dir->flags & DWF_ARCHIVE))
	{
		char srcdir[256], tempname[FILEBUF_SIZE];

		strcpy(srcdir, "T:");
		if(unarcfiledir(dir, srcdir, tempname, file))
		{
			AddPart(srcdir, tempname, 256);
			strcpy(buf, srcdir);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL unarcfiledir(const struct DirectoryWindow * dir, const char *path, char *namebuf, const char *file)
{
	if(dir->xai)
	{
		if(dir->arcname)
		{
			struct xadFileInfo *xfi;
			char arcname[256], arcdir[256], *c;
			int err = XADERR_UNKNOWN;

			if(dir->xai->xai_Flags & XADAIF_CRYPTED)
			{
				if(dir->arcpassword[0] == 0)
				{
					whatsit(globstring[STR_ENTER_PASSWORD], 32, dir->arcpassword, NULL);
				}
			}

			strcpy(arcname, dir->arcname);
			c = strstr(dir->directory, FilePart(arcname));
			if(c)
			{
				for(; c && (*c != '/'); c++);
			}
			if(c)
			{
				c++;
			}
			strcpy(arcdir, c ? c : "");

			AddPart(arcdir, file, 256);
			strcpy(namebuf, "dopustmp");
			sprintf(arcname, "%04lx", GetUniqueID());
			strcat(namebuf, arcname);
			c = strchr(file, '.');
			if(c)
			{
				strcat(namebuf, c);
			}
			strcpy(arcname, path);
			strcat(arcname, namebuf);
			for(xfi = dir->xai->xai_FileInfo; xfi; xfi = xfi->xfi_Next)
			{
				if(strcmp(xfi->xfi_FileName, arcdir) == 0)
				{
					break;
				}
			}

			if(xfi)
			{
				while (err != XADERR_OK)
				{
					err = xadFileUnArc(dir->xai, XAD_ENTRYNUMBER, xfi->xfi_EntryNumber, XAD_OUTFILENAME, (ULONG) arcname, dir->arcpassword[0] ? XAD_PASSWORD : TAG_IGNORE, (IPTR)dir->arcpassword, TAG_END);
					switch (err)
					{
					case XADERR_OK:
						{
							struct DateStamp ds;

							xadConvertDates(XAD_DATEXADDATE, (Tag) & xfi->xfi_Date, XAD_GETDATEDATESTAMP, (Tag) &ds, TAG_END);
							SetFileDate(arcname, &ds);
							SetProtection(arcname, xfi->xfi_Protection);
							if(xfi->xfi_Comment)
							{
								SetComment(arcname, xfi->xfi_Comment);
							}

							strcpy(str_arcorgname, file);
							return TRUE;
						}
					case XADERR_PASSWORD:
						if(!(whatsit(globstring[STR_ENTER_PASSWORD], 32, dir->arcpassword, NULL)))
							return FALSE;
						break;
					default:
						err = XADERR_OK;
					}
				}
			}
		}
	}
	return FALSE;
}

void removetemparcfile(const char *name)
{
	if(str_arcorgname[0])
	{
		DeleteFile(name);
		str_arcorgname[0] = 0;
	}
}

void arcfillfib(struct FileInfoBlock *fib, struct Directory *entry)
{
	if(entry == NULL)
		return;
	fib->fib_DirEntryType = entry->type;
	strcpy(fib->fib_FileName, entry->name);
	fib->fib_Protection = entry->protection;
	fib->fib_Size64 = (int)entry->size;
	fib->fib_Date = entry->date;
	if(entry->comment)
		strcpy(fib->fib_Comment, entry->comment);
	else
		fib->fib_Comment[0] = 0;
	fib->fib_OwnerUID = entry->owner_id;
	fib->fib_OwnerGID = entry->group_id;
}
