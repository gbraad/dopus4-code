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

enum SortBy
{
	sbName = DISPLAY_NAME + 1, sbSize, sbProt, sbDate, sbComment, sbType,
	sbOwner, sbGroup, sbNetprot, sbExt,
	sbKMG = 100,
	sbMix = 0x00010000, sbDirF = 0x00020000, sbFileF = 0x00030000,
	sbSortA = 0x01000000, sbSortD = 0x02000000, sbSortH = 0x03000000,
	sbReverse = 0x40000000	// So we can put in the same LONG, as a flag...
};

/* based on code by Henrik Isaksson */

ULONG MenuHandlerFunc(struct Hook *hook, struct PopupMenu *pm, APTR msg)
{
	ULONG id;
	BOOL on;

	IPopupMenu->PM_GetItemAttrs(pm, PM_ID, &id, PM_Checked, &on, TAG_END);
	if(id == sbReverse)
	{
		if(on)
		{
			*((ULONG *) hook->h_Data) |= sbReverse;	// Set flag
		}
		else
		{
			*((ULONG *) hook->h_Data) &= ~sbReverse;	// Clear flag
		}
	}
	else if(id >= sbName && id < sbKMG)
	{
		if(on)
		{
			*((ULONG *) hook->h_Data) &= 0xFFFF0000;	// We don't need all bits so we
			*((ULONG *) hook->h_Data) |= id;	// preserve the high ones for flags.
		}
	}
	else if(((id >> 16) & 0xF) > 0)
	{
		if(on)
		{
			*((ULONG *) hook->h_Data) &= 0xFFF0FFFF;
			*((ULONG *) hook->h_Data) |= id;
		}
	}
	else if(((id >> 24) & 0xF) > 0)
	{
		if(on)
		{
			*((ULONG *) hook->h_Data) &= 0xF0FFFFFF;
			*((ULONG *) hook->h_Data) |= id;
		}
	}
	return 0;
}

/* end of code */

void handlelistermenu(int a)
{
	struct Hook MenuHandler;
	ULONG sortorder;	// Holds the current sort order
	long r;
	BOOL changed = FALSE;
	char oldsort, newsort, sortbit;

	if(!sortmenu || !dopus_curwin[a]->firstentry || (ENTRYTYPE(dopus_curwin[a]->firstentry->type) == ENTRY_CUSTOM) || (ENTRYTYPE(dopus_curwin[a]->firstentry->type) == ENTRY_DEVICE))
		return;

	sortbit = a ? SORT_RREVERSE : SORT_LREVERSE;
	oldsort = config->sortflags & sortbit;
	MenuHandler.h_Entry = (HOOKFUNC) MenuHandlerFunc;
	MenuHandler.h_Data = &sortorder;

	for(r = DISPLAY_NAME; r <= DISPLAY_LAST; r++)
		IPopupMenu->PM_SetItemAttrs(IPopupMenu->PM_FindItem(sortmenu, 1 + r), PM_Checked, config->sortmethod[a] == r, TAG_END);

	IPopupMenu->PM_SetItemAttrs(IPopupMenu->PM_FindItem(sortmenu, sbReverse), PM_Checked, oldsort, TAG_END);

	for(r = 0; r < 3; r++)
		IPopupMenu->PM_SetItemAttrs(IPopupMenu->PM_FindItem(sortmenu, (r + 1) * 0x00010000), PM_Checked, config->separatemethod[a] == r, TAG_END);

	for(r = 0; r < 3; r++)
		IPopupMenu->PM_SetItemAttrs(IPopupMenu->PM_FindItem(sortmenu, (r + 1) * 0x01000000), PM_Checked, SORT_NAMEMODE(config->sortflags) == r, TAG_END);

	IPopupMenu->PM_SetItemAttrs(IPopupMenu->PM_FindItem(sortmenu, sbKMG), PM_Checked, (config->listerdisplayflags[a] & SIZE_KMG) == SIZE_KMG, TAG_END);	// HUX

	sortorder = (config->sortmethod[a] + 1) + (oldsort ? sbReverse : 0) + (config->separatemethod[a] + 1) * 0x00010000 + (SORT_NAMEMODE(config->sortflags) + 1) * 0x01000000;

	IPopupMenu->PM_OpenPopupMenu(Window, PM_Menu, sortmenu, PM_MenuHandler, &MenuHandler, PM_ForceFont, scr_font[FONT_MENUS], TAG_END);

	r = (sortorder & 0xFFFF) - 1;
	if(config->sortmethod[a] != r)
	{
		config->sortmethod[a] = r;
		changed = TRUE;
	}
	r = ((sortorder >> 16) & 0xF) - 1;
	if(config->separatemethod[a] != r)
	{
		config->separatemethod[a] = r;
		changed = TRUE;
	}
	r = ((sortorder >> 24) & 0xF) - 1;
	if(SORT_NAMEMODE(config->sortflags) != r)
	{
		config->sortflags &= ~SORT_NAMEMASK;
		config->sortflags |= r << SORT_BITPOS;
		changed = TRUE;
	}
	r = (sortorder & sbReverse) ? ~0 : 0;
	newsort = r & sortbit;

	if(oldsort != newsort)
	{
		if(newsort)
			config->sortflags |= sortbit;
		else
			config->sortflags &= ~sortbit;
		changed = TRUE;
	}

	r = IPopupMenu->PM_ItemChecked(sortmenu, sbKMG) ? SIZE_KMG : 0;
	if((config->listerdisplayflags[a] & SIZE_KMG) != r)
	{
		if(r & SIZE_KMG)
			config->listerdisplayflags[a] |= SIZE_KMG;
		else
			config->listerdisplayflags[a] &= ~SIZE_KMG;

		setsizedisplen(&dir_rp[a]);
		doselinfo(a);
		fixhlen(a);
		if(!changed)
			refreshwindow(a, 1);
	}

	if(changed)
	{
		sortdir(dopus_curwin[a], a);
		refreshwindow(a, 0);
	}
}

void initlistermenu(void)
{
	if(PopupMenuBase && IPopupMenu)
	{
		BOOL userinfo = FALSE;

		sortmenu = PMMenu(globstring[STR_LISTER_MENU]),
				PMInfo(globstring[STR_SORTBY]),
				PM_Center, TRUE,
			End,
			PMCheckItem(globstring[STR_FILE_NAME], sbName),
				PM_Exclude, IPopupMenu->PM_ExLst(sbSize, sbProt, sbDate, sbComment, sbType, sbOwner, sbGroup, sbNetprot, sbExt, 0),
			End,
			PMCheckItem(globstring[STR_FILE_EXTENSION], sbExt),
				PM_Exclude, IPopupMenu->PM_ExLst(sbName, sbSize, sbProt, sbDate, sbComment, sbType, sbOwner, sbGroup, sbNetprot, 0),
			End,
			PMCheckItem(globstring[STR_FILE_SIZE], sbSize),
				PM_Exclude, IPopupMenu->PM_ExLst(sbName, sbProt, sbDate, sbComment, sbType, sbOwner, sbGroup, sbNetprot, sbExt, 0),
			End,
			PMCheckItem(globstring[STR_PROTECTION_BITS], sbProt),
				PM_Exclude, IPopupMenu->PM_ExLst(sbName, sbSize, sbDate, sbComment, sbType, sbOwner, sbGroup, sbNetprot, sbExt,0),
			End,
			PMCheckItem(globstring[STR_CREATION_DATE], sbDate),
				PM_Exclude, IPopupMenu->PM_ExLst(sbName, sbSize, sbProt, sbComment, sbType, sbOwner, sbGroup, sbNetprot, sbExt, 0),
			End,
			PMCheckItem(globstring[STR_FILE_COMMENT], sbComment),
				PM_Exclude, IPopupMenu->PM_ExLst(sbName, sbSize, sbProt, sbDate, sbType, sbOwner, sbGroup, sbNetprot, sbExt, 0),
			End,
			PMCheckItem(globstring[STR_FILE_TYPE], sbType),
				PM_Exclude, IPopupMenu->PM_ExLst(sbName, sbSize, sbProt, sbDate, sbComment, sbOwner, sbGroup, sbNetprot, sbExt, 0),
			End,
			PMCheckItem(globstring[STR_OWNER], sbOwner),
				PM_Exclude, IPopupMenu->PM_ExLst(sbName, sbSize, sbProt, sbDate, sbComment, sbType, sbGroup, sbNetprot, sbExt, 0),
				PM_Hidden, !userinfo,
			End,
			PMCheckItem(globstring[STR_GROUP], sbGroup),
				PM_Exclude, IPopupMenu->PM_ExLst(sbName, sbSize, sbProt, sbDate, sbComment, sbType, sbOwner, sbNetprot, sbExt, 0),
				PM_Hidden, !userinfo,
			End,
			PMCheckItem(globstring[STR_NET_PROTECTION_BITS], sbNetprot),
				PM_Exclude, IPopupMenu->PM_ExLst(sbName, sbSize, sbProt, sbDate, sbComment, sbType, sbOwner, sbGroup, sbExt, 0),
				PM_Hidden, !userinfo,
			End,
			PMBar,
			End,
			PMCheckItem(globstring[STR_REVERSE_ORDER], sbReverse),
			End,
			PMBar,
			End,
			PMCheckItem(globstring[STR_MIXDIRSFILES], sbMix),
				PM_Exclude, IPopupMenu->PM_ExLst(sbDirF, sbFileF, 0),
			End,
			PMCheckItem(globstring[STR_DIRSFIRST], sbDirF),
				PM_Exclude, IPopupMenu->PM_ExLst(sbMix, sbFileF, 0),
			End,
			PMCheckItem(globstring[STR_FILESFIRST], sbFileF),
				PM_Exclude, IPopupMenu->PM_ExLst(sbMix, sbDirF, 0),
			End,
			PMBar,
			End,
			PMInfo(globstring[STR_NAMESORT_MENU]),
				PM_Center, TRUE,
			End,
			PMCheckItem(globstring[STR_SORTALPHA], sbSortA),
				PM_Exclude, IPopupMenu->PM_ExLst(sbSortD, sbSortH, 0),
			End,
			PMCheckItem(globstring[STR_SORTDEC], sbSortD),
				PM_Exclude, IPopupMenu->PM_ExLst(sbSortA, sbSortH, 0),
			End,
			PMCheckItem(globstring[STR_SORTHEX], sbSortH),
				PM_Exclude, IPopupMenu->PM_ExLst(sbSortA, sbSortD, 0),
			End,
			PMBar,
			End,
			PMCheckItem(globstring[STR_SIZE_AS_KMG], sbKMG),
			End,
		End;
	}
}
