/*
 *  $VER: StrCombine.c $Revision$ (01-Feb-2006)
 *
 *  This file is part of dopus.
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

/****** dopus/main/StrCombine ******************************************
*
*   NAME
*      StrCombine -- Description
*
*   SYNOPSIS
*      int StrCombine(char * s1, char * s2, char * s3, int len);
*
*   FUNCTION
*
*   INPUTS
*       s1 - 
*       s2 - 
*       s3 - 
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

int _DOpus_StrCombine(char *s1, char *s2, char *s3, int len)
{
	int a;

	a = strlen(s2);
	if(a >= len)
		a = len - 1;

	LStrnCpy(s1, s2, a);
	s1[a] = 0;

	return(StrConcat(s1, s3, len));
}

