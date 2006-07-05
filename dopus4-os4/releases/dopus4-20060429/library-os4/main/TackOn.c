/*
 *  $VER: TackOn.c $Revision$ (01-Feb-2006)
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
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/TackOn ******************************************
*
*   NAME
*      TackOn -- Description
*
*   SYNOPSIS
*      int TackOn(char * path, char * name, int len);
*
*   FUNCTION
*
*   INPUTS
*       path - 
*       name - 
*       len - 
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

int _DOpus_TackOn(struct DOpusIFace *Self, char *path, char *file, int len)
{
	int a;

	a = strlen(path) - 1;
	if((a >= 0) && (path[a] != ':') && (path[a] != '/'))
		Self->StrConcat(path, "/", len);
	if(file)
		Self->StrConcat(path, file, len);
	return((int)strlen(path));
}
