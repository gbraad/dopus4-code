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

#include "config.h"

char old_general_font[30];

int doscreenconfig()
{
	ULONG class;
	USHORT code, gadgetid = 0, qual;
	struct ConfigUndo *undo;
	struct Gadget *gad = NULL;
	int mode = -1, realpen = 1, x, y, cursel = -1, a, b, fontnum = 0, arrowact = 0, c;
	char buf[256], *ptr;
	UBYTE *fg = NULL, *bg = NULL;
	struct DOpusListView *view;
	struct Screen scrbuf;
	unsigned char k;

	showconfigscreen(CFG_SCREEN);
	undo = makeundo(UNDO_MAIN);
	curmode = NULL;
	fontsizelistview.h = 52;

	for(;;)
	{
		IExec->Wait(1 << Window->UserPort->mp_SigBit);
		while((IMsg = getintuimsg()))
		{
			if(!(mode == SCREEN_SCREENMODE && (view = IDOpus->ListViewIDCMP(&screenmodeview, IMsg)) != (struct DOpusListView *)-1) && !(mode == SCREEN_FONTS && (view = IDOpus->ListViewIDCMP(&fontplacelist, IMsg)) != (struct DOpusListView *)-1) && !(mode == SCREEN_PALETTE && (view = IDOpus->ListViewIDCMP(&palettelist, IMsg)) != (struct DOpusListView *)-1))
			{
				class = IMsg->Class;
				code = IMsg->Code;
				qual = IMsg->Qualifier;
				x = IMsg->MouseX;
				y = IMsg->MouseY;
				if(class == IDCMP_GADGETUP || class == IDCMP_GADGETDOWN)
				{
					gad = (struct Gadget *)IMsg->IAddress;
					gadgetid = gad->GadgetID;
				}
				IExec->ReplyMsg((struct Message *)IMsg);
				switch (class)
				{
				case IDCMP_MENUPICK:
					if(code == MENUNULL)
						break;
					if(mode > -1)
					{
						select_gadget(&maingad[mode], 0);
						removescreenedit(mode);
						mode = realpen = cursel = -1;
					}
					switch ((a = ITEMNUM(code)))
					{
					case 0:
					case 1:
						if(doload(CFG_SCREEN, a))
						{
							if(config->scrdepth != screen_depth)
								close_screen();
							open_screen();
							showconfigscreen(CFG_SCREEN);
						}
						break;
					}
					break;

				case IDCMP_MOUSEBUTTONS:
					if(code == SELECTDOWN)
					{
						if(mode == SCREEN_PALETTE)
						{
							if(x > x_off + 297 && y > y_off + 33 && x < x_off + 514 && y < y_off + 46)
							{
								for(a = 0; a < (1 << ((screen_depth > 4) ? 4 : screen_depth)); a++)
								{
									if(x >= palettegad_xy[a][0] && x <= palettegad_xy[a][2] && y >= palettegad_xy[a][1] && y <= palettegad_xy[a][3])
									{
										realpen = screen_pens[a].pen;
										showcolourvals(realpen, 1);
										showpalettegad(a);
										break;
									}
								}
							}
						}
						else if(mode == SCREEN_ARROWS)
						{
							for(a = 0; a < 3; a++)
							{
								if(x > x_off + 182 + (a * 129) && x < x_off + 307 + (a * 129) && y > y_off + 32 && y < y_off + 89)
								{
									if(arrowact != a)
									{
										showarrowborders(a, arrowact);
										arrowact = a;
										IDOpus->DoCycleGadget(&arrowgadgets[7], Window, arrowtypetxt, arrowact);
										fixarrowgads(arrowact);
										break;
									}
								}
							}
						}
						else if(mode == SCREEN_COLOURS && fg && bg)
						{
							a = *fg;
							b = *bg;
							if(docolourgad(rp, x_off + 208, y_off + 33, x, y, (cursel == COLBIT_3DBOXES) ? NULL : cfg_string[STR_EXAMPLE], &a, &b, CFG_GADGET))
							{
								*fg = a;
								*bg = b;
								if(cursel == COLBIT_3DBOXES)
								{
									showfuncob(rp, cfg_string[STR_EXAMPLE], 1, 0, CFG_GADGET, x_off + 302, y_off + 23);
									showscreencolours(COLOURS_ALL, NULL, NULL, 4);
								}
								else
									showscreencolours((1 << cursel), NULL, NULL, 0);
							}
							else
							{
								for(a = 0;; a++)
								{
									if(coloursel[a].item == -1)
										break;
									if(x >= x_off + coloursel[a].x1 && x <= x_off + coloursel[a].x2 && y >= y_off + coloursel[a].y1 && y <= y_off + coloursel[a].y2)
									{
										if((a != COLBIT_STRINGS) && (a != COLBIT_SELSTRINGS))
										{
											cursel = coloursel[a].item;
											showscreencolours((1 << cursel), &fg, &bg, 0);
											showcolourscycle(*fg, *bg, cursel);
											setupcolourbox(rp, x_off + 208, y_off + 33, *fg, *bg);
										}
										break;
									}
								}
							}
						}
					}
					break;

				case IDCMP_RAWKEY:
					if((code >= 0x60 && code <= 0x67) || code & 0x80)
						break;
					buf[0] = 0;
					IDOpus->RawkeyToStr(code, qual, NULL, buf, 10);
					k = IUtility->ToLower(buf[0]);
					if(mode == SCREEN_PALETTE)
					{
						if(k == getkeyshortcut(cfg_string[STR_PALETTE_RED]))
							IDOpus->ActivateStrGad(&palettegads[3], Window);
						else if(k == getkeyshortcut(cfg_string[STR_PALETTE_GREEN]))
							IDOpus->ActivateStrGad(&palettegads[4], Window);
						else if(k == getkeyshortcut(cfg_string[STR_PALETTE_BLUE]))
							IDOpus->ActivateStrGad(&palettegads[5], Window);
					}
					else if(mode == SCREEN_SCREENMODE)
					{
						if(k == getkeyshortcut(cfg_string[STR_SCREENMODE_WIDTH]))
							IDOpus->ActivateStrGad(&screenmodegads[0], Window);
						else if(k == getkeyshortcut(cfg_string[STR_SCREENMODE_HEIGHT]))
							IDOpus->ActivateStrGad(&screenmodegads[1], Window);
						else if(k == getkeyshortcut(cfg_string[STR_SCREENMODE_COLORS]))
							IDOpus->ActivateStrGad(&screenmodegads[2], Window);
					}
					break;

				case IDCMP_GADGETDOWN:
					if(gadgetid >= PALETTE_SLIDER && gadgetid < PALETTE_SLIDER + 3)
					{
						updatecolourvals(realpen, 0);
						for(;;)
						{
							IExec->Wait(1 << Window->UserPort->mp_SigBit);
							while((IMsg = getintuimsg()))
							{
								class = IMsg->Class;
								IExec->ReplyMsg((struct Message *)IMsg);
								if(class == IDCMP_GADGETUP)
									break;
								if(class == IDCMP_MOUSEMOVE)
									updatecolourvals(realpen, 0);
							}
							if(class == IDCMP_GADGETUP)
							{
								updatecolourvals(realpen, 1);
								break;
							}
						}
					}
					else if(gadgetid >= ARROWBASE + 3 && gadgetid < ARROWBASE + 7)
					{
						gadgetid -= ARROWBASE + 3;
						a = 0;
						for(;;)
						{
							if(((struct IntuitionBase *)(IIntuition->Data.LibBase))->ActiveWindow != Window || !(gad->Flags & GFLG_SELECTED))
								break;
							if((arrowact == 0 && gadgetid > 1) || (arrowact > 0 && gadgetid < 2))
							{
								b = config->arrowsize[arrowact];
								if((arrowact == 0 && gadgetid == 2) || (arrowact > 0 && gadgetid == 0))
									++config->arrowsize[arrowact];
								else
									--config->arrowsize[arrowact];
								switch (arrowact)
								{
								case 0:
									showupdownslider();
									break;
								case 1:
									showleftrightslider();
									break;
								case 2:
									showstringgadget();
									break;
								}
								if(b == config->arrowsize[arrowact])
									a = -1;
							}
							else
							{
								switch (arrowact)
								{
								case 0:
									b = config->sliderwidth;
									if(gadgetid == 1)
										config->sliderwidth -= 2;
									else
										config->sliderwidth += 2;
									showupdownslider();
									if(b == config->sliderwidth)
									{
										a = -1;
										break;
									}
									break;
								case 1:
									b = config->sliderheight;
									if(gadgetid == 3)
										config->sliderheight -= 2;
									else
										config->sliderheight += 2;
									showleftrightslider();
									if(b == config->sliderheight)
									{
										a = -1;
										break;
									}
									break;
								case 2:
									b = config->stringheight;
									if(gadgetid == 3)
										config->stringheight -= 2;
									else
										config->stringheight += 2;
									showstringgadget();
									if(b == config->stringheight)
									{
										a = -1;
										break;
									}
									break;
								}
							}
							if(a == -1)
								break;
							else if(a == 0)
							{
								IDOS->Delay(5);
								a = 1;
							}
							else
								IDOS->Delay(1);
							while((IMsg = getintuimsg()))
							{
								class = IMsg->Class;
								IExec->ReplyMsg((struct Message *)IMsg);
								if(class == IDCMP_GADGETUP)
									break;
							}
							if(class == IDCMP_GADGETUP)
								break;
						}
					}
					else if(gadgetid >= ARROWBASE && gadgetid < ARROWBASE + 3)
					{
						gadgetid -= ARROWBASE;
						if(config->arrowpos[arrowact] != gadgetid)
						{
							config->arrowpos[arrowact] = gadgetid;
							if(arrowact == 0)
								showupdownslider();
							else if(arrowact == 1)
								showleftrightslider();
							else
								showstringgadget();
						}
						fixarrowgads(arrowact);
					}
					break;

				case IDCMP_GADGETUP:
					if(mode == SCREEN_GENERAL && gadgetid > GAD_BASE)
					{
						config->generalscreenflags = processtickgad(scr_generalgadgets, config->generalscreenflags, gadgetid - GAD_BASE, -1);
						break;
					}
					if(gadgetid < SCREEN_OKAY)
					{
						if(mode == gadgetid)
						{
							removescreenedit(mode);
							mode = realpen = cursel = -1;
							makehelpname((char *)-1);
							doscreentitle(cfg_string[STR_SCREEN_SCREEN]);
							break;
						}
						if(mode > -1)
						{
							if(mode == SCREEN_SCREENMODE || mode == SCREEN_FONTS)
								select_gadget(&maingad[gadgetid], 0);
							select_gadget(&maingad[mode], 0);
							removescreenedit(mode);
							if(mode == SCREEN_SCREENMODE || mode == SCREEN_FONTS)
								select_gadget(&maingad[gadgetid], 1);
							mode = realpen = cursel = -1;
							makehelpname((char *)-1);
						}
						makehelpname(screengadgets[gadgetid]);
						doscreentitle(currenthelpname);
					}
					if(gadgetid >= PALETTE_STRING && gadgetid < PALETTE_STRING + 3)
					{
						updatecolourvals(realpen, -1);
						if(code != 0x9)
							getnextgadget(gad);
					}
					else
						switch (gadgetid)
						{
						case FONT_SIZE:
							IDOpus->RefreshStrGad(&fontsizegadget, Window);
							a = atoi(fontsize_buf);
							c = config->fontsizes[fontnum];
							config->fontsizes[fontnum] = a;
							for(b = 0; b < fontsizelistview.count; b++)
								if(atoi(fontsizelistview.items[b]) == a)
									break;
							if(b < fontsizelistview.count)
							{
								fontsizelistview.itemselected = b;
								if(b < fontsizelistview.topitem || b > fontsizelistview.topitem + fontsizelistview.lines - 1)
									fontsizelistview.topitem = b;
								IDOpus->RefreshListView(&fontsizelistview, 1);
								fontsize_buf[0] = 0;
								IDOpus->RefreshStrGad(&fontsizegadget, Window);
							}
							else
							{
								fontsizelistview.itemselected = -1;
								IDOpus->RefreshListView(&fontsizelistview, 1);
							}
							if(c != a)
								dofontdemo(config->fontbufs[fontnum], config->fontsizes[fontnum], sizeof(config->fontbufs[0]));
							break;

						case ARROWCYCLE:
							a = arrowact;
							if(qual & IEQUALIFIER_LSHIFT || qual & IEQUALIFIER_RSHIFT)
							{
								if((--arrowact) < 0)
									arrowact = 2;
							}
							else if((++arrowact) == 3)
								arrowact = 0;
							showarrowborders(arrowact, a);
							IDOpus->DoCycleGadget(&arrowgadgets[7], Window, arrowtypetxt, arrowact);
							fixarrowgads(arrowact);
							break;

						case SCREEN_SLIDER_LEFTCYCLE:
							config->slider_pos ^= 1;
							show_slider_setup(182, 32, 0);
							break;

						case SCREEN_SLIDER_RIGHTCYCLE:
							config->slider_pos ^= 2;
							show_slider_setup(373, 32, 1);
							break;

						case SCREEN_PALETTE:
							realpen = screen_pens[1].pen;
						case SCREEN_FONTS:
							fontnum = FONT_CLOCK;
						case SCREEN_ARROWS:
							arrowact = 0;
						case SCREEN_COLOURS:
						case SCREEN_GENERAL:
						case SCREEN_SCREENMODE:
						case SCREEN_SLIDERS:
							mode = gadgetid;
							makescreenedit(mode);
							if(gadgetid == SCREEN_COLOURS)
							{
								cursel = 0;
								fg = &config->statusfg;
								bg = &config->statusbg;
							}
							break;

						case COLOURS_SELECT:
							if(qual & IEQUALIFIER_LSHIFT || qual & IEQUALIFIER_RSHIFT)
							{
								if((--cursel) < 0)
									cursel = COLBIT_SELSTRINGS;
							}
							else
							{
								if(!coloursgadgets[++cursel])
									cursel = 0;
							}
							showscreencolours((1 << cursel), &fg, &bg, 0);
							showcolourscycle(*fg, *bg, cursel);
							setupcolourbox(rp, x_off + 208, y_off + 33, *fg, *bg);
							break;

						case SCREENMODE_WIDTH:
							a = atoi(screenwidth_buf);
							if(a % 2)
								snprintf(screenwidth_buf, sizeof(screenwidth_buf), "%d", a + 1);
							IDOpus->CheckNumGad(&screenmodegads[SCREENMODE_WIDTH - 300], Window, curmode->minw, curmode->maxw);
							config->scrw = atoi(screenwidth_buf);
							config->scr_winw = config->scrw;
							if(code != 0x9)
								getnextgadget(gad);
							break;

						case SCREENMODE_HEIGHT:
							IDOpus->CheckNumGad(&screenmodegads[SCREENMODE_HEIGHT - 300], Window, curmode->minh, curmode->maxh);
							config->scrh = atoi(screenheight_buf);
							config->scr_winh = config->scrh;
							if(code != 0x9)
								getnextgadget(gad);
							break;

						case SCREENMODE_DEPTH:
							a = atoi(screendepth_buf);
							for(b = curmode->maxdepth; b >= 2; b--)
							{
								if(a >= (1 << b))
								{
									a = b;
									break;
								}
							}
							if(b <= 2)
								a = 2;

						case SCREENMODE_SLIDER:
							if(gadgetid == SCREENMODE_SLIDER)
								a = IDOpus->GetSliderPos(&screenmodegads[SCREENMODE_SLIDER - 300], curmode->maxdepth - 1, 1) + 2;
							if(a > curmode->maxdepth)
								a = curmode->maxdepth;
							else if(a < 2)
								a = 2;
							snprintf(screendepth_buf, sizeof(screendepth_buf), "%d", (1 << a));
							IDOpus->RefreshStrGad(&screenmodegads[SCREENMODE_DEPTH - 300], Window);
							config->scrdepth = a;
							IDOpus->FixSliderPot(Window, &screenmodegads[SCREENMODE_SLIDER - 300], config->scrdepth - 2, curmode->maxdepth - 1, 1, 2);
							if(gadgetid == SCREENMODE_DEPTH && code != 0x9)
								getnextgadget(gad);
							break;

						case SCREENMODE_DEFWIDTH:
							if(screenmodegads[SCREENMODE_DEFWIDTH - 300].Flags & GFLG_SELECTED)
							{
								config->screenflags |= SCRFLAGS_DEFWIDTH;
								IDOpus->EnableGadget(&screenmodegads[SCREENMODE_WIDTH - 300], rp, 0, 0);
								config->scrw = curmode->defw;
								config->scr_winw = config->scrw;
								fixmodegads(curmode);
							}
							else
								config->screenflags &= ~SCRFLAGS_DEFWIDTH;
							fixdefaultgads(curmode);
							fixmodegads(curmode);
							break;

						case SCREENMODE_DEFHEIGHT:
							if(screenmodegads[SCREENMODE_DEFHEIGHT - 300].Flags & GFLG_SELECTED)
							{
								config->screenflags |= SCRFLAGS_DEFHEIGHT;
								IDOpus->EnableGadget(&screenmodegads[SCREENMODE_HEIGHT - 300], rp, 0, 0);
								config->scrh = curmode->defh;
								config->scr_winh = config->scrh;
								fixmodegads(curmode);
							}
							else
								config->screenflags &= ~SCRFLAGS_DEFHEIGHT;
							fixdefaultgads(curmode);
							fixmodegads(curmode);
							break;

						case SCREENMODE_HALFHEIGHT:
							if(screenmodegads[SCREENMODE_HALFHEIGHT - 300].Flags & GFLG_SELECTED)
								config->screenflags |= SCRFLAGS_HALFHEIGHT;
							else
								config->screenflags &= ~SCRFLAGS_HALFHEIGHT;
							break;

						case SCREEN_CANCEL:
							doundo(undo, UNDO_MAIN);
							if(config->scrdepth != screen_depth)
							{
								if(mode > -1)
								{
									removescreenedit(mode);
									mode = -1;
								}
								close_screen();
							}
							open_screen();
						case SCREEN_OKAY:
							if(mode > -1)
								removescreenedit(mode);
							doundo(undo, 0);
							return ((gadgetid == SCREEN_OKAY));
						}
					break;
				}
			}
			else if(view)
			{
				if(mode == SCREEN_FONTS)
				{
					if(view->listid == 1)
					{
						fontsize_buf[0] = 0;
						IDOpus->RefreshStrGad(&fontsizegadget, Window);

						fontnum = fontplacevals[view->itemselected];
						initfontlist(fontnum, 1, fontplaceflags[fontnum]);
					}
					else if(fontnum >= 0 && fontnum < NUMFONTS)
					{
						if(view->listid == 2)
						{
							strlcpy(buf, config->fontbufs[fontnum], sizeof(buf));
							if((ptr = strstri(buf, ".font")))
								*ptr = 0;
							if(strcmp(buf, view->items[view->itemselected]))
							{
								fontsize_buf[0] = 0;
								IDOpus->RefreshStrGad(&fontsizegadget, Window);

								strlcpy(config->fontbufs[fontnum], view->items[view->itemselected], sizeof(config->fontbufs[0]));
								config->fontsizes[fontnum] = initfontlist(fontnum, 0, fontplaceflags[fontnum]);
								strlcat(config->fontbufs[fontnum], ".font", sizeof(config->fontbufs[0]));
							}
						}
						else if(view->listid == 3)
						{
							fontsize_buf[0] = 0;
							IDOpus->RefreshStrGad(&fontsizegadget, Window);

							a = config->fontsizes[fontnum];
							config->fontsizes[fontnum] = atoi(view->items[view->itemselected]);
							if(a != config->fontsizes[fontnum])
								dofontdemo(config->fontbufs[fontnum], config->fontsizes[fontnum], sizeof(config->fontbufs[fontnum]));
						}
					}
				}
				else if(view->listid == 1)
				{
					curmode = showdisplaydesc();
					fixdefaultgads(curmode);
					if(screenmodegads[SCREENMODE_DEFWIDTH - 300].Flags & GFLG_SELECTED)
					{
						config->scrw = curmode->defw;
						config->scr_winw = config->scrw;
						IDOpus->EnableGadget(&screenmodegads[SCREENMODE_WIDTH - 300], rp, 0, 0);
					}
					if(screenmodegads[SCREENMODE_DEFHEIGHT - 300].Flags & GFLG_SELECTED)
					{
						config->scrh = curmode->defh;
						config->scr_winh = config->scrh;
						IDOpus->EnableGadget(&screenmodegads[SCREENMODE_HEIGHT - 300], rp, 0, 0);
					}
					fixmodegads(curmode);
					if(config->screenflags & SCRFLAGS_DEFWIDTH)
						IDOpus->DisableGadget(&screenmodegads[SCREENMODE_WIDTH - 300], rp, 0, 0);
					if(config->screenflags & SCRFLAGS_DEFHEIGHT)
						IDOpus->DisableGadget(&screenmodegads[SCREENMODE_HEIGHT - 300], rp, 0, 0);
					config->screenmode = curmode->mode;

					if(curmode->mode == MODE_PUBLICSCREENUSE)
					{
						for(a = 0; a < 80; a++)
						{
							if(!curmode->name[a])
								break;
							if(a > 0 && curmode->name[a - 1] == ':')
							{
								if((strcmp(&curmode->name[a], cfg_string[STR_SCREEN_MODE_USE])) == 0)
									break;
							}
						}
						if(a > 0 && curmode->name[a])
						{
							IUtility->Strlcpy(config->pubscreen_name, curmode->name, a);
							config->pubscreen_name[a - 1] = 0;
						}
					}
					else
						config->pubscreen_name[0] = 0;
				}
				else if(view->listid == 2)
				{
					if(view->itemselected >= 0 && view->itemselected < 13)
					{
						if(view->itemselected == 0)
						{
							load_palette(Screen, palette_backup, 1 << ((screen_depth > 4) ? 4 : screen_depth));
						}
						else if(view->itemselected == 1)
						{
							loadrgb4(Screen, def_dopus_palette, 1 << ((screen_depth > 4) ? 4 : screen_depth));
						}
						else if(view->itemselected == 3)
						{
							ULONG *palbuf;
							int wbdepth, size;

							IDOpus->GetWBScreen(&scrbuf);
							wbdepth = 1 << scrbuf.RastPort.BitMap->Depth;
							size = (wbdepth * 3) * sizeof(ULONG);
							if((palbuf = doAllocVec(size, MEMF_CLEAR)))
							{
								ULONG palette[48];

								get_palette(&scrbuf, palbuf, wbdepth);
								for(a = 0; a < 4; a++)
								{
									palette[(a * 3)] = palbuf[(a * 3)];
									palette[(a * 3) + 1] = palbuf[(a * 3) + 1];
									palette[(a * 3) + 2] = palbuf[(a * 3) + 2];
								}
								for(a = 4; a < 16; a++)
								{
									palette[(a * 3)] = config->new_palette[(a * 3)];
									palette[(a * 3) + 1] = config->new_palette[(a * 3) + 1];
									palette[(a * 3) + 2] = config->new_palette[(a * 3) + 2];
								}
								if(wbdepth > 4 && screen_depth > 2)
								{
									int a, endwb, endme;

									endwb = wbdepth - 4;
									endme = (1 << screen_depth) - 4;

									for(a = 0; a < 4; a++)
									{
										palette[((a + endme) * 3)] = palbuf[((a + endwb) * 3)];
										palette[((a + endme) * 3) + 1] = palbuf[((a + endwb) * 3) + 1];
										palette[((a + endme) * 3) + 2] = palbuf[((a + endwb) * 3) + 2];
									}
								}
								load_palette(Screen, palette, 1 << ((screen_depth > 4) ? 4 : screen_depth));
								IExec->FreeVec(palbuf);
							}
						}
						else
							loadrgb4(Screen, defpalettes[view->itemselected - 2], 4);
						get_palette(Screen, config->new_palette, 1 << ((screen_depth > 4) ? 4 : screen_depth));
						showcolourvals(realpen, 1);
					}
				}
				break;
			}
		}
	}
}

void makescreenedit(int mode)
{
	int a, b, wclone, shortage, size, fnum, fontnum, nopropnum, only8num, num;
	ULONG modeid;
	char *ptr, buf[256];
	DisplayInfoHandle *handle;
	struct DisplayInfo *dispinfo;
	struct AvailFonts *avail;

	borderkey = NULL;

	IGraphics->SetAPen(rp, screen_pens[1].pen);
	switch (mode)
	{
	case SCREEN_PALETTE:
		bpg = 4;
		modeid = IGraphics->GetVPModeID(vp);
		if((handle = IGraphics->FindDisplayInfo(config->screenmode)) || (handle = IGraphics->FindDisplayInfo(modeid)))
		{
			IGraphics->GetDisplayInfoData(handle, buf, 256, DTAG_DISP, 0);
			dispinfo = (struct DisplayInfo *)buf;
			bpg = (dispinfo->RedBits + dispinfo->GreenBits + dispinfo->BlueBits) / 3;
		}
		for(a = 0; a < 3; a++)
		{
			fix_slider(&palettegads[a]);
			IDOpus->FixSliderBody(Window, &palettegads[a], 1 << bpg, 1, 2);
		}
		IDOpus->AddGadgets(Window, palettegads, palettegadgets, 6, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen, 1);
		do3dbox(x_off + 296, y_off + 33, 220, 14);
		do3dbox(x_off + 250, y_off + 33, 40, 14);
		setuplist(&palettelist, 250, 93);
		IDOpus->AddListView(&palettelist, 1);
		showpalettegad(1);
		showcolourvals(screen_pens[1].pen, 1);
		get_palette(Screen, palette_backup, 1 << ((screen_depth > 4) ? 4 : screen_depth));
		break;

	case SCREEN_COLOURS:
		if(config->stringfgcol == config->stringbgcol)
			++config->stringfgcol;
		do3dbox(x_off + 142, y_off + 61, 464, 122);
		IDOpus->AddGadgetBorders(&borderkey, &coloursgad, 1, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);
		IDOpus->AddGadgets(Window, &coloursgad, NULL, 1, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen, 1);
		setupcolourbox(rp, x_off + 208, y_off + 33, config->statusfg, config->statusbg);
		showscreencolours(COLOURS_ALL, NULL, NULL, 0);
		showcolourscycle(config->statusfg, config->statusbg, 0);
		break;

	case SCREEN_SCREENMODE:
		screenkey = NULL;
		firstmode = NULL;
		screenmodeview.itemselected = -1;
		wclone = 0;
		busy();
		if((a = initscreenmodes()) && (screenmodelist = IDOpus->LAllocRemember(&screenkey, (a + 1) * 4, MEMF_CLEAR)))
		{
			screenmode = firstmode;
			for(b = 0; b < a; b++)
			{
				if((screenmodelist[b] = IDOpus->LAllocRemember(&screenkey, DISPLAYNAMELEN, MEMF_CLEAR)))
					strlcpy(screenmodelist[b], screenmode->name, DISPLAYNAMELEN);
				if(screenmode->mode == MODE_WORKBENCHCLONE)
					wclone = b;
				if(screenmode->mode == config->screenmode)
				{
					if(screenmode->mode == MODE_PUBLICSCREENUSE)
					{
						snprintf(buf, sizeof(buf), "%s:%s", config->pubscreen_name, cfg_string[STR_SCREEN_MODE_USE]);
						if(strcasecmp(buf, screenmode->name) == 0)
							screenmodeview.itemselected = b;
					}
					else
					{
						screenmodeview.itemselected = b;
					}
				}
				if((screenmode = screenmode->next) == NULL)
					break;
			}
		}
		unbusy();
		if(screenmodeview.itemselected == -1)
			screenmodeview.itemselected = wclone;
		setuplist(&screenmodeview, 238, 17);
		screenmodeview.items = screenmodelist;
		screenmodeview.topitem = screenmodeview.itemselected;
		IDOpus->AddListView(&screenmodeview, 1);
		screenmodegads[0].Flags &= ~GFLG_DISABLED;
		screenmodegads[1].Flags &= ~GFLG_DISABLED;
		for(a = SCREENMODE_DEFWIDTH - 300; a < SCREENMODE_SLIDER - 300; a++)
			screenmodegads[a].Flags &= ~(GFLG_SELECTED | GFLG_DISABLED);
		if(config->screenflags & SCRFLAGS_HALFHEIGHT)
			screenmodegads[SCREENMODE_HALFHEIGHT - 300].Flags |= GFLG_SELECTED;
		else
			screenmodegads[SCREENMODE_HALFHEIGHT - 300].Flags &= ~GFLG_SELECTED;
		fix_slider(&screenmodegads[SCREENMODE_SLIDER - 300]);
		IDOpus->AddGadgets(Window, screenmodegads, screenmodegadgets, 7, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen, 1);
		IDOpus->Do3DBox(rp, x_off + 238, y_off + 140, 272, 40, screen_pens[config->gadgetbotcol].pen, screen_pens[config->gadgettopcol].pen);
		IGraphics->SetAPen(rp, screen_pens[1].pen);
		IDOpus->UScoreText(rp, cfg_string[STR_DISPLAY_MODE_DESCRIPTION], x_off + 238, y_off + 136, -1);
		fixmodegads((curmode = showdisplaydesc()));
		fixdefaultgads(curmode);
		break;

	case SCREEN_FONTS:
		if(!fontdatabuf)
		{
			busy();
			size = (sizeof(struct AvailFonts) * 512) + sizeof(struct AvailFontsHeader);
			do
			{
				if(!(fontdatabuf = IDOpus->LAllocRemember(&fontkey, size, MEMF_CLEAR)))
					break;
				if((shortage = IDiskfont->AvailFonts(fontdatabuf, size, AFF_MEMORY | AFF_DISK)))
				{
					IDOpus->LFreeRemember(&fontkey);
					size += shortage;
				}
				else
					break;
			}
			while(shortage);
			unbusy();
			if(!fontdatabuf)
				break;
			num = ((struct AvailFontsHeader *)fontdatabuf)->afh_NumEntries;
			if(!num || !(fontlist = IDOpus->LAllocRemember(&fontkey, (num + 1) * 4, MEMF_CLEAR)) || !(fontsizelist = IDOpus->LAllocRemember(&fontkey, (num + 1) * 4, MEMF_CLEAR)))
				break;
			if(!(noproplist = IDOpus->LAllocRemember(&fontkey, (num + 1) * 4, MEMF_CLEAR)) || !(nopropsizelist = IDOpus->LAllocRemember(&fontkey, (num + 1) * 4, MEMF_CLEAR)))
			{
				noproplist = fontlist;
				nopropsizelist = fontsizelist;
			}
			if(!(only8list = IDOpus->LAllocRemember(&fontkey, (num + 1) * 4, MEMF_CLEAR)))
				only8list = fontlist;
			avail = (struct AvailFonts *)&fontdatabuf[sizeof(UWORD)];
			sortfontlist(avail, num, 0);
			sortfontlist(avail, num, 1);
			for(fnum = 0; fnum < num; fnum++)
			{
				if((ptr = strstri((STRPTR)avail[fnum].af_Attr.ta_Name, ".font")))
					*ptr = 0;
				avail[fnum].af_Attr.ta_Style = 0;
			}
			for(fnum = 0; fnum < num; fnum++)
			{
				for(a = 0; a < num; a++)
				{
					if(!strcasecmp((char *)avail[a].af_Attr.ta_Name, (char *)avail[fnum].af_Attr.ta_Name))
						++avail[fnum].af_Attr.ta_Style;
				}
			}

			for(fnum = 0, fontnum = 0, nopropnum = 0, only8num = 0; fnum < num; fnum++)
			{
				for(a = 0; a < fontnum; a++)
				{
					if(strcmp(fontlist[a], (char *)avail[fnum].af_Attr.ta_Name) == 0)
						break;
				}
				if(a == fontnum)
				{
					fontlist[fontnum] = (char *)avail[fnum].af_Attr.ta_Name;
					fontsizelist[fontnum] = IDOpus->LAllocRemember(&fontkey, (avail[fnum].af_Attr.ta_Style + 1) * 4, MEMF_CLEAR);
					if(!(avail[fnum].af_Attr.ta_Flags & FPF_PROPORTIONAL))
					{
						noproplist[nopropnum] = fontlist[fontnum];
						nopropsizelist[nopropnum] = fontsizelist[fontnum];
						++nopropnum;
						if(avail[fnum].af_Attr.ta_YSize == 8)
						{
							only8list[only8num] = fontlist[fontnum];
							++only8num;
						}
					}
					++fontnum;
				}
				if(fontsizelist[a])
				{
					for(b = 0;; b++)
					{
						if(!fontsizelist[a][b])
						{
							if((fontsizelist[a][b] = IDOpus->LAllocRemember(&fontkey, 8, MEMF_CLEAR)))
								snprintf(fontsizelist[a][b], 8, "%4d", avail[fnum].af_Attr.ta_YSize);
							break;
						}

						if(atoi(fontsizelist[a][b]) == avail[fnum].af_Attr.ta_YSize)
						{
							break;
						}
					}
				}
			}
		}
		setuplist(&fontplacelist, 146, 18);
		setuplist(&fontlistview, 360, 18);
		setuplist(&fontsizelistview, 566, 18);
		fontplacelist.itemselected = fontplacelist.topitem = 0;
		fontsizelistview.items = fontlistview.items = NULL;
		IDOpus->AddListView(&fontplacelist, 3);
		IGraphics->SetAPen(rp, screen_pens[1].pen);
		IDOpus->UScoreText(rp, cfg_string[STR_DISPLAY_ITEM], x_off + 146, y_off + 14, -1);
		IDOpus->UScoreText(rp, cfg_string[STR_FONT], x_off + 360, y_off + 14, -1);
		IDOpus->UScoreText(rp, cfg_string[STR_FONT_SIZE], x_off + 566, y_off + 14, -1);
		fontsize_buf[0] = 0;
		IDOpus->AddGadgets(Window, &fontsizegadget, NULL, 1, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen, 1);
		initfontlist(FONT_CLOCK, 1, 0);
		strlcpy(old_general_font, config->fontbufs[0], sizeof(old_general_font));
		break;

	case SCREEN_GENERAL:
		inittickgads(scr_generalgadgets, config->generalscreenflags, 0);
		break;

	case SCREEN_ARROWS:
		for(a = 0; a < 3; a++)
		{
			arrowgadgets[a].GadgetRender = (APTR) buttonoffimage;
			arrowgadgets[a].SelectRender = (APTR) buttononimage;
			do3dbox(x_off + 184 + (a * 129), y_off + 33, 121, 55);
		}

		IDOpus->AddGadgetBorders(&borderkey, &arrowgadgets[3], 2, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);
		IDOpus->AddGadgetBorders(&borderkey, &arrowgadgets[5], 2, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);
		IDOpus->AddGadgetBorders(&borderkey, &arrowgadgets[7], 1, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);

		IDOpus->AddGadgets(Window, arrowgadgets, arrowgadtxt, 8, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen, 1);
		for(a = 0; a < 2; a++)
		{
			IDOpus->DoArrow(rp, x_off + 420 + (a * 34), y_off + 144, 10, 9, screen_pens[1].pen, screen_pens[0].pen, 3 - a);
			IDOpus->DoArrow(rp, x_off + 436, y_off + 138 + (a * 12), 12, 9, screen_pens[1].pen, screen_pens[0].pen, a);
		}
		showupdownslider();
		showleftrightslider();
		showstringgadget();
		showarrowborders(0, -1);
		fixarrowgads(0);
		IGraphics->SetAPen(rp, screen_pens[1].pen);
		IDOpus->UScoreText(rp, cfg_string[STR_MODIFY_SIZE], x_off + 294, y_off + 151, -1);
		IDOpus->DoCycleGadget(&arrowgadgets[7], Window, arrowtypetxt, 0);
		break;

	case SCREEN_SLIDERS:
		for(a = 0; a < 2; a++)
		{
			IDOpus->AddGadgetBorders(&borderkey, &screen_sliders_gadgets[a], 2, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);
		}

		IDOpus->AddGadgets(Window, screen_sliders_gadgets, NULL, 2, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen, 1);

		for(a = 0; a < 2; a++)
			show_slider_setup(182 + (a * 191), 32, a);
		break;
	}
}

void removescreenedit(int mode)
{
	IGraphics->SetAPen(rp, screen_pens[0].pen);
	IGraphics->RectFill(rp, x_off + 120, y_off + 4, x_bot, y_bot);
	IGraphics->SetAPen(rp, screen_pens[1].pen);
	switch (mode)
	{
	case SCREEN_PALETTE:
		IIntuition->RemoveGList(Window, palettegads, 6);
		IDOpus->RemoveListView(&palettelist, 1);
		break;

	case SCREEN_COLOURS:
		IIntuition->RemoveGList(Window, &coloursgad, 1);
		doradiobuttons();
		break;

	case SCREEN_SCREENMODE:
		IDOpus->RemoveListView(&screenmodeview, 1);
		IIntuition->RemoveGList(Window, screenmodegads, 7);
		IDOpus->LFreeRemember(&screenkey);
		if(config->scrdepth != screen_depth)
		{
			close_screen();
			open_screen();
			showconfigscreen(CFG_SCREEN);
		}
		break;

	case SCREEN_FONTS:
		IDOpus->RemoveListView(&fontplacelist, 3);
		IIntuition->RemoveGList(Window, &fontsizegadget, 1);
		if(strncasecmp(config->fontbufs[0], old_general_font, sizeof(config->fontbufs[0])) != 0)
		{
			close_screen();
			open_screen();
			showconfigscreen(CFG_SCREEN);
		}
		break;

	case SCREEN_ARROWS:
		IIntuition->RemoveGList(Window, arrowgadgets, 8);
		break;

	case SCREEN_GENERAL:
		removetickgads();
		break;

	case SCREEN_SLIDERS:
		IIntuition->RemoveGList(Window, screen_sliders_gadgets, 2);
		break;
	}
	IDOpus->LFreeRemember(&borderkey);
}
