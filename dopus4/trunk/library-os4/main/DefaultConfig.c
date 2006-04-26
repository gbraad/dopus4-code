/*
 *  $VER: DefaultConfig.c $Revision$ (01-Feb-2006)
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
#include <dos/dos.h>
#include <dos/datetime.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

#include "extras.h"
#include "DefaultConfig.h"

/****** dopus/main/DefaultConfig ******************************************
*
*   NAME
*      DefaultConfig -- Description
*
*   SYNOPSIS
*      int DefaultConfig(struct ConfigStuff * cstuff);
*
*   FUNCTION
*
*   INPUTS
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

int _DOpus_DefaultConfig(struct DOpusIFace *Self, struct ConfigStuff *cstuff)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	int a, b, h, i;
	struct Config *config;
	struct dopusgadgetbanks *firstbank;
	struct dopusfiletype *type;
	char name[256];

	if(!(config = cstuff->config))
		return (0);
	Self->FreeConfig(cstuff);

	if((Self->FindSystemFile("DirectoryOpus.DefCFG", name, 256, SYSFILE_DATA)) && (Self->ReadConfig(name, cstuff)) == 1)
		return (1);

	/* Operation */
	config->copyflags = COPY_DATE | COPY_PROT | COPY_NOTE;
	config->dateformat = DATE_DOS | DATE_SUBST | DATE_12HOUR;
	config->existflags = REPLACE_ASK;
	config->deleteflags = DELETE_ASK;
	config->errorflags = ERROR_ENABLE_DOS | ERROR_ENABLE_OPUS;
	config->generalflags = GENERAL_DRAG | GENERAL_DOUBLECLICK | GENERAL_ACTIVATE;
	config->iconflags = ICONFLAG_MAKEDIRICON | ICONFLAG_DOUNTOICONS;

	for(h = 0; h < 2; h++)
	{
		for(i = 0; i < 5; i++)
			config->displaypos[h][i] = i;
		for(i = 5; i < 16; i++)
			config->displaypos[h][i] = -1;
		config->displaylength[h][DISPLAY_NAME] = 32 * 8;
		config->displaylength[h][DISPLAY_COMMENT] = 40 * 8;
		config->displaylength[h][DISPLAY_FILETYPE] = 16 * 8;
		config->displaylength[h][DISPLAY_OWNER] = 16 * 8;
		config->displaylength[h][DISPLAY_GROUP] = 16 * 8;
		config->sortmethod[h] = 0;
		config->separatemethod[h] = 1;
	}
	config->sortflags = 0;
	config->formatflags = 0;

	config->dynamicflags = UPDATE_FREE | UPDATE_SCROLL | UPDATE_LEFTJUSTIFY | UPDATE_PROGRESSINDICATOR | UPDATE_REDRAW;

	/* System */
	Self->LStrCpy(config->outputcmd, "NewCLI");
	Self->LStrCpy(config->output, "CON:20/10/600/180/Directory Opus Output/CLOSE");
	Self->LStrCpy(config->shellstartup, "Shell-Startup");
	config->priority = 0;

	config->icontype = ICON_MEMORY | ICON_DATE | ICON_TIME;
	config->scrclktype = SCRCLOCK_MEMORY | SCRCLOCK_DATE | SCRCLOCK_TIME;

	config->bufcount = 10;
	config->dirflags = DIRFLAGS_FINDEMPTY | DIRFLAGS_REREADOLD | DIRFLAGS_SMARTPARENT | DIRFLAGS_CHECKBUFS | DIRFLAGS_AUTODISKC | DIRFLAGS_AUTODISKL;
	config->showfree = SHOWFREE_KILO;

	config->hotkeycode = (USHORT) ~ 0;
	config->hotkeyqual = IEQUALIFIER_CONTROL | IEQUALIFIER_LSHIFT | IEQUALIFIER_LALT;
	config->hotkeyflags = 0;

	config->toolicon[0] = 0;
	config->projecticon[0] = 0;
	config->drawericon[0] = 0;
	config->defaulttool[0] = 0;
	config->addiconflags = 0;

	config->loadexternal = 0;
	config->language[0] = 0;

	config->hiddenbit = 0;
	config->showpat[0] = 0;
	config->hidepat[0] = 0;
	config->showpatparsed[0] = 0;
	config->hidepatparsed[0] = 0;


	config->autodirs[0][0] = 0;
	config->autodirs[1][0] = 0;
	config->startupscript[0] = 0;
	config->uniconscript[0] = 0;
	config->configreturnscript[0] = 0;

	config->viewbits = VIEWBITS_INWINDOW | VIEWBITS_SHOWBLACK | VIEWBITS_TEXTBORDERS;
	config->showdelay = 0;
	config->fadetime = 2;
	config->tabsize = 8;

	/* Screen */
	for(a = 0; a < 3; a++)
	{
		config->arrowpos[a] = 2;
		config->arrowsize[a] = 8;
	}
	config->sliderwidth = 10;
	config->sliderheight = 7;
	config->stringheight = 8;

	config->statusfg = 1;
	config->statusbg = 0;
	config->disknameselfg = 2;
	config->disknameselbg = 7;
	config->disknamefg = 1;
	config->disknamebg = 0;
	config->dirsselfg = 2;
	config->dirsselbg = 3;
	config->dirsfg = 3;
	config->dirsbg = 0;
	config->filesselfg = 2;
	config->filesselbg = 1;
	config->filesfg = 1;
	config->filesbg = 0;
	config->slidercol = 1;
	config->sliderbgcol = 0;
	config->arrowfg = 1;
	config->arrowbg = 0;
	config->littlegadfg = 1;
	config->littlegadbg = 0;
	config->clockfg = 1;
	config->clockbg = 0;
	config->requestfg = 1;
	config->requestbg = 0;
	config->gadgettopcol = 2;
	config->gadgetbotcol = 1;
	config->stringfgcol = 1;
	config->stringbgcol = 0;
	config->stringselfgcol = 1;
	config->stringselbgcol = 4;

	for(i = 0; i < NUMFONTS; i++)
	{
		config->fontsizes[i] = 8;
		Self->LStrCpy(config->fontbufs[i], "topaz.font");
	}

	config->generalscreenflags = SCR_GENERAL_TINYGADS | SCR_GENERAL_INDICATERMB | SCR_GENERAL_REQDRAG | SCR_GENERAL_WINBORDERS;

	IExec->CopyMem((char *)defaultpalette, (char *)config->new_palette, sizeof(defaultpalette));

//	config->screenmode = MODE_WORKBENCHCLONE;
	config->screenmode = MODE_WORKBENCHUSE;
	config->scrdepth = 3;
	config->scrw = -1;
	config->scrh = -1;
	config->screenflags = 0;

	config->scr_winx = 0;
	config->scr_winy = 0;
	config->scr_winw = 800;//-1;
	config->scr_winh = 600;//-1;

	/* Misc Stuff */
	config->gadgetrows = 6;

	config->iconx = 252;
	config->icony = 0;
	config->wbwinx = 0;
	config->wbwiny = 0;
	config->iconbutx = 100;
	config->iconbuty = 40;
	config->config_x = -1;
	config->config_y = -1;

	for(a = 0; a < 2; a++)
	{
		config->scrollborders[a].MinX = ~0;
		config->scrollborders[a].MaxX = ~0;
		config->scrollborders[a].MinY = ~0;
		config->scrollborders[a].MaxY = ~0;
	}

	config->windowdelta = 0;
	config->slider_pos = 1;

	/* Buttons */
	for(i = 0; i < 42; i++)
		Self->AssignGadget(cstuff, 0, i, defgads[i], deffuncs[i]);
	for(i = 0; i < 42; i++)
		Self->AssignGadget(cstuff, 0, i + 42, revgads[i], revfuncs[i]);
	for(i = 78; i < GADCOUNT; i++)
		Self->AssignGadget(cstuff, 0, i, NULL, NULL);

	if((firstbank = cstuff->firstbank))
	{
		for(i = 0; i < GADCOUNT; i++)
		{
			firstbank->gadgets[i].which = 0;
			firstbank->gadgets[i].type = 0;
			firstbank->gadgets[i].stack = 4000;
			firstbank->gadgets[i].pri = 0;
			firstbank->gadgets[i].delay = 2;
			if(i < 42)
			{
				firstbank->gadgets[i].key = default_gadflags[i].code;
				firstbank->gadgets[i].qual = default_gadflags[i].qual;
				firstbank->gadgets[i].fpen = default_gadflags[i].fpen;
				firstbank->gadgets[i].bpen = default_gadflags[i].bpen;
			}
			else
			{
				firstbank->gadgets[i].fpen = 1;
				firstbank->gadgets[i].bpen = 0;
				if(i == 57)
				{
					firstbank->gadgets[i].key = 0x28;
					firstbank->gadgets[i].qual = IEQUALIFIER_CONTROL;
				}
				else if(i == 68)
				{
					firstbank->gadgets[i].key = 0x17;
					firstbank->gadgets[i].qual = IEQUALIFIER_CONTROL | IEQUALIFIER_LSHIFT;
				}
				else
				{
					firstbank->gadgets[i].key = 0xff;
					firstbank->gadgets[i].qual = 0;
				}
			}
		}
		firstbank->gadgets[34].which = FLAG_ASYNC | FLAG_CDSOURCE;
		firstbank->gadgets[76].which = FLAG_ASYNC | FLAG_CDSOURCE;
	}
	cstuff->curbank = firstbank;

	/* Menus */
	Self->LStrCpy(config->menutit[0], "Project");
	Self->LStrCpy(config->menutit[1], "Function");
	for(i = 2; i < 5; i++)
		config->menutit[i][0] = 0;

	for(i = 0; i < 9; i++)
	{
		Self->AssignMenu(cstuff, i, defmenus1[i], deffuncs1[i]);
		config->menu[i].key = defmenkeys1[i];
		config->menu[i].qual = defmenqual1[i];
	}
	for(i = 20; i < 29; i++)
	{
		h = i - 20;
		Self->AssignMenu(cstuff, i, defmenus2[h], deffuncs2[h]);
		config->menu[i].key = defmenkeys2[h];
		config->menu[i].qual = defmenqual2[h];
	}
	for(i = 26; i < 29; i++)
	{
		config->menu[i].which = FLAG_OUTWIND | FLAG_CDSOURCE | FLAG_DOALL | FLAG_SCANDEST | ((i == 27) ? 0 : FLAG_RECURSE);
		config->menu[i].delay = -1;
	}

	/* Drives */
	Self->GetDevices(cstuff);

	for(i = 0; i < DRIVECOUNT; i++)
	{
		config->drive[i].key = 0xff;
		config->drive[i].qual = 0;
		config->drive[i].fpen = 1;
		config->drive[i].bpen = 0;
	}

	/* Filetypes */
	for(i = 0; i < 5; i++)
	{
		if((type = (struct dopusfiletype *)Self->LAllocRemember(&cstuff->typekey, sizeof(struct dopusfiletype), MEMF_CLEAR)))
		{
			Self->LStrCpy(type->type, deftype_type[i]);
			Self->LStrCpy(type->typeid, deftype_typeid[i]);
			if((type->recognition = Self->LAllocRemember(&cstuff->typekey, strlen(deftype_recog[i]) + 1, 0)))
				Self->LStrCpy(type->recognition, deftype_recog[i]);
			for(a = 0; a < 4; a++)
			{
				if(deftype_funcs[i][a])
				{
					b = deftype_funcpos[i][a];
					Self->LStrCpy(type->actionstring[b], deftype_action[i][a]);
					type->which[b] = deftype_which[i][a];
					type->delay[b] = deftype_delay[i][a];
					type->stack[b] = 4000;
					if(type->function[b] = Self->LAllocRemember(&cstuff->typekey, strlen(deftype_funcs[i][a]) + 1, 0))
						Self->LStrCpy(type->function[b], deftype_funcs[i][a]);
				}
			}
			linkinnewfiletype(cstuff, type);
		}
	}
	return (1);
}
