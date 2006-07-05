/*
 *  $VER: LStrnCpy.c $Revision$ (01-Feb-2006)
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
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/LStrnCpy ******************************************
*
*   NAME
*      LStrnCpy -- Description
*
*   SYNOPSIS
*      void LStrnCpy(char * to, char * from, int len);
*
*   FUNCTION
*
*   INPUTS
*       to - 
*       from - 
*       len - 
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

void _DOpus_LStrnCpy(char *to, char *from, int len)
{
	char c;

	while (len-- >= 0)
	{
		c = *from++;
		*to++ = c;
		if(!c)
			break;
	}
	*(--to) = 0;
}

