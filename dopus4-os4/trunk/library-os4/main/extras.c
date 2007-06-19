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

#include <string.h>
#include <stdarg.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/dopus.h>
#include <libraries/dopus.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <clib/alib_stdio_protos.h>


/* From borders.c */

void fill_out_border_data(short *shine, short *shadow, int w, int h)
{
	shine[0] = w - 1;     /* w-1,0  -> 0,0  -> 0,h  -> 1,h-1  -> 1,0 */
	shine[5] = h;
	shine[6] = 1;
	shine[7] = h - 1;
	shine[8] = 1;

	shadow[0] = 1;      /* 1,h  -> w,h  -> w,0  -> w-1,1  -> w-1,h */
	shadow[1] = h;
	shadow[2] = w;
	shadow[3] = h;
	shadow[4] = w;
	shadow[6] = w - 1;
	shadow[7] = 1;
	shadow[8] = w - 1;
	shadow[9] = h;
}

void fill_out_border(struct Border *border, int fpen, int bpen, short *shine, short *shadow)
{
	border[0].FrontPen = fpen;
	border[0].DrawMode = JAM1;
	border[0].Count = 5;
	border[0].XY = shine;
	border[0].NextBorder = &border[1];
	border[1].FrontPen = bpen;
	border[1].DrawMode = JAM1;
	border[1].Count = 5;
	border[1].XY = shadow;
}

/* END From borders.c */

/* From dopus_stuff.c */

char *getstringcopy(char *str)
{
	char *newstr = NULL;

	if(str && (newstr = IExec->AllocMem(strlen(str) + 1, 0)))
		IUtility->Strlcpy(newstr, str, strlen(str) + 1);
	return (newstr);
}

void freestring(char *str)
{
	if(str)
		IExec->FreeMem(str, strlen(str) + 1);
}

int writestring(int file, char *string)
{
	int b;
	char nl = 0;

	if(string)
	{
		if((IDOS->Write(file, string, (b = (strlen(string) + 1)))) < b)
			return (0);
	}
	else
		IDOS->Write(file, &nl, 1);
	return (1);
}

int readline(STRPTR buf, int pos, STRPTR buf1, int size)
{
	int a;

	for(a = 0; a < 4096; a++)
	{
		if(size == pos || buf[pos] == 0)
		{
			buf1[a] = 0;
			if(size == pos)
				return (-1);
			return (pos + 1);
		}
		buf1[a] = buf[pos];
		++pos;
	}
	buf1[pos] = 0;
	return (pos);
}

void AssignDrive(struct ConfigStuff *cstuff, int drv, char *name, char *path)
{
	struct Config *config;

	if(!(config = cstuff->config))
		return;
	if(name != (char *)-1)
	{
		if(!name)
			config->drive[drv].name[0] = 0;
		else
			IUtility->Strlcpy(config->drive[drv].name, name, 16);
	}
	if(path != (char *)-1)
	{
		freestring(config->drive[drv].function);
		config->drive[drv].function = NULL;
		if(path && path[0])
			config->drive[drv].function = getstringcopy(path);
	}
}

void linkinnewfiletype(struct ConfigStuff *cstuff, struct dopusfiletype *temp)
{
	struct dopusfiletype *pos;

	temp->next = NULL;
	if(!(pos = cstuff->firsttype))
		cstuff->firsttype = temp;
	else
	{
		while (pos->next)
		{
			if(IUtility->Stricmp(pos->next->type, "Default") == 0)
			{
				temp->next = pos->next;
				break;
			}
			pos = pos->next;
		}
		pos->next = temp;
	}
}

/* END From dopus_stuff.c */

/* From gui.c */

int makeusstring(char *from, char *to, int *uspos, int size)
{
	int len, c;

	*uspos = -1;
	for(c = 0, len = 0; len < size; c++)
	{
		if(!from[c])
			break;
		else if(from[c] == '_' && *uspos == -1)
			*uspos = len;
		else to[len++] = from[c];
	}
	to[len] = 0;
	return(len);
}

void ShowRMBGadName(struct DOpusIFace *IDOpus, struct RastPort *rp, struct RMBGadget *gad, int a)
{
	struct RMBGadgetText *text;
	int len, l, uspos, old;
	char buf[100];

	if((text=&gad->txt[a]))
	{
		old=rp->DrawMode;
		IGraphics->SetDrMd(rp, JAM2);
		IGraphics->SetAPen(rp, text->bg);
		IGraphics->RectFill(rp, gad->x + text->x, gad->y + text->y, gad->x + text->x + text->w - 1, gad->y + text->y + text->h - 1);
		IGraphics->SetAPen(rp, text->fg);
		IGraphics->SetBPen(rp, text->bg);
		if(text->text)
		{
			len = makeusstring(text->text, buf, &uspos, 99);
			while((l = IGraphics->TextLength(rp, buf, len)) > text->w)
			{
				if((--len) == 0)
					break;
			}
			buf[len] = 0;
			IDOpus->UScoreText(rp, buf, gad->x + text->x + ((text->w - l) / 2), gad->y + text->y + ((text->h - rp->Font->tf_YSize) / 2) + rp->Font->tf_Baseline, uspos);
		}
		IGraphics->SetDrMd(rp, old);
	}
}

void HighlightRMBGad(struct RastPort *rp, struct RMBGadget *gad, int state)
{
	if(gad->flags & RGF_ALTBORD)
		IIntuition->DrawBorder(rp,gad->high_bord[state],gad->x,gad->y);
	else
	{
		int old;

		old = rp->DrawMode;
		IGraphics->SetDrMd(rp, COMPLEMENT);
		IGraphics->RectFill(rp, gad->x, gad->y, gad->x + gad->w - 1, gad->y + gad->h - 1);
		IGraphics->SetDrMd(rp, old);
	}
}

/* END From gui.c */

/* From function.c */

void SwapMem(char *src, char *dst, int size)
{
	char tmp;

	for(; size; size--)
	{
		tmp = *dst;
		*dst++ = *src;
		*src++ = tmp;
	}
}

/*
void LSprintfA(char *buf, char *fmt, void *args)
{
	IExec->RawDoFmt(fmt, args, NULL, buf);
}

void VARARGS68K LSprintf(char *buf, char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	LSprintfA(buf, fmt, va_getlinearva(args, void *));
	va_end(args);
}
*/
/* END From functions.c */

/* From imagery.c */

struct Image *get_image_data(struct DOpusIFace *IDOpus, struct DOpusRemember **key, int width, int height, int depth, struct BitMap *bm, struct RastPort *rp)
{
	struct Image *image;
	USHORT *data;
	short a, words;

	words = (width + 15) / 16;

	if(!(image = IDOpus->LAllocRemember(key, sizeof(struct Image), MEMF_CLEAR)) || !(data = IDOpus->LAllocRemember(key, words * 2 * height * depth, MEMF_CHIP|MEMF_CLEAR)))
		return(NULL);

	IGraphics->InitBitMap(bm, depth, width, height);
	for(a = 0; a < depth; a++)
		bm->Planes[a] = (PLANEPTR)&data[words * height * a];
	IGraphics->InitRastPort(rp);
	rp->BitMap = bm;

	image->Width = width;
	image->Height = height;
	image->Depth = depth;
	image->ImageData = data;
	image->PlanePick = (1 << depth) - 1;

	return(image);
}

/* END From imagery.c */

/* From language.c */

char **string_table;

/* END From language.c */

/* From listview.c */

const char nullstring[] = "                                                                                                                                ";

void dohilite(struct DOpusListView *view, int a)
{
	int x, y, mode;

	mode = view->window->RPort->DrawMode;
	IGraphics->SetDrMd(view->window->RPort, COMPLEMENT);
	if(view->flags & DLVF_CHECK)
		x = view->xo - 22;
	else
		x = view->xo;
	y = view->yo + (view->fh * a);
	IGraphics->RectFill(view->window->RPort, x, y, view->mx, y + view->fh - 1);
	IGraphics->SetDrMd(view->window->RPort, mode);
}

void savepens(struct DOpusListView *view)
{
	view->ofg = view->window->RPort->FgPen;
	view->obg = view->window->RPort->BgPen;
	view->odm = view->window->RPort->DrawMode;
}

void restorepens(struct DOpusListView *view)
{
	IGraphics->SetAPen(view->window->RPort, view->ofg);
	IGraphics->SetBPen(view->window->RPort, view->obg);
	IGraphics->SetDrMd(view->window->RPort, view->odm);
}

void DisplayView(struct DOpusIFace *IDOpus, struct DOpusListView *view)
{
	struct RastPort *rp;
	int y, a, b, top, bot, dif, dir, start, end, step, w;
	char buf[128];
	static UWORD ditherdata[2] = { 0x8888, 0x2222 };

	savepens(view);
	if(view->oldoffset == -1 || (dif = (view->oldoffset - view->topitem)) >= view->lines || dif <= -view->lines)
	{
		top = view->topitem;
		bot = view->topitem + view->lines - 1;
		dir = 0;
	}
	else if(dif < 0)
	{
		top = view->topitem + view->lines + dif;
		bot = view->topitem + view->lines - 1;
		dir = view->fh;
	}
	else if(dif > 0)
	{
		top = view->topitem;
		bot = view->topitem + dif - 1;
		dir = -view->fh;
	}
	else
		return;
	rp = view->window->RPort;
	IGraphics->SetAPen(rp, view->itemfg);
	IGraphics->SetBPen(rp, view->itembg);
	IGraphics->SetDrMd(rp, JAM2);
	start = view->topitem;
	end = view->topitem + view->lines;
	step = 1;
	if(dir > 0)
		y = view->yo + view->fb + (view->fh * (view->lines - 1));
	else
	{
		y = view->yo + view->fb;
		if(dir)
		{
			start = end - 1;
			end = view->topitem - 1;
			step = -1;
		}
	}
	if(!dir && view->flags & DLVF_CHECK)
		IGraphics->ScrollRaster(rp, 0, view->lines * view->fh, view->x, view->yo, view->xo - 1, view->my);
	w = view->w - (view->xo - view->x);
	for(a = start; a != end; a += step)
	{
		if(a >= top && a <= bot)
		{
			IExec->CopyMem(nullstring, buf, view->columns);
			if(view->items && a < view->count && view->items[a] && (!(view->flags & DLVF_ENDNL) || view->items[a][0]))
			{
				b = strlen(view->items[a]);
				if(b > view->columns)
					b = view->columns;
				IExec->CopyMem(view->items[a], buf, b);
			}
			IGraphics->Move(rp, view->xo, y);
			if(dir)
			{
				if(dir < 0)
					IGraphics->ClipBlit(rp, view->xo, view->yo, rp, view->xo, view->yo + view->fh, w, (view->lines - 1) * view->fh, 0xc0);
				else
					IGraphics->ClipBlit(rp, view->xo, view->yo + view->fh, rp, view->xo, view->yo, w, (view->lines - 1) * view->fh, 0xc0);
				if(view->flags & DLVF_CHECK)
					IGraphics->ScrollRaster(rp, 0, dir, view->x, view->yo, view->xo - 1, view->my);
			}
			IGraphics->Text(rp, buf, view->columns);
			if(view->selectarray && view->selectarray[a] & LVARRAY_DISABLED)
			{
				rp->AreaPtrn = ditherdata;
				rp->AreaPtSz = 1;
				IGraphics->SetDrMd(rp, JAM1);
				IGraphics->SetAPen(rp, view->itembg);
				IGraphics->RectFill(rp, view->xo, y - view->fb, rp->cp_x - 1, (y - view->fb) + view->fh - 1);
				IGraphics->SetAPen(rp, view->itemfg);
				IGraphics->SetDrMd(rp, JAM2);
				rp->AreaPtrn = NULL;
				rp->AreaPtSz = 0;
			}
			if(a < view->count)
			{
				if((view->flags & DLVF_MULTI && view->selected[a] & LVARRAY_SELECTED) || (view->flags & DLVF_LEAVE && view->itemselected == a))
				{
					if(!(view->flags & DLVF_TOGGLE) || view->chk == view->itemselected)
					{
						if(view->flags & DLVF_CHECK)
							IDOpus->DrawCheckMark(rp, view->xo - 18, y + view->ty - view->fb, 1);
						else
						{
							IGraphics->SetDrMd(rp, COMPLEMENT);
							if(dir < 0)
								b = 0;
							else if(dir > 0)
								b = view->lines - 1;
							else
								b = a - view->topitem;
							dohilite(view, b);
							IGraphics->SetDrMd(rp, JAM2);
						}
					}
				}
			}
		}
		if(!dir)
			y += view->fh;
	}
	view->oldoffset = view->topitem;
	restorepens(view);
}

int scroll_view(struct DOpusIFace *IDOpus, struct DOpusListView *view, int offset, int *histate, int oldoffset)
{
	int draw = 0;

	if(view->count < view->lines && offset >= view->count)
		return (view->count - 1);

	if(offset < 0)
	{
		if(view->topitem > 0)
		{
			--view->topitem;
			draw = 1;
		}
		offset = 0;
	}
	else if(offset >= view->lines)
	{
		if(view->topitem <= view->count - (view->lines + 1))
		{
			++view->topitem;
			draw = 1;
		}
		offset = view->lines - 1;
	}

	if(draw)
	{
		if(*histate)
		{
			*histate = 0;
			dohilite(view, oldoffset);
		}
		IDOpus->FixSliderPot(view->window, &view->listgads[0], view->topitem, view->count, view->lines, 1);
		DisplayView(IDOpus, view);
	}
	return (offset);
}

int view_valid(struct DOpusListView *view, int itemnum)
{
	if(itemnum < view->count && (!view->selectarray || !(view->selectarray[itemnum] & LVARRAY_DISABLED)))
		return (1);
	return (0);
}

/* END From listview.c */
