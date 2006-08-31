/*
 *  $VER: AssignMenu.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/AssignMenu ******************************************
*
*   NAME
*      AssignMenu -- Description
*
*   SYNOPSIS
*      int AssignMenu(struct ConfigStuff * cstuff, int itemnum, char * name, 
*          char * func);
*
*   FUNCTION
*
*   INPUTS
*       cstuff - 
*       itemnum - 
*       name - 
*       func - 
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

void _DOpus_AssignMenu(struct DOpusIFace *Self, struct ConfigStuff *cstuff, int men, char *name, char *func)
{
	struct Config *config;

	if(!(config = cstuff->config))
		return;
	if(name != (char *)-1)
	{
		freestring(config->menu[men].name);
		config->menu[men].name = NULL;
		if(name && name[0])
			config->menu[men].name = (char *)getstringcopy(name);
	}
	if(func != (char *)-1)
	{
		freestring(config->menu[men].function);
		config->menu[men].function = NULL;
		if(func && func[0])
			config->menu[men].function = (char *)getstringcopy(func);
	}
}

