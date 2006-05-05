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

void makereselect(struct DirWindowPars *winpar, int win)
{
	int a, num;
	struct Directory *dir, *top;

	if(winpar->reselection_list)
	{
		IExec->FreeMem(winpar->reselection_list, winpar->reselection_size);
		winpar->reselection_list = NULL;
	}
	if(win == -1 || (dopus_curwin[win]->firstentry && dopus_curwin[win]->firstentry->type == ENTRY_CUSTOM))
		return;

	winpar->reselection_dir = dopus_curwin[win];
	winpar->reselection_win = win;
	winpar->reselection_size = (dopus_curwin[win]->dirsel + dopus_curwin[win]->filesel + 1) * FILEBUF_SIZE;

	if(!(winpar->reselection_list = IExec->AllocMem(winpar->reselection_size, MEMF_CLEAR)))
		return;

	dir = winpar->reselection_dir->firstentry;
	a = num = 0;
	top = NULL;
	while(dir)
	{
		if(dir->selected)
		{
			IExec->CopyMem(dir->name, &winpar->reselection_list[a], FILEBUF_SIZE);
			a += FILEBUF_SIZE;
		}
		if(!top && num == dopus_curwin[win]->offset)
			top = dir;
		dir = dir->next;
		++num;
	}
	if(top)
		strcpy(winpar->top_name, top->name);
	else
		winpar->top_name[0] = 0;
	winpar->offset = dopus_curwin[win]->offset;
	winpar->hoffset = dopus_curwin[win]->hoffset;
}

void doreselect(struct DirWindowPars *winpar, int moveold)
{
	int a, num = 0, top = -1;
	struct Directory *dir;

	if(winpar->reselection_list && winpar->reselection_dir == dopus_curwin[winpar->reselection_win])
	{
		dir = dopus_curwin[winpar->reselection_win]->firstentry;
		while(dir)
		{
			if(dir->selected)
			{
				dir->selected = 0;
				updateselectinfo(dir, winpar->reselection_win, 0);
			}
			if(top == -1 && winpar->top_name[0] && (strcmp(dir->name, winpar->top_name)) == 0)
				top = num;
			++num;
			dir = dir->next;
		}
		for(a = 0; a < winpar->reselection_size; a += FILEBUF_SIZE)
		{
			if(winpar->reselection_list[a])
			{
				dir = dopus_curwin[winpar->reselection_win]->firstentry;
				while(dir)
				{
					if(strcmp(&winpar->reselection_list[a], dir->name) == 0)
					{
						dir->selected = 1;
						updateselectinfo(dir, winpar->reselection_win, 0);
						break;
					}
					dir = dir->next;
				}
			}
		}
		if(!moveold)
		{
			if(top < 0)
				top = winpar->offset;
			dopus_curwin[winpar->reselection_win]->offset = top;
			dopus_curwin[winpar->reselection_win]->hoffset = winpar->hoffset;
		}
		refreshwindow(winpar->reselection_win, 1);
		doselinfo(winpar->reselection_win);
	}
}

void shutthingsdown(int louise)
{
	remclock();

	main_proc->pr_WindowPtr = windowptr_save;
	if(!status_iconified && status_publicscreen && Window)
	{
		if(MainScreen)
		{
			config->scr_winx = Window->LeftEdge;
			config->scr_winy = Window->TopEdge;
		}
		else
		{
			config->wbwinx = Window->LeftEdge;
			config->wbwiny = Window->TopEdge;
		}
	}
	if(louise < 1)
		closedisplay();
}

void setupwindreq(struct Window *wind)
{
	IGraphics->SetFont(wind->RPort, scr_font[FONT_GENERAL]);
	IGraphics->SetAPen(wind->RPort, screen_pens[config->requestbg].pen);
	IGraphics->RectFill(wind->RPort, wind->BorderLeft + 2, wind->BorderTop + 1, wind->Width - wind->BorderRight - 3, wind->Height - wind->BorderBottom - 2);
	if(!(wind->Flags & WFLG_DRAGBAR))
		do3dbox(wind->RPort, 2, 1, wind->Width - 4, wind->Height - 2);
	SetDrawModes(wind->RPort, config->requestfg, config->requestbg, JAM2);
}

void hilite_req_gadget(struct Window *win, USHORT gadid)
{
	struct Gadget *gad;

	gad = win->FirstGadget;
	while(gad)
	{
		if(gad->GadgetID == gadid && gad->GadgetType == GTYP_BOOLGADGET)
		{
			IDOpus->SelectGadget(win, gad);
			return;
		}
		gad = gad->NextGadget;
	}
}

int simplerequest(STRPTR txt, ...)
{
	char *gads[11], *cancelgad = NULL, *gad;
	int a, r = 1, rets[10], gnum = 0, rnum = 0;
	va_list ap;
	struct DOpusSimpleRequest request;

	va_start(ap, txt);
	for(a = 0; a < 10; a++)
	{
		if(!(gad = (STRPTR)va_arg(ap, STRPTR)))
		{
			  break;
		}
		if(a == 1)
		{
			cancelgad = gad;
		}
		else
		{
			gads[gnum++] = gad;
			if(gad[0] != '\n')
				rets[rnum++] = r++;
		}
	}
	va_end(ap);
	if(cancelgad)
	{
		gads[gnum] = cancelgad;
		rets[rnum] = 0;
		a = gnum + 1;
	}
	for(; a < 11; a++)
		gads[a] = NULL;

	request.strbuf = NULL;
	request.flags = 0;
	return (dorequest(&request, txt, gads, rets, NULL));
}

int whatsit(char *txt, int max, char *buffer, char *skiptxt)
{
	char *gads[4];
	int a = 1, rets[3];
	struct DOpusSimpleRequest request;

	request.strbuf = buffer;
	request.strlen = max;
	request.flags = 0;

	gads[0] = str_okaystring;
	rets[0] = 1;
	if(skiptxt == (char *)-1)
		request.flags = SRF_GLASS | SRF_DIRGLASS;
	else if(skiptxt == (char *)-2)
		request.flags = SRF_GLASS;
	else if(skiptxt)
	{
		rets[1] = 2;
		gads[a++] = skiptxt;
	}
	rets[a] = 0;
	gads[a++] = str_cancelstring;
	for(; a < 4; a++)
		gads[a] = NULL;

	return (dorequest(&request, txt, gads, rets, NULL));
}

int dorequest(struct DOpusSimpleRequest *request, char *txt, char **gads, int *rets, struct Window *window)
{
	int a;
	struct Window *win = NULL;

	request->text = txt;
	request->gads = gads;
	request->rets = rets;

	if(window)
	{
		win = window;
		request->font = window->RPort->Font;
	}
	else if(reqoverride)
	{
		request->font = scr_font[FONT_GENERAL];
		request->hi = 2;
		request->lo = 1;
		request->fg = 1;
		request->bg = 0;
		win = reqoverride;
	}
	else
	{
		if(!status_iconified || status_flags & STATUS_ISINBUTTONS)
			win = Window;
		if(win)
		{
			request->hi = screen_pens[config->gadgettopcol].pen;
			request->lo = screen_pens[config->gadgetbotcol].pen;
			request->fg = screen_pens[config->requestfg].pen;
			request->bg = screen_pens[config->requestbg].pen;
		}
		else
		{
			request->hi = -1;
			request->lo = -1;
			request->fg = -1;
			request->bg = -1;
		}
		request->font = scr_font[FONT_REQUEST];
	}
	if(config->generalscreenflags & SCR_GENERAL_REQDRAG)
	{
		request->flags |= SRF_BORDERS;
		request->title = globstring[STR_DIRECTORY_OPUS_REQUEST];
	}
	else
		request->flags &= ~SRF_BORDERS;

	request->flags |= SRF_RECESSHI | SRF_EXTEND;
	request->value = (int)&requester_stringex;
	fix_stringex(&requester_stringex);

	a = IDOpus->DoSimpleRequest(win, request);
	return (((a == 65535) ? 1 : a));
}

int checkfiletypefunc(char *name, int fn)
{
	struct dopusfiletype *type;
	struct dopusfuncpar par;

	if((type = checkfiletype(name, fn, 1)))
	{
		char title[256];

		par.which = type->which[fn];
		par.stack = type->stack[fn];
		par.key = par.qual = 0;
		par.pri = type->pri[fn];
		par.delay = type->delay[fn];

		if(type->actionstring[fn][0])
		{
			do_title_string(type->actionstring[fn], title, 0, IDOS->FilePart(name));
			dostatustext(title);
		}
		else
			title[0] = 0;

		dofunctionstring(type->function[fn], IDOS->FilePart(name), title, &par);
		return (1);
	}
	return (0);
}

struct dopusfiletype *checkfiletype(char *fullname, int ftype, int funconly)
{
	struct FileInfoBlock *info = IDOS->AllocDosObject(DOS_FIB, NULL);
	struct dopusfiletype *type;
	int file;

	if(!(lockandexamine(fullname, info)))
		return (NULL);
	if(!(file = IDOS->Open(fullname, MODE_OLDFILE)))
		return (NULL);

	type = dopus_firsttype;
	while(type)
	{
		if(status_haveaborted)
			break;
		if(ftype == -2)
		{
			if(type->iconpath && type->recognition && (dochecktype(type, fullname, file, info)))
			{
				IDOS->Close(file);
				return (type);
			}
		}
		else
		{
			if(!funconly || (type->function[ftype] && type->function[ftype][0]))
			{
				if(type->recognition && dochecktype(type, fullname, file, info) && (ftype == -1 || (type->function[ftype] && type->function[ftype][0])))
				{
					IDOS->Close(file);
					return (type);
				}
			}
		}
		type = type->next;
	}
	IDOS->Close(file);
	IDOS->FreeDosObject(DOS_FIB, info);
	return (NULL);
}

int dochecktype(struct dopusfiletype *type, char *name, int file, struct FileInfoBlock *info)
{
	unsigned char buf[514], buf2[1024], *recog;
	int a, b, c, d, len, operation, fail, prot[2], tprot, equ, val, oldpos, err, gotone = 0, test;
	struct DateStamp ds1, ds2;

	len = strlen((recog = type->recognition)) + 1;
	b = operation = 0;

	IDOS->Seek(file, 0, OFFSET_BEGINNING);
	for(a = 0; a < len; a++)
	{
		if(!operation)
		{
			if(recog[a] > 0 && recog[a] < FTYC_COMMANDOK)
				operation = recog[a];
		}
		else if(b == 511 || recog[a] > FTYC_ENDLIMIT || !recog[a])
		{
			buf[b] = 0;
			fail = 0;
			test = 1;
			switch (operation)
			{
			case FTYC_MATCH:
			case FTYC_MATCHI:
				if(!(checktypechars(file, buf, (operation == FTYC_MATCHI) ? 1 : 0)))
					fail = 1;
				break;
			case FTYC_MATCHNAME:
				IDOS->ParsePatternNoCase(buf, buf2, 1024);
				if(!(IDOS->MatchPatternNoCase(buf2, info->fib_FileName)))
					fail = 1;
				break;
			case FTYC_MATCHBITS:
				getprotselvals(buf, prot);
				tprot = ((~info->fib_Protection) & 15) + (info->fib_Protection & ~15);
				if(!((tprot & prot[0]) == prot[0] && ((tprot & ~prot[0]) & prot[1]) == 0))
					fail = 1;
				break;
			case FTYC_MATCHCOMMENT:
				IDOS->ParsePatternNoCase(buf, buf2, 1024);
				if(!(IDOS->MatchPatternNoCase(buf2, info->fib_Comment)))
					fail = 1;
				break;
			case FTYC_MATCHSIZE:
				d = strlen(buf);
				equ = 2;
				for(c = 0; c < d; c++)
				{
					if(equ == 2 && !(_isspace(buf[c])))
					{
						if(buf[c] == '<')
							equ = -1;
						else if(buf[c] == '=')
							equ = 0;
						else if(buf[c] == '>')
							equ = 1;
						else
						{
							fail = 1;
							break;
						}
					}
					else if(equ != 2 && (_isdigit(buf[c])))
					{
						val = atoi(&buf[c]);
						break;
					}
				}
				if(equ != 2)
				{
					if(equ == -1 && info->fib_Size >= val)
						fail = 1;
					else if(equ == 0 && info->fib_Size != val)
						fail = 1;
					else if(equ == 1 && info->fib_Size <= val)
						fail = 1;
				}
				break;
			case FTYC_MATCHDATE:
				getseldatestamps(buf, &ds1, &ds2);
				if(IDOS->CompareDates(&(info->fib_Date), &ds1) < 0 || IDOS->CompareDates(&ds2, &(info->fib_Date)) < 0)
					fail = 1;
				break;
			case FTYC_MOVETO:
				test = 0;
				if(buf[0] == '$')
					val = IDOpus->Atoh(&buf[1], -1);
				else
					val = atoi(buf);
				if(val == -1)
					err = IDOS->Seek(file, 0, OFFSET_END);
				else if(val > -1)
					err = IDOS->Seek(file, val, OFFSET_BEGINNING);
				else
					err = -1;
				if(err == -1)
					fail = 1;
				break;
			case FTYC_MOVE:
				test = 0;
				if(buf[0] == '$')
					val = IDOpus->Atoh(&buf[1], -1);
				else
					val = atoi(buf);
				if((IDOS->Seek(file, val, OFFSET_CURRENT)) == -1)
					fail = 1;
				if(err == -1)
					fail = 1;
				break;
			case FTYC_SEARCHFOR:
				oldpos = IDOS->Seek(file, 0, OFFSET_CURRENT);
				if((val = typesearch(file, buf, SEARCH_NOCASE | SEARCH_WILDCARD, NULL, 0)) == -1)
				{
					fail = 1;
					IDOS->Seek(file, oldpos, OFFSET_BEGINNING);
				}
				else
					IDOS->Seek(file, val, OFFSET_BEGINNING);
				break;
			default:
				test = 0;
				break;
			}
			if(!fail && test && recog[a] == FTYC_OR)
				gotone = 1;
			else if(fail)
			{
				while(recog[a] != FTYC_OR && recog[a] != FTYC_AND && recog[a])
					++a;
				if(recog[a] == FTYC_AND)
					break;
			}
			operation = b = 0;
		}
		else
			buf[b++] = recog[a];
	}
	if(!fail || gotone)
		return (1);
	return (0);
}

int checktypechars(int file, UBYTE *match, int nocase)
{
	unsigned char matchbuf[258], c1, c2;
	int len, clen, a, first = 1, m, val, bpos;

	len = strlen(match);

	switch (match[0])
	{
	case '$':
		clen = (len - 1) / 2;
		break;
	case '%':
		clen = (len - 1) / 8;
		break;
	default:
		clen = len;
		first = 0;
		break;
	}
	if(clen > 256)
		clen = 256;
	if((IDOS->Read(file, matchbuf, clen)) != clen)
		return (0);
	m = 0;
	bpos = 0;
	switch (match[0])
	{
	case '$':
		for(a = first; a < len; a += 2, m++)
		{
			if(match[a] != '?')
			{
				val = IDOpus->Atoh(&match[a], 2);
				if(val != matchbuf[m])
					return (0);
			}
		}
		break;
	case '%':
		for(a = first; a < len; a++)
		{
			if(match[a] != '?')
			{
				if(match[a] == '1' && !(matchbuf[m] & (1 << bpos)))
					return (0);
				else if(match[a] == '0' && (matchbuf[m] & (1 << bpos)))
					return (0);
			}
			if((++bpos) == 8)
			{
				bpos = 0;
				++m;
			}
		}
		break;
	default:
		for(a = first; a < len; a++, m++)
		{
			if(match[a] != '?' && match[a] != matchbuf[m])
			{
				if(nocase)
				{
					c1 = toupper(match[a]);
					c2 = toupper(matchbuf[m]);
					if(c1 == c2)
						continue;
				}
				return (0);
			}
		}
		break;
	}
	return (1);
}

int typesearch(int file, char *find, int flags, char *buffer, int bufsize)
{
	unsigned char *findbuf, matchbuf[256];
	int matchsize, a, len, size, oldpos;

	len = strlen(find);
	if(find[0] == '$')
	{
		for(a = 1, matchsize = 0; a < len; a += 2, matchsize++)
		{
			if(find[a] == '?')
				matchbuf[matchsize] = '?';
			else
				matchbuf[matchsize] = IDOpus->Atoh(&find[a], 2);
		}
		flags &= ~(SEARCH_NOCASE | SEARCH_ONLYWORDS);
	}
	else
	{
		for(a = 0, matchsize = 0; a < len; a++)
		{
			if(find[a] == '\\')
			{
				if(find[a + 1] == '\\')
				{
					matchbuf[matchsize++] = '\\';
					++a;
				}
				else
				{
					matchbuf[matchsize++] = atoi(&find[a + 1]);
					a += 3;
				}
			}
			else if(flags & SEARCH_NOCASE)
				matchbuf[matchsize++] = toupper(find[a]);
			else
				matchbuf[matchsize++] = find[a];
		}
	}

	search_found_lines = search_last_line_pos = 0;
	if(buffer)
		return (searchbuffer(buffer, bufsize, matchbuf, matchsize, flags));
	else
	{
		if(!(findbuf = IExec->AllocMem(32004, MEMF_CLEAR)))
			return (-1);
		FOREVER
		{
			if(status_haveaborted)
			{
				myabort();
				break;
			}
			oldpos = IDOS->Seek(file, 0, OFFSET_CURRENT);
			if((size = IDOS->Read(file, findbuf, 32000)) < 1)
				break;
			if((searchbuffer(findbuf, size, matchbuf, matchsize, flags)) == 1)
			{
				oldpos += ((int)search_found_position - (int)findbuf);
				IExec->FreeMem(findbuf, 32004);
				return (oldpos);
			}
			if(status_haveaborted)
				continue;
			if(size < 32000)
				break;
			IDOS->Seek(file, -matchsize, OFFSET_CURRENT);
		}
		IExec->FreeMem(findbuf, 32004);
	}
	return (-1);
}

int searchbuffer(char *findbuf, int size, char *matchbuf, int matchsize, int flags)
{
	char lastchar, mchar;
	int a, matchchar, matchstart, lastlines, lastpos;

	matchchar = matchstart = lastchar = 0;
	lastlines = search_found_lines;
	lastpos = search_last_line_pos;

	for(a = 0; a < size; a++)
	{
		if(status_haveaborted)
			return (-1);
		if((mchar = (flags & SEARCH_NOCASE) ? toupper(findbuf[a]) : findbuf[a]) == 10)
		{
			++search_found_lines;
			search_last_line_pos = a + 1;
		}
		if((!(flags & SEARCH_WILDCARD) || matchbuf[matchchar] != '?') && matchbuf[matchchar] != mchar)
		{
			if(matchchar > 0)
			{
				a = matchstart;
				search_found_lines = lastlines;
				search_last_line_pos = lastpos;
				matchchar = matchstart = 0;
			}
		}
		else
		{
			if(!(flags & SEARCH_ONLYWORDS) || matchchar || isonlyword(lastchar))
			{
				if(matchchar == 0)
				{
					matchstart = a;
					lastlines = search_found_lines;
					lastpos = search_last_line_pos;
				}
				if((++matchchar) == matchsize)
				{
					if(!(flags & SEARCH_ONLYWORDS) || isonlyword(findbuf[a + 1]))
					{
						search_found_position = findbuf + matchstart;
						search_found_size = matchchar;
						return (1);
					}
					a = matchstart;
					search_found_lines = lastlines;
					search_last_line_pos = lastpos;
					matchchar = matchstart = 0;
				}
			}
		}
		lastchar = mchar;
	}
	return (-1);
}
