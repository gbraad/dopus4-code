/*
 *  $VER: SaveConfig.c $Revision$ (01-Feb-2006)
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
#include <proto/dos.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/SaveConfig ******************************************
*
*   NAME
*      SaveConfig -- Description
*
*   SYNOPSIS
*      int SaveConfig(char * name, struct ConfigStuff * cstuff);
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

int _DOpus_SaveConfig(struct DOpusIFace *Self, char *name, struct ConfigStuff *cstuff)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *DOSBase = IExec->OpenLibrary("dos.library", 50L);
	struct DOSIFace *IDOS = (struct DOSIFace *)IExec->GetInterface(DOSBase, "main", 1, NULL);
	int a, out, ret = 0;
	struct dopusfiletype *type;
	struct dopushotkey *hotkey;
	struct dopusgadgetbanks *bank;
	struct Config *config;

	if(!(config = cstuff->config))
		return (0);

	config->version = CONFIG_VERSION;
	config->magic = CONFIG_MAGIC;

	if(!(out = IDOS->Open(name, MODE_NEWFILE)))
		return (FALSE);
	if((IDOS->Write(out, (char *)config, sizeof(struct Config))) < sizeof(struct Config))
	{
		IDOS->Close(out);
		return (0);
	}
	for(a = 0; a < MENUCOUNT; a++)
		if(!writestring(out, config->menu[a].function))
			goto error;
	for(a = 0; a < MENUCOUNT; a++)
		if(!writestring(out, config->menu[a].name))
			goto error;
	for(a = 0; a < DRIVECOUNT; a++)
		if(!writestring(out, config->drive[a].function))
			goto error;

	type = cstuff->firsttype;
	while(type)
	{
		if((IDOS->Write(out, "\006F", 2)) < 2)
			goto error;
		if((IDOS->Write(out, (char *)type, sizeof(struct wr_dopusfiletype))) < sizeof(struct wr_dopusfiletype))
			goto error;
		if(!writestring(out, type->recognition))
			goto error;
		for(a = 0; a < FILETYPE_FUNCNUM; a++)
			if(!writestring(out, type->function[a]))
				goto error;
		if(!writestring(out, type->iconpath))
			goto error;
		type = type->next;
	}
	bank = cstuff->firstbank;
	while (bank)
	{
		if((IDOS->Write(out, "\003G", 2)) < 2)
			goto error;
		if((IDOS->Write(out, (char *)bank->gadgets, sizeof(struct dopusfunction) * GADCOUNT)) < sizeof(struct dopusfunction) * GADCOUNT)
			goto error;
		for(a = 0; a < GADCOUNT; a++)
			if(!writestring(out, bank->gadgets[a].function))
				goto error;
		for(a = 0; a < GADCOUNT; a++)
			if(!writestring(out, bank->gadgets[a].name))
				goto error;
		bank = bank->next;
	}
	hotkey = cstuff->firsthotkey;
	while (hotkey)
	{
		if((IDOS->Write(out, "\005H", 2)) < 2)
			goto error;
		if((IDOS->Write(out, (char *)hotkey, sizeof(struct dopushotkey))) < sizeof(struct dopushotkey))
			goto error;
		if(!writestring(out, hotkey->func.function))
			goto error;
		hotkey = hotkey->next;
	}
	ret = 1;
      error:
	IDOS->Close(out);
	return (ret);
}

