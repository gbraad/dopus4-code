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

void makedir(int rexx)
{
	struct ExamineData *data = NULL;
	int a, success, win, err, addicon = 0;
	char dirname[FILEBUF_SIZE], new_directory[256];
	BPTR lock;

	win = data_active_window;
	if((dopus_curwin[win]->directory[0] == 0))
	{
		return;
	}
	dirname[0] = new_directory[0] = 0;

	FOREVER
	{
		if(rexx)
		{
			for(a = 0; a < 256; a++)
			{
				if(!rexx_args[0][a] || rexx_args[0][a] == '/' || rexx_args[0][a] == ':')
					break;
			}
			if(!rexx_args[0][a])
			{
				if(!(isvalidwindow(data_active_window)))
				{
					return;
				}
				strcpy(new_directory, str_pathbuffer[data_active_window]);
				IDOS->AddPart(new_directory, rexx_args[0], 256);
				if(rexx_args[1][0] && (rexx_args[1][0] == '1'))
					addicon = 1;
			}
			else
			{
				strcpy(new_directory, rexx_args[0]);
				win = -1;
			}
		}
		else if(status_iconified)
		{
			if(!(status_flags & STATUS_ISINBUTTONS) || (!(getdummypath(new_directory, STR_ENTER_DIRECTORY_NAME))))
			{
				return;
			}
			win = -1;
		}
		else
		{
			if(!(isvalidwindow(data_active_window)))
			{
				return;
			}
			strcpy(new_directory, str_pathbuffer[data_active_window]);

			if(!(whatsit(globstring[STR_ENTER_DIRECTORY_NAME], config->iconflags & ICONFLAG_MAKEDIRICON ? FILEBUF_SIZE - 7 : FILEBUF_SIZE - 2, dirname, NULL)) || !dirname[0])
			{
				myabort();
				return;
			}
			for(a = 0; dirname[a]; a++)
			{
				if(dirname[a] == '/' || dirname[a] == ':')
				{
					dirname[a] = 0;
					break;
				}
			}
			if(!dirname[0])
			{
				myabort();
				return;
			}
			IDOS->AddPart(new_directory, dirname, 256);
		}

		if(!(lock = IDOS->CreateDir(new_directory)))
		{
			doerror((err = IDOS->IoErr()));
			if(rexx || (a = checkerror(globstring[STR_CREATING_DIRECTORY], IDOS->FilePart(new_directory), -err)) == 3)
			{
				myabort();
				return;
			}
			continue;
		}
		IDOS->UnLock(lock);

		if((data = IDOS->ExamineObjectTags(EX_StringName, new_directory, TAG_END)))
		{
			if(win > -1)
			{
				addfile(dopus_curwin[win], win, data->Name, -1, 2, &data->Date, NULL, data->Protection, 0, TRUE, NULL, NULL, data->OwnerUID, data->OwnerGID);
				seename(win);
				update_buffer_stamp(win, 1);
			}
			IDOS->FreeDosObject(DOS_EXAMINEDATA, data);
		}

		success = 1;
		if((config->iconflags & ICONFLAG_MAKEDIRICON) || addicon)
		{
			struct ExamineData *data = NULL;

			strcat(new_directory, ".info");
			if((iconwrite(ICONTYPE_DRAWER, new_directory)) == 1 && (data = IDOS->ExamineObjectTags(EX_StringName, new_directory, TAG_END)))
			{
				if(win > -1)
				{
					addfile(dopus_curwin[win], win, data->Name, data->FileSize, ST_FILE, &data->Date, data->Comment, data->Protection, 0, TRUE, NULL, NULL, data->OwnerUID, data->OwnerGID);
					seename(win);
					update_buffer_stamp(win, 1);
				}
				IDOS->FreeDosObject(DOS_EXAMINEDATA, data);
			}
			else
			{
				success = 0;
			}
		}
		break;
	}
	if(success)
	{
		dostatustext(globstring[STR_DIRECTORY_CREATED]);
	}

	return;
}

int iconwrite(int type, STRPTR name)
{
	int suc, err;
	struct DiskObject *diskobj;
	char namebuf[256], *ptr, *originalicon, sourcebuf[256];
	struct dopusfiletype *ftype;

	if(!IconBase)
		return (0);

	strcpy(namebuf, name);
	if((ptr = (char *)strstri(namebuf, ".info")))
		*ptr = 0;

	if(type == ICONTYPE_DRAWER)
		originalicon = config->drawericon;
	else if(type == ICONTYPE_TRASH)
		originalicon = NULL;
	else
	{
		ftype = checkfiletype(namebuf, -2, 0);
		if(ftype && ftype->iconpath)
			originalicon = ftype->iconpath;
		else if(type == ICONTYPE_TOOL)
			originalicon = config->toolicon;
		else
			originalicon = config->projecticon;
	}

	if(originalicon)
	{
		strcpy(sourcebuf, originalicon);
		if(!(strstri(sourcebuf, ".info")))
			strcat(sourcebuf, ".info");
	}

	for(;;)
	{
		if(originalicon && originalicon[0] && (IDOpus->CheckExist(sourcebuf, NULL) < 0))
		{
			if((copyicon(originalicon, namebuf, &err)) > 0)
				return (1);
		}
		else
		{
			if((diskobj = IIcon->GetDefDiskObject(type + 2)))
			{
				if(diskobj->do_Type == WBPROJECT && config->defaulttool[0])
				{
					ptr = diskobj->do_DefaultTool;
					diskobj->do_DefaultTool = config->defaulttool;
				}
				else
					ptr = NULL;
				suc = IIcon->PutDiskObject(namebuf, diskobj);
				if(ptr)
					diskobj->do_DefaultTool = ptr;
				IIcon->FreeDiskObject(diskobj);
				if(suc)
					return (1);
				err = IDOS->IoErr();
			}
			else
			{
				if(type == ICONTYPE_PROJECT)
					icontable[ICONTYPE_PROJECT]->do_DefaultTool = config->defaulttool;
				if(IIcon->PutDiskObject(namebuf, icontable[type]))
					return (1);
				err = IDOS->IoErr();
			}
		}
		if((suc = checkerror(globstring[STR_ADDING_ICON], IDOS->FilePart(namebuf), err)) == 3)
			return (-1);
		if(suc == 0 || suc == 2)
			return (0);
	}
}

int copyicon(STRPTR srce, STRPTR dest, int *err)
{
	int suc;
	struct DiskObject *diskobj;
	char buf[256], *ptr;

	strcpy(buf, srce);
	if((ptr = (char *)strstri(buf, ".info")))
		*ptr = 0;

	if((diskobj = IIcon->GetDiskObject(buf)))
	{
		suc = IIcon->PutDiskObject(dest, diskobj);
		IIcon->FreeDiskObject(diskobj);
		if(!suc || IDOpus->CheckExist(dest, NULL) >= 0)
		{
			*err = IDOS->IoErr();
		}
	}
	else
	{
		IDOS->SetIoErr(ERROR_OBJECT_NOT_FOUND);
		suc = 0;
	}
	return (suc);
}

char *isicon(STRPTR name)
{
	int a;

	if((a = strlen(name)) < 6)
		return (NULL);
	if(strcmp(&name[a - 5], ".info") == 0)
		return (&name[a - 5]);
	return (NULL);
}

char *getarexxpath(int rexx, int win, int num, int argnum)
{
	int a, b;
	char *ptr;
	APTR save;

	if(!rexx || !rexx_argcount)
	{
		return (str_pathbuffer[win]);
	}
	if((strchr(rexx_args[argnum], ':') || strchr(rexx_args[argnum], '/')) && (a = IDOpus->CheckExist(rexx_args[argnum], &b)))
	{
		if(!num)
		{
			struct ExamineData *data = NULL;

			save = IDOS->SetProcWindow((APTR)-1L);
			dos_global_entry.subtype = 0;
			if((data = IDOS->ExamineObjectTags(EX_StringName, rexx_args[argnum], TAG_END)))
			{
				dos_global_entry.protection = data->Protection;
				dos_global_entry.comment = dos_copy_comment;
				strcpy(dos_copy_comment, data->Comment);
				dos_global_entry.dispstr = NULL;
				getprot(dos_global_entry.protection, dos_global_entry.protbuf);
				IExec->CopyMem(&data->Date, &dos_global_entry.date, sizeof(struct DateStamp));
				seedate(&dos_global_entry.date, dos_global_entry.datebuf, 1);
				dos_global_entry.selected = 0;

				IDOS->FreeDosObject(DOS_EXAMINEDATA, data);
			}
			IDOS->SetProcWindow(save);
			ptr = IDOS->FilePart(rexx_args[argnum]);
			strcpy(dos_global_entry.name, ptr);
			if(ptr > rexx_args[argnum])
			{
				*ptr = 0;
				strcpy(rexx_pathbuffer[win], rexx_args[argnum]);
				IDOpus->TackOn(rexx_pathbuffer[win], NULL, 256);
			}
			else
			{
				rexx_pathbuffer[win][argnum] = 0;
			}
			dos_global_entry.last = dos_global_entry.next = NULL;
			dos_global_entry.type = a;
			dos_global_entry.size = b;
			func_single_entry = &dos_global_entry;
			removeargstring(argnum);
			return (rexx_pathbuffer[win]);
		}
		else
		{
			strcpy(rexx_pathbuffer[win], rexx_args[argnum]);
			IDOpus->TackOn(rexx_pathbuffer[win], NULL, 256);
			removeargstring(argnum);
			return (rexx_pathbuffer[win]);
		}
	}
	return (str_pathbuffer[win]);
}

int readfile(STRPTR name, STRPTR *buf, int64 *size)
{
	int in, retval  = 0, filesize;

	if(IDOpus->CheckExist(name, &filesize) >= 0 || !(in = IDOS->Open(name, MODE_OLDFILE)))
		return (-1);

	if((*buf = IExec->AllocVec(filesize, MEMF_ANY)))
	{
		IDOS->Read(in, *buf, filesize);
	}
	else
	{
		retval = -2;
	}
	IDOS->Close(in);

	*size = (int64)filesize;

	return (retval);
}

int getnewprot(int old, int new, int mask)
{
	int val, a;

	if(!mask)
		return (new);
	val = old & (~255);
	for(a = 0; a < 8; a++)
	{
		if(mask & (1 << a))
		{
			if(old & (1 << a))
				val |= (1 << a);
		}
		else if(new & (1 << a))
			val |= (1 << a);
	}
	return (val);
}

int dateformat(int flag)
{
	int a;

	for(a = 0; a < 4; a++)
	{
		if(flag & (1 << a))
			return (a);
	}
	return (a);
}

int checkscreenmode(uint32 mode)
{
	DisplayInfoHandle *handle;
	struct DimensionInfo *dimension;
	char buf[256];
	int flags, minw, defw;

	if(mode == MODE_WORKBENCHUSE || mode == MODE_WORKBENCHCLONE || mode == MODE_PUBLICSCREEN)
		return (mode);

	if(!(handle = IGraphics->FindDisplayInfo(mode)) || !(IGraphics->GetDisplayInfoData(handle, buf, 256, DTAG_DISP, 0)) || ((struct DisplayInfo *)buf)->NotAvailable)
		return (MODE_WORKBENCHCLONE);

	flags = ((struct DisplayInfo *)buf)->PropertyFlags;

	if(flags & DIPF_IS_DUALPF || flags & DIPF_IS_PF2PRI || flags & DIPF_IS_HAM || !(IGraphics->GetDisplayInfoData(handle, buf, 256, DTAG_DIMS, 0)))
		return (MODE_WORKBENCHCLONE);

	dimension = (struct DimensionInfo *)buf;

	if(mode & HIRES_KEY || mode & HIRESLACE_KEY)
	{
		minw = 640;
	}
	else
	{
		minw = dimension->Nominal.MaxX + 1;
		if(minw < 160)
			minw *= 10;
	}

	defw = (dimension->Nominal.MaxX) + 1;

	if((minw < 640 || defw < 640) && dimension->MaxDepth >= 5)
		return (MODE_WORKBENCHCLONE);

	return (mode);
}

void doarrowgadgets(struct Gadget *gad, int dir)
{
	int a;

	for(a = 0; a < 2; a++)
	{
		if(!gad)
			break;
		do3dbox(main_rp, gad->LeftEdge + 2, gad->TopEdge + 1, gad->Width - 4, gad->Height - 2);
		IDOpus->DoArrow(main_rp, gad->LeftEdge + 2, gad->TopEdge + 1, gad->Width - 4, gad->Height - 2, screen_pens[config->arrowfg].pen, screen_pens[config->arrowbg].pen, dir);
		gad = gad->NextGadget;
	}
}

void makermbgad(struct RMBGadget *rmbgad, struct Gadget *gad, int high)
{
	rmbgad->x = gad->LeftEdge;
	rmbgad->y = gad->TopEdge;
	rmbgad->w = gad->Width;
	rmbgad->h = gad->Height;
	if(high && gad->GadgetRender)
	{
		rmbgad->flags |= RGF_ALTBORD;
		rmbgad->high_bord[0] = gad->GadgetRender;
		rmbgad->high_bord[1] = gad->SelectRender;
	}
	else
		rmbgad->flags &= ~RGF_ALTBORD;
}

void dolittlegads(struct Gadget *gad, STRPTR txt, int num)
{
	int a, y;
	struct TextFont *font;

	if(status_iconified || !Window)
		return;

	font = main_rp->Font;
	IGraphics->SetFont(main_rp, scr_font[FONT_GENERAL]);
	for(a = 0; a < num; a++)
	{
		do3dbox(main_rp, gad->LeftEdge + 2, gad->TopEdge + 1, gad->Width - 4, gad->Height - 2);
		IGraphics->SetAPen(main_rp, screen_pens[config->littlegadbg].pen);
		rectfill(main_rp, gad->LeftEdge + 2, gad->TopEdge + 1, gad->Width - 4, gad->Height - 2);
		IGraphics->SetAPen(main_rp, screen_pens[config->littlegadfg].pen);
		IGraphics->SetBPen(main_rp, screen_pens[config->littlegadbg].pen);
		y = gad->TopEdge + 1 + (((gad->Height - 2) - scr_font[FONT_GENERAL]->tf_YSize) / 2);
		if(y < gad->TopEdge + 1)
			y = gad->TopEdge + 1;
		y += scr_font[FONT_GENERAL]->tf_Baseline;
		IGraphics->Move(main_rp, gad->LeftEdge + ((gad->Width - scr_font[FONT_GENERAL]->tf_XSize) / 2), y);
		IGraphics->Text(main_rp, &txt[a], 1);
		IIntuition->RefreshGList(gad, Window, NULL, 1);
		if(!(gad = gad->NextGadget))
			break;
	}
	IGraphics->SetFont(main_rp, font);
}

void rectfill(struct RastPort *r, int x, int y, int w, int h)
{
	if(w < 0 || h < 0)
		return;
	IGraphics->RectFill(r, x, y, x + w - 1, y + h - 1);
}

int isvalidwindow(int win)
{
	if(!str_pathbuffer[win])
		return (0);
	if(dopus_curwin[win]->total > 0 && dopus_curwin[win]->firstentry->type == ENTRY_CUSTOM)
		return (0);
	return (1);
}
