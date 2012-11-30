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

int checkexec(STRPTR name)
{
	int file, check;

	if(!(file = IDOS->Open(name, MODE_OLDFILE)))
		return (0);
	IDOS->Read(file, (char *)&check, 4);
	IDOS->Close(file);
//	return ((check == 0x3f3));
	return ((check == 0x7f454c46) || (check == 0x3f3));
}

void newcli(char *spec)
{
	char buf[200];

	sprintf(buf, "%s \"%s\"", config->outputcmd, spec ? spec : config->output);
//	if(!(IDOS->Execute(buf, 0, nil_file_handle)))
	if(!(IDOS->System(buf, NULL)))
	{
		doerror(-1);
	}
	else
	{
		IIntuition->WBenchToFront();
		okay();
	}
}

void getprot(int protect, STRPTR buffer)
{
	int a;

	protect ^= 0x000F;
	for(a = 7; a >= 0; a--)
		buffer[7 - a] = (protect & (1 << a)) ? str_protbits[7 - a] : '-';
	buffer[8] = 0;
}

int getprotval(STRPTR buf)
{
	int a, b, c, prot = 15;

	a = strlen(buf);
	for(b = 0; b < a; b++)
	{
		buf[b] = toupper(buf[b]);
		for(c = 0; c < 8; c++)
		{
			if(buf[b] == str_protbits[7 - c])
			{
				if(c < 4)
					prot &= ~(1 << c);
				else
					prot |= (1 << c);
				break;
			}
		}
	}
	return (prot);
}

int checkexistreplace(STRPTR sourcename, STRPTR destname, struct DateStamp *date, int allabort, int type)
{
	struct ExamineData *sdata, *ddata;
	char buf[500], datebuf1[40], datebuf2[40], gadgetbuf[100];
	int a;
	struct DateStamp ds;

	if(config->existflags & REPLACE_ALWAYS)
	{
		return (REPLACE_OK);
	}

	if(!(sdata = IDOS->ExamineObjectTags(EX_StringName, sourcename, TAG_END)))
 	{
		doerror(IDOS->IoErr());
		return (REPLACE_SKIP);
//		return (REPLACE_OK);
	}
//	if(!(ddata = IDOS->ExamineObjectTags(EX_StringName, destname, TAG_END)))
	if(!(ddata = examineobject(destname)))
 	{
		if(IDOS->IoErr() == ERROR_OBJECT_NOT_FOUND)
		{
			IDOS->FreeDosObject(DOS_EXAMINEDATA, sdata);
			return (REPLACE_OK);
		}
		else
		{
			IDOS->FreeDosObject(DOS_EXAMINEDATA, sdata);
			doerror(IDOS->IoErr());
			return (REPLACE_SKIP);
		}
	}

	if(config->existflags & REPLACE_NEVER)
	{
		doerror(ERROR_OBJECT_EXISTS);
	}
	else if(config->existflags & REPLACE_OLDER)
	{
		if(!date)
		{
			IDOS->DateStamp(&ds);
			date = &ds;
		}
		if(ddata)
		{
			if(IDOS->CompareDates(date, &(ddata->Date)) > 0)
			{
				IDOS->FreeDosObject(DOS_EXAMINEDATA, sdata);
				IDOS->FreeDosObject(DOS_EXAMINEDATA, ddata);
				return (REPLACE_OK);
			}
			doerror(ERROR_OBJECT_EXISTS);
		}
	}
	else if(config->existflags & REPLACE_ASK)
	{
		BOOL dirtodir = FALSE;
		doerror(ERROR_OBJECT_EXISTS);
		if(sourcename == destname || !ddata)
		{
			snprintf(buf, 400, globstring[STR_FILE_EXISTS_REPLACE], IDOS->FilePart(destname));
		}
		else
		{
			seedate(&sdata->Date, datebuf1, 0);
			seedate(&ddata->Date, datebuf2, 0);

			if(EXD_IS_DIRECTORY(ddata) && EXD_IS_DIRECTORY(sdata))
			{
				/* Both entries are directories */
				uint32 size = IUtility->Strlcpy(buf, globstring[STR_DIRECTORY], 100);
				buf[size] = ' ';
				dirtodir = TRUE;
				snprintf(&buf[size +1], 400, globstring[STR_FILE_EXISTS_REPLACE], IDOS->FilePart(destname));
//				IDOS->FreeDosObject(DOS_EXAMINEDATA, sdata);
//				IDOS->FreeDosObject(DOS_EXAMINEDATA, ddata);
//				return (REPLACE_OK);
			}
			else if(EXD_IS_DIRECTORY(ddata))
			{
				/* Destination is directory, source is file */
				snprintf(buf, 400, globstring[STR_REPLACE_DIR_WITH_FILE], IDOS->FilePart(destname), sdata->FileSize, datebuf1, datebuf2);
			}
			else if(EXD_IS_DIRECTORY(sdata))
			{
				/* Source is directory, destination is file */
				snprintf(buf, 400, globstring[STR_REPLACE_FILE_WITH_DIR], IDOS->FilePart(destname), datebuf1, ddata->FileSize, datebuf2);
			}
			else
			{
				/* Both entries are files */
				snprintf(buf, 400, globstring[STR_OLD_NEW_FILE_REPLACE], IDOS->FilePart(destname), sdata->FileSize, datebuf1, ddata->FileSize, datebuf2);
			}
		}
		do
		{
			if ((dirtodir) && (type == FUNC_COPY))
				snprintf(gadgetbuf, 100, "%s|%s|%s|%s|%s|%s|%s", globstring[STR_REPLACE], globstring[allabort ? STR_REPLACE_ALL : STR_TRY_AGAIN], globstring[STR_RENAME_REQ], globstring[STR_SKIP], globstring[STR_SKIP_ALL], globstring[STR_MERGE], globstring[STR_ABORT]);
			else
				snprintf(gadgetbuf, 100, "%s|%s|%s|%s|%s|%s", globstring[STR_REPLACE], globstring[allabort ? STR_REPLACE_ALL : STR_TRY_AGAIN], globstring[STR_RENAME_REQ], globstring[STR_SKIP], globstring[STR_SKIP_ALL], globstring[STR_ABORT]);
			a = ra_simplerequest(buf, gadgetbuf, REQIMAGE_WARNING);
			if(a == REPLACE_RENAME)
			{
				char dname[FILEBUF_SIZE];

				strncpy(dname, IDOS->FilePart(sourcename), FILEBUF_SIZE);

				if(whatsit(globstring[STR_ENTER_NEW_NAME], FILEBUF_SIZE, dname, NULL))
				{
					*IDOS->FilePart(destname) = 0;
					strcat(destname, dname);
				}
			}
			else
			{
				break;
			}
		}
//		while(IDOpus->CheckExist(destname, NULL));
		while(checkexist(destname, NULL));
		IDOS->FreeDosObject(DOS_EXAMINEDATA, sdata);
		IDOS->FreeDosObject(DOS_EXAMINEDATA, ddata);
		return (a);
	}
	IDOS->FreeDosObject(DOS_EXAMINEDATA, sdata);
	IDOS->FreeDosObject(DOS_EXAMINEDATA, ddata);
	return REPLACE_SKIP;
}

void layoutcenter(int off)
{
	int a, max;
	int tiny_extra;

	removewindowgadgets(Window);

	scrdata_diskname_width[0] = scrdata_diskname_width[1] = (scrdata_width / 2) - 4;
	scrdata_dispwin_width[0] = scrdata_dispwin_width[1] = (scrdata_width - 24 - config->sliderwidth * 2) / 2;
	scrdata_dirwin_center = ((scrdata_width / 2) - 2) + scrdata_xoffset;

	max = (scrdata_dirwin_center - ((config->sliderwidth * 2) + (scrdata_font_xsize * 10))) /*/scrdata_font_xsize */ ;
	if(off > max)
		off = max;
	else if(off < -max)
		off = -max;

	scrdata_diskname_width[0] += /*scrdata_font_xsize* */ off;
	scrdata_diskname_width[1] -= /*scrdata_font_xsize* */ off;
	scrdata_dispwin_width[0] += /*scrdata_font_xsize* */ off;
	scrdata_dispwin_width[1] -= /*scrdata_font_xsize* */ off;

	scrdata_dispwin_center = scrdata_xoffset + scrdata_diskname_width[0] + 4;

	for(a = 0; a < 2; a++)
	{
		vert_propgad[a].Width = config->sliderwidth;
		vert_propgad[a].Height = scrdata_prop_height;
	}

	if(config->slider_pos & 1)
	{
		vert_propgad[0].LeftEdge = scrdata_dispwin_center - config->sliderwidth - 4;
		scrdata_dispwin_left[0] = scrdata_xoffset;
	}
	else
	{
		vert_propgad[0].LeftEdge = scrdata_xoffset + 4;
		scrdata_dispwin_left[0] = vert_propgad[0].LeftEdge + vert_propgad[0].Width + 4;
	}

	if(config->slider_pos & 2)
	{
		scrdata_dispwin_left[1] = scrdata_dispwin_center;
		vert_propgad[1].LeftEdge = scrdata_dispwin_left[1] + scrdata_dispwin_width[1] + 8;
		tiny_extra = config->sliderwidth + 8;
	}
	else
	{
		vert_propgad[1].LeftEdge = scrdata_dispwin_center + 4;
		scrdata_dispwin_left[1] = vert_propgad[1].LeftEdge + vert_propgad[0].Width + 4;
		tiny_extra = 0;
	}

	scrdata_dispwin_xpos[0] = scrdata_dispwin_left[0];
	scrdata_dispwin_xpos[1] = scrdata_dispwin_left[1];

	scrdata_diskname_xpos[0] = scrdata_xoffset;
	scrdata_diskname_xpos[1] = scrdata_xoffset + scrdata_diskname_width[0] + 4;

	for(a = 0; a < 2; a++)
	{
		scrdata_dispwin_nchars[a] = scrdata_dispwin_width[a];
		scrdata_dirwin_width[a] = scrdata_dispwin_nchars[a];
		scrdata_dirwin_xpos[a] = scrdata_dispwin_xpos[a] + 2 + ((scrdata_dispwin_width[a] - scrdata_dirwin_width[a]) / 2);
	}

	for(a = 0; a < 2; a++)
	{
		screen_gadgets[SCRGAD_MOVEUP1 + a].TopEdge = scrdata_dispwin_ypos - 1;
		screen_gadgets[SCRGAD_MOVEDOWN1 + a].TopEdge = scrdata_dispwin_ypos + scrdata_prop_height + config->arrowsize[0] + 4;

		switch (config->arrowpos[0])
		{
		case ARROWS_INSIDE:
			screen_gadgets[SCRGAD_MOVEUP1 + a].TopEdge = scrdata_dispwin_ypos + scrdata_prop_height + 3;
			vert_propgad[a].TopEdge = scrdata_dispwin_ypos + 1;
			break;
		case ARROWS_OUTSIDE:
			screen_gadgets[SCRGAD_MOVEDOWN1 + a].TopEdge = scrdata_dispwin_ypos + config->arrowsize[0];
			vert_propgad[a].TopEdge = scrdata_dispwin_ypos + ((config->arrowsize[0] * 2) + 3);
			break;
		case ARROWS_EACHSIDE:
			vert_propgad[a].TopEdge = scrdata_dispwin_ypos + config->arrowsize[0] + 2;
			break;
		}
	}

	for(a = 0; a < 2; a++)
	{
		path_strgadget[a].TopEdge = scrdata_dirwin_bottom + 30 + config->sliderheight;
		path_strgadget[a].Width = scrdata_dispwin_width[a] - ((config->arrowsize[2] * 2) + 20);
		path_strgadget[a].Height = scrdata_string_height;

		horiz_propgad[a].TopEdge = scrdata_dirwin_bottom + 26;
		horiz_propgad[a].Width = scrdata_dispwin_width[a] - ((config->arrowsize[1] * 2) + 20);
		horiz_propgad[a].Height = config->sliderheight;

		screen_gadgets[SCRGAD_TINYBUFFERLIST + a].TopEdge = scrdata_dirwin_bottom + 24;
		screen_gadgets[SCRGAD_TINYBUFFERLIST + a].Height = config->sliderheight + 4;
		screen_gadgets[SCRGAD_TINYSELECT + a].TopEdge = scrdata_dirwin_bottom + 28 + config->sliderheight;
		screen_gadgets[SCRGAD_TINYSELECT + a].Height = scrdata_string_height + 4;
		screen_gadgets[SCRGAD_MOVEUP1 + a].LeftEdge = screen_gadgets[SCRGAD_MOVEDOWN1 + a].LeftEdge = vert_propgad[a].LeftEdge - 4;
		screen_gadgets[SCRGAD_LEFTPARENT + a].Height = scrdata_dispwin_height;
		screen_gadgets[SCRGAD_LEFTPARENT + a].TopEdge = scrdata_dispwin_ypos;
	}

	switch (config->arrowpos[1])
	{
	case ARROWS_INSIDE:
		horiz_propgad[0].LeftEdge = scrdata_xoffset + 4;
		screen_gadgets[SCRGAD_MOVELEFT1].LeftEdge = horiz_propgad[0].LeftEdge + horiz_propgad[0].Width + 4;
		screen_gadgets[SCRGAD_MOVERIGHT1].LeftEdge = horiz_propgad[0].LeftEdge + horiz_propgad[0].Width + config->arrowsize[1] + 12;

		screen_gadgets[SCRGAD_MOVELEFT2].LeftEdge = scrdata_dispwin_xpos[1] + tiny_extra;
		screen_gadgets[SCRGAD_MOVERIGHT2].LeftEdge = screen_gadgets[SCRGAD_MOVELEFT2].LeftEdge + config->arrowsize[1] + 8;
		horiz_propgad[1].LeftEdge = screen_gadgets[SCRGAD_MOVERIGHT2].LeftEdge + config->arrowsize[1] + 12;
		break;

	case ARROWS_OUTSIDE:
		screen_gadgets[SCRGAD_MOVELEFT1].LeftEdge = scrdata_xoffset;
		screen_gadgets[SCRGAD_MOVERIGHT1].LeftEdge = scrdata_xoffset + config->arrowsize[1] + 8;
		horiz_propgad[0].LeftEdge = scrdata_xoffset + (config->arrowsize[1] * 2) + 20;

		horiz_propgad[1].LeftEdge = scrdata_dispwin_xpos[1] + 4 + tiny_extra;
		screen_gadgets[SCRGAD_MOVELEFT2].LeftEdge = horiz_propgad[1].LeftEdge + horiz_propgad[1].Width + 4;
		screen_gadgets[SCRGAD_MOVERIGHT2].LeftEdge = screen_gadgets[SCRGAD_MOVELEFT2].LeftEdge + config->arrowsize[1] + 8;
		break;

	case ARROWS_EACHSIDE:
		screen_gadgets[SCRGAD_MOVELEFT1].LeftEdge = scrdata_xoffset;
		horiz_propgad[0].LeftEdge = scrdata_xoffset + config->arrowsize[1] + 12;
		screen_gadgets[SCRGAD_MOVERIGHT1].LeftEdge = scrdata_xoffset + horiz_propgad[0].Width + config->arrowsize[1] + 16;

		screen_gadgets[SCRGAD_MOVELEFT2].LeftEdge = scrdata_dispwin_xpos[1] + tiny_extra;
		horiz_propgad[1].LeftEdge = screen_gadgets[SCRGAD_MOVELEFT2].LeftEdge + config->arrowsize[1] + 12;
		screen_gadgets[SCRGAD_MOVERIGHT2].LeftEdge = horiz_propgad[1].LeftEdge + horiz_propgad[1].Width + 4;
		break;
	}

	switch (config->arrowpos[2])
	{
	case ARROWS_INSIDE:
		path_strgadget[0].LeftEdge = scrdata_xoffset + 4;
		screen_gadgets[SCRGAD_BUFFERLEFT1].LeftEdge = path_strgadget[0].LeftEdge + path_strgadget[0].Width + 4;
		screen_gadgets[SCRGAD_BUFFERRIGHT1].LeftEdge = path_strgadget[0].LeftEdge + path_strgadget[0].Width + config->arrowsize[2] + 12;

		screen_gadgets[SCRGAD_BUFFERLEFT2].LeftEdge = scrdata_dispwin_xpos[1] + tiny_extra;
		screen_gadgets[SCRGAD_BUFFERRIGHT2].LeftEdge = screen_gadgets[SCRGAD_BUFFERLEFT2].LeftEdge + config->arrowsize[2] + 8;
		path_strgadget[1].LeftEdge = screen_gadgets[SCRGAD_BUFFERRIGHT2].LeftEdge + config->arrowsize[2] + 12;
		break;

	case ARROWS_OUTSIDE:
		screen_gadgets[SCRGAD_BUFFERLEFT1].LeftEdge = scrdata_xoffset;
		screen_gadgets[SCRGAD_BUFFERRIGHT1].LeftEdge = scrdata_xoffset + config->arrowsize[2] + 8;
		path_strgadget[0].LeftEdge = scrdata_xoffset + (config->arrowsize[2] * 2) + 20;

		path_strgadget[1].LeftEdge = scrdata_dispwin_xpos[1] + 4 + tiny_extra;
		screen_gadgets[SCRGAD_BUFFERLEFT2].LeftEdge = path_strgadget[1].LeftEdge + path_strgadget[1].Width + 4;
		screen_gadgets[SCRGAD_BUFFERRIGHT2].LeftEdge = screen_gadgets[SCRGAD_BUFFERLEFT2].LeftEdge + config->arrowsize[2] + 8;
		break;

	case ARROWS_EACHSIDE:
		screen_gadgets[SCRGAD_BUFFERLEFT1].LeftEdge = scrdata_xoffset;
		path_strgadget[0].LeftEdge = scrdata_xoffset + config->arrowsize[2] + 12;
		screen_gadgets[SCRGAD_BUFFERRIGHT1].LeftEdge = scrdata_xoffset + path_strgadget[0].Width + config->arrowsize[2] + 16;

		screen_gadgets[SCRGAD_BUFFERLEFT2].LeftEdge = scrdata_dispwin_xpos[1] + tiny_extra;
		path_strgadget[1].LeftEdge = screen_gadgets[SCRGAD_BUFFERLEFT2].LeftEdge + config->arrowsize[2] + 12;
		screen_gadgets[SCRGAD_BUFFERRIGHT2].LeftEdge = path_strgadget[1].LeftEdge + path_strgadget[1].Width + 4;
		break;
	}

	for(a = 0; a < 4; a++)
	{
		screen_gadgets[SCRGAD_BUFFERLEFT1 + a].TopEdge = scrdata_dirwin_bottom + 28 + config->sliderheight;
		screen_gadgets[SCRGAD_BUFFERLEFT1 + a].Height = scrdata_string_height + 4;
		screen_gadgets[SCRGAD_MOVELEFT1 + a].TopEdge = scrdata_dirwin_bottom + 24;
		screen_gadgets[SCRGAD_MOVELEFT1 + a].Height = config->sliderheight + 4;
		screen_gadgets[SCRGAD_MOVEUP1 + a].Width = screen_gadgets[SCRGAD_TINYBUFFERLIST + a].Width = config->sliderwidth + 8;
	}

	screen_gadgets[SCRGAD_MOVEUP1].Height = config->arrowsize[0] + 1;
	screen_gadgets[SCRGAD_MOVEDOWN1].Height = config->arrowsize[0] + 1;
	screen_gadgets[SCRGAD_MOVEUP2].Height = config->arrowsize[0] + 1;
	screen_gadgets[SCRGAD_MOVEDOWN2].Height = config->arrowsize[0] + 1;

	screen_gadgets[SCRGAD_MOVELEFT1].Width = config->arrowsize[1] + 8;
	screen_gadgets[SCRGAD_MOVERIGHT1].Width = config->arrowsize[1] + 8;
	screen_gadgets[SCRGAD_MOVELEFT2].Width = config->arrowsize[1] + 8;
	screen_gadgets[SCRGAD_MOVERIGHT2].Width = config->arrowsize[1] + 8;

	screen_gadgets[SCRGAD_BUFFERLEFT1].Width = config->arrowsize[2] + 8;
	screen_gadgets[SCRGAD_BUFFERRIGHT1].Width = config->arrowsize[2] + 8;
	screen_gadgets[SCRGAD_BUFFERLEFT2].Width = config->arrowsize[2] + 8;
	screen_gadgets[SCRGAD_BUFFERRIGHT2].Width = config->arrowsize[2] + 8;

	screen_gadgets[SCRGAD_TINYBUFFERLIST].LeftEdge = screen_gadgets[SCRGAD_TINYSELECT].LeftEdge = scrdata_dispwin_center - config->sliderwidth - 8;
	screen_gadgets[SCRGAD_TINYRESELECT].LeftEdge = screen_gadgets[SCRGAD_TINYAREXX].LeftEdge = scrdata_dispwin_center;

	screen_gadgets[SCRGAD_LEFTPARENT].LeftEdge = 0;
	screen_gadgets[SCRGAD_RIGHTPARENT].LeftEdge = scrdata_xoffset + (scrdata_width - 2);

	for(a = SCRGAD_TINYBUFFERLIST; a <= SCRGAD_BUFFERRIGHT2; a++)
	{
		screen_gadgets[a].GadgetRender = NULL;
		screen_gadgets[a].SelectRender = NULL;
	}
}

void fix_rmb_gadgets()
{
	int a;

	mainrmbgads[0].x = scrdata_gadget_offset;
	mainrmbgads[0].y = scrdata_gadget_ypos - 1;
	mainrmbgads[0].w = scrdata_drive_width;
	mainrmbgads[0].h = scr_gadget_rows * scrdata_gadget_height;

	if(drive_bank_unsel_border)
	{
		mainrmbgads[0].flags |= RGF_ALTBORD;
		mainrmbgads[0].high_bord[0] = drive_bank_unsel_border;
		mainrmbgads[0].high_bord[1] = drive_bank_sel_border;
	}
	else
		mainrmbgads[0].flags &= ~RGF_ALTBORD;

	for(a = 0; a < 4; a++)
		makermbgad(&mainrmbgads[a + 1], &screen_gadgets[SCRGAD_TINYBUFFERLIST + a], 1);
	for(a = 0; a < 2; a++)
		makermbgad(&mainrmbgads[a + 5], &screen_gadgets[SCRGAD_LEFTPARENT + a], 0);
}

void dosizedirwindows(int size)
{
	ULONG class;
	USHORT code;
	int x, ox, fail = 0, max;

	max = (scrdata_dirwin_center - ((config->sliderwidth * 2) + (scrdata_font_xsize * 10)));
	if(size != 65536)
	{
		if(size > max)
			size = max;
		else if(size < -max)
			size = -max;
		ox = size;
		fail = -1;
	}
	else
	{
		if((scrdata_width - 24 - config->sliderwidth * 2) < scrdata_font_xsize * 24)
			return;

		IGraphics->SetDrMd(main_rp, COMPLEMENT);
		sizedirwindow_gfx((ox = config->windowdelta));
		Window->Flags |= WFLG_REPORTMOUSE;

		FOREVER
		{
			while(getintuimsg())
			{
				if(IMsg->MouseX < scrdata_dirwin_center)
				{
					x = (IMsg->MouseX - (scrdata_dirwin_center + 4));
				}
				else
				{
					x = (IMsg->MouseX - scrdata_dirwin_center);
				}

				if(x > max)
					x = max;
				else if(x < -max)
					x = -max;
				class = IMsg->Class;
				code = IMsg->Code;
				IExec->ReplyMsg((struct Message *)IMsg);
				if(class == IDCMP_MOUSEBUTTONS)
				{
					if(code == MENUDOWN)
					{
						fail = 1;
						break;
					}
					if(code == SELECTUP)
					{
						fail = -1;
						break;
					}
				}
				else if(class == IDCMP_MOUSEMOVE)
				{
					if(x != ox)
					{
						sizedirwindow_gfx(ox);
						sizedirwindow_gfx((ox = x));
					}
				}
				else if(class == IDCMP_RAWKEY)
				{
					if(code == 0x45)
					{
						fail = -1;
						sizedirwindow_gfx(ox);
						sizedirwindow_gfx((ox = 0));
						break;
					}
				}
			}
			if(fail)
				break;
			IExec->Wait(1 << Window->UserPort->mp_SigBit);
		}
		sizedirwindow_gfx(ox);
		IGraphics->SetDrMd(main_rp, JAM2);
		Window->Flags &= ~WFLG_REPORTMOUSE;
	}
	if(fail == -1 && ox != config->windowdelta && !status_iconified)
	{
		busy();
		layoutcenter((config->windowdelta = ox));
		fix_rmb_gadgets();
		IGraphics->SetAPen(main_rp, screen_pens[0].pen);
		rectfill(main_rp, scrdata_dispwin_center - config->sliderwidth - 7, scrdata_dispwin_ypos - 1, (config->sliderwidth * 2) + 13, scrdata_dispwin_height + 2);
		IGraphics->RectFill(main_rp, scrdata_xoffset, horiz_propgad[0].TopEdge - 2, scrdata_xoffset + scrdata_width - 1, path_strgadget[0].TopEdge + path_strgadget[0].Height + 1);
		setupdisplay(0);
		for(x = 0; x < 2; x++)
			IDOpus->RefreshStrGad(&path_strgadget[x], Window);
		okay();
		unbusy();
	}
}

void sizedirwindow_gfx(int dx)
{
	int x;

	x = scrdata_dirwin_center + (dx);
	rectfill(main_rp, x, scrdata_dispwin_ypos, 4, scrdata_dispwin_height);
}

void fix_requester(struct RequesterBase *req, STRPTR title)
{
	req->rb_fg = screen_pens[config->requestfg].pen;
	req->rb_bg = screen_pens[config->requestbg].pen;
	req->rb_shine = screen_pens[config->gadgettopcol].pen;
	req->rb_shadow = screen_pens[config->gadgetbotcol].pen;
	req->rb_font = scr_font[FONT_REQUEST];
	req->rb_screen = Window->WScreen;
	req->rb_extend = &requester_stringex;
	fix_stringex(&requester_stringex);
	if(config->generalscreenflags & SCR_GENERAL_REQDRAG)
	{
		req->rb_title = title;
		req->rb_flags = RBF_BORDERS | RBF_STRINGS;
	}
	else
	{
		req->rb_title = NULL;
		req->rb_flags = RBF_STRINGS;
	}
	req->rb_string_table = globstring;
	if(status_publicscreen)
	{
		req->rb_window = Window;
		req->rb_flags |= RBF_WINDOWCENTER;
	}
	else
		req->rb_flags &= ~RBF_WINDOWCENTER;
}

void fix_stringex(struct StringExtend *extend)
{
	extend->Font = scr_font[FONT_REQUEST];
	extend->Pens[0] = screen_pens[config->stringfgcol].pen;
	extend->Pens[1] = screen_pens[config->stringbgcol].pen;
	extend->ActivePens[0] = screen_pens[config->stringselfgcol].pen;
	extend->ActivePens[1] = screen_pens[config->stringselbgcol].pen;
}

struct Gadget *addreqgadgets(struct RequesterBase *reqbase, struct TagItem **gadgets, int mask, int *count)
{
	int gad;
	struct Gadget *gadget = NULL, *newgadget, *firstgadget = NULL;

	for(gad = 0;; gad++)
	{
		if(!gadgets[gad])
			break;
		if(gad > 31 || !(mask & (1 << gad)))
		{
			if(!(newgadget = (struct Gadget *)IDOpus->AddRequesterObject(reqbase, gadgets[gad])))
				return (NULL);
			if(gadget)
				gadget->NextGadget = newgadget;
			else
				firstgadget = newgadget;
			gadget = newgadget;
			if(count)
				++ * count;
		}
	}
	if(firstgadget)
		IDOpus->AddGadgets(reqbase->rb_window, firstgadget, NULL, gad, reqbase->rb_shine, reqbase->rb_shadow, 1);
	return (firstgadget);
}

void set_reqobject(struct TagItem *object, uint32 tagmatch, uint32 data)
{
	int tag;

	for(tag = 0;; tag++)
	{
		if(object[tag].ti_Tag == TAG_END)
			break;
		if(object[tag].ti_Tag == tagmatch)
		{
			object[tag].ti_Data = data;
			break;
		}
	}
}

void removewindowgadgets(struct Window *window)
{
	if(window && window->FirstGadget)
	{
		struct Gadget *gad;

		FOREVER
		{
			gad = window->FirstGadget;
			while(gad)
			{
				if(!(gad->GadgetType & GTYP_SYSGADGET))
				{
					IIntuition->RemoveGList(window, gad, 1);
					break;
				}
				gad = gad->NextGadget;
			}
			if(!gad)
				break;
		}
	}
}
