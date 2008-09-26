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

uint32 recursive_delete(STRPTR directory)
{
	uint32 ret = 0, a, rd_continue = 0, rd_abort = 0;
	APTR context = IDOS->ObtainDirContextTags(EX_StringName, directory, EX_DoCurrentDir, TRUE, TAG_END);
	int32 errorcode = 0;
	char buf[300], buf2[100];
	struct Node *node;
	struct List *flist, *dlist;

	flist = IExec->AllocSysObjectTags(ASOT_LIST, TAG_END);
	dlist = IExec->AllocSysObjectTags(ASOT_LIST, TAG_END);

	if(context && flist && dlist)
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

				if(EXD_IS_DIRECTORY(dat))
				{
					recursive_delete(dat->Name);
				}

				if(IDOS->DeleteFile(dat->Name))
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
							dos_global_deletedbytes += dat->FileSize;
							rd_continue = 0;
						}
						else
						{
							doerror(ERROR_DELETE_PROTECTED);
							geterrorstring(buf2, ERROR_DELETE_PROTECTED);
							sprintf(buf, globstring[STR_ERROR_OCCURED], globstring[STR_DELETING], dat->Name, buf2);
							strcat(buf, globstring[STR_SELECT_UNPROTECT]);
							sprintf(buf2, "%s|%s|%s|%s", globstring[STR_UNPROTECT], globstring[STR_UNPROTECT_ALL], globstring[STR_SKIP], globstring[STR_ABORT]);
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

	IExec->FreeSysObject(ASOT_LIST, flist);
	IExec->FreeSysObject(ASOT_LIST, dlist);

	return (ret);
}

