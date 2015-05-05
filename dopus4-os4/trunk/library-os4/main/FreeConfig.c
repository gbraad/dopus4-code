/*
 *  $VER: FreeConfig.c $Revision$ (01-Feb-2006)
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


#include <exec/exec.h>
#include <proto/exec.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

#include "extras.h"

/****** dopus/main/FreeConfig ******************************************
*
*   NAME
*      FreeConfig -- Description
*
*   SYNOPSIS
*      int FreeConfig(struct ConfigStuff * cstuff);
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

void _DOpus_FreeConfig(struct DOpusIFace *Self, struct ConfigStuff *cstuff)
{
	int a;
	struct Config *config;
	struct dopusgadgetbanks *bank, *temp;
	struct dopushotkey *hotkey, *temphot;

	if(!(config = cstuff->config))
		return;

	for(a = 0; a < MENUCOUNT; a++)
	{
		Self->AssignMenu(cstuff, a, NULL, NULL);
		config->menu[a].which = 0;
		config->menu[a].type = 0;
		config->menu[a].stack = 8192;
		config->menu[a].pri = 0;
		config->menu[a].delay = 2;
		config->menu[a].fpen = 1;
		config->menu[a].bpen = 2;
		config->menu[a].key = 0;
		config->menu[a].qual = 0;
	}
	for(a = 0; a < DRIVECOUNT; a++)
	{
		AssignDrive(cstuff, a, NULL, NULL);
		config->drive[a].key = 0xff;
		config->drive[a].qual = 0;
		config->drive[a].fpen = 3;
		config->drive[a].bpen = 0;
	}

	bank = cstuff->firstbank;
	while (bank)
	{
		for(a = 0; a < GADCOUNT; a++)
		{
			freestring(bank->gadgets[a].name);
			freestring(bank->gadgets[a].function);
		}
		temp = bank->next;
		IExec->FreeVec(bank);
		bank = temp;
	}
	cstuff->firstbank = cstuff->curbank = NULL;

	hotkey = cstuff->firsthotkey;
	while (hotkey)
	{
		temphot = hotkey->next;
		freestring(hotkey->func.function);
		IExec->FreeVec(hotkey);
		hotkey = temphot;
	}
	cstuff->firsthotkey = NULL;

	Self->LFreeRemember(&cstuff->typekey);
	cstuff->firsttype = NULL;
}

