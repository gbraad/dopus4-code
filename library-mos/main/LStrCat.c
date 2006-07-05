/*
 *  $VER: LStrCat.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/LStrCat ******************************************
*
*   NAME
*      LStrCat -- Description
*
*   SYNOPSIS
*      void LStrCat(char * s1, char * s2);
*
*   FUNCTION
*
*   INPUTS
*       s1 - 
*       s2 - 
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

void _DOpus_LStrCat(char *s1, char *s2)
{
	LStrnCat(s1, s2, 0xffff);
}

