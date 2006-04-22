/*
 *  $VER: LStrCmp.c $Revision$ (01-Feb-2006)
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
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/LStrCmp ******************************************
*
*   NAME
*      LStrCmp -- Description
*
*   SYNOPSIS
*      int LStrCmp(char * s1, char * s2);
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

int _DOpus_LStrCmp(struct DOpusIFace *Self, char *s1, char *s2)
{
	return(Self->LStrnCmp(s1, s2, 0x7fff));
}

