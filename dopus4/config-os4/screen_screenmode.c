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

#include "config.h"

int initscreenmodes()
{
	int pal, count, minw, minh, defw, depth, needdef = 0, overx, overy, oldcount = 0;
	struct Screen scrbuf, *wbscreen;
	char buf[256], namebuf[180];
	struct DimensionInfo *dimension;
	DisplayInfoHandle *handle;
	struct List *pubscreenlist;
	struct PubScreenNode *pubscreen;
	uint32 modeid;

	dimension = (struct DimensionInfo *)buf;
	count = 0;
	depth = 8;

	if((wbscreen = IIntuition->LockPubScreen("Workbench")))
	{
		modeid = IGraphics->GetVPModeID(&(wbscreen->ViewPort));
		if((handle = IGraphics->FindDisplayInfo(modeid)))
		{
			IGraphics->GetDisplayInfoData(handle, buf, 256, DTAG_DIMS, 0);
			depth = dimension->MaxDepth;
		}
		IIntuition->UnlockPubScreen(NULL, wbscreen);
	}
	IDOpus->GetWBScreen(&scrbuf);

	modeid = INVALID_ID;
	while((modeid = IGraphics->NextDisplayInfo(modeid)) != INVALID_ID)
	{
		if(!IGraphics->ModeNotAvailable(modeid) && (handle = IGraphics->FindDisplayInfo(modeid)) && (IGraphics->GetDisplayInfoData(handle, namebuf, /*128*/180, DTAG_NAME, 0)) && (IGraphics->GetDisplayInfoData(handle, buf, 256, DTAG_DIMS, 0)))
		{
			if(modeid & (HIRES_KEY | HIRESLACE_KEY))
			{
				minw = 640;
			}
			else
			{
				minw = dimension->MinRasterWidth;
				if(minw < 160)
				{
					minw *= 10;
				}
			}

			minh = dimension->MinRasterHeight;

			if(minh < 480)
				minh = 480;
			defw = (dimension->TxtOScan.MaxX - dimension->TxtOScan.MinX) + 1;

			if(minw >= 640 && defw >= 640)
			{
				count += addscreenmode(((struct NameInfo *)namebuf)->Name, minw, minh, dimension->MaxRasterWidth, dimension->MaxRasterHeight, defw, (dimension->TxtOScan.MaxY - dimension->TxtOScan.MinY) + 1, dimension->MaxDepth, modeid);
			}
		}
	}

	if(count == 0)
	{
		needdef = 1;
	}
	else
	{
		sortscreenmodes(count, 0);
		oldcount = count;
	}
	if((pubscreenlist = IIntuition->LockPubScreenList()))
	{
		pubscreen = (struct PubScreenNode *)pubscreenlist->lh_Head;
		while(pubscreen->psn_Node.ln_Succ)
		{
			if(IUtility->Stricmp(pubscreen->psn_Node.ln_Name, "Workbench") != 0 && IUtility->Strnicmp(pubscreen->psn_Node.ln_Name, "DOPUS.", 6) != 0 && pubscreen->psn_Screen->Width >= 640 && pubscreen->psn_Screen->Height >= 480 && pubscreen->psn_Screen->RastPort.BitMap->Depth > 1)
			{

				IUtility->SNPrintf(namebuf, 180, "%s:%s", pubscreen->psn_Node.ln_Name, cfg_string[STR_SCREEN_MODE_USE]);
				count += addscreenmode(namebuf, 640, 480, pubscreen->psn_Screen->Width, pubscreen->psn_Screen->Height, pubscreen->psn_Screen->Width, pubscreen->psn_Screen->Height, pubscreen->psn_Screen->RastPort.BitMap->Depth, MODE_PUBLICSCREENUSE);

			}
			pubscreen = (struct PubScreenNode *)pubscreen->psn_Node.ln_Succ;
		}
		IIntuition->UnlockPubScreenList();
	}

	if(needdef)
	{
		pal = ((struct GfxBase *)(IGraphics->Data.LibBase))->DisplayFlags & PAL;
		overx = 704 - ((struct GfxBase *)(IGraphics->Data.LibBase))->NormalDisplayColumns;
		if(overx < 0)
			overx = 0;
		overy = ((pal) ? 290 : 240) - ((struct GfxBase *)(IGraphics->Data.LibBase))->NormalDisplayRows;
		if(overy < 0)
			overy = 0;

		count += addscreenmode(pal ? "PAL:High Res" : "NTSC:High Res", 640, 200, ((struct GfxBase *)(IGraphics->Data.LibBase))->NormalDisplayColumns + overx, ((struct GfxBase *)(IGraphics->Data.LibBase))->NormalDisplayRows + overy, ((struct GfxBase *)(IGraphics->Data.LibBase))->NormalDisplayColumns, ((struct GfxBase *)(IGraphics->Data.LibBase))->NormalDisplayRows, 4, HIRES_KEY);
		count += addscreenmode(pal ? "PAL:High Res Laced" : "NTSC:High Res Laced", 640, 200, ((struct GfxBase *)(IGraphics->Data.LibBase))->NormalDisplayColumns + overx, (((struct GfxBase *)(IGraphics->Data.LibBase))->NormalDisplayRows + overy) * 2, ((struct GfxBase *)(IGraphics->Data.LibBase))->NormalDisplayColumns, ((struct GfxBase *)(IGraphics->Data.LibBase))->NormalDisplayRows * 2, 4, HIRESLACE_KEY);
	}

	count += addscreenmode(cfg_string[STR_MODE_WORKBENCH_USE], 640, 480, scrbuf.Width, scrbuf.Height, scrbuf.Width, scrbuf.Height, scrbuf.RastPort.BitMap->Depth, MODE_WORKBENCHUSE);
	count += addscreenmode(cfg_string[STR_MODE_WORKBENCH_CLONE], scrbuf.Width, scrbuf.Height, 0, 0, 0, 0, depth, MODE_WORKBENCHCLONE);

	if(count > oldcount)
		sortscreenmodes(count - oldcount, oldcount);
	return (count);
}

int addscreenmode(STRPTR name, UWORD minw, UWORD minh, UWORD maxw, UWORD maxh, UWORD defw, UWORD defh, UWORD depth, uint32 mode)
{
	struct ScreenMode *scrmode;

	screenmode = firstmode;
	while(screenmode)
	{
		if(IDOpus->LStrCmpI(screenmode->name, name) == 0)
			return (0);
		if(!screenmode->next)
			break;
		screenmode = screenmode->next;
	}

	if((scrmode = IDOpus->LAllocRemember(&screenkey, sizeof(struct ScreenMode), MEMF_CLEAR)))
	{
		if(screenmode)
			screenmode->next = scrmode;
		else
			firstmode = scrmode;
		strcpy(scrmode->name, name);
		scrmode->minw = minw;
		scrmode->minh = minh;
		if(maxw == 0)
			scrmode->maxw = minw;
		else
			scrmode->maxw = maxw;
		if(maxh == 0)
			scrmode->maxh = minh;
		else
			scrmode->maxh = maxh;
		if(defw == 0)
			scrmode->defw = minw;
		else
			scrmode->defw = defw;
		if(defh == 0)
			scrmode->defh = minh;
		else
			scrmode->defh = defh;
		scrmode->maxdepth = (depth > 8) ? 8 : depth;
		return (1);
	}
	return (0);
}

struct ScreenMode *showdisplaydesc()
{
	struct ScreenMode *mode;
	char buf[80] = { 0, };

	IGraphics->SetAPen(rp, screen_pens[0].pen);
	IGraphics->RectFill(rp, x_off + 238, y_off + 140, x_off + 509, y_off + 179);
	IGraphics->SetAPen(rp, screen_pens[1].pen);

	if(!(mode = getscreenmode(screenmodeview.itemselected)))
	{
		return (NULL);
	}

	IUtility->SNPrintf(buf, 80, "%16s: %d %s %d", cfg_string[STR_MINIMUM_SIZE], mode->minw, cfg_string[STR_BY], mode->minh);
	IDOpus->UScoreText(rp, buf, x_off + 240, y_off + 147, -1);
	IUtility->SNPrintf(buf, 80, "%16s: %d %s %d", cfg_string[STR_MAXIMUM_SIZE], mode->maxw, cfg_string[STR_BY], mode->maxh);
	IDOpus->UScoreText(rp, buf, x_off + 240, y_off + 155, -1);
	IUtility->SNPrintf(buf, 80, "%16s: %d %s %d", cfg_string[STR_DEFAULT_SIZE], mode->defw, cfg_string[STR_BY], mode->defh);
	IDOpus->UScoreText(rp, buf, x_off + 240, y_off + 163, -1);
	IUtility->SNPrintf(buf, 80, "%16s: %d", cfg_string[STR_MAXIMUM_COLORS], (1 << mode->maxdepth));
	IDOpus->UScoreText(rp, buf, x_off + 240, y_off + 171, -1);

	return (mode);
}

void fixmodegads(struct ScreenMode *mode)
{
	IUtility->SNPrintf(screenwidth_buf, 6, "%d", config->scrw);
	IUtility->SNPrintf(screenheight_buf, 6, "%d", config->scrh);

	if(config->scrdepth < 2)
		config->scrdepth += 2;
	if(config->scrdepth > ((mode) ? mode->maxdepth : 8))
		config->scrdepth = ((mode) ? mode->maxdepth : 8);
//	config->scrdepth = 8;

	IUtility->SNPrintf(screendepth_buf, 4, "%d", (1 << config->scrdepth));

	if(mode && !(screenmodegads[SCREENMODE_WIDTH - 300].Flags & GFLG_DISABLED))
	{
		if(!(IDOpus->CheckNumGad(&screenmodegads[SCREENMODE_WIDTH - 300], Window, mode->minw, mode->maxw)))
			IDOpus->RefreshStrGad(&screenmodegads[SCREENMODE_WIDTH - 300], Window);
	}

	if(mode && !(screenmodegads[SCREENMODE_HEIGHT - 300].Flags & GFLG_DISABLED))
	{
		if(!(IDOpus->CheckNumGad(&screenmodegads[SCREENMODE_HEIGHT - 300], Window, mode->minh, mode->maxh)))
			IDOpus->RefreshStrGad(&screenmodegads[SCREENMODE_HEIGHT - 300], Window);
	}

	IDOpus->EnableGadget(&screenmodegads[SCREENMODE_DEPTH - 300], rp, 0, 0);
	IDOpus->RefreshStrGad(&screenmodegads[SCREENMODE_DEPTH - 300], Window);

	if(mode)
	{
		if(mode->maxdepth == 2)
		{
			IDOpus->DisableGadget(&screenmodegads[SCREENMODE_DEPTH - 300], rp, 0, 0);
			IDOpus->FixSliderBody(Window, &screenmodegads[SCREENMODE_SLIDER - 300], 1, 1, 0);
		}
		else
		{
			IDOpus->FixSliderBody(Window, &screenmodegads[SCREENMODE_SLIDER - 300], mode->maxdepth - 1, 1, 0);
		}
	}
	IDOpus->FixSliderPot(Window, &screenmodegads[SCREENMODE_SLIDER - 300], config->scrdepth - 2, mode->maxdepth - 1, 1, 2);
}

void fixdefaultgads(struct ScreenMode *mode)
{
	if(config->screenflags & SCRFLAGS_DEFWIDTH || mode->mode == MODE_WORKBENCHCLONE || mode->mode == MODE_PUBLICSCREENCLONE)
	{
		config->screenflags |= SCRFLAGS_DEFWIDTH;
		screenmodegads[SCREENMODE_DEFWIDTH - 300].Flags |= GFLG_SELECTED;
		IDOpus->DisableGadget(&screenmodegads[SCREENMODE_WIDTH - 300], rp, 0, 0);
	}
	else
	{
		screenmodegads[SCREENMODE_DEFWIDTH - 300].Flags &= ~GFLG_SELECTED;
		IDOpus->EnableGadget(&screenmodegads[SCREENMODE_WIDTH - 300], rp, 0, 0);
	}
	if(config->screenflags & SCRFLAGS_DEFHEIGHT || mode->mode == MODE_WORKBENCHCLONE || mode->mode == MODE_PUBLICSCREENCLONE)
	{
		config->screenflags |= SCRFLAGS_DEFHEIGHT;
		screenmodegads[SCREENMODE_DEFHEIGHT - 300].Flags |= GFLG_SELECTED;
		IDOpus->DisableGadget(&screenmodegads[SCREENMODE_HEIGHT - 300], rp, 0, 0);
	}
	else
	{
		screenmodegads[SCREENMODE_DEFHEIGHT - 300].Flags &= ~GFLG_SELECTED;
		IDOpus->EnableGadget(&screenmodegads[SCREENMODE_HEIGHT - 300], rp, 0, 0);
	}
	if(mode->maxdepth == 2)
		IDOpus->DisableGadget(&screenmodegads[SCREENMODE_DEPTH - 300], rp, 0, 0);
	else
		IDOpus->EnableGadget(&screenmodegads[SCREENMODE_DEPTH - 300], rp, 0, 0);

	IDOpus->EnableGadget(&screenmodegads[SCREENMODE_DEFWIDTH - 300], rp, 2, 1);
	IDOpus->EnableGadget(&screenmodegads[SCREENMODE_DEFHEIGHT - 300], rp, 2, 1);
	IIntuition->RefreshGList(&screenmodegads[SCREENMODE_DEFWIDTH - 300], Window, NULL, 2);

	if(mode->mode == MODE_WORKBENCHCLONE || mode->mode == MODE_PUBLICSCREENCLONE)
	{
		IDOpus->DisableGadget(&screenmodegads[SCREENMODE_DEFWIDTH - 300], rp, 2, 1);
		IDOpus->DisableGadget(&screenmodegads[SCREENMODE_DEFHEIGHT - 300], rp, 2, 1);
	}
}

void sortscreenmodes(int count, int off)
{
	int gap, i, j;
	struct ScreenMode *mode1, *mode2;

	for(gap = count / 2; gap > 0; gap /= 2)
		for(i = gap; i < count; i++)
			for(j = i - gap; j >= 0; j -= gap)
			{
				mode1 = getscreenmode(j + off);
				mode2 = getscreenmode(j + gap + off);
				if(!mode1 || !mode2 || IDOpus->LStrCmpI(mode1->name, mode2->name) <= 0)
					break;
				SwapMem((char *)mode1->name, (char *)mode2->name, sizeof(struct ScreenMode) - 4);
			}
}

struct ScreenMode *getscreenmode(int m)
{
	struct ScreenMode *mode;
	int a;

	mode = firstmode;
	for(a = 0; a < m; a++)
	{
		if(!mode)
			break;
		mode = mode->next;
	}
	return (mode);
}
