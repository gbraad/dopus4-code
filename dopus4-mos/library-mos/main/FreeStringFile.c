/*
 *  $VER: FreeStringFile.c $Revision$ (01-Feb-2006)
 *
 *  This file is part of dopus.
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

void _DOpus_FreeStringFile(struct StringData *stringdata)
{
	struct Library *LocaleBase = OpenLibrary("locale.library", 38);
	if(stringdata)
	{
		if(stringdata->string_buffer)
		{
			FreeMem(stringdata->string_buffer, stringdata->string_size);
			stringdata->string_buffer = NULL;
		}
		if(stringdata->string_table)
		{
			FreeMem(stringdata->string_table, stringdata->string_count * 4);
			stringdata->string_table = NULL;
		}

		CloseCatalog(stringdata->catalog);
		stringdata->catalog = NULL;
		stringdata->LocaleBase = NULL;
	}

	CloseLibrary(LocaleBase);
	return;
}

