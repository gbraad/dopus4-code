/*
 *  $VER: StrToLower.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/StrToLower ******************************************
*
*   NAME
*      StrToLower -- Description
*
*   SYNOPSIS
*      void StrToLower(char * from, char * to);
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

void _DOpus_StrToLower(struct DOpusIFace *Self, char *from, char *to)
{
	if(from != NULL)
	{
		while((*to++ = IUtility->ToLower(*from++)));
	}
	else
	{
		*to = '\0';
	}
	return;
}

