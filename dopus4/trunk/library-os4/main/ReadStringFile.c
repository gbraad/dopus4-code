/*
 *  $VER: ReadStringFile.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/ReadStringFile ******************************************
*
*   NAME
*      ReadStringFile -- Description
*
*   SYNOPSIS
*      int ReadStringFile(struct StringData * stringdata, char * filename);
*
*   FUNCTION
*
*   INPUTS
*       stringdata - 
*       filename - 
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

int _DOpus_ReadStringFile(struct DOpusIFace *Self, struct StringData *stringdata, char *filename)
{
	struct Library *LocaleBase = IExec->OpenLibrary("locale.library", 50L);
	struct LocaleIFace *ILocale = (struct LocaleIFace *)IExec->GetInterface(LocaleBase, "main", 1, NULL);
	int a;
	struct DefaultString *defstr;

	if(!stringdata)
		return (0);
	if(!stringdata->string_table)
	{
		if(!(stringdata->string_table = IExec->AllocMem(stringdata->string_count * 4, MEMF_CLEAR)))
			return (0);
	}

	if(stringdata->string_buffer)
	{
		IExec->FreeMem(stringdata->string_buffer, stringdata->string_size);
		stringdata->string_buffer = NULL;
	}
	defstr = stringdata->default_table;

	for(a = 0; a < stringdata->string_count; a++)
	{
		if(!defstr[a].string)
			break;
		stringdata->string_table[defstr[a].string_id] = defstr[a].string;
	}
	if(filename)
	{
		if(filename[0])
		{
			if((stringdata->catalog = ILocale->OpenCatalogA(NULL, filename, NULL /*OC_Language,NULL,TAG_END */)))
			{
				for(a = 0;
				    a < stringdata->string_count; a++)
				{
					if(!defstr[a].string)
						break;
					stringdata->string_table[defstr[a].string_id] = ILocale->GetCatalogStr(stringdata->catalog, defstr[a].string_id, defstr[a].string);
				}
			}
		}
	}

	IExec->DropInterface((struct Interface *)ILocale);
	IExec->CloseLibrary(LocaleBase);
	return (1);
}

