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
	while(!(lock = IDOS->Lock(arcname, ACCESS_READ)))
	{
		c = IDOS->FilePart(arcname);
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
	IDOS->UnLock(lock);
	if(!dir->xai)
	{
		dostatustext(globstring[STR_OPENING_ARCHIVE]);
		if((dir->arcname = doAllocVec(strlen(arcname) + 1, MEMF_ANY)))
		{
			strcpy(dir->arcname, arcname);
			if((dir->xai = IXadMaster->xadAllocObjectA(XADOBJ_ARCHIVEINFO, NULL)))
			{
				struct TagItem ti[2] = { { XAD_INFILENAME, (Tag)arcname} , { TAG_END, 0 } };

				if(!(IXadMaster->xadGetInfoA(dir->xai, ti)))
				{
					if(dir->xai->xai_DiskInfo)
					{
						struct xadArchiveInfo *xai2;
						int err;

						if((xai2 = IXadMaster->xadAllocObjectA(XADOBJ_ARCHIVEINFO, NULL)))
						{
							struct TagItem ti2[2];

							ti2[0].ti_Tag = XAD_ENTRYNUMBER;
							ti2[0].ti_Data = dir->xai->xai_DiskInfo->xdi_EntryNumber;
							ti2[1].ti_Tag = TAG_MORE;
							ti2[1].ti_Data = (ULONG) ti;

							if(!(err = IXadMaster->xadGetDiskInfo(xai2, XAD_NOEMPTYERROR, 1, XAD_INDISKARCHIVE, (ULONG) ti2, TAG_DONE)))
							{
								if(xai2->xai_FileInfo)
								{
									IXadMaster->xadFreeInfo(dir->xai);
									IXadMaster->xadFreeObjectA(dir->xai, NULL);
									dir->xai = xai2;
								}
								else
									IXadMaster->xadFreeInfo(xai2);
							}
							IXadMaster->xadFreeObjectA(xai2, NULL);
						}
					}
					dir->flags |= DWF_ARCHIVE;
				}
				else
				{
					IXadMaster->xadFreeObjectA(dir->xai, NULL);
					dir->xai = NULL;
					IExec->FreeVec(dir->arcname);
					dir->arcname = NULL;
					return (0);
				}
			}
			else
			{
				IExec->FreeVec(dir->arcname);
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
						type = ENTRY_DIRECTORY;
					}
					else
					{
						type = ENTRY_FILE;
					}

					if((xfi->xfi_Flags & XADFIF_DIRECTORY) == XADFIF_DIRECTORY)
					{
						type = ENTRY_DIRECTORY;
					}

					IXadMaster->xadConvertDates(XAD_DATEXADDATE, (Tag) & xfi->xfi_Date, XAD_GETDATEDATESTAMP, (Tag) & ds, TAG_END);

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
		IXadMaster->xadFreeInfo(dir->xai);
		if(dir->arcname)
			IExec->FreeVec(dir->arcname);
		dir->arcname = NULL;
		IXadMaster->xadFreeObject(dir->xai, TAG_END);
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
			IDOS->AddPart(srcdir, tempname, 256);
			strcpy(buf, srcdir);
			return TRUE;
		}
	}
	return FALSE;
}

//BOOL unarcfiledir(const struct DirectoryWindow * dir, const char *path, char *namebuf, const char *file)
BOOL unarcfiledir(struct DirectoryWindow * dir, const char *path, char *namebuf, const char *file)
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
			c = strstr(dir->directory, IDOS->FilePart(arcname));
			if(c)
			{
				for(; c && (*c != '/'); c++);
			}
			if(c)
			{
				c++;
			}
			strcpy(arcdir, c ? c : "");

			IDOS->AddPart(arcdir, file, 256);
			strcpy(namebuf, "dopustmp");
			sprintf(arcname, "%04lx", IUtility->GetUniqueID());
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
					err = IXadMaster->xadFileUnArc(dir->xai, XAD_ENTRYNUMBER, xfi->xfi_EntryNumber, XAD_OUTFILENAME, (ULONG) arcname, dir->arcpassword[0] ? XAD_PASSWORD : TAG_IGNORE, dir->arcpassword, TAG_END);
					switch (err)
					{
					case XADERR_OK:
						{
							struct DateStamp ds;

							IXadMaster->xadConvertDates(XAD_DATEXADDATE, (Tag) & xfi->xfi_Date, XAD_GETDATEDATESTAMP, (Tag) &ds, TAG_END);
							IDOS->SetDate(arcname, &ds);
							IDOS->SetProtection(arcname, xfi->xfi_Protection);
							if(xfi->xfi_Comment)
							{
								IDOS->SetComment(arcname, xfi->xfi_Comment);
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
		IDOS->Delete(name);
		str_arcorgname[0] = 0;
	}
}

void arcfillfib(struct FileInfoBlock *fib, struct Directory *entry)
{
	if(entry == NULL)
		return;
	fib->fib_DirEntryType = entry->type;
	strncpy(fib->fib_FileName, entry->name, 108);
	fib->fib_Protection = entry->protection;
	fib->fib_Size = (int)entry->size;
	fib->fib_Date = entry->date;
	if(entry->comment)
	{
		strncpy(fib->fib_Comment, entry->comment, 80);
	}
	else
	{
		fib->fib_Comment[0] = 0;
	}
}

/* Progress Hook */

uint32 ProgressFunc(struct Hook *hook, APTR *Obj, struct xadProgressInfo *xadp)
{
	int32 a;

	if(xadp && xadp->xpi_FileInfo && (config->dynamicflags & UPDATE_PROGRESSIND_COPY))
	{
		arbiter_command(ARBITER_PROGRESS_UPDATE, 0, 0, xadp->xpi_CurrentSize, xadp->xpi_FileInfo->xfi_Size, IDOS->FilePart(xadp->xpi_FileInfo->xfi_FileName), 0);
	}

	switch(xadp->xpi_Mode)
	{
	case XADPMODE_ERROR:
		IDOS->Printf("XAD ERROR!\n");
		break;
	case XADPMODE_END:
		return XADPIF_OK;
		break;
	case XADPMODE_ASK:
		if(xadskipall == 1)
		{
			return XADPIF_OK;
		}
		if(xadoverwrite == 0)
		{
			a = ra_simplerequest(formatstring, gadgetstring, REQIMAGE_WARNING);
			switch(a)
			{
			case 0:
				status_haveaborted = 1;
				return XADPIF_OK;
				break;
			case 1:
				return (XADPIF_OK|XADPIF_OVERWRITE);
				break;
			case 2:
				xadoverwrite = 1;
				return (XADPIF_OK|XADPIF_OVERWRITE);
				break;
			case 3:
				return XADPIF_OK;
				break;
			case 4:
				xadskipall = 1;
				return XADPIF_OK;
				break;
			}
		}

		return (XADPIF_OK|XADPIF_OVERWRITE);
		break;
	}

	return(XADPIF_OK);
}

uint32 extractarchive(char *archivename, char *source, char *destination)
{
	struct Hook *ProgressHook;
	int32 xad_result;
	uint32 retval = 0;
	char sourcename[2048] = { 0, }, destname[2048] = { 0, };

	struct xadArchiveInfo *xadai = NULL;
	struct xadFileInfo *xadfi = NULL;

	xadoverwrite = 0;
	xadskipall = 0;

	snprintf(gadgetstring, 100, "%s|%s|%s|%s|%s", globstring[STR_REPLACE], globstring[STR_REPLACE_ALL], globstring[STR_SKIP], globstring[STR_SKIP_ALL], globstring[STR_CANCEL]);
	snprintf(sourcename, 2048, "%s%s", source, archivename);

	if((ProgressHook = IExec->AllocSysObjectTags(ASOT_HOOK, ASOHOOK_Entry, ProgressFunc, TAG_END)))
	{
		if((xadai = IXadMaster->xadAllocObject(XADOBJ_ARCHIVEINFO, TAG_END)))
		{
			if((xad_result = IXadMaster->xadGetInfo(xadai, XAD_INFILENAME, (uint32)sourcename, TAG_END )) == 0L)
			{
				xadfi = xadai->xai_FileInfo;

				while(xadfi != NULL)
				{
					if(status_haveaborted)
					{
						IXadMaster->xadFreeInfo(xadai);
						IXadMaster->xadFreeObject(xadai, TAG_END);
						IExec->FreeSysObject(ASOT_HOOK, ProgressHook);
						return 0;
					}
					else if(xadfi->xfi_Flags != XADFIF_DIRECTORY)
					{
						memset(destname, 0, 2048);
						snprintf(destname, 2048, "%s%s", destination, xadfi->xfi_FileName);
						if((config->dynamicflags & UPDATE_PROGRESSIND_COPY))
						{
							arbiter_command(ARBITER_PROGRESS_UPDATE, 0, 0, 0, 100, IDOS->FilePart(xadfi->xfi_FileName), 0);
						}
						snprintf(formatstring, 1024, globstring[STR_FILE_EXISTS_REPLACE], xadfi->xfi_FileName);
						if((xad_result = IXadMaster->xadFileUnArc(xadai, XAD_ENTRYNUMBER, xadfi->xfi_EntryNumber, XAD_OUTFILENAME, (uint32)destname, XAD_MAKEDIRECTORY, TRUE, XAD_OVERWRITE, xadoverwrite, XAD_PROGRESSHOOK, ProgressHook, TAG_END)) == 0L)
						{
							struct DateStamp d;

							if((xad_result = IXadMaster->xadConvertDates(XAD_DATEXADDATE, &xadfi->xfi_Date, XAD_GETDATEDATESTAMP, &d, TAG_DONE)) == 0L)
							{
								IDOS->SetDate(destname, &d);
							}

							if(xadfi->xfi_Comment)
							{
								IDOS->SetComment(destname, xadfi->xfi_Comment);
							}

							IDOS->SetProtection(destname, xadfi->xfi_Protection);
						}
					}
					xadfi = xadfi->xfi_Next;
				}
				retval = 1;
				IXadMaster->xadFreeInfo(xadai);
			}
			IXadMaster->xadFreeObject(xadai, TAG_END);
		}
		IExec->FreeSysObject(ASOT_HOOK, ProgressHook);
	}

	return retval;
}

