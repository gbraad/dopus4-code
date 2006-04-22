/*
 *  $VER: ReadConfig.c $Revision$ (01-Feb-2006)
 *
 *  This file is part of dopus.
 *
 *  (C) Copyright 2006 Hyperion Entertainment
 *      All rights reserved
 *
 * $Id$
 *
 * $Log$
 *
 *
 */

#include <string.h>

#include <exec/exec.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <dos/dos.h>
#include <graphics/gfxbase.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

#include "extras.h"

/****** dopus/main/ReadConfig ******************************************
*
*   NAME
*      ReadConfig -- Description
*
*   SYNOPSIS
*      int ReadConfig(STRPTR name, struct ConfigStuff * cstuff);
*
*   FUNCTION
*
*   INPUTS
*       name - 
*       cstuff - 
*
*   RESULT
*       The result ...
*
*   EXAMPLE
*
*   NOTES
*
*   BUGS
*
*   SEE ALSO
*
*****************************************************************************
*
*/

int _DOpus_ReadConfig(struct DOpusIFace *Self, STRPTR name, struct ConfigStuff *cstuff)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *DOSBase = IExec->OpenLibrary("dos.library", 50L);
	struct DOSIFace *IDOS = (struct DOSIFace *)IExec->GetInterface(DOSBase, "main", 1, NULL);
	struct Library *GfxBase = IExec->OpenLibrary("graphics.library", 50L);
	struct GraphicsIFace *IGraphics = (struct GraphicsIFace *)IExec->GetInterface(GfxBase, "main", 1, NULL);
	int a, in, size, pos, b, bk, gad, mv;
	USHORT ver, mag;
	STRPTR cbuf, buf, tbuf;
	char buf2[102], buf3[102];
	struct dopusfiletype *newtype;
	struct olddopusfiletype otype;
	struct dopusgadgetbanks *bank = NULL, *temp;
	struct Config *config;
	struct DOpusRemember *key;
	struct dopusfunction *tempfunc;
	struct dopusdrive *driveptr;
	struct dopushotkey *hotkey, *curhotkey = NULL;

	if(!(config = cstuff->config))
		return (-1);

	Self->FreeConfig(cstuff);
	if(Self->CheckExist(name, &size) >= 0)
		return (ERROR_OBJECT_NOT_FOUND);
	size -= sizeof(struct Config);
	if(!(in = IDOS->Open(name, MODE_OLDFILE)))
		return (IDOS->IoErr());

	if((IDOS->Read(in, (STRPTR)&ver, 2)) < 2 || (IDOS->Read(in, (STRPTR)&mag, 2)) < 2 || mag != CONFIG_MAGIC || ver == 0xde)
	{
		if(mag == CONFIG_MAGIC && ver == 0xde)
		{
			struct DOpusSimpleRequest req;
			STRPTR gads[2];
			int rets[1];

			req.text = string_table[STR_CONFIG_TOO_OLD];
			gads[0] = string_table[STR_CONTINUE];
			gads[1] = NULL;
			rets[0] = 0;
			req.gads = gads;
			req.rets = rets;
			req.hi = -1;
			req.lo = -1;
			req.fg = -1;
			req.bg = -1;
			req.strbuf = NULL;
			req.flags = SRF_BORDERS;
			req.font = ((struct GfxBase *)(IGraphics->Data.LibBase))->DefaultFont;
			req.title = string_table[STR_REQUEST];

			Self->DoSimpleRequest(NULL, &req);
		}
		IDOS->Close(in);
		return (ERROR_NOT_CONFIG);
	}
	IDOS->Seek(in, 0, OFFSET_BEGINNING);
	if((IDOS->Read(in, (STRPTR)config, sizeof(struct Config))) < sizeof(struct Config))
	{
		IDOS->Close(in);
		return (IDOS->IoErr());
	}
	if(config->version < CONFIG_CHANGE_DISPLAY)
	{
		for(a = 0; a < 2; a++)
		{
			for(b = 0; b < 8; b++)
			{
				config->displaypos[a][b] = config->old_displaypos[a][b];
				config->displaylength[a][b] = (config->old2_displaylength[a][b]) * 8;
				config->old_displaypos[a][b] = 0;
				config->old2_displaylength[a][b] = 0;
			}
			for(b = 8; b < 16; b++)
			{
				config->displaypos[a][b] = -1;
				config->displaylength[a][b] = 0;
			}
		}
	}
	else if(config->version < CONFIG_BIG_DISPLEN)
	{
		for(a = 0; a < 2; a++)
		{
			for(b = 0; b < 16; b++)
			{
				config->displaylength[a][b] = (config->old_displaylength[a][b]) * 8;
				config->old_displaylength[a][b] = 0;
			}
		}
	}

	if(config->version < CONFIG_CHANGE_CONFIGXY)
	{
		config->config_x = -1;
		config->config_y = -1;
	}

	if(config->version < CONFIG_CHANGE_DIMENS)
	{
		config->scr_winx = config->wbwinx;
		config->scr_winy = config->wbwiny;
		config->scr_winw = config->scrw;
		config->scr_winh = config->scrh;
		config->pubscreen_name[0] = 0;
	}

	if(config->version < CONFIG_NEW_SLIDERS)
	{
		config->slider_pos = 1;
	}

	if(config->version <= OLD_CONFIG_VERSION)
	{
		config->scrdepth += 2;
		config->dateformat = 1;
		for(a = 0; a < 2; a++)
		{
			for(b = 0; b < 5; b++)
				config->displaypos[a][b] = b;
			for(b = 5; b < 16; b++)
				config->displaypos[a][b] = -1;
			config->displaylength[a][DISPLAY_NAME] = 32 * 8;
			config->displaylength[a][DISPLAY_COMMENT] = 40 * 8;
			config->displaylength[a][DISPLAY_FILETYPE] = 16 * 8;
			config->displaylength[a][DISPLAY_OWNER] = 16 * 8;
			config->displaylength[a][DISPLAY_GROUP] = 16 * 8;
			config->sortmethod[a] = 0;
			config->separatemethod[a] = 1;
		}
		config->sortflags = 0;
		config->scrclktype = SCRCLOCK_MEMORY | SCRCLOCK_DATE | SCRCLOCK_TIME;
		if(config->icontype == 3)
			config->icontype = ICON_NOWINDOW;
		else
			config->icontype = 0;
		config->icontype |= ICON_MEMORY | ICON_DATE | ICON_TIME;
		config->showfree = 1 << config->showfree;
		config->hotkeyqual = config->hotkeycode & 0xff;
		config->hotkeycode >>= 8;
		config->stringfgcol = 1;
		config->stringbgcol = 0;
		config->stringselfgcol = 1;
		config->stringselbgcol = 0;
	}
	if(config->version <= CONFIG_CHANGE_PALETTE)
	{
		for(a = 0, b = 0; a < 16; a++)
		{
			config->new_palette[b++] = (((config->Palette[a] >> 8) & 0xf) << 28) | 0x0fffffff;
			config->new_palette[b++] = (((config->Palette[a] >> 4) & 0xf) << 28) | 0x0fffffff;
			config->new_palette[b++] = ((config->Palette[a] & 0xf) << 28) | 0x0fffffff;
		}
	}
	if(config->version <= CONFIG_CHANGE_BUFCOUNT)
	{
		config->bufcount /= 2;
		if(config->bufcount < 1)
			config->bufcount = 1;
	}
	if(config->version < CONFIG_LESS_DODRIVES)
	{
		if(!(tempfunc = IExec->AllocMem(sizeof(struct dopusfunction) * DRIVECOUNT, MEMF_CLEAR)))
		{
			IDOS->Close(in);
			return (ERROR_NO_FREE_STORE);
		}
		driveptr = (struct dopusdrive *)config->drive;
		for(a = 0; a < OLDDRIVECOUNT; a++)
		{
			Self->LStrCpy(tempfunc[a].name, driveptr[a].name);
			tempfunc[a].key = driveptr[a].key;
			tempfunc[a].qual = driveptr[a].qual;
			tempfunc[a].fpen = driveptr[a].fpen;
			tempfunc[a].bpen = driveptr[a].bpen;
			tempfunc[a].function = (STRPTR)getstringcopy(driveptr[a].path);
		}
		for(a = OLDDRIVECOUNT; a < DRIVECOUNT; a++)
			tempfunc[a].fpen = 3;
		IExec->CopyMem((STRPTR)tempfunc, (STRPTR)driveptr, sizeof(struct dopusfunction) * DRIVECOUNT);
		IExec->FreeMem(tempfunc, sizeof(struct dopusfunction) * DRIVECOUNT);
	}
	if(config->version <= CONFIG_CHANGE_DOSREQ)
	{
		config->errorflags ^= 1;
		config->hotkeyflags = 0;
		config->pad3 = config->pad4 = config->pad8 = 0;
		for(a = 0; a < 2; a++)
			config->pad7[a] = 0;
		for(a = 0; a < 224; a++)
			config->morepadding[a] = 0;
		for(a = 0; a < 2; a++)
		{
			config->scrollborders[a].MinX = ~0;
			config->scrollborders[a].MaxX = ~0;
			config->scrollborders[a].MinY = ~0;
			config->scrollborders[a].MaxY = ~0;
		}
		config->generalscreenflags = 2;
	}
	if(config->version <= CONFIG_NEW_FUNCTIONS)
	{
		for(a = 0; a < MENUCOUNT; a++)
		{
			Self->LStrCpy(buf2, (STRPTR)&config->menu[a]);
			config->menu[a].name = (STRPTR)getstringcopy(buf2);
			config->menu[a].pad2[0] = 0;
			config->menu[a].pad2[1] = 0;
			config->menu[a].pad2[2] = 0;
		}
	}
	if(config->version <= CONFIG_CHANGE_ARROWS)
		for(a = 0; a < 3; a++)
			config->arrowsize[a] = 8;
	if(config->version <= CONFIG_CHANGE_EXTERN)
	{
		for(a = 0; a < 397; a++)
			config->pad9a[a] = 0;
		config->loadexternal = 0;
	}
	key = NULL;
	if(!(cbuf = (STRPTR)Self->LAllocRemember(&key, size, MEMF_CLEAR)) || !(buf = (STRPTR)Self->LAllocRemember(&key, 4096, MEMF_CLEAR)) || !(tbuf = (STRPTR)Self->LAllocRemember(&key, 256, MEMF_CLEAR)))
	{
		IDOS->Close(in);
		Self->LFreeRemember(&key);
		return (ERROR_NO_FREE_STORE);
	}
	IDOS->Read(in, cbuf, size);
	IDOS->Close(in);
	for(a = 0; a < MENUCOUNT; a++)
	{
		config->menu[a].function = NULL;
		if(config->version > CONFIG_NEW_FUNCTIONS)
			config->menu[a].name = NULL;
	}
	if(config->version > OLD_CONFIG_VERSION)
	{
		for(a = 0; a < DRIVECOUNT; a++)
			config->drive[a].function = NULL;
	}

	pos = 0;
	for(a = 0; a < MENUCOUNT; a++)
	{
		pos = readline(cbuf, pos, buf, size);
		if(buf[0])
			Self->AssignMenu(cstuff, a, (STRPTR)-1, buf);
		if(pos == -1)
			goto endthis;
	}
	if(config->version > CONFIG_NEW_FUNCTIONS)
	{
		for(a = 0; a < MENUCOUNT; a++)
		{
			pos = readline(cbuf, pos, buf, size);
			if(buf[0])
				Self->AssignMenu(cstuff, a, buf, (STRPTR)-1);
			if(pos == -1)
				goto endthis;
		}
	}
	if(config->version > OLD_CONFIG_VERSION)
	{
		for(a = 0; a < DRIVECOUNT; a++)
		{
			pos = readline(cbuf, pos, buf, size);
			if(buf[0])
				AssignDrive(cstuff, a, (STRPTR)-1, buf);
			if(pos == -1)
				goto endthis;
		}
	}
	else
	{
		for(a = 0; a < ARCHIVECOUNT; a++)
			pos = readline(cbuf, pos, buf, size);
	}
	bk = -1;

	for(;;)
	{
		if(pos >= size || pos == -1)
			break;
		if(cbuf[pos] == 5 && cbuf[pos + 1] == 'H')
		{
			pos += 2;
			if((pos + sizeof(struct dopushotkey)) >= size)
				break;
			if((hotkey = IExec->AllocMem(sizeof(struct dopushotkey), MEMF_CLEAR)))
			{
				IExec->CopyMem((STRPTR)&cbuf[pos], (STRPTR)hotkey, sizeof(struct dopushotkey));
				hotkey->func.function = NULL;
				hotkey->next = NULL;
			}
			pos += sizeof(struct dopushotkey);
			if((pos = readline(cbuf, pos, buf, size)) == -1)
				break;
			if(hotkey)
			{
				hotkey->func.function = (STRPTR)getstringcopy(buf);
				if(curhotkey)
					curhotkey->next = hotkey;
				else
					cstuff->firsthotkey = hotkey;
				curhotkey = hotkey;
			}
		}
		else if(cbuf[pos] == 6 && cbuf[pos + 1] == 'F')
		{
			pos += 2;
			if((pos + sizeof(struct wr_dopusfiletype)) >= size)
				break;
			if((newtype = (struct dopusfiletype *) Self->LAllocRemember(&cstuff->typekey, sizeof(struct dopusfiletype), MEMF_CLEAR)))
			{
				IExec->CopyMem((STRPTR)&cbuf[pos], (STRPTR)newtype, sizeof(struct wr_dopusfiletype));
				if(config->version <= CONFIG_CHANGE_FTYPE)
				{
					newtype->type[31] = 0;
					newtype->typeid[0] = 0;
				}
				pos += sizeof(struct wr_dopusfiletype);
				newtype->recognition = NULL;
				for(a = 0; a < FILETYPE_FUNCNUM; a++)
					newtype->function[a] = NULL;
				if((pos =
				    readline(cbuf, pos, buf, size)) == -1)
					break;
				if((newtype->recognition = Self->LAllocRemember(&cstuff->typekey, strlen(buf) + 1, 0)))
					Self->LStrCpy((STRPTR)newtype->recognition, buf);
				for(a = 0; a < FILETYPE_FUNCNUM; a++)
				{
					if((pos = readline(cbuf, pos, buf, size)) == -1)
						break;
					if((newtype->function[a] = Self->LAllocRemember(&cstuff->typekey, strlen(buf) + 1, 0)))
						Self->LStrCpy(newtype->function[a], buf);
				}
				newtype->iconpath = NULL;
				if(config->version > CONFIG_CHANGE_FILETYPE)
				{
					pos = readline(cbuf, pos, buf, size);
					if(buf[0] && (newtype->iconpath = Self->LAllocRemember(&cstuff->typekey, strlen(buf) + 1, 0)))
						Self->LStrCpy(newtype->iconpath, buf);
				}
				linkinnewfiletype(cstuff, newtype);
			}
		}
		else if(cbuf[pos] == 1 && cbuf[pos + 1] == 'F')
		{
			pos += 2;
			if((pos + sizeof(struct olddopusfiletype)) >= size)
				break;
			if((newtype = (struct dopusfiletype *)Self->LAllocRemember(&cstuff->typekey, sizeof(struct dopusfiletype), MEMF_CLEAR)))
			{
				IExec->CopyMem((STRPTR)&cbuf[pos], (STRPTR)&otype, sizeof(struct olddopusfiletype));
				pos += sizeof(struct olddopusfiletype);
				Self->LStrnCpy(newtype->type, otype.type, 32);
				newtype->type[31] = 0;
				newtype->typeid[0] = 0;
				for(a = 0; a < FILETYPE_FUNCNUM; a++)
					newtype->function[a] = NULL;
				for(a = 0; a < 4; a++)
				{
					Self->LStrnCpy(newtype->actionstring[a], otype.actionstring[a], 39);
					newtype->actionstring[a][39] = 0;
					newtype->which[a] = otype.which[a];
					newtype->stack[a] = otype.stack[a];
					newtype->pri[a] = otype.pri[a];
					newtype->delay[a] = otype.delay[a];
				}
				for(a = 4; a < FILETYPE_FUNCNUM; a++)
				{
					newtype->stack[a] = 4000;
					newtype->delay[a] = 2;
				}
				for(a = 0; a < 4; a++)
				{
					if((pos = readline(cbuf, pos, buf, size)) == -1)
						break;
					if((newtype->function[a] = Self->LAllocRemember(&cstuff->typekey, strlen(buf) + 1, MEMF_CLEAR)))
						Self->LStrCpy(newtype->function[a], buf);
				}
				buf[0] = 0;
				if(otype.filepat[0])
					LSprintf(buf, "%lc%s%lc", FTYC_MATCHNAME, otype.filepat, ((otype.recogchars[0]) ? ((otype.and) ? FTYC_AND : FTYC_OR) : FTYC_ENDSECTION));
				if(otype.recogchars[0])
				{
					b = mv = 0;
					for(a = 0; a < 100; a++)
					{
						if(!otype.recogchars[a] || otype.recogchars[a] == ',')
						{
							buf2[b] = 0;
							if(mv == 0)
							{
								if((b = Self->Atoh(buf2, 0)) > 0)
								{
									LSprintf(buf3, "%lc%ld%lc", FTYC_MOVETO, b, FTYC_ENDSECTION);
									Self->StrConcat(buf, buf3, 4096);
								}
								mv = 1;
							}
							else
							{
								LSprintf(buf3, "%lc$%s%lc", FTYC_MATCH, buf2, FTYC_AND);
								Self->StrConcat(buf, buf3, 4096);
								mv = 0;
							}
							if(!otype.recogchars[a])
								break;
							b = 0;
						}
						else
						{
							if(mv == 1 && otype.recogchars[a] == '.')
							{
								buf2[b++] = '?';
								buf2[b++] = '?';
							}
							else
								buf2[b++] = otype.recogchars[a];
						}
					}
					b = strlen(buf);
					if(buf[b - 1] == (char)FTYC_AND)
						buf[b - 1] = 0;
				}
				if((newtype->recognition = Self->LAllocRemember(&cstuff->typekey, strlen(buf) + 1, MEMF_CLEAR)))
					Self->LStrCpy((STRPTR)newtype->recognition, buf);
				linkinnewfiletype(cstuff, newtype);
			}
		}
		else if((cbuf[pos] == 2 || cbuf[pos] == 3) && cbuf[pos + 1] == 'G')
		{
			if(cbuf[pos] == 2)
			{
				a = 0;
				b = sizeof(struct olddopusgadget) * GADCOUNT;
			}
			else
			{
				a = 1;
				b = sizeof(struct newdopusfunction) * GADCOUNT;
			}
			pos += 2;
			if((pos + b) >= size)
				break;
			++bk;
			if(!(temp = IExec->AllocMem(sizeof(struct dopusgadgetbanks), MEMF_CLEAR)))
				goto endthis;
			if(!cstuff->firstbank)
				cstuff->firstbank = temp;
			else
				bank->next = temp;
			bank = temp;
			if(a)
			{
				IExec->CopyMem((STRPTR)&cbuf[pos], (STRPTR)bank->gadgets, sizeof(struct newdopusfunction) * GADCOUNT);
				pos += (sizeof(struct newdopusfunction) * GADCOUNT);
			}
			else
			{
				for(a = 0; a < GADCOUNT; a++)
				{
					IExec->CopyMem((STRPTR)&cbuf[pos], (STRPTR)&bank->gadgets[a], 10);
					IExec->CopyMem((STRPTR)&cbuf[pos + 10], (STRPTR)&bank->gadgets[a].which, 16);
					pos += sizeof(struct olddopusgadget);
				}
			}
			for(gad = 0; gad < GADCOUNT; gad++)
			{
				bank->gadgets[gad].function = NULL;
				buf[0] = 0;
				if(pos > -1)
				{
					pos = readline(cbuf, pos, buf, size);
					Self->AssignGadget(cstuff, bk, gad, (STRPTR)-1, buf);
				}
			}
			if(config->version > CONFIG_NEW_FUNCTIONS)
			{
				for(gad = 0; gad < GADCOUNT; gad++)
				{
					bank->gadgets[gad].name = NULL;
					buf[0] = 0;
					if(pos > -1)
					{
						pos = readline(cbuf, pos, buf, size);
						Self->AssignGadget(cstuff, bk, gad, buf, (STRPTR)-1);
					}
				}
			}
			else
			{
				for(gad = 0; gad < GADCOUNT; gad++)
				{
					Self->LStrCpy(buf, (STRPTR)&bank->gadgets[gad]);
					bank->gadgets[gad].name = (STRPTR)getstringcopy(buf);
					bank->gadgets[gad].pad2[0] = 0;
					bank->gadgets[gad].pad2[1] = 0;
					bank->gadgets[gad].pad2[2] = 0;
				}
			}
			if(pos == -1)
				break;
		}
	}
      endthis:
	Self->LFreeRemember(&key);

	if(config->version < CONFIG_USERAW0_HOTKEY)
	{
		for(a = 0; a < MENUCOUNT; a++)
			if(config->menu[a].key == 0)
				config->menu[a].key = 0xff;
		for(a = 0; a < DRIVECOUNT; a++)
			if(config->drive[a].key == 0)
				config->drive[a].key = 0xff;
		for(bank = cstuff->firstbank; bank; bank = bank->next)
			for(a = 0; a < GADCOUNT; a++)
				if(bank->gadgets[a].key == 0)
					bank->gadgets[a].key = 0xff;

		Self->LStrCpy(config->autodirs[1], config->autodirs[0] + 30);
	}

	cstuff->curbank = cstuff->firstbank;

	return (1);
}

