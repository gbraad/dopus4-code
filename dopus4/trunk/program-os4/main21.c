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

void fixdrivegadgets()
{
	int i, b, l, len, ty;

	if(!scrdata_drive_width || !scr_gadget_rows)
		return;
	IGraphics->SetFont(main_rp, scr_font[FONT_GADGETS]);

	ty = scrdata_gadget_ypos + scr_font[FONT_GADGETS]->tf_Baseline + ((scrdata_gadget_height - 2 - scr_font[FONT_GADGETS]->tf_YSize) / 2);

	for(i = 0; i < scr_gadget_rows; i++)
	{
		b = i + data_drive_offset;
		IGraphics->SetAPen(main_rp, screen_pens[config->drive[b].bpen].pen);
		rectfill(main_rp, scrdata_gadget_offset + 2, (i * scrdata_gadget_height) + scrdata_gadget_ypos, scrdata_drive_width - 4, scrdata_gadget_height - 2);
		IGraphics->SetAPen(main_rp, screen_pens[config->drive[b].fpen].pen);
		IGraphics->SetBPen(main_rp, screen_pens[config->drive[b].bpen].pen);
		if(config->drive[b].name && config->drive[b].name[0])
		{
			len = 0;
			l = dotextlength(main_rp, config->drive[b].name, &len, scrdata_drive_width - 4);
			IGraphics->Move(main_rp, scrdata_gadget_offset + ((scrdata_drive_width - l) / 2), (i * scrdata_gadget_height) + ty);
			IGraphics->Text(main_rp, config->drive[b].name, len);
		}
		drive_gadgets[i].Flags = (config->drive[b].name && config->drive[b].name[0] && config->drive[b].function && config->drive[b].function[0]) ? ((drive_unsel_border) ? GFLG_GADGHIMAGE : GFLG_GADGHCOMP) : GFLG_GADGHNONE;
	}
	IGraphics->SetFont(main_rp, scr_font[FONT_GENERAL]);
}

void nextdrives()
{
	int old, a, b;

	if(scrdata_drive_width && scr_gadget_rows)
	{
		old = data_drive_offset;
		FOREVER
		{
			if(data_drive_offset % scr_gadget_rows)
				data_drive_offset += scr_gadget_rows - (data_drive_offset % scr_gadget_rows);
			else
				data_drive_offset += scr_gadget_rows;
			if(data_drive_offset >= USEDRIVECOUNT || data_drive_offset < 0)
				data_drive_offset = 0;
			a = data_drive_offset + scr_gadget_rows;
			for(b = data_drive_offset; b < a; b++)
				if(config->drive[b].name[0] && config->drive[b].function && config->drive[b].function[0])
					break;
			if(b < a || data_drive_offset == old)
				break;
		}
		if(config->generalscreenflags & SCR_GENERAL_GADSLIDERS)
		{
			IDOpus->FixSliderPot(Window, &drive_propgad, data_drive_offset, USEDRIVECOUNT, scr_gadget_rows, !status_iconified);
		}
		if(!status_iconified)
			fixdrivegadgets();
	}
}

void setupgadgets()
{
	int a;

	IIntuition->AddGList(Window, &path_strgadget[0], -1, 2, NULL);
	IIntuition->AddGList(Window, &vert_propgad[0], -1, 2, NULL);
	IIntuition->AddGList(Window, &horiz_propgad[0], -1, 2, NULL);
	IIntuition->AddGList(Window, screen_gadgets, -1, (config->generalscreenflags & SCR_GENERAL_TINYGADS) ? 24 : 18, NULL);
	if(status_publicscreen)
	{
		IIntuition->AddGList(Window, size_gadgets, -1, 2, NULL);
	}
	IIntuition->RefreshGList(&path_strgadget[0], Window, NULL, 2);

	IIntuition->AddGadget(Window, iconifygadget, -1); //~0);
	IIntuition->RefreshGadgets(iconifygadget, Window, NULL);

	for(a = 0; a < 2; a++)
	{
		IDOpus->ShowSlider(Window, &vert_propgad[a]);
		IDOpus->ShowSlider(Window, &horiz_propgad[a]);
	}
	if(config->generalscreenflags & SCR_GENERAL_GADSLIDERS && scr_gadget_rows)
	{
		if(scrdata_drive_width > 0)
		{
			IDOpus->AddGadgets(Window, &drive_propgad, NULL, 1, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen, 1);
		}
		if(scr_gadget_bank_count > 1)
		{
			IDOpus->AddGadgets(Window, &gadget_propgad, NULL, 1, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen, 1);
		}
	}
}

void allocate_borders()
{
	int a, b;

	IDOpus->LFreeRemember(&border_key);

	IDOpus->CreateGadgetBorders(&border_key, scrdata_gadget_width, scrdata_gadget_height, &gadget_sel_border, &gadget_unsel_border, 0, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);

	if(config->generalscreenflags & SCR_GENERAL_INDICATERMB)
		IDOpus->CreateGadgetBorders(&border_key, scrdata_gadget_width, scrdata_gadget_height, &gadget_dog_sel_border, &gadget_dog_unsel_border, 1, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);

	if(scrdata_drive_width && scr_gadget_rows)
	{
		IDOpus->CreateGadgetBorders(&border_key, scrdata_drive_width, scrdata_gadget_height, &drive_sel_border, &drive_unsel_border, 0, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);

		if((drive_bank_sel_border = IDOpus->LAllocRemember(&border_key, sizeof(struct Border) * 2 * scr_gadget_rows, 0)) && (drive_bank_unsel_border = IDOpus->LAllocRemember(&border_key, sizeof(struct Border) * 2 * scr_gadget_rows, 0)))
		{
			for(a = 0; a < scr_gadget_rows; a++)
			{
				b = a * 2;
				IExec->CopyMem((char *)drive_sel_border, (char *)&drive_bank_sel_border[b], sizeof(struct Border) * 2);
				IExec->CopyMem((char *)drive_unsel_border, (char *)&drive_bank_unsel_border[b], sizeof(struct Border) * 2);
				drive_bank_sel_border[b].NextBorder = &drive_bank_sel_border[b + 1];
				drive_bank_sel_border[b + 1].NextBorder = (a < scr_gadget_rows - 1) ? &drive_bank_sel_border[b + 2] : NULL;
				drive_bank_unsel_border[b].NextBorder = &drive_bank_unsel_border[b + 1];
				drive_bank_unsel_border[b + 1].NextBorder = (a < scr_gadget_rows - 1) ? &drive_bank_unsel_border[b + 2] : NULL;
				drive_bank_sel_border[b].TopEdge = a * scrdata_gadget_height;
				drive_bank_sel_border[b + 1].TopEdge = a * scrdata_gadget_height;
				drive_bank_unsel_border[b].TopEdge = a * scrdata_gadget_height;
				drive_bank_unsel_border[b + 1].TopEdge = a * scrdata_gadget_height;
			}
		}
	}
	else
	{
		drive_sel_border = NULL;
		drive_unsel_border = NULL;
		drive_bank_sel_border = NULL;
		drive_bank_unsel_border = NULL;
	}

	IDOpus->AddGadgetBorders(&border_key, &screen_gadgets[SCRGAD_TINYBUFFERLIST], 2, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);

	IDOpus->AddGadgetBorders(&border_key, &screen_gadgets[SCRGAD_TINYSELECT], 2, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);

	if(config->generalscreenflags & SCR_GENERAL_TINYGADS)
		IDOpus->AddGadgetBorders(&border_key, &screen_gadgets[SCRGAD_TINYHELP], 6, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);

	IDOpus->AddGadgetBorders(&border_key, &screen_gadgets[SCRGAD_MOVELEFT1], 4, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);

	IDOpus->AddGadgetBorders(&border_key, &screen_gadgets[SCRGAD_MOVEUP1], 4, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);

	IDOpus->AddGadgetBorders(&border_key, &screen_gadgets[SCRGAD_BUFFERLEFT1], 4, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);
}

void drawgadgets(int addgads, int bankoffset)
{
	int x, y, yoff, a, b, len, l, ty, num, uspos, x1, y1;
	char buf[256];

	if(addgads)
		allocate_borders();

	if(!scr_gadget_rows)
		return;
	if(status_iconified > 0)
		yoff = Window->BorderTop + 1;
	else
	{
		yoff = scrdata_gadget_ypos;
		IGraphics->SetFont(main_rp, scr_font[FONT_GADGETS]);
	}

	if(addgads != -1)
	{
		if(scrdata_drive_width)
		{
			for(y = 0; y < scr_gadget_rows; y++)
				do3dbox(main_rp, scrdata_gadget_offset + 2, (y * scrdata_gadget_height) + yoff, scrdata_drive_width - 4, scrdata_gadget_height - 2);
		}

		for(x = 0; x < 7; x++)
		{
			for(y = 0; y < scr_gadget_rows; y++)
			{
				do3dbox(main_rp, (x * scrdata_gadget_width) + scrdata_gadget_xpos + 2, (y * scrdata_gadget_height) + yoff, scrdata_gadget_width - 4, scrdata_gadget_height - 2);
			}
		}
	}

	if(status_iconified > 0)
		ty = Window->BorderTop + main_rp->Font->tf_Baseline + 1;
	else
		ty = yoff + scr_font[FONT_GADGETS]->tf_Baseline + ((scrdata_gadget_height - 2 - scr_font[FONT_GADGETS]->tf_YSize) / 2);

	num = 0;
	if(addgads != -1)
	{
		for(y = 0; y < scr_gadget_rows; y++)
		{
			a = ((data_gadgetrow_offset + y) * 7) + bankoffset;
			for(x = 0; x < 7; x++, a++)
			{
				main_gadgets[num].Flags = GFLG_GADGHNONE;
				if(dopus_curgadbank)
				{
					IGraphics->SetAPen(main_rp, screen_pens[dopus_curgadbank->gadgets[a].bpen].pen);
					rectfill(main_rp, (x1 = ((x * scrdata_gadget_width) + scrdata_gadget_xpos + 2)), (y1 = ((y * scrdata_gadget_height) + yoff)), scrdata_gadget_width - 4, scrdata_gadget_height - 2);
					if(dopus_curgadbank->gadgets[a].name && dopus_curgadbank->gadgets[a].name[0])
					{
						IGraphics->SetAPen(main_rp, screen_pens[dopus_curgadbank->gadgets[a].fpen].pen);
						IGraphics->SetBPen(main_rp, screen_pens[dopus_curgadbank->gadgets[a].bpen].pen);
						len = makeusstring(dopus_curgadbank->gadgets[a].name, buf, &uspos);
						l = dotextlength(main_rp, buf, &len, scrdata_gadget_width - 4);
						buf[len] = 0;
						IDOpus->UScoreText(main_rp, buf, (x * scrdata_gadget_width) + scrdata_gadget_xpos + ((scrdata_gadget_width - l) / 2), (y * scrdata_gadget_height) + ty, uspos);
					}
					main_gadgets[num].Activation &= ~GACT_BOOLEXTEND;
					main_gadgets[num].SpecialInfo = NULL;
					if(config->generalscreenflags & SCR_GENERAL_INDICATERMB && !bankoffset && isvalidgad(&dopus_curgadbank->gadgets[a + (GADCOUNT / 2)]))
					{
						drawgadgetcorner(main_rp, x1, y1);
						fix_gadget_highlight(&dopus_curgadbank->gadgets[a], &main_gadgets[num], 1);
					}
					else
						fix_gadget_highlight(&dopus_curgadbank->gadgets[a], &main_gadgets[num], 0);
				}
				++num;
			}
		}
	}

	if(addgads)
	{
		num = 0;
		for(y = 0; y < scr_gadget_rows; y++)
		{
			b = ((data_gadgetrow_offset + y) * 7) + bankoffset;
			for(x = 0; x < 7; x++, b++, num++)
			{
				main_gadgets[num].Activation |= GACT_RELVERIFY;
				main_gadgets[num].LeftEdge = (x * scrdata_gadget_width) + scrdata_gadget_xpos;
				main_gadgets[num].TopEdge = (y * scrdata_gadget_height) + scrdata_gadget_ypos - 1;
				main_gadgets[num].Width = scrdata_gadget_width;
				main_gadgets[num].Height = scrdata_gadget_height;
				main_gadgets[num].GadgetType = GTYP_BOOLGADGET;
				main_gadgets[num].GadgetID = MAIN_GAD_BASE + b;
				main_gadgets[num].NextGadget = (b < 41) ? &main_gadgets[num + 1] : NULL;

				if(dopus_curgadbank)
				{
					if(config->generalscreenflags & SCR_GENERAL_INDICATERMB && !bankoffset && isvalidgad(&dopus_curgadbank->gadgets[b + (GADCOUNT / 2)]))
					{
						fix_gadget_highlight(&dopus_curgadbank->gadgets[b], &main_gadgets[num], 1);
					}
					else
						fix_gadget_highlight(&dopus_curgadbank->gadgets[b], &main_gadgets[num], 0);
				}
				else
					main_gadgets[num].Flags = GFLG_GADGHNONE;
			}
		}
		IIntuition->AddGList(Window, main_gadgets, -1, num, NULL);
		if(scrdata_drive_width)
		{
			for(y = 0; y < scr_gadget_rows; y++)
			{
				drive_gadgets[y].Flags = (config->drive[y + data_drive_offset].name[0] && config->drive[y + data_drive_offset].function && config->drive[y + data_drive_offset].function[0]) ? ((drive_unsel_border) ? GFLG_GADGHIMAGE : GFLG_GADGHCOMP) : GFLG_GADGHNONE;
				drive_gadgets[y].LeftEdge = scrdata_gadget_offset;
				drive_gadgets[y].Width = scrdata_drive_width;
				drive_gadgets[y].TopEdge = (y * scrdata_gadget_height) + scrdata_gadget_ypos - 1;
				drive_gadgets[y].Height = scrdata_gadget_height;
				drive_gadgets[y].Activation = GACT_RELVERIFY;
				drive_gadgets[y].GadgetType = GTYP_BOOLGADGET;
				drive_gadgets[y].GadgetRender = drive_unsel_border;
				drive_gadgets[y].SelectRender = drive_sel_border;
				drive_gadgets[y].GadgetID = 200 + y;
				drive_gadgets[y].NextGadget = (y < scr_gadget_rows - 1) ? &drive_gadgets[y + 1] : NULL;
			}
			IIntuition->AddGList(Window, drive_gadgets, -1, scr_gadget_rows, NULL);
		}
	}
	if(status_iconified < 1)
		IGraphics->SetFont(main_rp, scr_font[FONT_GENERAL]);
}

void fix_gadget_highlight(struct newdopusfunction *func, struct Gadget *gad, int rmb)
{
	struct Border *selborder, *unselborder;

	if(rmb)
	{
		selborder = gadget_dog_sel_border;
		unselborder = gadget_dog_unsel_border;
	}
	else
	{
		selborder = gadget_sel_border;
		unselborder = gadget_unsel_border;
	}

	gad->GadgetRender = NULL;
	gad->SelectRender = NULL;

	if(isvalidgad(func))
	{
		if(unselborder)
		{
			gad->Flags = GFLG_GADGHIMAGE;
			gad->GadgetRender = unselborder;
			gad->SelectRender = selborder;
		}
		else
			gad->Flags = GFLG_GADGHCOMP;
	}
	else
		gad->Flags = GFLG_GADGHNONE;
}

void init_menus()
{
	int a;

	for(a = 0; a < 5; a++)
	{
		menu_menus[a].MenuName = config->menutit[a];
		menu_menus[a].TopEdge = 0;
		menu_menus[a].Flags = MENUENABLED;
	}
	for(a = 0; a < MENUCOUNT; a++)
	{
		menu_intuitext[a * 2].DrawMode = JAM2;
		menu_intuitext[a * 2].ITextFont = &menu_attr;
		menu_intuitext[a * 2].IText = (UBYTE *) menu_spaceitemnames[a / 20];
		menu_intuitext[a * 2].NextText = &menu_intuitext[(a * 2) + 1];
		menu_intuitext[(a * 2) + 1].DrawMode = JAM2;
		menu_intuitext[(a * 2) + 1].ITextFont = &menu_attr;
		menu_intuitext[(a * 2) + 1].IText = (UBYTE *) & menu_itemnames[a * MAXMENULENGTH];
		menu_menuitem[a].Flags = ITEMTEXT | ITEMENABLED | HIGHCOMP;
		menu_menuitem[a].ItemFill = (APTR) & menu_intuitext[a * 2];
	}
}

void layout_menus()
{
	int a, b, c, d, e, w, f, x = 0, l, rn = 0, iwid[20], spacew, numspace, snum, barflag, t, top;
	struct Menu *firstmenu = NULL, *lastmenu = NULL;
	struct TextFont *title_font;
	struct DrawInfo *drinfo = NULL;
	struct Image *barimage;
	UBYTE fpen, bpen, menu_fpen, menu_bpen;
	char *name;
	int spacewidth;

	if(Window->MenuStrip)
		IIntuition->ClearMenuStrip(Window);
	IDOpus->LFreeRemember(&menu_key);

	title_font = Window->WScreen->RastPort.Font;
	if(!MainScreen)
		drinfo = IIntuition->GetScreenDrawInfo(Window->WScreen);

	spacew = gettextlength(scr_font[FONT_MENUS], " ", NULL, 0);

	if(drinfo)
	{
		menu_fpen = drinfo->dri_Pens[BARDETAILPEN];
		menu_bpen = drinfo->dri_Pens[BARBLOCKPEN];
	}
	else
	{
		menu_fpen = screen_pens[1].pen;
		menu_bpen = screen_pens[2].pen;
	}

	for(a = 0; a < MENUCOUNT; a++)
	{
		fpen = screen_pens[config->menu[a].fpen].pen;
		bpen = screen_pens[config->menu[a].bpen].pen;

		menu_intuitext[a * 2].BackPen = (UBYTE) bpen;
		menu_intuitext[(a * 2) + 1].FrontPen = (UBYTE) fpen;
		menu_intuitext[(a * 2) + 1].BackPen = (UBYTE) bpen;
	}

	for(a = 0; a < 5; a++)
	{
		c = 0;
		d = a * 20;
		l = 0;

		menu_menus[a].Width = gettextlength(title_font, config->menutit[a], &l, 0) + title_font->tf_XSize;
		menu_menus[a].LeftEdge = x;

		for(b = 19; b >= 0; b--)
		{
			if(config->menu[d + b].name && config->menu[d + b].name[0])
			{
				c = 1;
				break;
			}
		}

		if(c)
		{
			menu_real_number[rn++] = a;
			x += menu_menus[a].Width + (title_font->tf_XSize - 1);
			f = a * 20;
			w = menu_menus[a].Width;
			for(e = 0; e < 20; e++)
			{
				menu_menuitem[f].NextItem = NULL;
				if(config->menu[f].name)
					iwid[e] = gettextlength(scr_font[FONT_MENUS], config->menu[f].name, NULL, 0);
				else
					iwid[e] = 0;
				if(iwid[e] > w)
					w = iwid[e];
				++f;
			}

			f = a * 20;

			numspace = w / spacew;
			IExec->CopyMem(str_space_string, menu_spaceitemnames[a], numspace);
			menu_spaceitemnames[a][numspace] = 0;
			snum = (numspace > 15) ? 15 : numspace;
			spacewidth = gettextlength(scr_font[FONT_MENUS], menu_spaceitemnames[a], NULL, 0);

			top = 0;
			barimage = NULL;

			for(e = 0; e < 20; e++)
			{

				menu_menuitem[f].TopEdge = top;
				menu_menuitem[f].Width = w;
				menu_menuitem[f].Height = scr_font[FONT_MENUS]->tf_YSize;

				menu_menuitem[f].Flags = ITEMTEXT | ITEMENABLED | ((isvalidgad(&config->menu[f])) ? HIGHCOMP : HIGHNONE);

				menu_intuitext[f * 2].LeftEdge = (menu_menuitem[f].LeftEdge + w) - spacewidth;

				name = &menu_itemnames[f * MAXMENULENGTH];
				barflag = 0;
				if(config->menu[f].name && config->menu[f].name[0])
				{
					IDOpus->LStrnCpy(name, config->menu[f].name, MAXMENULENGTH - 1);
					if(name[(t = (strlen(name) - 1))] == '~')
					{
						barflag = 1;
						name[t] = 0;
					}
				}
				else
				{
					IExec->CopyMem(str_space_string, name, snum);
					name[snum] = 0;
				}
				top += scr_font[FONT_MENUS]->tf_YSize;
				if(barflag)
				{
					top += 6;
					menu_menuitem[f].MutualExclude = 1;
					if(!barimage)
						barimage = get_bar_image(menu_fpen, menu_bpen, w - 1);
				}
				++f;
			}
			menu_menus[a].Flags = MENUENABLED;
			menu_menus[a].FirstItem = &menu_menuitem[d];
			for(c = 0, t = d; c < b; c++, t++)
				get_bar_item(&menu_menuitem[t], &menu_menuitem[t + 1], barimage);
			menu_menuitem[t].MutualExclude = 1;
			get_bar_item(&menu_menuitem[t], NULL, NULL);
			if(firstmenu)
				lastmenu->NextMenu = &menu_menus[a];
			else
				firstmenu = &menu_menus[a];
			lastmenu = &menu_menus[a];
			lastmenu->NextMenu = NULL;
		}
	}
	if(firstmenu)
	{
		IIntuition->SetMenuStrip(Window, firstmenu);
	}
	if(drinfo)
		IIntuition->FreeScreenDrawInfo(Window->WScreen, drinfo);
}

int maxgadwidth(struct TextFont *font, char **gads, int num)
{
	int a, mw = 0, w, len;

	for(a = 0; a < num; a++)
	{
		len = 0;
		if((w = gettextlength(font, gads[a], &len, -1)) > mw)
			mw = w;
	}
	return (mw);
}

int gettextlength(struct TextFont *font, char *text, int *len, int width)
{
	struct RastPort testrast;

	IGraphics->InitRastPort(&testrast);
	if(font)
		IGraphics->SetFont(&testrast, font);
	return (dotextlength(&testrast, text, len, width));
}

int dotextlength(struct RastPort *r, char *text, int *len, int width)
{
	int l, x = 0;
	struct TextExtent ext;

	if(!len || !(l = *len))
		l = strlen(text);
	if(l)
	{
		if(width > 0)
		{
			l = IGraphics->TextFit(r, text, l, &ext, NULL, 1, width, r->Font->tf_YSize + 1);
			x = ext.te_Width;
		}
		else
		{
			while((x = IGraphics->TextLength(r, text, l)) > width && width > 0)
			{
				if(!(--l))
					break;
			}
		}
	}
	if(len)
		*len = l;
	return (x);
}

void doposdriveprop()
{
	int off, old;

	if(scr_gadget_rows)
	{
		IDOpus->ShowSlider(Window, &drive_propgad);
		old = data_drive_offset;
		off = IDOpus->GetSliderPos(&drive_propgad, USEDRIVECOUNT, scr_gadget_rows);
		if(old != off)
		{
			data_drive_offset = off;
			fixdrivegadgets();
		}
	}
}

int getgadbankcount(void)
{
	struct dopusgadgetbanks *bank = dopus_firstgadbank;
	int bankcount = 0;

	for(;; bankcount++)
	{
		if(!bank)
			break;
		bank = bank->next;
	}
	return bankcount;
}

void doposgadgetprop(int ref)
{
	if(scr_gadget_rows)
	{
		struct dopusgadgetbanks *oldbank = dopus_curgadbank;
		int a, bankcount = getgadbankcount(), bankstep, oldoff = data_gadgetrow_offset, num;

		IDOpus->ShowSlider(Window, &gadget_propgad);
		bankstep = 6 / scr_gadget_rows;
		data_gadgetrow_offset = IDOpus->GetSliderPos(&gadget_propgad, (bankcount * 6) / scr_gadget_rows, 1);
		a = data_gadgetrow_offset / bankstep;
		data_gadgetrow_offset %= bankstep;
		data_gadgetrow_offset *= scr_gadget_rows;
		dopus_curgadbank = dopus_firstgadbank;
		num = a;
		while((num--) && dopus_curgadbank->next)
			dopus_curgadbank = dopus_curgadbank->next;
		if(dopus_curgadbank != oldbank || data_gadgetrow_offset != oldoff)
			drawgadgets(0, 0);
		if(ref)
		{
			IDOpus->FixSliderPot(Window, &gadget_propgad, (a * bankstep) + (data_gadgetrow_offset / scr_gadget_rows), (bankcount * 6) / scr_gadget_rows, 1, 1);
		}
	}
}

void fixgadgetprop()
{
	struct dopusgadgetbanks *bank;
	int a, bankcount, num;

	if(scr_gadget_rows && ((status_iconified && status_flags & STATUS_ISINBUTTONS) || (scr_gadget_bank_count > 1 && config->generalscreenflags & SCR_GENERAL_GADSLIDERS)))
	{
		bank = dopus_firstgadbank;
		num = bankcount = 0;
		a = 1;
		while(bank)
		{
			if(bank == dopus_curgadbank)
				a = 0;
			else if(a)
				++num;
			++bankcount;
			bank = bank->next;
		}
		if(!a)
		{
			IDOpus->FixSliderPot(Window, (status_iconified && status_flags & STATUS_ISINBUTTONS) ? &iconbutpropgad : &gadget_propgad, (num * (6 / scr_gadget_rows)) + (data_gadgetrow_offset / scr_gadget_rows), (bankcount * 6) / scr_gadget_rows, 1, 1);
		}
	}
}

int makeusstring(char *from, char *to, int *uspos)
{
	int len, c;

	*uspos = -1;
	for(c = 0, len = 0;; c++)
	{
		if(!from[c])
			break;
		else if(from[c] == '_' && *uspos == -1)
			*uspos = len;
		else
			to[len++] = from[c];
	}
	to[len] = 0;
	return (len);
}

void drawgadgetcorner(struct RastPort *r, int x1, int y1)
{
	x1 += scrdata_gadget_width - 2;
	drawcornerimage(r, x1, y1, screen_pens[0].pen, screen_pens[config->gadgetbotcol].pen, screen_pens[config->gadgettopcol].pen);
}

void drawcornerimage(struct RastPort *r, int x1, int y1, int bg, int bc, int tc)
{
	IGraphics->SetAPen(r, bg);
	rectfill(r, x1 - 5, y1 - 1, 5, 4);
	IGraphics->SetDrMd(r, JAM1);
	IGraphics->SetAPen(r, bc);
	IGraphics->BltTemplate((char *)pageflip_data1, 0, 2, r, x1 - 6, y1 - 1, 6, 5);
	IGraphics->SetAPen(r, tc);
	IGraphics->BltTemplate((char *)pageflip_data2, 0, 2, r, x1 - 6, y1, 6, 3);
	IGraphics->SetDrMd(r, JAM2);
}

void get_bar_item(struct MenuItem *item, struct MenuItem *nextitem, struct Image *image)
{
	struct MenuItem *baritem;

	if(item->MutualExclude == 1 && (baritem = IDOpus->LAllocRemember(&menu_key, sizeof(struct MenuItem), MEMF_CLEAR)))
	{

		if(nextitem)
		{
			baritem->NextItem = nextitem;
			baritem->TopEdge = item->TopEdge + item->Height + 2;
			baritem->Width = item->Width;
			baritem->Height = 2;
			baritem->Flags = HIGHNONE;
			baritem->ItemFill = image;
		}
		else
		{
			baritem->TopEdge = item->TopEdge + item->Height;
			baritem->Width = item->Width + 1;
			baritem->Height = 0;
			baritem->Flags = HIGHNONE;
		}
		item->NextItem = baritem;
	}
	else
		item->NextItem = nextitem;
	item->MutualExclude = 0;
}

struct Image *get_bar_image(int fg, int bg, int width)
{
	struct Image *image;

	if(!(image = IDOpus->LAllocRemember(&menu_key, sizeof(struct Image), MEMF_CLEAR)))
		return (NULL);

	image->Width = width;
	image->Height = 2;
	image->PlaneOnOff = fg;
/*
    if (newlook) image->PlaneOnOff=fg;
    else {
        USHORT *imagedata;
        int words,a,b,depth,pos;

        words=(width+15)/16;
        for (depth=0;;depth++)
            if ((1<<depth)>fg && (1<<depth)>bg) break;

        if (!(imagedata=LAllocRemember(&menu_key,words*2*depth*sizeof(USHORT),MEMF_CLEAR|MEMF_CHIP)))
            return(NULL);

        for (a=0,pos=0;a<depth;a++) {
            for (b=0;b<words;b++,pos++) {
                if (fg&(1<<a)) imagedata[pos]=0xdddd;
                if (bg&(1<<a)) imagedata[pos]|=0x2222;
            }
            for (b=0;b<words;b++,pos++) {
                if (fg&(1<<a)) imagedata[pos]=0x7777;
                if (bg&(1<<a)) imagedata[pos]|=0x8888;
            }
        }

        image->ImageData=imagedata;
        image->Depth=depth;
        image->PlanePick=(1<<depth)-1;
    }
*/
	return (image);
}
