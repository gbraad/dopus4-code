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

#include "print.h"

void get_vis_info(struct VisInfo *vis, char *portname);

int main(int argc, char **argv)
{
	struct VisInfo vis;
	char **dummy_args, *port = NULL;
	struct DOpusArgsList *arglist = NULL;
	BPTR oldcurdir = 0, *dirlocks;
	int arg, printdir = 0, pdwind = -1;
	struct StringData *stringdata;
	struct DOpusRemember *memkey = NULL;

	/* Attempt to open the DOPUS.LIBRARY. Look first in default search path, and then look for it on the distribution disk. If we can't find it exit */
	if(!(DOpusBase = IExec->OpenLibrary("dopus.library", LIB_VER)))
	{
		if(!(DOpusBase = IExec->OpenLibrary("PROGDIR:/libs/dopus.library", LIB_VER)))
		{
			DOpusBase = NULL;
		}
	}
	if(DOpusBase && !(IDOpus = (struct DOpusIFace *)IExec->GetInterface(DOpusBase, "main", 1, NULL)))
	{
		IDOpus = NULL;
	}
	if(!DOpusBase || !IDOpus)
	{
		IDOS->Printf("Can't Open dopus.library and get IDOpus interface\n");
		IExec->DropInterface((struct Interface *)IDOpus);
		IExec->CloseLibrary(DOpusBase);
		return 5;
	}
	/* OpenLibrary() GetInterface() above  */

	if((dummy_args = IDOpus->LAllocRemember(&memkey, 16 * sizeof(char *), MEMF_CLEAR)) &&
	   (dirlocks = IDOpus->LAllocRemember(&memkey, 16 * sizeof(BPTR), MEMF_CLEAR)) &&
	   (stringdata = IDOpus->LAllocRemember(&memkey, sizeof(struct StringData), MEMF_CLEAR)))
	{

		if(argc == 0)
		{
			struct WBStartup *startup = (struct WBStartup *)argv;
			int arg;

			for(arg = 0; arg < startup->sm_NumArgs && arg < 16; arg++)
			{
				dummy_args[arg] = startup->sm_ArgList[arg].wa_Name;
				dirlocks[arg] = startup->sm_ArgList[arg].wa_Lock;
				++argc;
			}
			argv = dummy_args;
		}

		for(arg = 1; arg < argc; arg++)
		{
			if(argv[arg][0] == '&')
				port = &argv[arg][1];
			else if(argv[arg][0] == '!')
				arglist = (struct DOpusArgsList *)atoi(&argv[arg][1]);
			else if(argv[arg][0] == '@')
			{
				printdir = 1;
				if(argv[arg][1] == '0')
					pdwind = 0;
				else if(argv[arg][1] == '1')
					pdwind = 1;
			}
			else
			{
				break;
			}
		}

		get_vis_info(&vis, port);

		stringdata->default_table = default_strings;
		stringdata->string_count = STR_STRING_COUNT;
		stringdata->min_version = STRING_VERSION;

		if(IDOpus->ReadStringFile(stringdata, "dopus4_print.catalog"))
		{
			string_table = stringdata->string_table;

			if(printdir && port)
				do_printdir(&vis, port, pdwind);
			else
			{
				if(!arglist)
					port = NULL;
				if(dirlocks[arg])
//					oldcurdir = IDOS->CurrentDir(dirlocks[arg]);
					oldcurdir = IDOS->SetCurrentDir(dirlocks[arg]);
				do_print(&vis, port, arglist, argc - arg, &argv[arg]);
				if(dirlocks[arg])
//					IDOS->CurrentDir(oldcurdir);
					IDOS->SetCurrentDir(oldcurdir);
			}
			IDOpus->FreeStringFile(stringdata);
		}
	}
	IDOpus->LFreeRemember(&memkey);

	if(IDOpus)
		IExec->DropInterface((struct Interface *)IDOpus);
	if(DOpusBase)
		IExec->CloseLibrary(DOpusBase);

	return(0);
}

void get_vis_info(struct VisInfo *vis, char *portname)
{
	struct DrawInfo *drinfo;
	struct Screen *pub;

	vis->vi_fg = 1;
	vis->vi_bg = 0;
	vis->vi_shine = 2;
	vis->vi_shadow = 1;
	vis->vi_font = NULL;
	vis->vi_screen = NULL;
	vis->vi_stringcol[0] = 1;
	vis->vi_stringcol[1] = 0;
	vis->vi_activestringcol[0] = 1;
	vis->vi_activestringcol[1] = 0;
	vis->vi_flags = VISF_WINDOW;
	vis->vi_language = NULL;

	if(dopus_message(DOPUSMSG_GETVIS, (APTR) vis, portname))
		return;

	if ((pub = IIntuition->LockPubScreen(NULL)))
	{
		drinfo = IIntuition->GetScreenDrawInfo(pub);
		vis->vi_shine = drinfo->dri_Pens[SHINEPEN];
		vis->vi_shadow = drinfo->dri_Pens[SHADOWPEN];
		vis->vi_fg = drinfo->dri_Pens[TEXTPEN];
		vis->vi_bg = drinfo->dri_Pens[BACKGROUNDPEN];
		IIntuition->FreeScreenDrawInfo(pub, drinfo);
		IIntuition->UnlockPubScreen(NULL, pub);
	}
}

int dopus_message(int cmd, APTR data, char *portname)
{
	struct MsgPort *port, *replyport;
	struct DOpusMessage msg;

	if (!(portname && portname[0])) return (0);

	IExec->Forbid();
	if((port = IExec->FindPort(portname)) &&
	    (replyport = IExec->AllocSysObject(ASOT_PORT,NULL)))
	{
		msg.msg.mn_Node.ln_Type = NT_MESSAGE;
		msg.msg.mn_Node.ln_Name = NULL;
		msg.msg.mn_ReplyPort = replyport;
		msg.msg.mn_Length = (UWORD) sizeof(struct DOpusMessage);
		msg.command = cmd;
		msg.data = data;
		IExec->PutMsg(port, (struct Message *)&msg);
		IExec->Permit();
		IExec->WaitPort(replyport);
		IExec->GetMsg(replyport);
		IExec->FreeSysObject(ASOT_PORT, replyport);
		IExec->Permit();
		return (1);
	}
	IExec->Permit();
	return (0);
}

void fill_out_req(struct RequesterBase *req, struct VisInfo *vis)
{
	req->rb_fg = vis->vi_fg;
	req->rb_bg = vis->vi_bg;
	req->rb_shine = vis->vi_shine;
	req->rb_shadow = vis->vi_shadow;
	req->rb_font = vis->vi_font;
	req->rb_flags &= ~RBF_WINDOWCENTER;
	if(vis->vi_screen)
	{
		if(vis->vi_flags & VISF_WINDOW)
		{
			req->rb_screen = ((struct Window *)vis->vi_screen)->WScreen;
			req->rb_window = (struct Window *)vis->vi_screen;
			req->rb_flags |= RBF_WINDOWCENTER;
		}
		else
		{
			req->rb_screen = vis->vi_screen;
		}
	}
	else
	{
		req->rb_screen = NULL;
	}
}

struct Gadget *addreqgadgets(struct RequesterBase *reqbase, struct TagItem **gadgets, struct Gadget **gadgetlist)
{
	int gad;
	struct Gadget *gadget = NULL, *newgadget, *firstgadget = NULL;

	for(gad = 0;; gad++)
	{
		if(!gadgets[gad])
			break;
		if(!(newgadget = (struct Gadget *)IDOpus->AddRequesterObject(reqbase, gadgets[gad])))
			return (NULL);
		if(gadget)
			gadget->NextGadget = newgadget;
		else
			firstgadget = newgadget;
		gadget = newgadget;
		gadgetlist[gad] = newgadget;
	}
	IDOpus->AddGadgets(reqbase->rb_window, firstgadget, NULL, gad, reqbase->rb_shine, reqbase->rb_shadow, 1);
	return (firstgadget);
}

void border_text(struct RequesterBase *reqbase, Object_Border *border, char *infobuf)
{
	struct RastPort *rp;

	rp = reqbase->rb_window->RPort;
	IGraphics->SetAPen(rp, reqbase->rb_bg);
	IGraphics->RectFill(rp, border->ob_left, border->ob_top, border->ob_left + border->ob_width - 1, border->ob_top + border->ob_height - 1);
	IGraphics->SetAPen(rp, reqbase->rb_fg);

	if(infobuf)
	{
		IDOpus->ObjectText(reqbase, border->ob_left, border->ob_top, border->ob_width, border->ob_height, infobuf, TEXTPOS_CENTER);
	}
}

void set_print_env(PrintData *printdata)
{
	int file;

	if(!(file = IDOS->Open("env:DOpus_print.prefs", MODE_NEWFILE)))
		return;
	IDOS->Write(file, (char *)printdata, sizeof(PrintData));
	IDOS->Close(file);
}

void get_print_env(PrintData *printdata)
{
	int file;

	if(!(file = IDOS->Open("env:DOpus_print.prefs", MODE_OLDFILE)))
		return;
	IDOS->Read(file, (char *)printdata, sizeof(PrintData));
	IDOS->Close(file);
}

void activate_next_gadget(struct Gadget *gadget, struct Window *window)
{
	struct Gadget *gad;
	int try;

	gad = gadget->NextGadget;
	for(try = 0; try < 2; try++)
	{
		while(gad)
		{
			if(gad->GadgetType == GTYP_STRGADGET && !(gad->Flags & GFLG_DISABLED))
			{
				IDOpus->ActivateStrGad(gad, window);
				return;
			}
			gad = gad->NextGadget;
		}
		gad = window->FirstGadget;
	}
}

int get_file_byrequest(struct Gadget *gadget, struct Window *window, int save)
{
	struct DOpusFileReq filereq;
	char dirbuf[PATHBUF_SIZE], filebuf[FILEBUF_SIZE], *ptr, *path;

	path = ((struct StringInfo *)gadget->SpecialInfo)->Buffer;

	filereq.title = string_table[STR_SELECT_FILE];
	filereq.dirbuf = dirbuf;
	filereq.filebuf = filebuf;
	filereq.window = window;
	filereq.x = -2;
	filereq.y = -2;
	filereq.lines = 15;
	filereq.flags = (save) ? DFRF_SAVE : 0;
	filereq.filearraykey = NULL;
	filereq.filearray = NULL;

	strlcpy(dirbuf, path, PATHBUF_SIZE);
	if((ptr = (char *)IDOS->FilePart(dirbuf)) > dirbuf)
	{
		strlcpy(filebuf, ptr, FILEBUF_SIZE);
		*ptr = 0;
	}
	else if(ptr == dirbuf)
	{
		dirbuf[0] = 0;
		strlcpy(filebuf, path, FILEBUF_SIZE);
	}
	else
		filebuf[0] = 0;

	if(!(IDOpus->FileRequest(&filereq)) || !filebuf[0])
		return (0);

	strlcpy(path, dirbuf, PATHBUF_SIZE);
	if (!(IDOS->AddPart(path, filebuf, PATHBUF_SIZE)))
		return (0);

	IDOpus->RefreshStrGad(gadget, window);
	return (1);
}

int error_rets[2] = { 1, 0 };

int check_error(struct RequesterBase *reqbase, char *str, int abort)
{
	struct DOpusSimpleRequest *req = NULL;
	char *error_gads[3];
	int ret;

	req = IExec->AllocVecTags(sizeof(struct DOpusSimpleRequest),
	      AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END);
	if (!req) return (0);

	req->text = str;
	if(abort)
	{
		error_gads[0] = string_table[STR_ABORT];
		error_gads[1] = string_table[STR_CONTINUE];
	}
	else
	{
		error_gads[0] = string_table[STR_OKAY];
		error_gads[1] = string_table[STR_CANCEL];
	}
	error_gads[2] = NULL;
	req->gads = error_gads;
	req->rets = error_rets;
	req->hi = reqbase->rb_shine;
	req->lo = reqbase->rb_shadow;
	req->fg = reqbase->rb_fg;
	req->bg = reqbase->rb_bg;
	req->font = reqbase->rb_font;
	req->flags = SRF_RECESSHI;
	if(reqbase->rb_flags & RBF_BORDERS)
		req->flags |= SRF_BORDERS;
	req->title = "DOpus_Print";
	ret = IDOpus->DoSimpleRequest(reqbase->rb_window, req);
	IExec->FreeVec(req);
	return (ret);
}
