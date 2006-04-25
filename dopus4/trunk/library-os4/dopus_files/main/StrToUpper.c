/*
 *  $VER: StrToUpper.c $Revision$ (01-Feb-2006)
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
#include <proto/utility.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/StrToUpper ******************************************
*
*   NAME
*      StrToUpper -- Description
*
*   SYNOPSIS
*      void StrToUpper(char * from, char * to);
*
*   FUNCTION
*
*   INPUTS
*       from - 
*       to - 
*
*   RESULT
*       This function does not return a result
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

void _DOpus_StrToUpper(struct DOpusIFace *Self, char *from, char *to)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *UtilityBase = IExec->OpenLibrary("utility.library", 50L);
	struct UtilityIFace *IUtility = (struct UtilityIFace *)IExec->GetInterface(UtilityBase, "main", 1, NULL);

	while((*to++ = IUtility->ToUpper(*from++)));

	IExec->DropInterface((struct Interface *)IUtility);
	IExec->CloseLibrary(UtilityBase);
	return;
}

