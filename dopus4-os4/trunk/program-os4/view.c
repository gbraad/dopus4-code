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
#include "view.h"
//#include "searchdata.h"
#include <proto/texteditor.h>
#include <gadgets/texteditor.h>

int32 view_file_process(char *, int32, struct ExecBase *);
Object *makeviewwindow(struct MsgPort *, STRPTR);
void DisplayFile(struct Window *, STRPTR);
struct ViewNode *allocviewnode(STRPTR, STRPTR, STRPTR, int, STRPTR, int);
void cleanupviewnode(struct ViewNode *);

int32 viewfile(STRPTR filename, STRPTR name, int function, STRPTR initialsearch, int wait, int noftype)
{
	int32 ret = 0;
	char processname[15];
	struct Process *viewproc = NULL;
	struct MsgPort *deathmsg_replyport;
	struct DeathMessage *deathmsg = NULL;
	struct ViewNode *viewnode;

	if(!noftype)
	{
		switch(function)
		{
		case FUNC_HEXREAD:
			if(checkfiletypefunc(filename, FTFUNC_HEXREAD))
				return 1;
			break;
		case FUNC_ANSIREAD:
			if(checkfiletypefunc(filename, FTFUNC_ANSIREAD))
				return 1;
			break;
		default:
			if(checkfiletypefunc(filename, FTFUNC_READ))
				return 1;
			break;
		}
	}

	IUtility->SNPrintf(processname, 15, "dopus_view.%ld", system_dopus_runcount);

	IExec->Forbid();
	viewproc = (struct Process *)IExec->FindTask(processname);
	IExec->Permit();

	viewnode = allocviewnode(processname, filename, name, function, initialsearch, wait);

	if(viewproc)
	{
		return 0;
	}
	else
	{
		if(wait)
		{
			if((deathmsg_replyport = IExec->AllocSysObjectTags(ASOT_PORT, TAG_END)))
			{
				if((deathmsg = IExec->AllocSysObjectTags(ASOT_MESSAGE, ASOMSG_Size, sizeof(struct DeathMessage), ASOMSG_ReplyPort, deathmsg_replyport, TAG_END)) == NULL)
				{
					deathmsg = NULL;
				}
			}
		}
		viewproc = IDOS->CreateNewProcTags(NP_Name, processname, NP_Entry, &view_file_process, NP_EntryData, viewnode, NP_StackSize, 65356, wait ? NP_NotifyOnDeathMessage : TAG_IGNORE, deathmsg, NP_Child, TRUE, TAG_END);

		if(wait)
		{
			if(deathmsg)
			{
				IExec->WaitPort(deathmsg_replyport);
				IExec->GetMsg(deathmsg_replyport);
				ret = deathmsg->dm_ReturnCode;
				IExec->FreeSysObject(ASOT_MESSAGE, deathmsg);
				IExec->FreeSysObject(ASOT_PORT, deathmsg_replyport);
			}
		}
	}

	return 0;
}

/* The View File Process, with it's variables */

enum
{
	VIEW_SEARCH,
	VIEW_PRINT,
	VIEW_QUIT,

	VIEW_NUM_GADGETS
};

STRPTR ViewBuf = NULL;
Object *ViewWin, *ViewTE, *ViewScroller;
struct Screen *ViewScreen = NULL;


struct TagItem text2prop[] = {{GA_TEXTEDITOR_Prop_First, SCROLLER_Top}, {GA_TEXTEDITOR_Prop_Visible, SCROLLER_Visible},{GA_TEXTEDITOR_Prop_Entries, SCROLLER_Total},{TAG_DONE}};
struct TagItem prop2text[] = {{SCROLLER_Top, GA_TEXTEDITOR_Prop_First}, {TAG_DONE}};

int32 view_file_process(char *argStr, int32 argLen, struct ExecBase *sysbase)
{
	struct Window *ViewWindow = NULL;
	struct MsgPort *ViewMsgPort = NULL;
	BOOL running = TRUE;
	struct ViewNode *viewnode;

	viewnode = (struct ViewNode *)IDOS->GetEntryData();

	ViewMsgPort = IExec->AllocSysObjectTags(ASOT_PORT, TAG_END);

	ViewWin = makeviewwindow(ViewMsgPort, viewnode->name);

	IIntuition->RefreshSetGadgetAttrs((struct Gadget *)ViewTE, ViewWindow, NULL, ICA_MAP, text2prop, TAG_END);
	IIntuition->RefreshSetGadgetAttrs((struct Gadget *)ViewTE, ViewWindow, NULL, ICA_TARGET, ViewScroller, TAG_END);
	IIntuition->RefreshSetGadgetAttrs((struct Gadget *)ViewScroller, ViewWindow, NULL, ICA_MAP, prop2text, TAG_END);
	IIntuition->RefreshSetGadgetAttrs((struct Gadget *)ViewScroller, ViewWindow, NULL, ICA_TARGET, ViewTE, TAG_END);

	if((ViewWindow = RA_OpenWindow(ViewWin)))
	{
		uint32 sigmask = 0, siggot = 0, result = 0, query1 = 0, query2 = 0, query3 = 0;
		uint16 code = 0;

		DisplayFile(ViewWindow, viewnode->filename);

		IIntuition->GetAttr(WINDOW_SigMask, ViewWin, &sigmask);
		while(running)
		{
			siggot = IExec->Wait(sigmask | SIGBREAKF_CTRL_C);
			if(siggot & SIGBREAKF_CTRL_C)
			{
				running = FALSE;
			}

			while((result = RA_HandleInput(ViewWin, &code)))
			{
				switch(result & WMHI_CLASSMASK)
				{
				case WMHI_CLOSEWINDOW:
					running = FALSE;
					break;
				case WMHI_GADGETUP:
					switch(result & WMHI_GADGETMASK)
					{
					case VIEW_QUIT:
						running = FALSE;
						break;
					}
					break;
				case WMHI_RAWKEY:
					switch(code)
					{
					case RAWKEY_CRSRUP:
						IIntuition->GetAttrs(ViewTE, GA_TEXTEDITOR_Prop_Entries, &query1, GA_TEXTEDITOR_Prop_First, &query2, TAG_END);
						IIntuition->RefreshSetGadgetAttrs((struct Gadget *)ViewTE, ViewWindow, NULL, GA_TEXTEDITOR_Prop_First, ((query2 - 1) > query1) ? 0 : (query2 - 1), TAG_END);
						break;
					case RAWKEY_CRSRDOWN:
						IIntuition->GetAttrs(ViewTE, GA_TEXTEDITOR_Prop_Entries, &query1, GA_TEXTEDITOR_Prop_First, &query2, TAG_END);
						IIntuition->RefreshSetGadgetAttrs((struct Gadget *)ViewTE, ViewWindow, NULL, GA_TEXTEDITOR_Prop_First, ((query2 + 1) > query1) ? 0 : (query2 + 1), TAG_END);
						break;
					case RAWKEY_HOME:
						IIntuition->RefreshSetGadgetAttrs((struct Gadget *)ViewTE, ViewWindow, NULL, GA_TEXTEDITOR_Prop_First, 0, TAG_END);
						break;
					case RAWKEY_END:
						IIntuition->GetAttrs(ViewTE, GA_TEXTEDITOR_Prop_Entries, &query1, TAG_END);
						IIntuition->RefreshSetGadgetAttrs((struct Gadget *)ViewTE, ViewWindow, NULL, GA_TEXTEDITOR_Prop_First, query1, TAG_END);
						break;
					case RAWKEY_PAGEUP:
						IIntuition->GetAttrs(ViewTE, GA_TEXTEDITOR_Prop_First, &query1, GA_TEXTEDITOR_Prop_Visible, &query2, GA_TEXTEDITOR_Prop_Entries, &query3, TAG_END);
						IIntuition->RefreshSetGadgetAttrs((struct Gadget *)ViewTE, ViewWindow, NULL, GA_TEXTEDITOR_Prop_First, ((query1 - query2) > query3) ? 0 : (query1 - query2), TAG_END);
						break;
					case RAWKEY_PAGEDOWN:
						IIntuition->GetAttrs(ViewTE, GA_TEXTEDITOR_Prop_First, &query1, GA_TEXTEDITOR_Prop_Visible, &query2, TAG_END);
						IIntuition->RefreshSetGadgetAttrs((struct Gadget *)ViewTE, ViewWindow, NULL, GA_TEXTEDITOR_Prop_First, query1 + query2, TAG_END);
						break;
					case RAWKEY_ESC:
						running = FALSE;
						break;
					}
					break;
				case WMHI_MENUPICK:
					switch(result & WMHI_MENUMASK)
					{
					}
					break;
				case WMHI_ICONIFY:
					if(RA_Iconify(ViewWin))
					{
						ViewWindow = NULL;
					}
					break;
				case WMHI_UNICONIFY:
					ViewWindow = RA_OpenWindow(ViewWin);
					if(ViewWindow)
					{
						IIntuition->GetAttr(WINDOW_SigMask, ViewWin, &sigmask);
					}
					else
					{
						running = FALSE; // error re-opening window!
					}
					break;
				}
			}
		}
		IIntuition->DisposeObject(ViewWin);
	}

	IExec->FreeSysObject(ASOT_PORT, ViewMsgPort);

	if(ViewBuf)
	{
		IExec->FreeVec(ViewBuf);
		ViewBuf = NULL;
	}

	if(ViewScreen)
	{
		IIntuition->CloseScreen(ViewScreen);
		ViewScreen = NULL;
	}

	cleanupviewnode(viewnode);

	return 0;
}

Object *makeviewwindow(struct MsgPort *viewmsgport, STRPTR title)
{
	int16 Left = 128, Top = 128, Width = 1024, Height = 768;
	char arg[3][2] = {{0,0},{0,0},{0,0}};

	if(config->viewbits & VIEWBITS_INWINDOW)
	{
		if(config->viewtext_topleftx < 0)
			Left = 0;
		else
			Left = config->viewtext_topleftx;

		if(config->viewtext_toplefty < 0)
			Top = 0;
		else
			Top = config->viewtext_toplefty;

		if(config->viewtext_width <= 0)
			Width = Window->WScreen->Width + config->viewtext_width;
		else
			Width = config->viewtext_width;
		if(Width < 50)
			Width = 50;

		if(config->viewtext_height <= 0)
			Height = Window->WScreen->Height + config->viewtext_height;
		else
			Height = config->viewtext_height;
		if(Height < 50)
			Height = 50;
	}
	else
	{
		if(!(ViewScreen = IIntuition->OpenScreenTags(NULL, SA_Type, CUSTOMSCREEN, SA_Title, globstring[STR_TEXT_VIEWER_TITLE], SA_LikeWorkbench, TRUE)))
		{
			ViewScreen = NULL;
		}

		Left = 0;
		Top = ViewScreen->BarHeight + 1; // + 2;
		Width = ViewScreen->Width;
		Height = ViewScreen->Height - Top;
	}

	arg[0][0] = globstring[STR_VIEW_BUTTONS][0];
	arg[1][0] = globstring[STR_VIEW_BUTTONS][1];
	arg[2][0] = globstring[STR_VIEW_BUTTONS][2];

	return (WindowObject,
		(config->viewbits & VIEWBITS_TEXTBORDERS) ? WA_Title : TAG_IGNORE, title,
		(config->viewbits & VIEWBITS_TEXTBORDERS) ? WA_DragBar : TAG_IGNORE, TRUE,
		(config->viewbits & VIEWBITS_TEXTBORDERS) ? WA_CloseGadget : TAG_IGNORE, TRUE,
		(config->viewbits & VIEWBITS_TEXTBORDERS) ? WA_SizeGadget : TAG_IGNORE, TRUE,
		(config->viewbits & VIEWBITS_TEXTBORDERS) ? WA_DepthGadget : TAG_IGNORE, TRUE,
		WA_Activate, TRUE,
		WA_CustomScreen, ViewScreen,
		WA_Left, Left,
		WA_Top, Top,
		WA_Width, Width,
		WA_Height, Height,
		(config->viewbits & VIEWBITS_TEXTBORDERS) ? TAG_IGNORE : WA_Borderless, TRUE,
		WINDOW_AppPort, viewmsgport,
		WINDOW_IconifyGadget, TRUE,
		WINDOW_ParentGroup, VLayoutObject,
			LAYOUT_SpaceInner, TRUE,
			LAYOUT_SpaceOuter, TRUE,
			LAYOUT_AddChild, HLayoutObject,
				LAYOUT_AddChild, ViewTE = TextEditorObject,
					GA_ReadOnly, TRUE,
					GA_TEXTEDITOR_ReadOnly, TRUE,
					GA_TEXTEDITOR_FixedFont, TRUE,
					ICA_MAP, text2prop,
					ICA_TARGET, (struct Gadget *)ViewScroller,
				End,
				LAYOUT_AddChild, ViewScroller = ScrollerObject,
					SCROLLER_Orientation, SORIENT_VERT,
					SCROLLER_Arrows, TRUE,
					ICA_MAP, prop2text,
					ICA_TARGET, (struct Gadget *)ViewTE,
				End,
			End,
			LAYOUT_AddChild, HLayoutObject,
				LAYOUT_EvenSize, TRUE,
				LAYOUT_AddChild, SpaceObject, 
				End,
				LAYOUT_AddChild, ButtonObject,
					GA_ID, VIEW_SEARCH,
					GA_Text, arg[0],
					GA_RelVerify, TRUE,
				End,
				CHILD_WeightedWidth, 0,
				LAYOUT_AddChild, ButtonObject,
					GA_ID, VIEW_PRINT,
					GA_Text, arg[1],
					GA_RelVerify, TRUE,
				End,
				CHILD_WeightedWidth, 0,
				LAYOUT_AddChild, ButtonObject,
					GA_ID, VIEW_QUIT,
					GA_Text, arg[2],
					GA_RelVerify, TRUE,
				End,
				CHILD_WeightedWidth, 0,
			End,
			CHILD_WeightedHeight, 0,
		End,
	End);
}

void DisplayFile(struct Window *window, STRPTR file)
{
	int64 bytesread = 0;
	BPTR filehandle;
	struct ExamineData *data;

	if(ViewBuf != NULL)
	{
		IExec->FreeVec(ViewBuf);
	}

	if((filehandle = IDOS->Open(file, MODE_OLDFILE)))
	{
		if((data = IDOS->ExamineObjectTags(EX_FileHandleInput, filehandle, TAG_END)))
		{
			if((ViewBuf = IExec->AllocVecTags(data->FileSize + 1, AVT_Type, MEMF_PRIVATE, AVT_ClearWithValue, 0, TAG_END)))
			{
				while(bytesread < data->FileSize)
				{
					bytesread = bytesread + IDOS->Read(filehandle, ViewBuf, 64 * 1024);
				}

				if(bytesread == data->FileSize)
				{
					IIntuition->RefreshSetGadgetAttrs((struct Gadget *)ViewTE, window, NULL, GA_TEXTEDITOR_Contents, ViewBuf, TAG_DONE);
				}
			}
			IDOS->FreeDosObject(DOS_EXAMINEDATA, data);
		}
		IDOS->Close(filehandle);
	}
}

struct ViewNode *allocviewnode(STRPTR processname, STRPTR filename, STRPTR name, int function, STRPTR initialsearch, int wait)
{
	struct ViewNode *viewnode = NULL;

	if((viewnode = IExec->AllocSysObjectTags(ASOT_NODE, ASONODE_Size, sizeof(struct ViewNode), ASONODE_Name, filename, TAG_END)))
	{
		if((viewnode->processname = IExec->AllocVecTags(strlen(processname) + 1, AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END)))
		{
			IUtility->Strlcpy(viewnode->processname, processname, strlen(processname) + 1);
		}
		if((viewnode->filename = IExec->AllocVecTags(strlen(filename) + 1, AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END)))
		{
			if((viewnode->name = IExec->AllocVecTags(strlen(name) + 1, AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END)))
			{
				IUtility->Strlcpy(viewnode->filename, filename, strlen(filename) + 1);
				IUtility->Strlcpy(viewnode->name, name, strlen(name) + 1);
			}
		}
		if(initialsearch)
		{
			if((viewnode->initialsearch = IExec->AllocVecTags(strlen(initialsearch) + 1, AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END)))
			{
				IUtility->Strlcpy(viewnode->initialsearch, initialsearch, strlen(initialsearch) + 1);
			}
		}
		else
		{
			viewnode->initialsearch = NULL;
		}
		viewnode->function = function;
		viewnode->wait = wait;
		if(dopus_curwin[data_active_window]->flags & DWF_ARCHIVE)
		{
			viewnode->deleteonexit = TRUE;
		}
	}

	return viewnode;
}
void cleanupviewnode(struct ViewNode *viewnode)
{
	if(viewnode->filename)
	{
		IExec->FreeVec(viewnode->filename);
	}
	if(viewnode->name)
	{
		IExec->FreeVec(viewnode->name);
	}
	if(viewnode->initialsearch)
	{
		IExec->FreeVec(viewnode->initialsearch);
	}
	if(viewnode->processname)
	{
		IExec->FreeVec(viewnode->processname);
	}
	if(viewnode)
	{
		IExec->FreeSysObject(ASOT_NODE, viewnode);
	}

	return;
}
