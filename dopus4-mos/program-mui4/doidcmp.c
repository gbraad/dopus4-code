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

#include <devices/rawkeycodes.h>
#include <libraries/mui.h>
#include <proto/alib.h>

#include "dopus.h"
#include "mui.h"

void doidcmp()
{
	int x, y, a, function, wmes, stringgd = 0, waitbits, x1, y1, c, win, num, class;
	USHORT code, gadgetid = 0, menunum, itemnum, qual;
	UBYTE buf[80], ch;
	struct dopusgadgetbanks *bank, *bank1;
	struct dopushotkey *hotkey;
	struct dopusfuncpar par;
	struct Gadget *gad = NULL;
	struct MenuItem *item;

	wmes = 0;

	for(;;)
	{
		for (;;)
		{
			ULONG ret = DoMethod(dopusapp, MUIM_Application_NewInput, &wmes);

			if (ret == MUIV_Application_ReturnID_Quit)
			{
				ULONG a = 1;

				if (!getv(dopusapp, MUIA_Application_ForceQuit))
				{
					set(dopusapp, MUIA_Application_Sleep, TRUE);

					if(!(config->generalflags & GENERAL_FORCEQUIT))
						a = simplerequest(globstring[STR_REALLY_QUIT], globstring[STR_QUIT], str_cancelstring, globstring[STR_ICONIFY], NULL);

					set(dopusapp, MUIA_Application_Sleep, FALSE);
				}

				if (a == 2)
				{
					set(dopusapp, MUIA_Application_Iconified, TRUE);
				}
				else if (a == 1)
				{
					rexx_argcount = 1;
					strcpy(rexx_args[0], "force");
					internal_function(FUNC_QUIT, 0, NULL, NULL);
				}
			}

			if (ret >= MAIN_GAD_BASE && ret < (MAIN_GAD_BASE + GADCOUNT))
			{
				ULONG a = (ret - MAIN_GAD_BASE) + (data_gadgetrow_offset * 7);

				if(dopus_curgadbank && isvalidgad(&dopus_curgadbank->gadgets[a]))
				{
					dofunctionstring(dopus_curgadbank->gadgets[a].function, dopus_curgadbank->gadgets[a].name, NULL, (struct dopusfuncpar *)&dopus_curgadbank->gadgets[a].which);
				}
			}
			else if (ret >= 200 && ret < 250)
			{
				internal_function(ret - 200 + DRIVE1, 0, NULL, NULL);
			}

			if (wmes)
				break;
		}

		waitbits = 1 << Window->UserPort->mp_SigBit | 1 << count_port->mp_SigBit | rexx_signalbit | INPUTSIG_HOTKEY;

		if((wmes = Wait(waitbits | wmes)) & INPUTSIG_HOTKEY)
		{
			if(!dopus_globalhotkey)
				continue;
			hotkey = dopus_globalhotkey;
			dopus_globalhotkey = NULL;
			CopyMem((STRPTR) & hotkey->func.which, (STRPTR) & par, sizeof(struct dopusfuncpar));
			par.which &= FLAG_OUTWIND | FLAG_NOQUOTE | FLAG_SHELLUP | FLAG_DOPUSUP;
			par.which |= FLAG_ASYNC;
			if(par.which & FLAG_OUTWIND)
				par.which |= FLAG_WB2F;
			status_flags |= STATUS_FROMHOTKEY;
			dofunctionstring(hotkey->func.function, hotkey->name, NULL, &par);
			status_flags &= ~STATUS_FROMHOTKEY;
			continue;
		}
		if(wmes & rexx_signalbit)
		{
			rexx_dispatch(1);
			if(func_global_function)
				internal_function(func_global_function, rexx_global_flag, NULL, NULL);
			status_haveaborted = 0;
			continue;
		}

/// "DOS notify"
		if(wmes & (1 << count_port->mp_SigBit))
		{
			struct NotifyMessage *note;
			int got = 0;

			while((note = (struct NotifyMessage *)GetMsg(count_port)))
			{
				if(config->dynamicflags & UPDATE_NOTIFY)
				{
					a = note->nm_NReq->nr_UserData;
					ReplyMsg((struct Message *)note);
					if(!(got & (1 << a)))
					{
						got |= 1 << a;
						if(a == 0 || a == 1)
						{
							if((config->dirflags & DIRFLAGS_REREADOLD) || IntuitionBase->ActiveWindow == Window)
							{
								struct DirWindowPars notifypars;

								notifypars.reselection_list = NULL;
								makereselect(&notifypars, a);
								startgetdir(a, SGDFLAGS_CANMOVEEMPTY);
								doreselect(&notifypars, 0);
								makereselect(&notifypars, -1);
							}
						}
					}
				}
				else
				{
					ReplyMsg((struct Message *)note);
				}
			}
		}
		while(getintuimsg())
		{
			x = IMsg->MouseX;
			y = IMsg->MouseY;
			time_current_sec = IMsg->Seconds;
			time_current_micro = IMsg->Micros;
			class = IMsg->Class;
			code = IMsg->Code;
			qual = IMsg->Qualifier;
			if(class == IDCMP_GADGETUP || class == IDCMP_GADGETDOWN)
			{
				gad = (struct Gadget *)IMsg->IAddress;
				gadgetid = gad->GadgetID;
			}
			ReplyMsg((struct Message *)IMsg);
			if(Window->FirstRequest)
				continue;

			function = status_haveaborted = 0;
			switch (class)
			{
/// "IDCMP_ACTIVEWINDOW"
			case IDCMP_ACTIVEWINDOW:
				for(a = 0; a < 2; a++)
					check_old_buffer(a);
				break;

/// "IDCMP_MENUPICK"
			case IDCMP_MENUPICK:
				Window->Flags |= WFLG_RMBTRAP;
				if(!Window->MenuStrip)
					break;
				if(stringgd)
				{
					checkstringgads(stringgd);
					stringgd = 0;
				}
				if(code == MENUNULL)
					break;
				menunum = menu_real_number[MENUNUM(code)];
				num = ITEMNUM(code);
				item = menu_menus[menunum].FirstItem;
				for(a = 0, itemnum = 0; a < num && item; a++)
				{
					if(item->Flags & ITEMTEXT)
						++itemnum;
					item = item->NextItem;
				}
				if(item)
				{
					a = (menunum * 20) + itemnum;
					if(!isvalidgad(&config->menu[a]))
						break;
					dofunctionstring(config->menu[a].function, config->menu[a].name, NULL, (struct dopusfuncpar *)&config->menu[a].which);
				}
				break;
/// "IDCMP_GADGETDOWN"
			case IDCMP_GADGETDOWN:
				if(stringgd)
				{
					checkstringgads(stringgd);
					stringgd = 0;
				}
				break;

/// "IDCMP_GADGETUP"
			case IDCMP_GADGETUP:
				if(stringgd)
				{
					if(gadgetid != SCRGAD_LEFTSTRING && gadgetid != SCRGAD_RIGHTSTRING)
						checkstringgads(stringgd);
					stringgd = 0;
				}
				if(gadgetid >= MAIN_GAD_BASE && gadgetid < MAIN_GAD_BASE + GADCOUNT)
				{
					a = (gadgetid - MAIN_GAD_BASE) + (data_gadgetrow_offset * 7);
					if(!dopus_curgadbank || !(isvalidgad(&dopus_curgadbank->gadgets[a])))
						break;
					dofunctionstring(dopus_curgadbank->gadgets[a].function, dopus_curgadbank->gadgets[a].name, NULL, (struct dopusfuncpar *)&dopus_curgadbank->gadgets[a].which);
					break;
				}
				switch (gadgetid)
				{
				case SCRGAD_LEFTSTRING:
				case SCRGAD_RIGHTSTRING:
					if(code == 0x5f)
						do_path_completion(gadgetid, qual);
					else if(code != 0x9)
					{
						makeactive(gadgetid, 0);
						startgetdir(gadgetid, SGDFLAGS_CANMOVEEMPTY);
					}
					break;

				case SCRGAD_BUFFERLEFT1:
					incrementbuf(0, -1, 1);
					break;

				case SCRGAD_BUFFERRIGHT1:
					incrementbuf(0, 1, 1);
					break;

				case SCRGAD_BUFFERLEFT2:
					incrementbuf(1, -1, 1);
					break;

				case SCRGAD_BUFFERRIGHT2:
					incrementbuf(1, 1, 1);
					break;

				case FUNC_PARENT1:
					makeactive(0, 0);
					function = FUNC_PARENT;
					break;

				case FUNC_PARENT2:
					makeactive(1, 0);
					function = FUNC_PARENT;
					break;

				case SCRGAD_TINYFILTER:
					if(status_flags & STATUS_HELP)
					{
						dohelp(NULL, "*Filter", 0, 0, NULL);
						screen_gadgets[SCRGAD_TINYFILTER].Flags ^= GFLG_SELECTED;
						dolittlegads(&screen_gadgets[SCRGAD_TINYFILTER], &globstring[STR_TINY_BUTTONS][6], 1);
					}
					else if(screen_gadgets[SCRGAD_TINYFILTER].Flags & GFLG_SELECTED)
					{
						busy();
						if(!(whatsit(globstring[STR_ENTER_FILTER], 40, str_filter, NULL)) || !str_filter[0])
						{
							screen_gadgets[SCRGAD_TINYFILTER].Flags &= ~GFLG_SELECTED;
							dolittlegads(&screen_gadgets[SCRGAD_TINYFILTER], &globstring[STR_TINY_BUTTONS][6], 1);
							str_filter_parsed[0] = 0;
						}
						else
						{
							ParsePatternNoCase(str_filter, str_filter_parsed, 84);
						}
						unbusy();
					}
					else
						str_filter_parsed[0] = 0;
					break;

				default:
					function = gadgetid;
					break;
				}
				break;

/// "IDCMP_RAWKEY"
			case IDCMP_RAWKEY:
				if(code & IECODE_UP_PREFIX)
				{
					break;
				}
				qual &= VALID_QUALIFIERS;
				for(a = 0; a < MENUCOUNT; a++)
				{
					if(check_key_press((struct dopusfunction *)&config->menu[a], code, qual))
					{
						dofunctionstring(config->menu[a].function, config->menu[a].name, NULL, (struct dopusfuncpar *)&config->menu[a].which);
						goto foobarbaz;
					}
				}
				for(a = 0; a < USEDRIVECOUNT; a++)
				{
					if(check_key_press(&config->drive[a], code, qual))
					{
						strcpy(str_pathbuffer[data_active_window], config->drive[a].function);
						startgetdir(data_active_window, SGDFLAGS_CANMOVEEMPTY);
						goto foobarbaz;
					}
				}
				if(qual & (IEQUALIFIER_RCOMMAND | IEQUALIFIER_LCOMMAND))
				{
					RawkeyToStr(code, qual, NULL, (char *)buf, 0);
					switch (toupper(buf[0]))
					{
					case 'R':
						function = FUNC_RESELECT;
						break;
					case 'A':
						function = FUNC_AREXX;
						break;
					case 'S':
						function = FUNC_SELECT;
						break;
					case 'B':
						function = FUNC_BUFFERLIST;
						break;
					case ' ':
						if(qual & IEQUALIFIER_LCOMMAND)
							findfirstsel(data_active_window, ENTRY_FILE);
						else
							findfirstsel(data_active_window, ENTRY_DIRECTORY);
						break;
					}
				}
				else
				{
					win = -1;
					switch (code)
					{
					case 0x5f:	// HELP
						function = FUNC_HELP;
						break;
					case 0x40:	// SPACE
					case 0x42:	// TAB
						makeactive(1 - data_active_window, 1);
						break;
					case 0x44:	// RETURN
						if(qual & IEQUALIFIER_ANYSHIFT)
							function = FUNC_BUFFERLIST;
						else if(qual & (IEQUALIFIER_LALT | IEQUALIFIER_RALT))
							function = FUNC_DEVICELIST;
						else
							ActivateStrGad(&path_strgadget[data_active_window], Window);
						break;
					case 0x0b:	// - _
						dosizedirwindows(-60000);
						break;
					case 0x0c:	// = +
						dosizedirwindows(0);
						break;
					case 0x0d:	// \ |
						dosizedirwindows(60000);
						break;
					case 0x1a:	// [ {
						goto prevgadgetbank;
						break;
					case 0x1b:	// ] }
						goto nextgadgetbank;
						break;
					default:
						if(code < 0x40 && !(qual & IEQUALIFIER_CONTROL))
						{	// a digit or char
							RawkeyToStr(code, qual, NULL, (char *)buf, 0);
							ch = buf[0];
							if(_isprint(ch))
								findfirstchar(data_active_window, ch);
						}
						break;
					}
				}
				unbusy();
				break;

/// "IDCMP_MOUSEBUTTONS"
			case IDCMP_MOUSEBUTTONS:
				if(stringgd)
				{
					checkstringgads(stringgd);
					stringgd = 0;
				}
				if(code == SELECTDOWN)
				{
					if(y >= scrdata_diskname_ypos && y < scrdata_diskname_ypos + scrdata_diskname_height)	// disknamebar
					{
						a = (x < scrdata_dispwin_center) ? 0 : 1;
						if(data_active_window == 1 - a)
						{
							if(DoubleClick(time_previous_sec, time_previous_micro, time_current_sec, time_current_micro))
							{
								copydirwin(dopus_curwin[1 - a], dopus_curwin[a], a);
							}
							makeactive(a, 1);
						}
						else
						{
							if(DoubleClick(time_previous_sec, time_previous_micro, time_current_sec, time_current_micro))
							{
								busy();
								if(str_pathbuffer[data_active_window][0] && do_parent_multi(str_pathbuffer[data_active_window]))
									startgetdir(data_active_window, SGDFLAGS_CANMOVEEMPTY | SGDFLAGS_CANCHECKBUFS);
								unbusy();
							}
							time_previous_sec = time_current_sec;
							time_previous_micro = time_current_micro;
						}
						checksize(a);
						break;
					}
					else if(x > scrdata_dispwin_center - 3 && x < scrdata_dispwin_center + 2 && y >= scrdata_dispwin_ypos && y < scrdata_dispwin_height + scrdata_dispwin_ypos)	// lister inner edge
					{
						dosizedirwindows(65536);
					}
					else if(x >= scrdata_xoffset && x < scrdata_xoffset + scrdata_clock_width && y > scrdata_clock_ypos - 3)	// buttonbank area
					{
					      nextgadgetbank:
						if((data_gadgetrow_offset += scr_gadget_rows) >= 6)
						{
							data_gadgetrow_offset = 0;
							bank = dopus_curgadbank;
							if(dopus_curgadbank && dopus_curgadbank->next)
								dopus_curgadbank = dopus_curgadbank->next;
							else
								dopus_curgadbank = dopus_firstgadbank;
							if(bank != dopus_curgadbank || scr_gadget_rows < 6)
								drawgadgets(0, 0);
						}
						else
							drawgadgets(0, 0);
						fixgadgetprop();
					}
				}
				else if(code == MENUDOWN)
				{
					if((a = DoRMBGadget(mainrmbgads, Window)) > -1)	// check buttons reacting on RMB click
					{
						if(a == FUNC_ROOT)
						{
							char buf[256];
							int w;

							if(x < scrdata_xoffset + 3)
								w = 0;
							else
								w = 1;
							makeactive(w, 0);

							if(!(dopus_curwin[w]->flags & DWF_ARCHIVE))
							{
								strcpy(buf, dopus_curwin[w]->directory);
								if(!(doroot(buf)))
								{
									function = FUNC_DEVICELIST;
									break;
								}
							}
						}
						function = a;
					}
					else if(y < scrdata_diskname_ypos || y > scrdata_yoffset + scrdata_height || x < scrdata_xoffset || x > scrdata_xoffset + scrdata_width)	// window frame
						quickfixmenus();

					else if(y >= scrdata_diskname_ypos && y <= scrdata_dispwin_ypos)	// diskname bar
					{
						if(x < scrdata_dispwin_center)
							a = 1;
						else
							a = 0;
						if(data_active_window == a)
						{
							if(DoubleClick(time_previous_sec, time_previous_micro, time_current_sec, time_current_micro))
							{
								swapdirwin();
								time_previous_sec = 0;
							}
							makeactive(1 - a, 1);
						}
						checksize(1 - a);
					}
					else if(x >= scrdata_xoffset && x < scrdata_xoffset + scrdata_clock_width && y > scrdata_clock_ypos - 3)	// clock bar
					{
						if(qual & IEQUALIFIER_ANYSHIFT)
						{
							drawgadgets(0, GADCOUNT / 2);
							FOREVER
							{
								while(getintuimsg())
								{
									class = IMsg->Class;
									code = IMsg->Code;
									ReplyMsg((struct Message *)IMsg);
									if(class == IDCMP_MOUSEBUTTONS && code == MENUUP)
										break;
								}
								if(class == IDCMP_MOUSEBUTTONS && code == MENUUP)
									break;
								Wait(1 << Window->UserPort->mp_SigBit);
							}
							drawgadgets(0, 0);
						}
						else
						{
						      prevgadgetbank:
							if((data_gadgetrow_offset -= scr_gadget_rows) < 0)
							{
								data_gadgetrow_offset = 6 - scr_gadget_rows;
								bank1 = dopus_curgadbank;
								bank = dopus_firstgadbank;
								while(bank && bank->next && bank->next != dopus_curgadbank)
									bank = bank->next;
								if(!bank)
									dopus_curgadbank = dopus_firstgadbank;
								else
									dopus_curgadbank = bank;
								if(bank1 != dopus_curgadbank || scr_gadget_rows < 6)
									drawgadgets(0, 0);
							}
							else
								drawgadgets(0, 0);
							fixgadgetprop();
						}
					}
					else if(y > scrdata_dispwin_ypos + 1 && y < scrdata_dispwin_height + scrdata_dispwin_ypos + 2)	// lister area
					{
						if(x > 1 && x < scrdata_dispwin_center - 13)
						{
							if(config->generalflags & GENERAL_FMPARENT)
								function = FUNC_PARENT;
							else
							{
							}
						}
						else if(x > scrdata_dispwin_center + 12 && x < screen_gadgets[SCRGAD_RIGHTPARENT].LeftEdge)
						{
							if(config->generalflags & GENERAL_FMPARENT)
								function = FUNC_PARENT;
							else
							{
							}
						}
					}
					else if(x >= scrdata_gadget_xpos && y >= scrdata_gadget_ypos && y < scrdata_gadget_ypos + (scr_gadget_rows * scrdata_gadget_height) + 1 && dopus_curgadbank)	// gadget bank area
					{

						if((c = gadgetfrompos(x, y)) > -1)
						{
							a = c + (GADCOUNT / 2) + (data_gadgetrow_offset * 7);
							if(isvalidgad(&dopus_curgadbank->gadgets[a]))
							{
								x1 = scrdata_gadget_xpos + ((c % 7) * scrdata_gadget_width);
								y1 = scrdata_gadget_ypos + ((c / 7) * scrdata_gadget_height) - 1;
								if(!(dormbgadget(x1, y1, &dopus_curgadbank->gadgets[c + (data_gadgetrow_offset * 7)], &dopus_curgadbank->gadgets[a])))
								{
									dofunctionstring(dopus_curgadbank->gadgets[a].function, dopus_curgadbank->gadgets[a].name, NULL, (struct dopusfuncpar *)&dopus_curgadbank->gadgets[a].which);
								}
							}
						}
					}
				}
				break;
			}
		      foobarbaz:
			if(function)
				internal_function(function, 0, NULL, NULL);
			status_haveaborted = 0;
		}
	}
}

struct IntuiMessage *getintuimsg(void)
{
	return ((IMsg = (struct IntuiMessage *)GetMsg(Window->UserPort)));
}
