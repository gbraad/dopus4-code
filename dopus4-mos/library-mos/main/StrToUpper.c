/*
 *  $VER: StrToUpper.c $Revision$ (01-Feb-2006)
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

void _DOpus_StrToUpper(char *from, char *to)
{
	while((*to++ = ToUpper(*from++)));
	return;
}

