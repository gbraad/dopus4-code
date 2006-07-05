/*
 *  $VER: ReadStringFile.c $Revision$ (01-Feb-2006)
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

int _DOpus_ReadStringFile(struct StringData *stringdata, char *filename)
{
	struct Library *LocaleBase = OpenLibrary("locale.library", 38);
	int a;
	struct DefaultString *defstr;

	if(!stringdata)
		return (0);
	if(!stringdata->string_table)
	{
		if(!(stringdata->string_table = AllocMem(stringdata->string_count * 4, MEMF_CLEAR)))
			return (0);
	}

	if(stringdata->string_buffer)
	{
		FreeMem(stringdata->string_buffer, stringdata->string_size);
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
			if((stringdata->catalog = OpenCatalogA(NULL, filename, NULL /*OC_Language,NULL,TAG_END */)))
			{
				for(a = 0;
				    a < stringdata->string_count; a++)
				{
					if(!defstr[a].string)
						break;
					stringdata->string_table[defstr[a].string_id] = GetCatalogStr(stringdata->catalog, defstr[a].string_id, defstr[a].string);
				}
			}
		}
	}

	CloseLibrary(LocaleBase);
	return (1);
}

