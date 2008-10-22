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

void do_path_completion(int win, USHORT qual)
{
	char path[2048], match[FILEBUF_SIZE], *ptr;
	struct complete_entry *entry, *curentry, *addpos;
	int new = 0, a;

//	IUtility->Strlcpy(path, str_pathbuffer[win], 2048);
	strncpy(path, str_pathbuffer[win], 2048);
	a = strlen(path);

	if(a > 0 && (path[a - 1] == '/' || path[a - 1] == ':'))
	{
		match[0] = 0;
	}
	else if((ptr = IDOS->FilePart(path)))
	{
//		IDOpus->LStrnCpy(match, ptr, FILEBUF_SIZE - 1);
		strncpy(match, ptr, FILEBUF_SIZE - 1);
		match[FILEBUF_SIZE - 1] = 0;
		*ptr = 0;
	}
	else
	{
		return;
	}

//	if(IDOpus->LStrCmp(completion[win].path, path) != 0)
	if(strcmp(completion[win].path, path) != 0)
	{
		new = 1;
	}
//	else if(IDOpus->LStrCmp(completion[win].match, match) != 0)
	else if(strcmp(completion[win].match, match) != 0)
	{
		entry = completion[win].firstentry;
		while (entry)
		{
			if(strcmp(match, entry->name) == 0)
				break;
			entry = entry->next;
		}
		if(!entry)
			new = 1;
		else if(entry != completion[win].currententry)
		{
			completion[win].currententry = entry;
			goto docomplete;
		}
	}

	if(new)
	{
		APTR context = NULL;

		strcpy(completion[win].match, match);
		strcpy(completion[win].path, path);
		IDOpus->LFreeRemember(&completion[win].memory);
		completion[win].firstentry = NULL;
		completion[win].currententry = NULL;

		busy();

		if((context = IDOS->ObtainDirContextTags(EX_StringName, path, TAG_END)))
		{
			struct ExamineData *data = NULL;

			while((data = IDOS->ExamineDir(context)))
			{
				if(status_haveaborted)
					break;

				if(EXD_IS_DIRECTORY(data) && (!match[0] || IUtility->Strnicmp(data->Name, match, strlen(match)) == 0))
				{
					if((entry = IDOpus->LAllocRemember(&completion[win].memory, sizeof(struct complete_entry), MEMF_CLEAR)))
					{
						strcpy(entry->name, data->Name);
						addpos = completion[win].firstentry;
						curentry = NULL;
						while (addpos)
						{
							curentry = addpos;
							if(strcmp(addpos->name, entry->name) > 0)
								break;

							addpos = addpos->next;
						}
						if(addpos)
						{
							entry->next = addpos;
							entry->last = addpos->last;
							addpos->last = entry;
							if(entry->last)
							{
								entry->last->next = entry;
							}
							if(addpos == completion[win].firstentry)
							{
								completion[win].firstentry = entry;
							}
						}
						else
						{
							if(curentry)
							{
								curentry->next = entry;
								entry->last = curentry;
							}
							else
							{
								completion[win].firstentry = entry;
							}
						}
					}
				}
			}
			IDOS->ReleaseDirContext(context);
		}

		unbusy();
	}

	if(completion[win].currententry)
	{
		if(qual & IEQUALIFIER_ANYSHIFT)
			completion[win].currententry = completion[win].currententry->last;
		else
			completion[win].currententry = completion[win].currententry->next;
	}

	if(!completion[win].currententry)
	{
		if(qual & IEQUALIFIER_ANYSHIFT)
		{
			entry = completion[win].firstentry;
			while (entry && entry->next)
				entry = entry->next;
			completion[win].currententry = entry;
		}
		else
			completion[win].currententry = completion[win].firstentry;
		if(!completion[win].currententry)
		{
			IIntuition->DisplayBeep(NULL);
			IDOpus->ActivateStrGad(&path_strgadget[win], Window);
			return;
		}
	}
      docomplete:
	strcpy(str_pathbuffer[win], path);
	IDOS->AddPart(str_pathbuffer[win], completion[win].currententry->name, 256);
	IDOpus->RefreshStrGad(&path_strgadget[win], Window);
	IDOpus->ActivateStrGad(&path_strgadget[win], Window);

	return;
}
