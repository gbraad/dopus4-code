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

enum
{
	PM_CANCEL,
	PM_OKAY
};

struct RequesterBase pm_req =
{
	50, 12, 32, 19, 8, 6, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, NULL, 0, NULL
};

struct TagItem pm_lister[] =
{
	{ RO_Type, OBJECT_LISTVIEW },
	{ RO_ListViewID, 0 },
	{ RO_Top, 1 },
	{ RO_Width, 50 },
	{ RO_Height, 10 },
	{ RO_HighRecess, TRUE },
	{ TAG_END, 0 }
}, pm_okay_gadget[] =
{
	{ RO_Type, OBJECT_GADGET },
	{ RO_GadgetType, GADGET_BOOLEAN },
	{ RO_GadgetID, PM_OKAY },
	{ RO_Top, 11 },
	{ RO_TopFine, 5 },
	{ RO_Left, 4 },
	{ RO_LeftFine, -2 },
	{ RO_Width, 12 },
	{ RO_Height, 1 },
	{ RO_HeightFine, 6 },
	{ RO_TextNum, STR_OKAY },
	{ RO_TextPos, TEXTPOS_CENTER },
	{ RO_HighRecess, TRUE },
	{ TAG_END, 0 }
}, pm_cancel_gadget[] =
{
	{ RO_Type, OBJECT_GADGET },
	{ RO_GadgetType, GADGET_BOOLEAN },
	{ RO_GadgetID, PM_CANCEL },
	{ RO_Top, 11 },
	{ RO_TopFine, 5 },
	{ RO_Width, 12 },
	{ RO_Height, 1 },
	{ RO_HeightFine, 6 },
	{ RO_Left, 34 },
	{ RO_LeftFine, 18 },
	{ RO_TextNum, STR_CANCEL },
	{ RO_TextPos, TEXTPOS_CENTER },
	{ RO_HighRecess, TRUE },
	{ TAG_END, 0 }
}, *pm_gadgets[] =
{
	pm_okay_gadget, pm_cancel_gadget, NULL
};

int do_parent_multi(STRPTR path)
{
	ULONG class;
	uint16 gadgetid = 0;
	struct Window *rwindow;
	struct Gadget *gadlist;
	struct DOpusListView *listview, *view;
	BPTR lock, parentlock, temp;
	char buf[256], **table;
	char *uscore, key[2];
	int a, count, lastsel = -1;
	ULONG lastseconds = 0, lastmicros = 0, seconds, micros;

	if(!(lock = IDOS->Lock(path, ACCESS_READ)))
		return (0);

	fix_requester(&pm_req, globstring[STR_SELECT_A_DIRECTORY]);

	if(!(rwindow = IDOpus->OpenDORequester(&pm_req)) || !(gadlist = addreqgadgets(&pm_req, pm_gadgets, 0, NULL)) || !(listview = (struct DOpusListView *)IDOpus->AddRequesterObject(&pm_req, pm_lister)))
	{
		IDOpus->CloseRequester(&pm_req);
		IDOS->UnLock(lock);
		return (0);
	}

	listview->window = rwindow;
	listview->flags |= DLVF_LEAVE | DLVF_SLOW | DLVF_TTOP;
	listview->sliderwidth = 8;
	listview->slidercol = pm_req.rb_fg;
	listview->sliderbgcol = pm_req.rb_bg;
	listview->textcol = pm_req.rb_fg;
	listview->boxhi = pm_req.rb_shine;
	listview->boxlo = pm_req.rb_shadow;
	listview->arrowfg = pm_req.rb_fg;
	listview->arrowbg = pm_req.rb_bg;
	listview->itemfg = pm_req.rb_fg;
	listview->itembg = pm_req.rb_bg;
	listview->title = globstring[STR_PARENT_MULTI];

	IDOpus->SetBusyPointer(rwindow);

	parentlock = IDOS->DupLock(lock);
	count = 0;
	while(parentlock)
	{
		++count;
		count += get_multi_volume(parentlock, NULL, NULL);
		temp = parentlock;
		parentlock = IDOS->ParentDir(parentlock);
		IDOS->UnLock(temp);
	}

	if(!(table = IDOpus->LAllocRemember(&pm_req.rb_memory, (count + 1) * sizeof(char *), MEMF_CLEAR)))
	{
		IDOpus->CloseRequester(&pm_req);
		return (0);
	}

	parentlock = IDOS->DupLock(lock);
	count = 0;
	while(parentlock)
	{
		IDOS->NameFromLock(parentlock, buf, 256);
		if((table[count] = IDOpus->LAllocRemember(&pm_req.rb_memory, (a = (strlen(buf) + 3)), 0)))
		{
			strncpy(table[count], buf, a - 1);
//			strncat(table[count], "/", a - 1);
			IDOpus->TackOn(table[count], NULL, a - 1);
			++count;
		}
		count += get_multi_volume(parentlock, &table[count], &pm_req.rb_memory);
		temp = parentlock;
		parentlock = IDOS->ParentDir(parentlock);
		IDOS->UnLock(temp);
	}

	IDOS->UnLock(lock);

	listview->items = table;

	if(!(IDOpus->AddListView(listview, 1)))
	{
		IDOpus->CloseRequester(&pm_req);
		return (0);
	}

	IDOpus->RefreshRequesterObject(&pm_req, NULL);

	IIntuition->ClearPointer(rwindow);

	if(table[0])
		strcpy(buf, table[0]);

	for(a = 0; a < 2; a++)
	{
		if((uscore = strchr(globstring[a ? STR_CANCEL : STR_OKAY], '_')))
			key[a] = uscore[1];
		else
			key[a] = globstring[a ? STR_CANCEL : STR_OKAY][0];
		key[a] = tolower(key[a]);
	}

	for(;;)
	{
		while((IMsg = (struct IntuiMessage *)IExec->GetMsg(rwindow->UserPort)))
		{
			seconds = IMsg->Seconds;
			micros = IMsg->Micros;
			if((view = (struct DOpusListView *)IDOpus->ListViewIDCMP(listview, IMsg)) == (struct DOpusListView *)-1)
			{
				if((class = IMsg->Class) == IDCMP_VANILLAKEY)
				{
					if((IMsg->Code == 0x1b) || (tolower(IMsg->Code) == key[1]))
					{
						class = IDCMP_GADGETUP;
						gadgetid = PM_CANCEL;
					}
					else if(tolower(IMsg->Code) == key[0])
					{
						class = IDCMP_GADGETUP;
						gadgetid = PM_OKAY;
					}
				}
				else if(class == IDCMP_GADGETUP)
					gadgetid = ((struct Gadget *)IMsg->IAddress)->GadgetID;
				IExec->ReplyMsg((struct Message *)IMsg);

				switch (class)
				{
				case IDCMP_GADGETUP:
					IDOpus->RemoveListView(listview, 1);
					IDOpus->CloseRequester(&pm_req);
					if(gadgetid == PM_OKAY)
						strcpy(path, buf);
					return ((int)gadgetid);
				}
			}
			else if(view)
			{
				if(table[view->itemselected])
				{
//					if(IDOpus->LStrnCmp(table[view->itemselected], "  + ", 4) == 0)
					if(strncmp(table[view->itemselected], "  + ", 4) == 0)
//						strcpy(buf, &table[view->itemselected][4]);
						strncpy(buf, &table[view->itemselected][4], 256);
					else
//						strcpy(buf, table[view->itemselected]);
						strncpy(buf, table[view->itemselected], 256);
					if(view->itemselected == lastsel && (IIntuition->DoubleClick(lastseconds, lastmicros, seconds, micros)))
					{
						IDOpus->RemoveListView(listview, 1);
						IDOpus->CloseRequester(&pm_req);
						strcpy(path, buf);
						return (PM_OKAY);
					}
					lastseconds = seconds;
					lastmicros = micros;
					lastsel = view->itemselected;
				}
			}
		}
		IExec->Wait(1 << rwindow->UserPort->mp_SigBit);
	}
}

int get_multi_volume(BPTR lock, STRPTR *table, struct DOpusRemember **key)
{
/*	struct DosList *doslist;
	struct RootNode *rootnode;
	struct DosInfo *dosinfo;
	struct AssignList *list;
	char buf[256];
	int count = 0, tabcount = 0, a;

	IExec->Forbid();

	rootnode = (struct RootNode *)DOSBase->dl_Root;
	dosinfo = (struct DosInfo *)BADDR(rootnode->rn_Info);
	doslist = (struct DosList *)BADDR(dosinfo->di_DevInfo);

	while(doslist)
	{
		if(doslist->dol_Type == DLT_DIRECTORY && (SameLock(lock, doslist->dol_Lock) == LOCK_SAME))
		{
			list = doslist->dol_misc.dol_assign.dol_List;
			while(list)
			{
				if(table)
				{
					IDOS->NameFromLock(list->al_Lock, buf, 256);
					if(table[tabcount] = IDOpus->LAllocRemember(key, (a = (strlen(buf) + 7)), 0))
					{
						IDOpus->StrCombine(table[tabcount], "  + ", buf, a - 1);
						IDOpus->TackOn(table[tabcount], a - 1);
						++tabcount;
					}
				}
				++count;
				list = list->al_Next;
			}
		}
		doslist = (struct DosList *)BADDR(doslist->dol_Next);
	}

	IExec->Permit();
	if(table)
		return (tabcount);
	else
		return (count);*/
	return 0;
}
