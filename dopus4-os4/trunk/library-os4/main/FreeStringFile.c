/*
 *  $VER: FreeStringFile.c $Revision$ (01-Feb-2006)
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
#include <proto/locale.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/FreeStringFile ******************************************
*
*   NAME
*      FreeStringFile -- Description
*
*   SYNOPSIS
*      int FreeStringFile(struct StringData * stringdata);
*
*   FUNCTION
*
*   INPUTS
*       stringdata - 
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

void _DOpus_FreeStringFile(struct DOpusIFace *Self, struct StringData *stringdata)
{
	struct Library *LocaleBase = IExec->OpenLibrary("locale.library", 50L);
	struct LocaleIFace *ILocale = (struct LocaleIFace *)IExec->GetInterface(LocaleBase, "main", 1, NULL);
	if(stringdata)
	{
		if(stringdata->string_buffer)
		{
			IExec->FreeVec(stringdata->string_buffer);
			stringdata->string_buffer = NULL;
		}
		if(stringdata->string_table)
		{
			IExec->FreeVec(stringdata->string_table);
			stringdata->string_table = NULL;
		}

		ILocale->CloseCatalog(stringdata->catalog);
		stringdata->catalog = NULL;
		stringdata->LocaleBase = NULL;
	}

	IExec->DropInterface((struct Interface *)ILocale);
	IExec->CloseLibrary(LocaleBase);
	return;
}

