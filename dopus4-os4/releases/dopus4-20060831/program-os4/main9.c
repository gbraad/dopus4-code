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

int64 bytes(STRPTR name, int64 *total, int *block)
{
	struct InfoData *infodata = IDOS->AllocDosObject(DOS_INFODATA, NULL);
	BPTR mylock, testlock;
	int64 free = -1;

	if(!(mylock = IDOS->Lock(name, ACCESS_READ)))
	{
		*total = 0;
		*block = 0;
		IDOS->FreeDosObject(DOS_INFODATA, infodata);
		return (0);
	}
	IDOS->Info(mylock, infodata);
	if(ramdisk_lock)
	{
		testlock = IDOS->DupLock(mylock);
		testlock = getrootlock(testlock);
		if(IDOS->SameLock(testlock, ramdisk_lock) == LOCK_SAME)
			free = IExec->AvailMem(0);
		IDOS->UnLock(testlock);
		if(free > -1)
		{
			*total = free;
			*block = free / infodata->id_BytesPerBlock;
			IDOS->UnLock(mylock);
			IDOS->FreeDosObject(DOS_INFODATA, infodata);
			return (free);
		}
	}
	*total = ((int64)infodata->id_BytesPerBlock) * infodata->id_NumBlocks;
	free = *total - (infodata->id_BytesPerBlock * ((int64)infodata->id_NumBlocksUsed));
	*block = infodata->id_NumBlocks - infodata->id_NumBlocksUsed;
	IDOS->UnLock(mylock);
	IDOS->FreeDosObject(DOS_INFODATA, infodata);
	return (free);
}

struct TagItem obtain_tags[] =
{
	{ OBP_Precision, PRECISION_EXACT },
	{ OBP_FailIfBad, TRUE },
	{ TAG_DONE, 0 }
};

void get_colour_table()
{
	int a, pen;
	int num;
	struct ColorMap *cm;

	for(a = 0; a < 16; a++)
	{
		screen_pens[a].red = config->new_palette[(a * 3)];
		screen_pens[a].green = config->new_palette[(a * 3) + 1];
		screen_pens[a].blue = config->new_palette[(a * 3) + 2];
		screen_pens[a].alloc = 0;
	}

	num = 1 << ((config->scrdepth > 4) ? 4 : config->scrdepth);
	cm = Window->WScreen->ViewPort.ColorMap;

	for(a = 0; a < num; a++)
	{
		pen = IGraphics->ObtainBestPenA(cm, screen_pens[a].red, screen_pens[a].green, screen_pens[a].blue, (MainScreen) ? obtain_tags : NULL);
		if(pen == -1)
			screen_pens[a].pen = IGraphics->FindColor(cm, screen_pens[a].red, screen_pens[a].green, screen_pens[a].blue, -1);
		else
		{
			screen_pens[a].pen = pen;
			screen_pens[a].alloc = 1;
		}
	}
}

void free_colour_table()
{
	int a;
	struct ColorMap *cm;

	cm = Window->WScreen->ViewPort.ColorMap;

	for(a = 0; a < 16; a++)
	{
		if(screen_pens[a].alloc)
		{
			IGraphics->ReleasePen(cm, screen_pens[a].pen);
			screen_pens[a].alloc = 0;
		}
	}
}

void SetDrawModes(struct RastPort *r, UBYTE fg, UBYTE bg, UBYTE mode)
{
	IGraphics->SetABPenDrMd(r, screen_pens[fg].pen, screen_pens[bg].pen, mode);
}

void do3dbox(struct RastPort *r, int x, int y, int w, int h)
{
	IDOpus->Do3DBox(r, x, y, w, h, screen_pens[config->gadgettopcol].pen, screen_pens[config->gadgetbotcol].pen);
}
