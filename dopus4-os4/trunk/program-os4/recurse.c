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

int32 recursive_delete(STRPTR directory)
{
	uint32 a, rd_continue = 0, rd_abort = 0;
	APTR context = IDOS->ObtainDirContextTags(EX_StringName, directory, EX_DoCurrentDir, TRUE, TAG_END);
	int32 ret = 0, errorcode = 0;
	char buf[300], buf2[100];
	STRPTR nodename;

	if(context)
	{
		struct ExamineData *dat;
		while((dat = IDOS->ExamineDir(context)))
		{
			if(status_haveaborted)
			{
				myabort();
				ret = -10;
				break;
			}

			do
			{
				dofilename(dat->Name);

				nodename = IExec->AllocVec(strlen(directory) + strlen(dat->Name) + 2, MEMF_ANY);
				strncpy(nodename, directory, strlen(directory) + 1);
				IDOS->AddPart(nodename, dat->Name, strlen(directory) + strlen(dat->Name) + 2);

				if(EXD_IS_DIRECTORY(dat))
				{
					recursive_delete(nodename);
				}
				else if(EXD_IS_FILE(dat))
				{
				}

				if(!str_filter_parsed[0] || IDOS->MatchPatternNoCase(str_filter_parsed, dat->Name))
				{
					if(IDOS->DeleteFile(nodename))
					{
						if(EXD_IS_FILE(dat))
						{
							dos_global_deletedbytes += dat->FileSize;
						}
						rd_continue = 0;
					}
					else
					{
						if((errorcode = IDOS->IoErr()) == ERROR_OBJECT_NOT_FOUND)
						{
						}
						else if(errorcode == ERROR_DELETE_PROTECTED)
						{
							if((config->deleteflags & DELETE_SET) || (glob_unprotect_all == 1))
							{
								IDOS->SetProtection(dat->Name, 0);
								IDOS->DeleteFile(dat->Name);
								if(EXD_IS_FILE(dat))
								{
									dos_global_deletedbytes += dat->FileSize;
								}
								rd_continue = 0;
							}
							else
							{
								doerror(ERROR_DELETE_PROTECTED);
								geterrorstring(buf2, ERROR_DELETE_PROTECTED);
								snprintf(buf, 300, globstring[STR_ERROR_OCCURED], globstring[STR_DELETING], dat->Name, buf2);
								strncat(buf, globstring[STR_SELECT_UNPROTECT], 300);
								snprintf(buf2, 100, "%s|%s|%s|%s", globstring[STR_UNPROTECT], globstring[STR_UNPROTECT_ALL], globstring[STR_SKIP], globstring[STR_ABORT]);
								if((a = ra_simplerequest(buf, buf2, REQIMAGE_WARNING)) == 1) // Unprotect
								{
									rd_continue = 1;
									IDOS->SetProtection(dat->Name, 0);
								}
								else if(a == 2) // Unprotect All
								{
									rd_continue = 1;
									IDOS->SetProtection(dat->Name, 0);
									glob_unprotect_all = 1;
								}
								else if(a == 3) // Skip
								{
									rd_continue = 0;
								}
								else if(a == 0) // Cancel
								{
									rd_continue = 0;
									rd_abort = 1;
								}
							}
						}
						else
						{
							doerror((a = IDOS->IoErr()));
							if((a = checkerror(globstring[STR_DELETING], dat->Name, a)) == 3)
							{
								rd_continue = 0;
							}
							else if(a == 2)
							{
								rd_continue = 0;
							}
							else if(a == 1)
							{
								rd_continue = 1;
							}
							else if(a == 0)
							{
								rd_abort = 1;
								rd_continue = 0;
							}
						}
					}
				}
			}
			while(rd_continue == 1);

			if(rd_abort == 1)
			{
				myabort();
				ret = -10;
				break;
			}

			rd_continue = 0;
		}
		if(ERROR_NO_MORE_ENTRIES == IDOS->IoErr())
		{
			ret = 0;
		}
		else
		{
		}
	}
	else
	{
	}

	IDOS->ReleaseDirContext(context);

	return (ret);
}

int32 recursive_getbytes(STRPTR sourcename, int32 blocksize, int32 ignore)
{
	int32 ret = 0, blocks;
	struct ExamineData *data;
	APTR context;

	if(ignore == 0)
	{
		dos_global_bytecount = 0;
		dos_global_blocksneeded = 0;
		dos_global_files = 0;
	}

	if((context = IDOS->ObtainDirContextTags(EX_StringName, sourcename, EX_DoCurrentDir, TRUE, TAG_END)))
	{
		while((data = IDOS->ExamineDir(context)))
		{
			if(status_haveaborted)
			{
				myabort();
				ret = -10;
				break;
			}

			if(EXD_IS_DIRECTORY(data))
			{
				++dos_global_blocksneeded;
				ret = recursive_getbytes(data->Name, blocksize, 1);
			}
			if(EXD_IS_FILE(data))
			{
				dos_global_bytecount += data->FileSize;
				dos_global_files++;
				if(blocksize)
				{
					blocks = (data->FileSize + (blocksize - 1)) / blocksize;
					dos_global_blocksneeded += blocks + (blocks / 72) + 1;
				}
			}
			if(ret == -10)
			{
				myabort();
				break;
			}
		}
		IDOS->ReleaseDirContext(context);
	}
	return ret;
}

int32 huntfunc(struct Hook *hook, STRPTR matchstring, struct ExamineData *data)
{
	int32 ret = TRUE;

	if(matchstring)
	{
		if(EXD_IS_FILE(data))
		{
			ret = IDOS->MatchPatternNoCase(matchstring, data->Name);
		}
	}

	return ret;
}

int32 recursive_hunt(STRPTR sourcename)
{
	int32 ret = 0, y;
	struct ExamineData *data;
	APTR context;
	STRPTR newsourcename;
	struct Hook *hunthook;

	if((hunthook = IExec->AllocSysObjectTags(ASOT_HOOK, ASOHOOK_Entry, huntfunc, TAG_END)))
	{
		if((context = IDOS->ObtainDirContextTags(EX_StringName, sourcename, EX_DoCurrentDir, TRUE, EX_MatchString, str_hunt_name_parsed, EX_MatchFunc, hunthook, TAG_END)))
		{
			while((data = IDOS->ExamineDir(context)))
			{
				if(status_haveaborted)
				{
					myabort();
					ret = -10;
					break;
				}

				if(EXD_IS_DIRECTORY(data))
				{
					if((newsourcename = IExec->AllocVec(strlen(sourcename) + strlen(data->Name) + 2, MEMF_ANY)))
					{
						strncpy(newsourcename, sourcename, strlen(sourcename) + strlen(data->Name) + 2);
						IDOS->AddPart(newsourcename, data->Name, strlen(sourcename) + strlen(data->Name) + 2);
						y = recursive_hunt(newsourcename);
						if(y > 0)
						{
							ret += y;
						}
						else if(y == -10)
						{
							ret = -10;
						}
						IExec->FreeVec(newsourcename);
					}
					else
					{
						ret = -10;
					}
				}
				if(EXD_IS_FILE(data))
				{
					STRPTR textfmt;
					char gadfmt[100];
					int x, len = strlen(globstring[STR_FOUND_A_MATCH]) + strlen(sourcename) + strlen(data->Name) + 4;

					if((textfmt = IExec->AllocVec(len, MEMF_ANY)))
					{
						snprintf(textfmt, len, globstring[STR_FOUND_A_MATCH], data->Name, sourcename);
						snprintf(gadfmt, 100, "%s|%s|%s", globstring[STR_OKAY], globstring[STR_SKIP], globstring[STR_ABORT]);
						if((x = ra_simplerequest(textfmt, gadfmt, REQIMAGE_INFO)) == 1)
						{
							if(!status_iconified)
							{
								unbusy();
								advancebuf(data_active_window, 1);
								strncpy(str_pathbuffer[data_active_window], sourcename, 256);
								startgetdir(data_active_window, 3);
								busy();
							}
							ret = -3;
						}
						else if(x == 2)
						{
							ret++; // = 0;
						}
						else if(x == 0)
						{
							ret = -10;
						}
						IExec->FreeVec(textfmt);
					}
					else
					{
						ret = -3;
					}
				}

				if(ret == -3)
				{
					break;
				}
				if(ret == -10)
				{
					status_haveaborted = 1;
					myabort();
					break;
				}
			}
			IDOS->ReleaseDirContext(context);
		}
		IExec->FreeSysObject(ASOT_HOOK, hunthook);
	}

	return ret;
}
