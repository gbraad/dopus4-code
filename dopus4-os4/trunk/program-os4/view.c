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
#include "searchdata.h"

void view_file_process(void);
int view_loadfile(struct ViewData *);
int view_idcmp(struct ViewData *);
void view_display(struct ViewData *, int, int);
void view_displayall(struct ViewData *);
void view_print(struct ViewData *, char *, int, int);
void view_update_status(struct ViewData *);
void view_pageup(struct ViewData *);
void view_pagedown(struct ViewData *);
void view_gotop(struct ViewData *);
void view_gobottom(struct ViewData *);
void view_search(struct ViewData *, int);
void view_busy(struct ViewData *);
void view_unbusy(struct ViewData *);
void view_doscroll(struct ViewData *, int, int);
int view_lineup(struct ViewData *);
int view_linedown(struct ViewData *);
void view_status_text(struct ViewData *, char *);
void view_printtext(struct ViewData *, int);
void view_checkprint(struct ViewData *, int);
void view_makeuphex(struct ViewData *, char *, unsigned char *, int);
void view_togglescroll(struct ViewData *);
int view_setupdisplay(struct ViewData *);
void view_viewhilite(struct ViewData *, int, int, int, int);
void view_clearhilite(struct ViewData *, int);
void view_fix_scroll_gadget(struct ViewData *);
void view_clearsearch(struct ViewData *);
int view_simplerequest(struct ViewData *, char *, ...);
int view_whatsit(struct ViewData *, char *, int, char *);

struct ViewMessage
{
	char *filename;
	char *name;
	int function;
	char *initialsearch;
	struct ViewData *viewdata;
	BOOL deleteonexit;
};

struct viewhilite
{
	struct viewhilite *next;
	int x, y, x1, y1;
};

enum
{
	PEN_BACKGROUND,
	PEN_SHADOW,
	PEN_SHINE,
	PEN_TEXT,
	PEN_TEXTBACKGROUND,
	VIEWPEN_STATUSTEXT,
	VIEWPEN_STATUSBACKGROUND,

	VIEWPEN_LAST_COLOUR
};

enum
{
	VIEW_SCROLLGADGET,
	VIEW_PAGEUP,
	VIEW_PAGEDOWN,
	VIEW_GOTOP,
	VIEW_GOBOTTOM,
	VIEW_SEARCH,
	VIEW_PRINT,
	VIEW_QUIT,
	VIEW_JUMPTOPERCENT,
	VIEW_JUMPTOPERCENTNUM,
	VIEW_JUMPTOLINE,
	VIEW_JUMPTOLINE1,
	VIEW_JUMPTOLINE2,
	VIEW_LINECOUNT,
	VIEW_FILENAME,
	VIEW_ICONIFY,

	VIEW_GAD_COUNT
};

static struct NewWindow viewwin =
{
	0, 0, 0, 0, 255, 255, IDCMP_RAWKEY | IDCMP_VANILLAKEY | IDCMP_MOUSEBUTTONS | IDCMP_EXTENDEDMOUSE | IDCMP_GADGETUP | IDCMP_GADGETDOWN | IDCMP_MOUSEMOVE, 0, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, CUSTOMSCREEN
};

/* iconifygagdet stuff */ 
static struct Gadget *viewGadgets[VIEW_GAD_COUNT];
static struct Gadget *viewiconifygadget;
static struct Image *viewiconifyimage;

ULONG view_runcount;

int viewfile(STRPTR filename, STRPTR name, int function, STRPTR initialsearch, struct ViewData *viewdata, int wait, int noftype)
{
	struct ArbiterLaunch launch;
	struct ViewMessage *view_message;
	struct DOpusRemember *memkey = NULL;
	int flags;

	if(!noftype)
	{
		switch (function)
		{
		case FUNC_HEXREAD:
			if(checkfiletypefunc(filename, FTFUNC_HEXREAD))
				return (1);
			break;
		case FUNC_ANSIREAD:
			if(checkfiletypefunc(filename, FTFUNC_ANSIREAD))
				return (1);
			break;
		default:
			if(checkfiletypefunc(filename, FTFUNC_READ))
				return (1);
			break;
		}
	}

	if((view_message = IDOpus->LAllocRemember(&memkey, sizeof(struct ViewMessage), MEMF_CLEAR)) && copy_string(filename, &view_message->filename, &memkey) && copy_string(name, &view_message->name, &memkey) && copy_string(initialsearch, &view_message->initialsearch, &memkey))
	{

		launch.launch_code = (void *)&view_file_process;
		launch.launch_name = "dopus_view";
		launch.launch_memory = memkey;
		launch.data = (APTR)view_message;

		view_message->function = function;
		view_message->viewdata = NULL; //viewdata;
		if(dopus_curwin[data_active_window]->flags & DWF_ARCHIVE)
			view_message->deleteonexit = TRUE;

		if(wait || viewdata)
			flags = ARB_WAIT;
		else
			flags = 0;

		return(arbiter_command(ARBITER_LAUNCH, (APTR)&launch, flags));
	}

	IDOpus->LFreeRemember(&memkey);
	return (0);
}

void view_file_process()
{
//	int64 size;
	int size;
	int a, retcode = 100;
	char buf[60];
	struct ConUnit *view_console_unit = NULL;
	struct ViewData *vdata = NULL;
	struct Process *my_process;
	struct ArbiterMessage *my_startup_message;
	struct ViewMessage *view_msg;
	char *filename, *name;
	int function;
	char *initialsearch;
	struct ViewData *viewdata;
	struct MsgPort *view_port;
	char portname[20], titlebuf[300];

	my_process = (struct Process *)IExec->FindTask(NULL);
	IExec->WaitPort(&my_process->pr_MsgPort);
	my_startup_message = (struct ArbiterMessage *)IExec->GetMsg(&my_process->pr_MsgPort);

	view_msg = (struct ViewMessage *)(my_startup_message->data);
	filename = view_msg->filename;
	name = view_msg->name;
	function = view_msg->function;
	initialsearch = view_msg->initialsearch;
	viewdata = view_msg->viewdata;

	IExec->Forbid();
	for(a = 0;; a++)
	{
		sprintf(portname, "DOPUS_VIEW%d", a);
		if(!(IExec->FindPort(portname)))
			break;
	}
	IExec->Permit();

	if((view_port = IExec->CreatePort(portname, 0)))
	{
		if(IDOpus->CheckExist(filename, &size) < 0)
		{
			if(size > 0)
			{
				if(viewdata)
				{
					vdata = viewdata;

					vdata->view_search_string[0] = vdata->view_scroll_dir = vdata->view_scroll = vdata->view_last_line = vdata->view_last_charpos = vdata->view_text_offset = vdata->view_old_offset = vdata->view_line_count = vdata->view_top_buffer_pos = vdata->view_bottom_buffer_pos = 0;

					vdata->view_last_char = NULL;

					vdata->view_first_hilite = vdata->view_current_hilite = NULL;
				}
				else
				{
					if(!(vdata = IExec->AllocVecTags(sizeof(struct ViewData), AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END)))
					{
						retcode = -4;
					}
				}
				if(vdata)
				{
					strcpy(vdata->view_port_name, portname);
					vdata->view_port = view_port;
					vdata->view_file_size = size;
					vdata->view_file_name = name;
					vdata->view_path_name = filename;
					vdata->view_tab_size = config->tabsize;

					view_clearsearch(vdata);
					vdata->view_search_flags = search_flags;

					if(vdata->view_window)
					{
						IExec->Forbid();
						vdata->view_window->UserPort->mp_SigTask = (void *)my_process;
						IExec->Permit();
					}
					else
					{
						retcode = view_setupdisplay(vdata);
						if(retcode)
							goto view_end;
					}

					sprintf(titlebuf, "%s - \"%s\"", globstring[STR_FILE], vdata->view_path_name);
					if(config->viewbits & VIEWBITS_INWINDOW)
						IIntuition->SetWindowTitles(vdata->view_window, titlebuf, "Directory Opus Reader");
					else
						IIntuition->SetWindowTitles(vdata->view_window, (char *)-1, titlebuf);

					IIntuition->ActivateWindow(vdata->view_window);

					vdata->view_char_width = ((vdata->view_display_right - vdata->view_display_left) + 1) / vdata->view_font->tf_XSize;

					if(function == FUNC_HEXREAD)
					{
						vdata->view_char_width = (vdata->view_char_width > 62) ? 62 : vdata->view_char_width;
						vdata->view_max_line_length = 16;
						vdata->view_display_as_hex = 1;
					}
					else
					{
						vdata->view_max_line_length = vdata->view_char_width + 1;
						vdata->view_display_as_hex = 0;
					}

					view_status_text(vdata, globstring[STR_READING_FILE]);

					if(view_loadfile(vdata))
					{
						vdata->view_last_char = vdata->view_text_buffer + vdata->view_buffer_size - 16;
						vdata->view_last_charpos = vdata->view_file_size - (vdata->view_last_char - vdata->view_text_buffer);

						view_status_text(vdata, globstring[STR_COUNTING_LINES]);

						if(function == FUNC_SMARTREAD)
						{
							if((vdata->view_line_count = smartcountlines(vdata)) == -1)
							{
								vdata->view_display_as_hex = 1;
								vdata->view_max_line_length = 16;
								vdata->view_char_width = 62;
							}
							else if(vdata->view_line_count == -2)
								function = FUNC_ANSIREAD;
						}

						if(function == FUNC_ANSIREAD)
						{
							vdata->view_line_count = ansicountlines(vdata);
						}
						else
						{
							if(vdata->view_display_as_hex)
							{
								vdata->view_line_count = vdata->view_file_size / 16;
								if(vdata->view_file_size < 16)
								{
									vdata->view_line_count = 1;
								}
								else if(vdata->view_file_size % 16 != 0)
								{
									++vdata->view_line_count;
								}
							}
							else
							{
								vdata->view_line_count = countlines(vdata);
							}
						}
						vdata->view_last_line = vdata->view_line_count - vdata->view_lines_per_screen;
						if(vdata->view_last_line < 0)
							vdata->view_last_line = 0;

						if(config->viewbits & VIEWBITS_TEXTBORDERS)
						{
							IGadTools->GT_SetGadgetAttrs(viewGadgets[VIEW_LINECOUNT], vdata->view_window, NULL, GTNM_Number, vdata->view_line_count, TAG_END);
							view_update_status(vdata);
							view_status_text(vdata, name);
						}

						if(function == FUNC_ANSIREAD && !vdata->view_display_as_hex)
						{
							vdata->view_display_as_ansi = 1;

							if((vdata->view_ansiread_window = IIntuition->OpenWindowTags(NULL, WA_Left, viewwin.LeftEdge, WA_Top, viewwin.TopEdge, WA_Width, vdata->view_char_width * vdata->view_font->tf_XSize, WA_Height, vdata->view_font->tf_YSize * 6, WA_Flags, WFLG_SMART_REFRESH | WFLG_BORDERLESS | WFLG_BACKDROP, WA_CustomScreen, (Tag) vdata->view_screen, WA_AutoAdjust, TRUE, TAG_END)))
							{
								IIntuition->WindowToBack(vdata->view_ansiread_window);
								IGraphics->SetFont(vdata->view_ansiread_window->RPort, vdata->view_font);
								vdata->view_console_request.io_Data = (APTR) vdata->view_ansiread_window;
								vdata->view_console_request.io_Length = sizeof(struct Window);

								if(IExec->OpenDevice("console.device", CONU_STANDARD, (struct IORequest *)&vdata->view_console_request, 0))
								{
									vdata->view_display_as_ansi = 0;
								}
								else
								{
									view_console_unit = (struct ConUnit *)vdata->view_console_request.io_Unit;

									for(a = 0; a < MAXTABS; a++)
										view_console_unit->cu_TabStops[a] = a * config->tabsize;
									view_console_unit->cu_TabStops[MAXTABS - 1] = 0xffff;

									sprintf(buf, "\x9b\x30\x20\x70\x9b%d\x75\x9b%d\x74", vdata->view_max_line_length + 1, vdata->view_lines_per_screen);
									view_print(vdata, buf, 1, strlen(buf));

									vdata->view_max_line_length = 255;
								}
							}
							else
							{
								vdata->view_display_as_ansi = 0;
							}
						}
						else
						{
							vdata->view_display_as_ansi = 0;
						}

						vdata->view_scroll_width = vdata->view_char_width * vdata->view_font->tf_XSize;

						view_displayall(vdata);
						view_unbusy(vdata);

						if(initialsearch)
						{
							strcpy(vdata->view_search_string, initialsearch);
							view_search(vdata, 1);
						}

						retcode = view_idcmp(vdata);
					}
					if(vdata->view_text_buffer)
						IExec->FreeVec(vdata->view_text_buffer);
					if(view_console_unit)
						IExec->CloseDevice((struct IORequest *)&vdata->view_console_request);
					if(vdata->view_ansiread_window)
					{
						IIntuition->CloseWindow(vdata->view_ansiread_window);
						vdata->view_ansiread_window = NULL;
					}
					view_clearhilite(vdata, 0);
					if(!viewdata)
					{
						cleanupviewfile(vdata);
						IExec->FreeVec(vdata);
					}
					else
					{
						IDOpus->SetBusyPointer(vdata->view_window);
					}
				}
			}
			else
				retcode = -3;
		}
		else
			retcode = -2;
	      view_end:
		IExec->DeletePort(view_port);
	}
	my_startup_message->command = retcode;
	if(view_msg->deleteonexit)
		IDOS->DeleteFile(filename);
	IExec->Forbid();
	IExec->ReplyMsg((struct Message *)my_startup_message);
	return;
}

void cleanupviewfile(struct ViewData *vdata)
{
	if(config->viewbits & VIEWBITS_INWINDOW)
	{
		if(vdata->view_window)
		{
			config->viewtext_topleftx = vdata->view_window->LeftEdge;
			config->viewtext_toplefty = vdata->view_window->TopEdge;
			IIntuition->RemoveGadget(vdata->view_window, viewiconifygadget);
			if(view_runcount == 0)
			{
				IIntuition->DisposeObject(viewiconifygadget);
				IIntuition->DisposeObject(viewiconifyimage);
			}

			IIntuition->CloseWindow(vdata->view_window);
		}
	}
	else if(vdata->view_screen)
	{
		IIntuition->ScreenToBack(vdata->view_screen);
		if(vdata->view_window)
			IIntuition->CloseWindow(vdata->view_window);
		IIntuition->CloseScreen(vdata->view_screen);
		if(vdata->view_font)
			IGraphics->CloseFont(vdata->view_font);
	}
	IGadTools->FreeGadgets(vdata->view_gadgets);
	vdata->view_gadgets = NULL;
	IGadTools->FreeVisualInfo(vdata->view_GTvi);
	vdata->view_GTvi = NULL;
	IDOpus->LFreeRemember(&vdata->view_memory);
}

int view_loadfile(struct ViewData *vdata)
{
	struct IntuiMessage *msg;
	int a, in, fsize = 0;
	char *bufpos;
	int chunk, done, stop = 0;

	vdata->view_buffer_size = ((vdata->view_file_size >> 4) + 1) << 4;

	for(a = 0; a < 2; a++)
	{
		if((vdata->view_text_buffer = IExec->AllocVecTags(vdata->view_buffer_size, AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END)))
			break;
		view_status_text(vdata, globstring[STR_NO_MEMORY_TO_DECRUNCH]);
		if((vdata->view_buffer_size = IExec->AvailMem(MEMF_PUBLIC | MEMF_LARGEST)) < 16)
			break;
		if(vdata->view_buffer_size <= vdata->view_file_size)
			vdata->view_file_size = vdata->view_buffer_size;
	}
	if(!vdata->view_text_buffer)
		return 0;;
	if(!(in = IDOS->Open(vdata->view_path_name, MODE_OLDFILE)))
		return 0;

	bufpos = vdata->view_text_buffer;
	chunk = 64 * 1024;
	for(done = 0; (done < vdata->view_file_size) && !stop;)
	{
		if((done + chunk) > vdata->view_file_size)
			chunk = vdata->view_file_size - done;
		fsize = IDOS->Read(in, bufpos, chunk);
		while((msg = IGadTools->GT_GetIMsg(vdata->view_window->UserPort)))
		{
			if(msg->Class == IDCMP_REFRESHWINDOW)
			{
				IGadTools->GT_BeginRefresh(vdata->view_window);
				IGadTools->GT_EndRefresh(vdata->view_window, TRUE);
				IIntuition->RefreshGadgets(viewiconifygadget, vdata->view_window, NULL);
			}
			else if((msg->Class == IDCMP_MOUSEBUTTONS) && (msg->Code == MENUDOWN))
			{
				stop = 1;
			}
			IGadTools->GT_ReplyIMsg(msg);
		}
		if(fsize <= 0)
			break;
		bufpos += fsize;
		done += fsize;
		fsize = done;
	}
	if((vdata->view_buffer_size != vdata->view_file_size) || ((fsize > -1) && (fsize != vdata->view_file_size)))
		vdata->view_file_size = fsize;
	IDOS->Close(in);
	if(fsize == -1)
		return 0;
	view_busy(vdata);
	return 1;
}

int view_idcmp(struct ViewData *vdata)
{
	struct IntuiMessage *msg;
	short scroll_speed, scroll_pos[7];
	ULONG iclass;
	USHORT code, qual, gadgetid;
	struct Gadget *gadget;
	char buf[60];
	int a, b, c;
	int retcode = 1;
	BOOL quit = FALSE;

	struct DiskObject *viewdobj = NULL;
	struct AppIcon *viewappicon = NULL;
	struct MsgPort *view_appport = NULL;

	for(a = 0; a < 7; a++)
	{
		scroll_pos[a] = (vdata->view_window->Height / 8) * (a + 1);
	}

	view_runcount++;

	while(!quit)
	{
		while((msg = IGadTools->GT_GetIMsg(vdata->view_window->UserPort)))
		{
			iclass = msg->Class;
			code = msg->Code;
			qual = msg->Qualifier;
			gadget = (struct Gadget *)msg->IAddress;
			IGadTools->GT_ReplyIMsg(msg);

			switch (iclass)
			{
			case IDCMP_REFRESHWINDOW:
				IGadTools->GT_BeginRefresh(vdata->view_window);
				IGadTools->GT_EndRefresh(vdata->view_window, TRUE);
				IIntuition->RefreshGadgets(viewiconifygadget, vdata->view_window, NULL);
				break;
			case IDCMP_CLOSEWINDOW:
				view_runcount--;
				retcode = -1;
				quit = TRUE;
				break;

			case IDCMP_MOUSEMOVE:
				if(gadget->GadgetID == VIEW_SCROLLGADGET)
					view_fix_scroll_gadget(vdata);
				break;
			case IDCMP_EXTENDEDMOUSE:
				{
					struct IntuiWheelData *iwd = msg->IAddress;
					if(code & IMSGCODE_INTUIWHEELDATA)
					{
						if(iwd->WheelY < 0)
						{
							view_lineup(vdata);
						}
						else if(iwd->WheelY > 0)
						{
							view_linedown(vdata);
						}
					}
				}
				break;
			case IDCMP_MOUSEBUTTONS:
				if(code == SELECTDOWN)
				{
					view_togglescroll(vdata);
				}
				else if(code == MENUDOWN)
				{
					retcode = -1;
					quit = TRUE;
				}
				break;

			case IDCMP_GADGETDOWN:
				if(vdata->view_scroll)
				{
					view_togglescroll(vdata);
					break;
				}
				switch (gadget->GadgetID)
				{
				case VIEW_SCROLLGADGET:
					view_fix_scroll_gadget(vdata);
					break;
				}
				break;
			case IDCMP_GADGETUP:
				if(vdata->view_scroll)
				{
					view_togglescroll(vdata);
					break;
				}
				gadgetid = gadget->GadgetID;
			      testgad:
				switch (gadgetid)
				{
				case VIEW_ICONIFY:
					if(!view_appport)
					{
						view_appport = IExec->CreatePort(NULL, 0);
					}
					if(!viewdobj)
					{
						viewdobj = IIcon->GetDiskObject("ENV:Sys/def_ascii");
					}
					if(viewdobj && view_appport)
					{
						struct AppMessage *viewappmsg;
						viewappicon = IWorkbench->AddAppIcon(0, 0, vdata->view_file_name, view_appport, 0, viewdobj, NULL);
						IIntuition->HideWindow(vdata->view_window);
						IExec->Wait(1 << view_appport->mp_SigBit);
						while((viewappmsg = (struct AppMessage *)IExec->GetMsg(view_appport)))
						{
							switch(viewappmsg->am_Type)
							{
							case AMTYPE_APPICON:
								if((viewappmsg->am_NumArgs == 0) && (viewappmsg->am_ArgList == NULL))
								{
									IWorkbench->RemoveAppIcon(viewappicon);
									IIcon->FreeDiskObject(viewdobj);
									IIntuition->ShowWindow(vdata->view_window, WINDOW_FRONTMOST);
									IExec->DeletePort(view_appport);
								}
								break;
							}
							IExec->ReplyMsg((struct Message *)viewappmsg);
						}

					}
					else
					{
						IIcon->FreeDiskObject(viewdobj);
						IExec->DeletePort(view_appport);
					}
					break;
				case VIEW_SCROLLGADGET:
					view_fix_scroll_gadget(vdata);
					break;
				case VIEW_PAGEUP:
					view_pageup(vdata);
					break;
				case VIEW_PAGEDOWN:
					view_pagedown(vdata);
					break;
				case VIEW_GOTOP:
					view_gotop(vdata);
					break;
				case VIEW_GOBOTTOM:
					view_gobottom(vdata);
					break;
				case VIEW_SEARCH:
					view_search(vdata, 1);
					break;
				case VIEW_QUIT:
					quit = TRUE;
					break;
				case VIEW_PRINT:
					view_checkprint(vdata, 0);
					break;
				case VIEW_JUMPTOLINE:
				case VIEW_JUMPTOPERCENT:
					view_busy(vdata);
					buf[0] = 0;
					if(gadgetid == VIEW_JUMPTOLINE)
					{
						a = 10;
						b = STR_JUMP_TO_LINE;
					}
					else
					{
						a = 4;
						b = STR_JUMP_TO_PERCENTAGE;
					}
					if(!(view_whatsit(vdata, globstring[b], a, buf)) || !buf[0])
					{
						view_unbusy(vdata);
						break;
					}
					a = atoi(buf);
					if(a < 0)
						a = 0;
					if(gadgetid == VIEW_JUMPTOPERCENTNUM)
						a = ((a + 1) * (vdata->view_last_line)) / 100;
					if(a > vdata->view_last_line)
						a = vdata->view_last_line;

					if(vdata->view_display_as_hex)
					{
						if(vdata->view_text_offset != a)
						{
							vdata->view_text_offset = a;
							view_displayall(vdata);
						}
						view_unbusy(vdata);
						break;
					}

					if(vdata->view_text_offset != a)
					{
						if(vdata->view_text_offset < a)
						{
							if(a - vdata->view_text_offset < vdata->view_last_line - a)
							{
								for(b = vdata->view_text_offset; b < a; b++)
								{
									for(c = vdata->view_top_buffer_pos; c < vdata->view_file_size; c++)
										if(vdata->view_text_buffer[c] == 10)
											break;
									vdata->view_top_buffer_pos = c + 1;
								}
							}
							else
							{
								vdata->view_text_offset = vdata->view_last_line;
								vdata->view_top_buffer_pos = vdata->view_file_size;
								for(c = 0; c < vdata->view_lines_per_screen; c++)
								{
									for(b = vdata->view_top_buffer_pos - 2; b >= 0; b--)
										if(vdata->view_text_buffer[b] == 10)
											break;
									vdata->view_top_buffer_pos = b + 1;
								}
								for(b = a; b < vdata->view_text_offset; b++)
								{
									for(c = vdata->view_top_buffer_pos - 2; c >= 0; c--)
										if(vdata->view_text_buffer[c] == 10)
											break;
									vdata->view_top_buffer_pos = c + 1;
								}
							}
						}
						else
						{
							if(vdata->view_text_offset - a < a)
							{
								for(b = a; b < vdata->view_text_offset; b++)
								{
									for(c = vdata->view_top_buffer_pos - 2; c >= 0; c--)
										if(vdata->view_text_buffer[c] == 10)
											break;
									vdata->view_top_buffer_pos = c + 1;
								}
							}
							else
							{
								vdata->view_top_buffer_pos = 0;
								for(b = 0; b < a; b++)
								{
									for(c = vdata->view_top_buffer_pos; c < vdata->view_file_size; c++)
										if(vdata->view_text_buffer[c] == 10)
											break;
									vdata->view_top_buffer_pos = c + 1;
								}
							}
						}
						vdata->view_text_offset = a;
						view_displayall(vdata);
					}
					view_unbusy(vdata);
					break;
				}
				break;
			case IDCMP_VANILLAKEY:
				code = toupper(code);
				if(strchr(globstring[STR_VIEW_BUTTONS], code))
				{
					switch ((ULONG) strchr(globstring[STR_VIEW_BUTTONS], code) - (ULONG) globstring[STR_VIEW_BUTTONS])
					{
					case 0:
						view_pageup(vdata);
						break;
					case 1:
						view_pagedown(vdata);
						break;
					case 2:
						view_gotop(vdata);
						break;
					case 3:
						view_gobottom(vdata);
						break;
					case 4:
						view_search(vdata, 1);
						break;
					case 5:
						view_checkprint(vdata, 0);
						break;
					case 6:
						quit = TRUE;
						break;
					}
				}
				else
					switch (code)
					{
					case 0x08:	// BackSpace
						view_pageup(vdata);
						break;
					case ' ':	// Space
						view_pagedown(vdata);
						break;
					case '/':
						view_search(vdata, 1);
						break;
					case 0x1B:	// ESC
						retcode = -1;
					case 'X':
						quit = TRUE;
						break;
					case 'J':
						gadgetid = VIEW_JUMPTOLINE;
						goto testgad;
					case 'N':
						view_search(vdata, 0);
						break;
					case 'C':
						view_checkprint(vdata, 1);
						break;
					default:
						if(qual & IEQUALIFIER_NUMERICPAD)
							switch (code)
							{
							case '1':	// End
								view_gobottom(vdata);
								break;
							case '7':	// Home
								view_gotop(vdata);
								break;
							case '9':	// PageUp
								view_pageup(vdata);
								break;
							case '3':	// PageDown
								view_pagedown(vdata);
								break;
							case '8':	// CrsrUp
								if(vdata->view_line_count <= vdata->view_lines_per_screen)
									break;
								{
									if(qual & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
										view_pageup(vdata);
									else if(qual & IEQUALIFIER_CONTROL)
										view_gotop(vdata);
									else
										view_lineup(vdata);
								}
								break;
							case '2':	// CrsrDown
								if(vdata->view_line_count <= vdata->view_lines_per_screen)
									break;
								{
									if(qual & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
										view_pagedown(vdata);
									else if(qual & IEQUALIFIER_CONTROL)
										view_gobottom(vdata);
									else
										view_linedown(vdata);
								}
								break;
							}
						break;
					}
				break;
			case IDCMP_RAWKEY:
				if(vdata->view_scroll && code != 0x40)
				{
					view_togglescroll(vdata);
					break;
				}
				switch (code)
				{

				case RAWKEY_PAGEUP:
					view_pageup(vdata);
					break;
				case RAWKEY_PAGEDOWN:
					view_pagedown(vdata);
					break;
				case RAWKEY_HOME:
					view_gotop(vdata);
					break;
				case RAWKEY_END:
					view_gobottom(vdata);
					break;
				case RAWKEY_CRSRUP:
					if(vdata->view_line_count <= vdata->view_lines_per_screen)
						break;
					{
						if(qual & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
							view_pageup(vdata);
						else if(qual & IEQUALIFIER_CONTROL)
							view_gotop(vdata);
						else
							view_lineup(vdata);
					}
					break;
				case RAWKEY_CRSRDOWN:
					if(vdata->view_line_count <= vdata->view_lines_per_screen)
						break;
					{
						if(qual & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
							view_pagedown(vdata);
						else if(qual & IEQUALIFIER_CONTROL)
							view_gobottom(vdata);
						else
							view_linedown(vdata);
					}
					break;
				}
				break;
			}
		}
		if(((struct IntuitionBase *)(IIntuition->Data.LibBase))->ActiveWindow == vdata->view_window && vdata->view_scroll)
		{
			a = vdata->view_window->MouseY;
			if(a < scroll_pos[3])
			{
				if(a < scroll_pos[0])
					scroll_speed = 1;
				else if(a < scroll_pos[1])
					scroll_speed = vdata->view_font->tf_YSize / 4;
				else if(a < scroll_pos[2])
					scroll_speed = vdata->view_font->tf_YSize / 2;
				else if(a < scroll_pos[3] - 10)
					scroll_speed = vdata->view_font->tf_YSize;
				else
					scroll_speed = 0;

				if(scroll_speed > 0 && vdata->view_line_count > vdata->view_lines_per_screen)
				{
					--vdata->view_text_offset;
					if(vdata->view_text_offset < 0)
					{
						vdata->view_text_offset = 0;
						view_togglescroll(vdata);
					}
					else
					{
						if(vdata->view_scroll_dir == 1)
							view_display(vdata, scroll_speed, 1);
						else
							view_display(vdata, scroll_speed, 0);
					}
					vdata->view_scroll_dir = -1;
				}
			}
			else
			{
				if(a > scroll_pos[6])
					scroll_speed = 1;
				else if(a > scroll_pos[5])
					scroll_speed = vdata->view_font->tf_YSize / 4;
				else if(a > scroll_pos[4])
					scroll_speed = vdata->view_font->tf_YSize / 2;
				else if(a > scroll_pos[3] + 10)
					scroll_speed = vdata->view_font->tf_YSize;
				else
					scroll_speed = 0;
				if(scroll_speed > 0 && vdata->view_line_count > vdata->view_lines_per_screen)
				{
					++vdata->view_text_offset;
					vdata->view_scroll_dir = 1;
					if(vdata->view_text_offset > vdata->view_last_line)
					{
						vdata->view_text_offset = vdata->view_last_line;
						view_togglescroll(vdata);
					}
					else
						view_display(vdata, scroll_speed, 0);
				}
			}
			continue;
		}
		IExec->Wait((1 << vdata->view_window->UserPort->mp_SigBit));
	}
	return retcode;
}

int view_setupdisplay(struct ViewData *vdata)
{
	static char viewGNames[7 * 2];
	static struct TextAttr viewGta;
	static ULONG viewGTags[] =
	{
		GTSC_Arrows, 12, PGA_Freedom, LORIENT_VERT, GA_RelVerify, TRUE, GA_Immediate, TRUE, TAG_DONE,
		GTNM_FrontPen, 0, GTNM_BackPen, 0, GTNM_Justification, GTJ_RIGHT, GTNM_Format, (ULONG) "%3ld%%", TAG_DONE,
		GTNM_FrontPen, 0, GTNM_BackPen, 0, GTNM_Justification, GTJ_RIGHT, TAG_DONE,
		GTNM_FrontPen, 0, GTNM_BackPen, 0, GTNM_Format, (ULONG) " - %ld", TAG_DONE,
		GTNM_FrontPen, 0, GTNM_BackPen, 0, GTNM_Format, (ULONG) " / %ld", TAG_DONE,
		GTTX_FrontPen, 0, GTTX_BackPen, 0, GTTX_Clipped, TRUE, TAG_DONE,
	};

	struct Gadget *g;
	struct NewGadget ng = { 0 };
	int tc = 0;
	short int a, width, fontx;
	uint16 zoom[4] = { ~0, ~0, config->viewtext_width, config->viewtext_height };
	struct TextAttr screenattr;

	vdata->view_colour_table[PEN_BACKGROUND] = 0;
	vdata->view_colour_table[PEN_SHADOW] = config->gadgetbotcol;
	vdata->view_colour_table[PEN_SHINE] = config->gadgettopcol;
	vdata->view_colour_table[PEN_TEXT] = 1;
	vdata->view_colour_table[PEN_TEXTBACKGROUND] = 0;
	vdata->view_colour_table[VIEWPEN_STATUSTEXT] = config->statusfg;
	vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND] = config->statusbg;
	vdata->view_colour_table[VIEWPEN_LAST_COLOUR] = -1;

	viewwin.BlockPen = vdata->view_colour_table[PEN_SHINE];

	vdata->view_vis_info.vi_fg = vdata->view_colour_table[VIEWPEN_STATUSTEXT];
	vdata->view_vis_info.vi_bg = vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND];
	vdata->view_vis_info.vi_shine = vdata->view_colour_table[PEN_SHINE];
	vdata->view_vis_info.vi_shadow = vdata->view_colour_table[PEN_SHADOW];

	vdata->view_vis_info.vi_stringcol[0] = vdata->view_colour_table[PEN_TEXT];
	vdata->view_vis_info.vi_stringcol[1] = vdata->view_colour_table[PEN_TEXTBACKGROUND];
	vdata->view_vis_info.vi_activestringcol[0] = vdata->view_colour_table[PEN_TEXT];
	vdata->view_vis_info.vi_activestringcol[1] = vdata->view_colour_table[PEN_TEXTBACKGROUND];
	if(config->viewbits & VIEWBITS_INWINDOW)
	{
		vdata->view_font = scr_font[FONT_TEXT];

		if(config->viewtext_topleftx < 0)
			viewwin.LeftEdge = 0;
		else
			viewwin.LeftEdge = config->viewtext_topleftx;

		if(config->viewtext_toplefty < 0)
			viewwin.TopEdge = 0;
		else
			viewwin.TopEdge = config->viewtext_toplefty;

		if(config->viewtext_width <= 0)
			viewwin.Width = Window->WScreen->Width + config->viewtext_width;
		else
			viewwin.Width = config->viewtext_width;
		if(viewwin.Width < 50)
			viewwin.Width = 50;

		if(config->viewtext_height <= 0)
			viewwin.Height = Window->WScreen->Height + config->viewtext_height;
		else
			viewwin.Height = config->viewtext_height;
		if(viewwin.Height < 50)
			viewwin.Height = 50;

		viewwin.Flags = WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_RMBTRAP;
		viewwin.IDCMPFlags |= IDCMP_CLOSEWINDOW;
		viewwin.Screen = Window->WScreen;
	}
	else
	{
		screenattr.ta_Name=scr_font[FONT_TEXT]->tf_Message.mn_Node.ln_Name;
		screenattr.ta_YSize=scr_font[FONT_TEXT]->tf_YSize;

//		if(!(vdata->view_screen = open_subprocess_screen(globstring[STR_TEXT_VIEWER_TITLE], scr_font[FONT_TEXT], &vdata->view_memory, NULL)) || !(vdata->view_font = IDiskfont->OpenDiskFont(vdata->view_screen->Font)))
		if(!(vdata->view_screen = IIntuition->OpenScreenTags(NULL, SA_Type, CUSTOMSCREEN, SA_Title, globstring[STR_TEXT_VIEWER_TITLE], SA_Font, &screenattr, SA_Interleaved, TRUE, SA_LikeWorkbench, TRUE, TAG_END)) || !(vdata->view_font = IDiskfont->OpenDiskFont(vdata->view_screen->Font)))
		{
			return -4;
		}
		viewwin.Screen = vdata->view_screen;
		viewwin.TopEdge = vdata->view_screen->BarHeight + 2;
		viewwin.Width = vdata->view_screen->Width;
		viewwin.Height = vdata->view_screen->Height - viewwin.TopEdge;
		viewwin.Flags = WFLG_BORDERLESS | WFLG_RMBTRAP;
	}

	if(config->viewbits & VIEWBITS_TEXTBORDERS)
	{
		viewwin.IDCMPFlags |= ARROWIDCMP | SCROLLERIDCMP | NUMBERIDCMP | TEXTIDCMP;
	}

	if(config->viewbits & VIEWBITS_INWINDOW)
	{
		vdata->view_window = IIntuition->OpenWindowTags(&viewwin, WA_AutoAdjust, TRUE, WA_Zoom, &zoom, TAG_END);
	}
	else
	{
		vdata->view_window = IIntuition->OpenWindowTags(&viewwin, WA_AutoAdjust, TRUE, TAG_END);
	}
	if(!(vdata->view_window))
	{
		if(!(config->viewbits & VIEWBITS_INWINDOW))
		{
			IIntuition->CloseScreen(vdata->view_screen);
		}
		IDOpus->LFreeRemember(&vdata->view_memory);
		return -4;
	}
	else
	{
		if(config->viewbits & VIEWBITS_INWINDOW)
		{
			struct DrawInfo *DRI;

			DRI = IIntuition->GetScreenDrawInfo(vdata->view_window->WScreen);
			viewiconifyimage = (struct Image *)IIntuition->NewObject(NULL, "sysiclass", SYSIA_DrawInfo, DRI, SYSIA_Which, ICONIFYIMAGE, TAG_END);
			if(viewiconifyimage)
			{
				viewiconifygadget = (struct Gadget *)IIntuition->NewObject(NULL, "buttongclass", GA_ID, VIEW_ICONIFY, GA_RelVerify, TRUE, GA_Image, viewiconifyimage, GA_TopBorder, TRUE, GA_RelRight, 0, GA_Titlebar, TRUE, TAG_END);
				if(viewiconifygadget)
				{
					IIntuition->AddGadget(vdata->view_window, viewiconifygadget, ~0);
					IIntuition->RefreshGadgets(viewiconifygadget, vdata->view_window, NULL);
				}
			}
			IIntuition->FreeScreenDrawInfo(vdata->view_window->WScreen, DRI);
		}
	}
	vdata->view_screen = vdata->view_window->WScreen;
	vdata->view_rastport = vdata->view_window->RPort;

	fontx = vdata->view_font->tf_XSize;

	viewGta.ta_Name = vdata->view_font->tf_Message.mn_Node.ln_Name, viewGta.ta_YSize = vdata->view_font->tf_YSize, vdata->view_status_bar_ypos = vdata->view_window->Height - (vdata->view_font->tf_YSize + 1);

	if(config->viewbits & VIEWBITS_INWINDOW)
	{
		vdata->view_status_bar_ypos -= vdata->view_window->BorderBottom;

		vdata->view_display_left = vdata->view_window->BorderLeft;
		vdata->view_display_top = vdata->view_window->BorderTop;
		vdata->view_display_right = -vdata->view_window->BorderRight;
	}

	if(config->viewbits & VIEWBITS_TEXTBORDERS)
	{
		vdata->view_display_left += 2;
		vdata->view_display_top += 2;
		vdata->view_display_right += vdata->view_window->Width - 20;
		vdata->view_display_bottom = vdata->view_status_bar_ypos - 3;

		vdata->view_lines_per_screen = (vdata->view_display_bottom - vdata->view_display_top) / vdata->view_font->tf_YSize;
	}
	else
	{
		vdata->view_lines_per_screen = (vdata->view_window->Height - vdata->view_display_top - 1) / vdata->view_font->tf_YSize;

		vdata->view_display_bottom = vdata->view_display_top + (vdata->view_lines_per_screen * vdata->view_font->tf_YSize) - 1;
		vdata->view_display_right += vdata->view_window->Width - 1;
	}

	vdata->view_display_height = vdata->view_lines_per_screen * vdata->view_font->tf_YSize;

	width = vdata->view_window->Width;
	if(config->viewbits & VIEWBITS_INWINDOW)
		width -= vdata->view_window->BorderRight + vdata->view_window->BorderLeft;

	IGraphics->SetFont(vdata->view_rastport, vdata->view_font);

	IGraphics->SetAPen(vdata->view_rastport, vdata->view_colour_table[PEN_TEXTBACKGROUND]);
	IGraphics->RectFill(vdata->view_rastport, vdata->view_display_left, vdata->view_display_top, vdata->view_display_right, vdata->view_display_bottom);

	g = IGadTools->CreateContext(&vdata->view_gadgets);

	if(config->viewbits & VIEWBITS_TEXTBORDERS)
	{
		/* text display border */
		IDOpus->Do3DBox(vdata->view_rastport, vdata->view_display_left, vdata->view_display_top - 1, (vdata->view_display_right - vdata->view_display_left) + 2, vdata->view_display_bottom - vdata->view_display_top + 2, vdata->view_colour_table[PEN_SHINE], vdata->view_colour_table[PEN_SHADOW]);
		/* status border */
		IDOpus->Do3DBox(vdata->view_rastport, vdata->view_display_left, vdata->view_status_bar_ypos, width - ((fontx * 7 * 2) + 4), vdata->view_font->tf_YSize, vdata->view_colour_table[PEN_SHINE], vdata->view_colour_table[PEN_SHADOW]);
		/* status bar */
		IGraphics->SetAPen(vdata->view_rastport, vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND]);
		IGraphics->RectFill(vdata->view_rastport, vdata->view_display_left, vdata->view_status_bar_ypos, vdata->view_window->Width - ((config->viewbits & VIEWBITS_INWINDOW) ? vdata->view_window->BorderRight : 0) - ((fontx * 7 * 2) + 3), vdata->view_window->Height - 2 - ((config->viewbits & VIEWBITS_INWINDOW) ? vdata->view_window->BorderBottom : 0));

		if(config->viewbits & VIEWBITS_INWINDOW)
			width += vdata->view_window->BorderLeft;

		vdata->view_GTvi = IGadTools->GetVisualInfoA(vdata->view_screen, NULL);

		ng.ng_VisualInfo = vdata->view_GTvi;
		ng.ng_TextAttr = &viewGta;

		ng.ng_LeftEdge = vdata->view_display_right + 4;
		ng.ng_TopEdge = vdata->view_display_top - 2;
		ng.ng_Height = vdata->view_display_bottom - vdata->view_display_top + 4;
		ng.ng_Width = 16;
		ng.ng_GadgetID = VIEW_SCROLLGADGET;

		viewGadgets[VIEW_SCROLLGADGET] = g = IGadTools->CreateGadgetA(SCROLLER_KIND, g, &ng, (struct TagItem *)&viewGTags[tc]);

		while(viewGTags[tc])
			tc += 2;
		tc++;

		ng.ng_TopEdge = vdata->view_status_bar_ypos - 1;
		ng.ng_Height = vdata->view_font->tf_YSize + 2;
		ng.ng_Flags = PLACETEXT_IN;
		ng.ng_Width = fontx * 2;
		ng.ng_LeftEdge = width - ng.ng_Width;
		ng.ng_GadgetID = VIEW_QUIT;

		for(a = 6; a >= 0; a--)
		{
			viewGNames[a * 2] = globstring[STR_VIEW_BUTTONS][a];

			ng.ng_GadgetText = viewGNames + a * 2;

			viewGadgets[VIEW_PAGEUP + a] = g = IGadTools->CreateGadgetA(BUTTON_KIND, g, &ng, NULL);

			if(a)
				ng.ng_LeftEdge -= ng.ng_Width;
			ng.ng_GadgetID--;
		}

		ng.ng_GadgetText = NULL;
		ng.ng_Flags = 0;

		ng.ng_Width = fontx * 5;
		ng.ng_LeftEdge -= ng.ng_Width;
		ng.ng_GadgetID = VIEW_JUMPTOPERCENT;

		viewGadgets[VIEW_JUMPTOPERCENT] = g = IGadTools->CreateGadgetA(GENERIC_KIND, g, &ng, NULL);
		g->Flags = GFLG_GADGHCOMP;
		g->Activation = GACT_RELVERIFY;
		g->GadgetType |= GTYP_BOOLGADGET;

		ng.ng_Width -= fontx;
		ng.ng_GadgetID = VIEW_JUMPTOPERCENTNUM;

		viewGTags[tc + 1] = vdata->view_colour_table[VIEWPEN_STATUSTEXT];
		viewGTags[tc + 3] = vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND];

		viewGadgets[VIEW_JUMPTOPERCENTNUM] = g = IGadTools->CreateGadgetA(NUMBER_KIND, g, &ng, (struct TagItem *)&viewGTags[tc]);

		while(viewGTags[tc])
			tc += 2;
		tc++;

		ng.ng_LeftEdge -= fontx * (6 + 9 + 9 + 2);
		ng.ng_Width = fontx * (6 + 9);
		ng.ng_GadgetID = VIEW_JUMPTOLINE;

		viewGadgets[VIEW_JUMPTOLINE] = g = IGadTools->CreateGadgetA(GENERIC_KIND, g, &ng, NULL);
		g->Flags = GFLG_GADGHCOMP;
		g->Activation = GACT_RELVERIFY;
		g->GadgetType |= GTYP_BOOLGADGET;

		ng.ng_Width = fontx * 6;
		ng.ng_GadgetID = VIEW_JUMPTOLINE1;

		for(a = 0; a < 3; a++)
		{
			viewGTags[tc + 1] = vdata->view_colour_table[VIEWPEN_STATUSTEXT];
			viewGTags[tc + 3] = vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND];

			viewGadgets[a + VIEW_JUMPTOLINE1] = g = IGadTools->CreateGadgetA(NUMBER_KIND, g, &ng, (struct TagItem *)&viewGTags[tc]);

			while(viewGTags[tc])
				tc += 2;
			tc++;

			ng.ng_LeftEdge += ng.ng_Width;
			if(a == 0)
				ng.ng_Width = fontx * 9;
			ng.ng_GadgetID++;
		}

		viewGTags[tc + 1] = vdata->view_colour_table[VIEWPEN_STATUSTEXT];
		viewGTags[tc + 3] = vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND];

		ng.ng_LeftEdge = 3;
		if(config->viewbits & VIEWBITS_INWINDOW)
			ng.ng_LeftEdge += vdata->view_window->BorderLeft;

		ng.ng_Width = viewGadgets[VIEW_JUMPTOLINE1]->LeftEdge - ng.ng_LeftEdge;
		ng.ng_GadgetID = VIEW_FILENAME;

		viewGadgets[VIEW_FILENAME] = g = IGadTools->CreateGadgetA(TEXT_KIND, g, &ng, (struct TagItem *)&viewGTags[tc]);
	}

	IIntuition->AddGList(vdata->view_window, vdata->view_gadgets, -1, -1, NULL);
	IIntuition->RefreshGList(vdata->view_gadgets, vdata->view_window, NULL, -1);
	IGadTools->GT_RefreshWindow(vdata->view_window, NULL);
	IIntuition->RefreshGadgets(viewiconifygadget, vdata->view_window, NULL);

	vdata->view_vis_info.vi_font = vdata->view_font;
	vdata->view_vis_info.vi_language = config->language;

	if(vdata->view_vis_info.vi_flags & VISF_WINDOW)
		vdata->view_vis_info.vi_screen = (struct Screen *)vdata->view_window;
	else
		vdata->view_vis_info.vi_screen = vdata->view_screen;

	if(config->generalscreenflags & SCR_GENERAL_REQDRAG)
		vdata->view_vis_info.vi_flags |= VISF_BORDERS;

	return 0;
}

void view_display(struct ViewData *vdata, int scroll_speed, int w)
{
	int a, b, c, d;

	view_clearhilite(vdata, 1);
	view_clearsearch(vdata);

	d = vdata->view_font->tf_YSize / scroll_speed;

	if(vdata->view_text_offset < vdata->view_old_offset)
	{
		--vdata->view_old_offset;
		a = 0;
		while(a < vdata->view_font->tf_YSize)
		{
			if(a + d > vdata->view_font->tf_YSize)
				d = vdata->view_font->tf_YSize - a;
			view_doscroll(vdata, -d, vdata->view_scroll_width);
			a += d;
		}
		if(vdata->view_display_as_hex)
		{
			view_print(vdata, &vdata->view_text_buffer[vdata->view_text_offset * 16], 0, 16);
			view_update_status(vdata);
			return;
		}
		else
		{
			for(a = vdata->view_top_buffer_pos - 2, b = 0; a >= 0; a--, b++)
			{
				if(vdata->view_text_buffer[a] == 10 || b == vdata->view_max_line_length)
					break;
			}
			c = vdata->view_top_buffer_pos - a - 1;
			vdata->view_top_buffer_pos = a + 1;
			for(a = vdata->view_bottom_buffer_pos - 2, b = 0; a >= 0; a--, b++)
			{
				if(vdata->view_text_buffer[a] == 10 || b == vdata->view_max_line_length)
					break;
			}
			vdata->view_bottom_buffer_pos = a + 1;
		}
		if(vdata->view_top_buffer_pos + c > vdata->view_file_size)
			c = vdata->view_file_size - vdata->view_top_buffer_pos;
		view_print(vdata, &vdata->view_text_buffer[vdata->view_top_buffer_pos], 0, c);
	}
	else if(vdata->view_text_offset > vdata->view_old_offset)
	{
		++vdata->view_old_offset;
		a = 0;
		while(a < vdata->view_font->tf_YSize)
		{
			if(a + d > vdata->view_font->tf_YSize)
				d = vdata->view_font->tf_YSize - a;
			view_doscroll(vdata, d, vdata->view_scroll_width);
			a += d;
		}
		if(!w && !vdata->view_display_as_hex)
		{
			for(a = vdata->view_top_buffer_pos, b = 0; a < vdata->view_file_size; a++, b++)
			{
				if(vdata->view_text_buffer[a] == 10 || b == vdata->view_max_line_length)
					break;
			}
			vdata->view_top_buffer_pos = a + 1;
		}
		if(!vdata->view_display_as_hex)
		{
			for(a = vdata->view_bottom_buffer_pos, b = 0; a < vdata->view_file_size; a++, b++)
			{
				if(vdata->view_text_buffer[a] == 10 || b == vdata->view_max_line_length)
					break;
			}
			vdata->view_bottom_buffer_pos = a + 1;
		}
		if(vdata->view_bottom_buffer_pos < vdata->view_file_size)
		{
			if(vdata->view_display_as_hex)
			{
				view_print(vdata, &vdata->view_text_buffer[(vdata->view_text_offset + vdata->view_lines_per_screen - 1) * 16], vdata->view_lines_per_screen - 1, 16);
			}
			else
			{
				for(a = vdata->view_bottom_buffer_pos, b = 0; a < vdata->view_file_size; a++, b++)
				{
					if(vdata->view_text_buffer[a] == 10 || b == vdata->view_max_line_length)
						break;
				}
				c = a - vdata->view_bottom_buffer_pos + 1;
				if(vdata->view_bottom_buffer_pos + c > vdata->view_file_size)
					c = vdata->view_file_size - vdata->view_bottom_buffer_pos;
				view_print(vdata, &vdata->view_text_buffer[vdata->view_bottom_buffer_pos], vdata->view_lines_per_screen - 1, c);
			}
		}
	}
	view_update_status(vdata);
}

void view_displayall(struct ViewData *vdata)
{
	int a, c, d, f;

	IGraphics->SetAPen(vdata->view_rastport, vdata->view_colour_table[PEN_TEXTBACKGROUND]);
	IGraphics->RectFill(vdata->view_rastport, vdata->view_display_left, vdata->view_display_top, vdata->view_display_right, vdata->view_status_bar_ypos - 3);
	IGraphics->SetAPen(vdata->view_rastport, vdata->view_colour_table[PEN_TEXT]);

	view_clearhilite(vdata, 0);
	view_clearsearch(vdata);

	f = d = vdata->view_bottom_buffer_pos = vdata->view_top_buffer_pos;

	if(vdata->view_display_as_hex)
	{
		d = vdata->view_text_offset * 16;
		for(a = 0; a < vdata->view_lines_per_screen; a++)
		{
			if(d < vdata->view_file_size)
				view_print(vdata, &vdata->view_text_buffer[d], a, 16);
			d += 16;
		}
	}
	else
	{
		for(a = 0; a < vdata->view_lines_per_screen; a++)
		{
			for(c = 0; c < vdata->view_max_line_length; c++)
			{
				if(vdata->view_text_buffer[f++] == 10)
					break;
				if(f > vdata->view_file_size)
					break;
			}
			if((d + c + 1) > vdata->view_file_size)
				c = vdata->view_file_size - d - 1;
			if(d < vdata->view_file_size)
				view_print(vdata, &vdata->view_text_buffer[d], a, c + 1);
			d = f;
			if(a < (vdata->view_lines_per_screen - 1))
				vdata->view_bottom_buffer_pos = d;
		}
	}

	vdata->view_old_offset = vdata->view_text_offset;
	view_update_status(vdata);
}

void view_print(struct ViewData *vdata, STRPTR str, int y, int len)
{
	UBYTE textbuf[300];

	if(vdata->view_display_as_ansi)
	{
		if(len > 0)
		{
			if(str[len - 1] == '\n')
				--len;
			vdata->view_console_request.io_Data = (APTR) "\f" /*"\x9b;0m\f" */ ;
			vdata->view_console_request.io_Length = 1 /*5 */ ;
			vdata->view_console_request.io_Command = CMD_WRITE;
			IExec->DoIO((struct IORequest *)&vdata->view_console_request);

			vdata->view_console_request.io_Data = (APTR) str;
			vdata->view_console_request.io_Length = len;
			vdata->view_console_request.io_Command = CMD_WRITE;
			IExec->DoIO((struct IORequest *)&vdata->view_console_request);

			if(y > -1)
			{
				IGraphics->ClipBlit(vdata->view_ansiread_window->RPort, 0, 0, vdata->view_rastport, vdata->view_display_left, vdata->view_display_top + (y * vdata->view_font->tf_YSize), vdata->view_ansiread_window->Width, vdata->view_font->tf_YSize, 0xc0);
			}
		}
	}
	else
	{
		IGraphics->Move(vdata->view_rastport, vdata->view_display_left, vdata->view_display_top + (y * vdata->view_font->tf_YSize) + vdata->view_font->tf_Baseline);

		if(vdata->view_display_as_hex)
		{
			view_makeuphex(vdata, str, textbuf, vdata->view_text_offset + y);
			len = vdata->view_char_width;
		}
		else
		{
			unsigned short int i = 0, j, k;

			if(len > vdata->view_char_width)
				len = vdata->view_char_width;
			for(; i < len; i++, str++)
			{
				if(*str == 0x09)
				{
					k = i % (UWORD) config->tabsize;
					k = config->tabsize - k;
					for(j = 0; j < k; j++, i++)
						textbuf[i] = ' ';
					len += k - 1;
					i--;
				}
				else
					textbuf[i] = *str;
			}
			if(len > vdata->view_char_width)
				len = vdata->view_char_width;
			if(textbuf[len - 1] == 10)
				textbuf[len - 1] = ' ';
		}
		if(len > 0)
			IGraphics->Text(vdata->view_rastport, (char *)textbuf, len);
	}
}

void view_update_status(struct ViewData *vdata)
{
	if(config->viewbits & VIEWBITS_TEXTBORDERS)
	{
		int a;

		if(vdata->view_line_count <= vdata->view_lines_per_screen)
			a = 100;
		else
			a = (vdata->view_text_offset * 100) / vdata->view_last_line;

		IGadTools->GT_SetGadgetAttrs(viewGadgets[VIEW_JUMPTOLINE1], vdata->view_window, NULL, GTNM_Number, vdata->view_text_offset, TAG_END);
		IGadTools->GT_SetGadgetAttrs(viewGadgets[VIEW_JUMPTOLINE2], vdata->view_window, NULL, GTNM_Number, (vdata->view_text_offset + vdata->view_lines_per_screen > vdata->view_line_count) ? vdata->view_line_count : vdata->view_text_offset + vdata->view_lines_per_screen, TAG_END);
		IGadTools->GT_SetGadgetAttrs(viewGadgets[VIEW_JUMPTOPERCENTNUM], vdata->view_window, NULL, GTNM_Number, a, TAG_END);

		a = vdata->view_line_count >> 15;
		a++;
		IGadTools->GT_SetGadgetAttrs(viewGadgets[VIEW_SCROLLGADGET], vdata->view_window, NULL, GTSC_Top, vdata->view_text_offset / a, GTSC_Total, vdata->view_line_count / a, GTSC_Visible, vdata->view_lines_per_screen / a, TAG_END);
	}
}

void view_pageup(struct ViewData *vdata)
{
	int a, b, c;

	if(vdata->view_text_offset == 0)
		return;
	vdata->view_text_offset -= vdata->view_lines_per_screen;
	if(vdata->view_text_offset < 0)
	{
		vdata->view_text_offset = 0;
		vdata->view_top_buffer_pos = 0;
	}
	else if(!vdata->view_display_as_hex)
	{
		for(c = 0; c < vdata->view_lines_per_screen; c++)
		{
			for(a = vdata->view_top_buffer_pos - 2, b = 0; a >= 0; a--, b++)
			{
				if(vdata->view_text_buffer[a] == 10 || b == vdata->view_max_line_length)
					break;
			}
			vdata->view_top_buffer_pos = a + 1;
		}
	}
	view_displayall(vdata);
}

void view_pagedown(struct ViewData *vdata)
{
	int a, b, c;

	if(vdata->view_text_offset == vdata->view_last_line || vdata->view_line_count <= vdata->view_lines_per_screen)
		return;

	vdata->view_text_offset += vdata->view_lines_per_screen;

	if(vdata->view_text_offset > vdata->view_last_line)
	{
		vdata->view_text_offset = vdata->view_last_line;

		if(!vdata->view_display_as_hex)
		{
			vdata->view_top_buffer_pos = vdata->view_file_size;
			for(c = 0; c < vdata->view_lines_per_screen; c++)
			{
				for(a = vdata->view_top_buffer_pos - 2, b = 0; a >= 0; a--, b++)
				{
					if(vdata->view_text_buffer[a] == 10 || b == vdata->view_max_line_length)
						break;
				}
				vdata->view_top_buffer_pos = a + 1;
			}
		}
	}
	else
	{
		if(!vdata->view_display_as_hex)
		{
			vdata->view_top_buffer_pos = vdata->view_bottom_buffer_pos;
			for(a = vdata->view_top_buffer_pos, b = 0; a < vdata->view_file_size; a++, b++)
			{
				if(vdata->view_text_buffer[a] == 10 || b == vdata->view_max_line_length)
					break;
			}
			vdata->view_top_buffer_pos = a + 1;
		}
	}
	view_displayall(vdata);
}

void view_gotop(struct ViewData *vdata)
{
	if(vdata->view_text_offset)
	{
		vdata->view_text_offset = 0;
		vdata->view_top_buffer_pos = 0;
		view_displayall(vdata);
	}
}

void view_gobottom(struct ViewData *vdata)
{
	int a, b, c;

	if(vdata->view_text_offset != vdata->view_last_line && vdata->view_line_count > vdata->view_lines_per_screen)
	{

		vdata->view_text_offset = vdata->view_last_line;

		if(!vdata->view_display_as_hex)
		{
			vdata->view_top_buffer_pos = vdata->view_file_size;

			for(c = 0; c < vdata->view_lines_per_screen; c++)
			{
				for(a = vdata->view_top_buffer_pos - 2, b = 0; a >= 0; a--, b++)
				{
					if(vdata->view_text_buffer[a] == 10 || b == vdata->view_max_line_length)
						break;
				}
				vdata->view_top_buffer_pos = a + 1;
			}
		}
		view_displayall(vdata);
	}
}

void view_search(struct ViewData *vdata, int ask)
{
	int a, off, bpos, c, d, e, f, mlen;
	char temp[128];

	status_haveaborted = 0;
	view_clearhilite(vdata, 1);

	if(!vdata->view_search_string[0])
		ask = 1;
	strcpy(temp, vdata->view_search_string);
	view_busy(vdata);

	if(ask)
	{
		if(!(get_search_data(vdata->view_search_string, &vdata->view_search_flags, vdata->view_window, vdata->view_font)))
		{
			view_unbusy(vdata);
			return;
		}
	}

	if(!vdata->view_search_string[0])
	{
		view_unbusy(vdata);
		return;
	}

	if(IDOpus->LStrCmpI(temp, vdata->view_search_string))
		view_clearsearch(vdata);

	if(vdata->view_display_as_hex)
	{
		if(vdata->view_search_offset != -1)
		{
			a = (vdata->view_search_offset * 16) + vdata->view_search_charoffset;
			off = vdata->view_search_offset;
		}
		else
		{
			a = vdata->view_text_offset * 16;
			off = vdata->view_text_offset;
		}
	}
	else
	{
		if(vdata->view_search_charoffset != -1)
		{
			a = vdata->view_search_charoffset;
			off = vdata->view_search_offset;
		}
		else
		{
			a = vdata->view_top_buffer_pos;
			off = vdata->view_text_offset;
		}
	}
      dosearch:
	if((typesearch(0, vdata->view_search_string, vdata->view_search_flags, &vdata->view_text_buffer[a], vdata->view_file_size - a)) > -1)
	{

		if(vdata->view_display_as_hex)
		{
			bpos = search_found_position - vdata->view_text_buffer;
			off = bpos / 16;
			c = bpos % 16;
			mlen = search_found_size;
			if(vdata->view_pick_charoffset != bpos)
			{
				if(!(vdata->view_text_offset == vdata->view_last_line || vdata->view_line_count < vdata->view_lines_per_screen || vdata->view_text_offset > vdata->view_last_line || off < vdata->view_text_offset + vdata->view_lines_per_screen))
				{

					vdata->view_text_offset = vdata->view_search_offset = off;

					if(vdata->view_text_offset > vdata->view_last_line)
						view_gobottom(vdata);
					else
						view_displayall(vdata);
				}
				else if(vdata->view_text_offset > vdata->view_last_line)
				{
					view_gobottom(vdata);
				}

				d = (10 + (c * 2) + (c / 4)) * vdata->view_font->tf_XSize;
				if((f = c + mlen) > 16)
					f = 16;
				e = (10 + (f * 2) + (f / 4)) * vdata->view_font->tf_XSize;
				if((f % 4) == 0)
					e -= vdata->view_font->tf_XSize;

				for(a = 0; a < 2; a++)
				{
					view_viewhilite(vdata, d, (off - vdata->view_text_offset) * vdata->view_font->tf_YSize, e - 1, (off - vdata->view_text_offset) * vdata->view_font->tf_YSize + (vdata->view_font->tf_YSize - 1));
					d = (46 + c) * vdata->view_font->tf_XSize;
					e = (46 + f) * vdata->view_font->tf_XSize;
				}
				vdata->view_pick_offset = -1;
				vdata->view_pick_charoffset = bpos;
				vdata->view_search_offset = off;
				vdata->view_search_charoffset = c + 1;
			}
			else
			{
				a = bpos + 1;
				goto dosearch;
			}
		}
		else
		{
			off += search_found_lines;
			bpos = search_last_line_pos + a;
			if(vdata->view_pick_offset != bpos && vdata->view_pick_charoffset != off)
			{
				if(vdata->view_text_offset == vdata->view_last_line || vdata->view_line_count < vdata->view_lines_per_screen || off < vdata->view_text_offset + vdata->view_lines_per_screen || off > vdata->view_last_line)
				{

					if(off > vdata->view_last_line)
						view_gobottom(vdata);

					vdata->view_search_charoffset = bpos;
					vdata->view_search_offset = off;

					view_viewhilite(vdata, 0, (off - vdata->view_text_offset) * vdata->view_font->tf_YSize, vdata->view_char_width * vdata->view_font->tf_XSize, (off - vdata->view_text_offset) * vdata->view_font->tf_YSize + (vdata->view_font->tf_YSize - 1));
				}
				else
				{
					vdata->view_top_buffer_pos = bpos;
					vdata->view_text_offset = off;
					view_displayall(vdata);
					view_viewhilite(vdata, 0, 0, vdata->view_char_width * vdata->view_font->tf_XSize, vdata->view_font->tf_YSize - 1);
				}
				vdata->view_pick_offset = bpos;
				vdata->view_pick_charoffset = off;
			}
			else
			{
				for(a = bpos; a < vdata->view_file_size; a++)
				{
					if(vdata->view_text_buffer[a] == 10)
						break;
				}
				++a;
				++off;
				goto dosearch;
			}
		}
	}
	else
	{
		view_clearsearch(vdata);
		view_simplerequest(vdata, globstring[STR_STRING_NOT_FOUND], globstring[STR_CONTINUE], NULL);
	}
	view_unbusy(vdata);
	return;
}

void view_busy(struct ViewData *vdata)
{
	if(!vdata->view_window->Pointer)
		IDOpus->SetBusyPointer(vdata->view_window);
	if(!vdata->view_window->FirstRequest)
	{
		IIntuition->InitRequester(&vdata->view_busy_request);
		IIntuition->Request(&vdata->view_busy_request, vdata->view_window);
	}
}

void view_unbusy(struct ViewData *vdata)
{
	IIntuition->ClearPointer(vdata->view_window);
	if(vdata->view_window->FirstRequest)
		IIntuition->EndRequest(&vdata->view_busy_request, vdata->view_window);
}

void view_doscroll(struct ViewData *vdata, int dy, int w)
{
	if(!vdata->view_display_as_ansi)
		SetWrMsk(vdata->view_rastport, vdata->view_colour_table[PEN_TEXT]);
	IGraphics->ScrollRaster(vdata->view_rastport, 0, dy, vdata->view_display_left, vdata->view_display_top, vdata->view_display_left + w, vdata->view_display_top + vdata->view_display_height - 1);
	if(!vdata->view_display_as_ansi)
		SetWrMsk(vdata->view_rastport, 0xff);
}

int view_lineup(struct ViewData *vdata)
{
	if(vdata->view_text_offset == 0)
		return (0);

	--vdata->view_text_offset;
	if(vdata->view_scroll_dir == 1)
		view_display(vdata, 1, 1);
	else
		view_display(vdata, 1, 0);
	vdata->view_scroll_dir = -1;
	return (1);
}

int view_linedown(struct ViewData *vdata)
{
	if(vdata->view_text_offset >= vdata->view_last_line)
		return (0);

	++vdata->view_text_offset;
	vdata->view_scroll_dir = 1;
	view_display(vdata, 1, 0);
	return (1);
}

void view_status_text(struct ViewData *vdata, char *str)
{
	static char buf[108];
	struct Gadget *g = viewGadgets[VIEW_FILENAME];

	if(config->viewbits & VIEWBITS_TEXTBORDERS)
	{
		strcpy(buf, str);
		IGadTools->GT_SetGadgetAttrs(g, vdata->view_window, NULL, GTTX_Text, (Tag) buf, TAG_END);
	}
}

void view_printtext(struct ViewData *vdata, int state)
{
	int out, a, tb;
	char temp[60], *str;
	UBYTE buf1[80];

	status_haveaborted = 0;

	if(state == 0 && !vdata->view_display_as_hex)
	{
		struct DOpusArgsList print_args;

		print_args.single_file = vdata->view_path_name;
		print_args.file_window = -1;
		print_args.file_list = NULL;
		print_args.last_select = NULL;

		dopus_print(0, &print_args, 0, vdata->view_port_name, vdata);
		return;
	}

	if(!(out = IDOS->Open("PRT:", MODE_NEWFILE)))
	{
		view_simplerequest(vdata, globstring[STR_CANT_OPEN_PRINTER], globstring[STR_CONTINUE], NULL);
		return;
	}
	if(!vdata->view_display_as_hex)
	{
		char *print_ptr;
		int print_size;

		if(vdata->view_bottom_buffer_pos >= vdata->view_file_size)
		{
			print_size = vdata->view_file_size;
			print_ptr = vdata->view_text_buffer;
		}
		else
		{
			for(a = vdata->view_bottom_buffer_pos; a < vdata->view_file_size; a++)
			{
				if(vdata->view_text_buffer[a] == 10)
					break;
			}
			print_size = a - vdata->view_top_buffer_pos + 1;
			print_ptr = &vdata->view_text_buffer[vdata->view_top_buffer_pos];
		}

		if((IDOS->Write(out, print_ptr, print_size)) < print_size)
		{
			IDOS->Close(out);
			sprintf(temp, globstring[STR_ERROR_PRINTING_FILE], IDOS->IoErr());
			view_simplerequest(vdata, temp, globstring[STR_CONTINUE], NULL);
			return;
		}
	}
	else
	{
		if(state == 0)
		{
			str = vdata->view_text_buffer;
			for(a = 0; a < vdata->view_line_count; a++)
			{
				view_makeuphex(vdata, str, buf1, a);
				if((IDOS->Write(out, (char *)buf1, 63)) < 63)
				{
					IDOS->Close(out);
					sprintf(temp, globstring[STR_ERROR_PRINTING_FILE], IDOS->IoErr());
					view_simplerequest(vdata, temp, globstring[STR_CONTINUE], NULL);
					return;
				}
				str += 16;
				if(status_haveaborted)
					break;
			}
		}
		else
		{
			str = &vdata->view_text_buffer[vdata->view_text_offset * 16];
			tb = vdata->view_text_offset * 16;
			for(a = 0; a < vdata->view_lines_per_screen; a++)
			{
				view_makeuphex(vdata, str, buf1, vdata->view_text_offset + a);
				if((IDOS->Write(out, (char *)buf1, 63)) < 63)
				{
					IDOS->Close(out);
					sprintf(temp, globstring[STR_ERROR_PRINTING_FILE], IDOS->IoErr());
					view_simplerequest(vdata, temp, globstring[STR_CONTINUE], NULL);
					return;
				}
				str += 16;
				tb += 16;
				if(tb > vdata->view_file_size || status_haveaborted)
					break;
			}
		}
	}
	IDOS->Close(out);
	return;
}

void view_checkprint(struct ViewData *vdata, int code)
{
	view_busy(vdata);
	if((code == 0 && !vdata->view_display_as_hex) || (view_simplerequest(vdata, globstring[STR_READY_PRINTER], str_okaystring, str_cancelstring, NULL)))
		view_printtext(vdata, code);
	view_unbusy(vdata);
}

void view_makeuphex(struct ViewData *vdata, STRPTR hex, UBYTE *textbuf, int line)
{
	UBYTE buf2[20];
	int at, b, c, e;

	if(vdata->view_last_char && hex >= vdata->view_last_char)
		b = vdata->view_last_charpos;
	else
		b = 16;
	for(at = 0; at < b; at++)
	{
		if(!(isprint(hex[at])))
			buf2[at] = '.';
		else
			buf2[at] = hex[at];
	}
	if(vdata->view_last_char && hex >= vdata->view_last_char)
	{
		for(at = b; at < 16; at++)
		{
			buf2[at] = ' ';
		}
		e = b / 4;
		c = 10 + (b * 2) + e;
	}
	else
		c = -1;
	buf2[16] = 0;
	sprintf((char *)textbuf, "%08lx: %08lx %08lx %08lx %08lx %s\n", (long)line * 16, ((long *)hex)[0], ((long *)hex)[1], ((long *)hex)[2], ((long *)hex)[3], buf2);
	if(c > -1)
	{
		for(b = c; b < 46; b++)
			textbuf[b] = ' ';
	}
}

void view_togglescroll(struct ViewData *vdata)
{
	if(vdata->view_scroll)
	{
		vdata->view_scroll = 0;
		IIntuition->ClearPointer(vdata->view_window);
	}
	else if(vdata->view_line_count > vdata->view_lines_per_screen)
	{
		vdata->view_scroll = 1;
	}
}

void view_viewhilite(struct ViewData *vdata, int x, int y, int x1, int y1)
{
	struct viewhilite *hi;

	if(!(hi = IExec->AllocVecTags(sizeof(struct viewhilite), AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END)))
		return;
	if(vdata->view_current_hilite)
		vdata->view_current_hilite->next = hi;
	else
		vdata->view_first_hilite = hi;
	vdata->view_current_hilite = hi;
	hi->x = x + vdata->view_display_left;
	hi->y = y + vdata->view_display_top;
	hi->x1 = x1 + vdata->view_display_left;
	hi->y1 = y1 + vdata->view_display_top;
	IGraphics->SetDrMd(vdata->view_rastport, COMPLEMENT);
	IGraphics->RectFill(vdata->view_rastport, hi->x, hi->y, hi->x1, hi->y1);
	IGraphics->SetDrMd(vdata->view_rastport, JAM1);
}

void view_clearhilite(struct ViewData *vdata, int show)
{
	struct viewhilite *hi, *next;

	hi = vdata->view_first_hilite;
	if(show)
		IGraphics->SetDrMd(vdata->view_rastport, COMPLEMENT);
	while(hi)
	{
		next = hi->next;
		if(show)
			IGraphics->RectFill(vdata->view_rastport, hi->x, hi->y, hi->x1, hi->y1);
		IExec->FreeVec(hi);
		hi = next;
	}
	vdata->view_first_hilite = vdata->view_current_hilite = NULL;
	if(show)
		IGraphics->SetDrMd(vdata->view_rastport, JAM1);
}

void view_fix_scroll_gadget(struct ViewData *vdata)
{
	int a, b, c, d;

	IGadTools->GT_GetGadgetAttrs(viewGadgets[VIEW_SCROLLGADGET], vdata->view_window, NULL, GTSC_Top, (Tag) & a, TAG_END);
	if(vdata->view_line_count > 0x7FFF)
	{
		b = vdata->view_line_count >> 15;
		a *= (b + 1);
	}
	if(a == vdata->view_text_offset)
		return;

	d = vdata->view_text_offset - a;

	if(d > 0 && d < vdata->view_lines_per_screen / 2)
	{
		while(d--)
			view_lineup(vdata);
	}
	else if(d < 0 && d > -(vdata->view_lines_per_screen / 2))
	{
		while(d++)
			view_linedown(vdata);
	}
	else if(vdata->view_display_as_hex)
	{
		vdata->view_text_offset = a;
		if(vdata->view_text_offset > vdata->view_last_line)
			vdata->view_text_offset = vdata->view_last_line;
		view_displayall(vdata);
	}
	else
	{
		IDOpus->SetBusyPointer(vdata->view_window);
		if(a > vdata->view_last_line)
			a = vdata->view_last_line;
		if(vdata->view_text_offset < a)
		{
			if(a - vdata->view_text_offset < vdata->view_last_line - a)
			{
				for(b = vdata->view_text_offset; b < a; b++)
				{
					for(c = vdata->view_top_buffer_pos; c < vdata->view_file_size; c++)
					{
						if(vdata->view_text_buffer[c] == 10)
							break;
					}
					vdata->view_top_buffer_pos = c + 1;
				}
			}
			else
			{
				vdata->view_text_offset = vdata->view_last_line;
				vdata->view_top_buffer_pos = vdata->view_file_size;
				for(c = 0; c < vdata->view_lines_per_screen; c++)
				{
					for(b = vdata->view_top_buffer_pos - 2; b >= 0; b--)
					{
						if(vdata->view_text_buffer[b] == 10)
							break;
					}
					vdata->view_top_buffer_pos = b + 1;
				}
				for(b = a; b < vdata->view_text_offset; b++)
				{
					for(c = vdata->view_top_buffer_pos - 2; c >= 0; c--)
					{
						if(vdata->view_text_buffer[c] == 10)
							break;
					}
					vdata->view_top_buffer_pos = c + 1;
				}
			}
		}
		else
		{
			if(vdata->view_text_offset - a < a)
			{
				for(b = a; b < vdata->view_text_offset; b++)
				{
					for(c = vdata->view_top_buffer_pos - 2; c >= 0; c--)
					{
						if(vdata->view_text_buffer[c] == 10)
							break;
					}
					vdata->view_top_buffer_pos = c + 1;
				}
			}
			else
			{
				vdata->view_top_buffer_pos = 0;
				for(b = 0; b < a; b++)
				{
					for(c = vdata->view_top_buffer_pos; c < vdata->view_file_size; c++)
					{
						if(vdata->view_text_buffer[c] == 10)
							break;
					}
					vdata->view_top_buffer_pos = c + 1;
				}
			}
		}
		vdata->view_text_offset = a;
		view_displayall(vdata);
		IIntuition->ClearPointer(vdata->view_window);
	}
}

void view_clearsearch(struct ViewData *vdata)
{
	vdata->view_search_offset = vdata->view_search_charoffset = vdata->view_pick_offset = vdata->view_pick_charoffset = -1;
}

int view_simplerequest(struct ViewData *vdata, char *txt, ...)
{
	char *gads[4], *cancelgad = NULL, *gad;
	int a, r, rets[3], num;
	va_list ap;
	struct DOpusSimpleRequest request;

	va_start(ap, txt);
	r = 1;
	num = 0;
	for(a = 0; a < 3; a++)
	{
		if(!(gad = (char *)va_arg(ap, char *)))
		{
			  break;
		}
		if(a == 1)
		{
			cancelgad = gad;
		}
		else
		{
			gads[num] = gad;
			rets[num++] = r++;
		}
	}
	if(cancelgad)
	{
		gads[num] = cancelgad;
		rets[num] = 0;
		a = num + 1;
	}
	for(; a < 4; a++)
		gads[a] = NULL;

	request.hi = vdata->view_colour_table[PEN_SHINE];
	request.lo = vdata->view_colour_table[PEN_SHADOW];
	request.fg = vdata->view_colour_table[VIEWPEN_STATUSTEXT];
	request.bg = vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND];

	request.strbuf = NULL;

	return (dorequest(&request, txt, gads, rets, vdata->view_window));
}

int view_whatsit(struct ViewData *vdata, STRPTR txt, int max, STRPTR buffer)
{
	char *gads[3];
	int rets[2];
	struct DOpusSimpleRequest request;

	gads[0] = str_okaystring;
	rets[0] = 1;
	gads[1] = str_cancelstring;
	rets[1] = 0;
	gads[2] = NULL;

	request.hi = vdata->view_colour_table[PEN_SHINE];
	request.lo = vdata->view_colour_table[PEN_SHADOW];
	request.fg = vdata->view_colour_table[VIEWPEN_STATUSTEXT];
	request.bg = vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND];

	request.strbuf = buffer;
	request.strlen = max;
	request.flags = 0;
	return (dorequest(&request, txt, gads, rets, vdata->view_window));
}
