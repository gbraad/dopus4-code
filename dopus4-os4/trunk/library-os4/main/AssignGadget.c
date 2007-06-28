/*
 *  $VER: AssignGadget.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/AssignGadget ******************************************
*
*   NAME
*      AssignGadget -- Description
*
*   SYNOPSIS
*      int AssignGadget(struct ConfigStuff * cstuff, int banknum, 
*          int gadnum, char * name, char * func);
*
*   FUNCTION
*
*   INPUTS
*       cstuff - 
*       banknum - 
*       gadnum - 
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

void _DOpus_AssignGadget(struct DOpusIFace *Self, struct ConfigStuff *cstuff, int bk, int gad, char *name, char *func)
{
	struct dopusgadgetbanks *bank, *temp;
	int a;

	bank = cstuff->firstbank;
	for(a = 0; a < bk; a++)
	{
		if(!bank || !bank->next)
			break;
		bank = bank->next;
	}
	if(a < bk || !bank)
	{
		if(!(temp = IExec->AllocMem(sizeof(struct dopusgadgetbanks), MEMF_CLEAR)))
			return;
		if(bank)
			bank->next = temp;
		else
			cstuff->firstbank = temp;
		bank = temp;
	}

	if(name != (char *)-1)
	{
		freestring(bank->gadgets[gad].name);
		bank->gadgets[gad].name = NULL;
		if(name && name[0])
			bank->gadgets[gad].name = (char *)getstringcopy(name);
	}
	if(func != (char *)-1)
	{
		freestring(bank->gadgets[gad].function);
		bank->gadgets[gad].function = NULL;
		if(func && func[0])
			bank->gadgets[gad].function = (char *)getstringcopy(func);
	}
}

