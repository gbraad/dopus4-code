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

static struct Directory *selectedentry;

void defselect(int win, int o, int state)
{
	int a, foundcount;
	struct Directory *temp, *temp2;
	char sbuf[FILEBUF_SIZE];

	if(o < 0 || o > scrdata_dispwin_lines - 1)
		return;
	a = o + dopus_curwin[win]->offset;
	if(a >= dopus_curwin[win]->total)
		return;
	temp = dopus_curwin[win]->firstentry;
	while(a--)
		temp = temp->next;
	if(state)
		selectedentry = temp;
	else
		selectedentry = NULL;
	if(temp && ((ENTRYTYPE(temp->type) != ENTRY_CUSTOM) || (temp->subtype != CUSTOMENTRY_USER) || (temp->size & CUSTENTRY_CANSELECT)))
	{
		if(temp->selected != state)
		{
			temp->selected = state;
			updateselectinfo(temp, win, 1);
			display_entry(temp, win, scrdata_dirwin_xpos[win] - dopus_curwin[win]->hoffset + 1, scrdata_font_baseline + (scrdata_font_ysize * o) + scrdata_dirwin_ypos[win]);
		}
		if(config->iconflags & ICONFLAG_AUTOSELECT)
		{
			StrCombine(sbuf, temp->name, ".info", FILEBUF_SIZE - 1);
			if((temp2 = findfile(dopus_curwin[win], sbuf, &foundcount)))
			{
				if(temp2->selected != temp->selected)
				{
					temp2->selected = temp->selected;
					updateselectinfo(temp2, win, 1);
					if(foundcount >= dopus_curwin[win]->offset && foundcount < dopus_curwin[win]->offset + scrdata_dispwin_lines)
					{
						o = foundcount - dopus_curwin[win]->offset;
						display_entry(temp2, win, scrdata_dirwin_xpos[win] - dopus_curwin[win]->hoffset + 1, scrdata_font_baseline + (scrdata_font_ysize * o) + scrdata_dirwin_ypos[win]);
					}
				}
			}
		}
	}
}

void globalselect(int win, int all)
{
	struct Directory *temp;
	APTR obj;

	if (win > 1)
		return;

	obj = dopusdirlist[win];

	DoMethod(obj, MUIM_List_GetEntry, 0, &temp);

	if (temp && temp->type != ENTRY_CUSTOM)
	{
		DoMethod(obj, MUIM_List_Select, MUIV_List_Select_All, all ? MUIV_List_Select_On : MUIV_List_Select_Off, NULL);

		if(all)
		{
			dopus_curwin[win]->filesel = dopus_curwin[win]->filetot;
			dopus_curwin[win]->dirsel = dopus_curwin[win]->dirtot;
			dopus_curwin[win]->bytessel = dopus_curwin[win]->bytestot;
		}
		else
		{
			dopus_curwin[win]->filesel = 0;
			dopus_curwin[win]->dirsel = 0;
			dopus_curwin[win]->bytessel = 0;
		}
	}
}

void updateselectinfo(struct Directory *temp, int win, int show)
{
	if(temp->selected)
	{
		switch (ENTRYTYPE(temp->type))
		{
		case ENTRY_DIRECTORY:
			if(temp->size != -1)
				dopus_curwin[win]->bytessel += temp->size;
		case ENTRY_DEVICE:
			++dopus_curwin[win]->dirsel;
			break;
		case ENTRY_FILE:
			++dopus_curwin[win]->filesel;
			dopus_curwin[win]->bytessel += temp->size;
			break;
		}
		if(show)
		{
			if(config->generalflags & GENERAL_DISPLAYINFO)
				doinfodisplay(temp, TRUE);
			else
			{
				//doselinfo(data_active_window);
			}
		}
	}
	else
	{
		switch (ENTRYTYPE(temp->type))
		{
		case ENTRY_DIRECTORY:
			if(temp->size != -1)
				dopus_curwin[win]->bytessel -= temp->size;
		case ENTRY_DEVICE:
			--dopus_curwin[win]->dirsel;
			break;
		case ENTRY_FILE:
			--dopus_curwin[win]->filesel;
			dopus_curwin[win]->bytessel -= temp->size;
			break;
		}
//		if(show)
//			doselinfo(data_active_window);
	}
}

void globaltoggle(int win)
{
	struct Directory *temp;

	temp = dopus_curwin[win]->firstentry;
	while(temp)
	{
		switch (ENTRYTYPE(temp->type))
		{
		case ENTRY_FILE:
			if(temp->selected)
			{
				dopus_curwin[win]->bytessel -= temp->size;
				--dopus_curwin[win]->filesel;
				temp->selected = 0;
			}
			else
			{
				dopus_curwin[win]->bytessel += temp->size;
				++dopus_curwin[win]->filesel;
				temp->selected = 1;
			}
			break;
		case ENTRY_DIRECTORY:
			if(temp->selected)
			{
				if(temp->size > 0)
					dopus_curwin[win]->bytessel -= temp->size;
				--dopus_curwin[win]->dirsel;
				temp->selected = 0;
			}
			else
			{
				if(temp->size > 0)
					dopus_curwin[win]->bytessel += temp->size;
				++dopus_curwin[win]->dirsel;
				temp->selected = 1;
			}
			break;
		}
		temp = temp->next;
	}
//	doselinfo(win);
}

void doselect(int rexx)
{
	char buf[160];
	int boobs, prot[2];
	struct DateStamp ds1, ds2;
	int selecttype;

	if(dopus_curwin[data_active_window]->total == 0 || dopus_curwin[data_active_window]->firstentry->type == ENTRY_CUSTOM)
		return;

	if(!rexx)
	{
		boobs = getselectdata(str_select_pattern, &selecttype);
	}
	else
	{
		boobs = rexx_arg_value[0];
		selecttype = rexx_arg_value[1];
		strcpy(str_select_pattern[selecttype], rexx_args[0]);
	}
	if(boobs)
	{
		if(dopus_curwin[data_active_window]->total == 0)
			return;
		switch (selecttype)
		{
		case 0:
			ParsePatternNoCase(str_select_pattern[0], buf, 160);
			wildselect(buf, boobs, 1, WILDSELECT_NAME);
			findfirstsel(data_active_window, -2);
			break;
		case 1:
			if(str_select_pattern[1][0] == 0)
				break;
			getseldatestamps(str_select_pattern[1], &ds1, &ds2);
			dateselect(&ds1, &ds2, boobs, 1);
			findfirstsel(data_active_window, -2);
			break;
		case 2:
			if(str_select_pattern[2][0] == 0)
				break;
			getprotselvals(str_select_pattern[2], prot);
			protselect(prot[0], prot[1], boobs, 1);
			findfirstsel(data_active_window, -2);
			break;
		case 3:
			ParsePatternNoCase(str_select_pattern[3], buf, 160);
			wildselect(buf, boobs, 1, WILDSELECT_COMMENT);
			findfirstsel(data_active_window, -2);
			break;
		}
	}
}

void getseldatestamps(STRPTR buf, struct DateStamp *ds1, struct DateStamp *ds2)
{
	char datebuf[2][85], timebuf[2][85], *ptr;
	int a;

	datebuf[0][0] = datebuf[1][0] = timebuf[0][0] = timebuf[1][0] = a = 0;
	ptr = parsedatetime(buf, datebuf[0], timebuf[0], &a);
	switch (a)
	{
	case 0:
		strcpy(datebuf[1], datebuf[0]);
		strcpy(timebuf[1], "23:59:59");
		break;
	case -1:
		strcpy(datebuf[1], datebuf[0]);
		strcpy(timebuf[1], timebuf[0]);
		break;
	default:
		parsedatetime(ptr, datebuf[1], timebuf[1], &a);
		break;
	}
	strtostamp(datebuf[0], timebuf[0], ds1);
	strtostamp(datebuf[1], timebuf[1], ds2);
	ds1->ds_Tick = ((ds1->ds_Tick / 50) * 50);
	ds2->ds_Tick = ((ds2->ds_Tick / 50) * 50) + 49;
}

void getprotselvals(STRPTR buf, int *prot)
{
	int a, b, c, notflag = 0;
	char ch;

	prot[0] = prot[1] = 0;
	b = strlen(buf);
	for(a = 0; a < b; a++)
	{
		ch = toupper(buf[a]);
		if(ch == '-')
			notflag = 1;
		else if(ch == '+')
			notflag = 0;
		else
		{
			for(c = 0; c < 8; c++)
			{
				if(ch == str_protbits[c])
				{
					prot[notflag] |= (1 << (7 - c));
					break;
				}
			}
		}
	}
}

void wildselect(STRPTR wild, int boobs, int and, int mode)
{
	char buf[256];
	struct Directory *temp;

	temp = dopus_curwin[data_active_window]->firstentry;
	if(!and)
	{
		dopus_curwin[data_active_window]->dirsel = dopus_curwin[data_active_window]->filesel = dopus_curwin[data_active_window]->bytessel = 0;
	}
	while(temp)
	{
		if(temp->type != ENTRY_CUSTOM || temp->subtype != CUSTOMENTRY_USER || temp->size & CUSTENTRY_CANSELECT)
		{
			if(!and)
			{
				temp->selected = 0;
			}
			if(temp->type == ENTRY_CUSTOM)
			{
				if(!temp->selected && temp->comment)
				{
					StrToUpper(temp->comment, buf);
					if(MatchPatternNoCase(wild, buf))
					{
						wildselectthisone(temp, data_active_window, boobs);
					}
				}
			}
			else
			{
				if(!temp->selected)
				{
					StrToUpper((mode == WILDSELECT_NAME) ? temp->name : temp->comment, buf);
					if(MatchPatternNoCase(wild, buf))
					{
						wildselectthisone(temp, data_active_window, boobs);
					}
				}
			}
		}
		temp = temp->next;
	}
}

void dateselect(struct DateStamp *ds1, struct DateStamp *ds2, int boobs, int and)
{
	struct Directory *temp;

	temp = dopus_curwin[data_active_window]->firstentry;
	if(!and)
	{
		dopus_curwin[data_active_window]->dirsel = dopus_curwin[data_active_window]->filesel = dopus_curwin[data_active_window]->bytessel = 0;
	}
	while(temp)
	{
		if(temp->type != ENTRY_CUSTOM || temp->subtype != CUSTOMENTRY_USER || temp->size & CUSTENTRY_CANSELECT)
		{
			if(!and)
				temp->selected = 0;
			if(!temp->selected && CompareDates(&(temp->date), ds1) >= 0 && CompareDates(ds2, &(temp->date)) >= 0)
				wildselectthisone(temp, data_active_window, boobs);
		}
		temp = temp->next;
	}
}

void protselect(int protyes, int protno, int boobs, int and)
{
	struct Directory *temp;
	int prot;

	temp = dopus_curwin[data_active_window]->firstentry;
	if(!and)
		dopus_curwin[data_active_window]->dirsel = dopus_curwin[data_active_window]->filesel = dopus_curwin[data_active_window]->bytessel = 0;
	while(temp)
	{
		if(temp->type != ENTRY_CUSTOM || temp->subtype != CUSTOMENTRY_USER || temp->size & CUSTENTRY_CANSELECT)
		{
			if(!and)
				temp->selected = 0;
			if(!temp->selected)
			{
				prot = ((~temp->protection) & 15) + (temp->protection & ~15);
				if((prot & protyes) == protyes && ((prot & ~protyes) & protno) == 0)
					wildselectthisone(temp, data_active_window, boobs);
			}
		}
		temp = temp->next;
	}
}

void wildselectthisone(struct Directory *temp, int win, int boobs)
{
	if(boobs == 1 && temp->type != ENTRY_DEVICE)
		temp->selected = 1;
	else if(boobs == 2 && temp->type <= ENTRY_FILE)
		temp->selected = 1;
	else if(boobs == 3 && temp->type >= ENTRY_DIRECTORY)
		temp->selected = 1;
	if(temp->selected)
	{
		if(temp->type >= ENTRY_DIRECTORY)
		{
			++dopus_curwin[win]->dirsel;
			if(temp->size != -1)
			{
				dopus_curwin[win]->bytessel += temp->size;
			}
		}
		else if(temp->type <= ENTRY_FILE)
		{
			++dopus_curwin[win]->filesel;
			dopus_curwin[win]->bytessel += temp->size;
		}
	}
}

#if 0
void doselinfo(int win)
{
	char b1[24], b2[24];

	if(!dopus_curwin[win]->firstentry || dopus_curwin[win]->firstentry->type != ENTRY_CUSTOM)
	{
		buildkmgstring(b1, dopus_curwin[win]->bytessel, config->listerdisplayflags[win] & SIZE_KMG);
		buildkmgstring(b2, dopus_curwin[win]->bytestot, config->listerdisplayflags[win] & SIZE_KMG);
		sprintf(str_select_info, globstring[STR_DIRS_FILES_BYTES_COUNT], dopus_curwin[win]->dirsel, dopus_curwin[win]->dirtot, dopus_curwin[win]->filesel, dopus_curwin[win]->filetot, b1, b2);
	}
	else
	{
		switch (dopus_curwin[win]->firstentry->subtype)
		{
		case CUSTOMENTRY_DIRTREE:
			sprintf(str_select_info, globstring[STR_ENTRIES_IN_TREE], dopus_curwin[win]->total);
			break;
		case CUSTOMENTRY_BUFFERLIST:
			sprintf(str_select_info, globstring[STR_DIRS_IN_BUFFERS], dopus_curwin[win]->total);
			break;
		case CUSTOMENTRY_USER:
			sprintf(str_select_info, globstring[STR_USER_ENTRIES], dopus_curwin[win]->total);
			break;
		}
	}
	dostatustext(str_select_info);
}
#endif

int makeactive(int win, int state)
{
	if(data_active_window != win)
	{
		data_active_window = win;
		return (doactive(state, 1));
	}
	return (0);
}

int doactive(int state, int showinfo)
{
	int oe;
	char buf[256];
	struct dopusfiletype *type;
	struct dopusfuncpar par;
	struct Directory *entry;

//	if(showinfo)
//		doselinfo(data_active_window);
	if(last_selected_entry && last_selected_entry->type <= ENTRY_FILE)
	{
		if(state && config->generalflags & GENERAL_DOUBLECLICK)
		{
			if(DoubleClick(time_previous_sec, time_previous_micro, time_current_sec, time_current_micro))
			{
				data_active_window = 1 - data_active_window;
				entry = last_selected_entry;
				dostatustext(globstring[STR_INTERROGATING_FILE]);
				StrCombine(buf, str_pathbuffer[data_active_window], last_selected_entry->name, 256);
				busy();
				if((type = checkfiletype(buf, FTFUNC_CLICKMCLICK, 0)))
				{
					char title[256];

					strcpy(func_single_file, last_selected_entry->name);
					par.which = type->which[FTFUNC_CLICKMCLICK];
					par.stack = type->stack[FTFUNC_CLICKMCLICK];
					par.key = par.qual = 0;
					par.type = 3;
					par.pri = type->pri[FTFUNC_CLICKMCLICK];
					par.delay = type->delay[FTFUNC_CLICKMCLICK];

					if(type->actionstring[FTFUNC_CLICKMCLICK][0])
					{
						do_title_string(type->actionstring[FTFUNC_CLICKMCLICK], title, 0, NULL);
						dostatustext(title);
					}
					else
						title[0] = 0;

					dofunctionstring(type->function[FTFUNC_CLICKMCLICK], last_selected_entry->name, title, &par);
					unbusy();
					checkselection(entry);
					return (1);
				}
				unbusy();
				return (0);
			}
		}
		last_selected_entry = NULL;
	}
	for(oe = 0; oe < 2; oe++)
		displayname(oe, 1);
	if(last_selected_entry && last_selected_entry->type >= ENTRY_DEVICE && state)
	{
		if(DoubleClick(time_previous_sec, time_previous_micro, time_current_sec, time_current_micro))
		{
			if(last_selected_entry->type == ENTRY_CUSTOM && last_selected_entry->subtype == CUSTOMENTRY_USER)
			{
				if(last_selected_entry->size & CUSTENTRY_CANSELECT)
				{
					userentrymessage(dopus_curwin[1 - data_active_window], last_selected_entry, USERENTRY_CLICKMCLICK);
					time_previous_sec = 0;
				}
			}
			else
			{
				advancebuf(data_active_window, 1);
				if(last_selected_entry->type == ENTRY_CUSTOM)
				{
					if(last_selected_entry->subtype == CUSTOMENTRY_BUFFERLIST)
					{
						bringinbuffer(last_selected_entry->dispstr, data_active_window, 1);
						time_previous_sec = 0;
						return (1);
					}
					else
						strcpy(str_pathbuffer[data_active_window], last_selected_entry->dispstr);
				}
				else
				{
					strcpy(str_pathbuffer[data_active_window], dopus_curwin[1 - data_active_window]->directory);
					TackOn(str_pathbuffer[data_active_window], last_selected_entry->name, 256);
				}
				startgetdir(data_active_window, SGDFLAGS_CANMOVEEMPTY | SGDFLAGS_CANCHECKBUFS);
				time_previous_sec = 0;
				return (1);
			}
		}
	}
	return (0);
}

void unbyte(int win)
{
	struct Directory *entry;

	entry = dopus_curwin[win]->firstentry;
	while(entry)
	{
		if(entry->selected && ENTRYTYPE(entry->type) == ENTRY_DIRECTORY && entry->size != -1)
		{
			setdirsize(entry, -1, win);
		}
		if(entry->next)
			entry = entry->next;
		else
			entry = NULL;
	}
}

void checkselection(struct Directory *entry)
{
	struct Directory *temp;

	last_selected_entry = NULL;
	func_single_file[0] = 0;
	temp = dopus_curwin[data_active_window]->firstentry;
	while(temp)
	{
		if(temp == entry)
			break;
		temp = temp->next;
	}
}
