/*
 *  $VER: LStrCmpI.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/LStrCmpI ******************************************
*
*   NAME
*      LStrCmpI -- Description
*
*   SYNOPSIS
*      int LStrCmpI(char * s1, char * s2);
*
*   FUNCTION
*
*   INPUTS
*       s1 - 
*       s2 - 
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

int _DOpus_LStrCmpI(char *s1, char *s2)
{
	return(LStrnCmpI(s1, s2, 0x7fff));
}

