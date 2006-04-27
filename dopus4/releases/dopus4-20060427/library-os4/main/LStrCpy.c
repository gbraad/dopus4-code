/*
 *  $VER: LStrCpy.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/LStrCpy ******************************************
*
*   NAME
*      LStrCpy -- Description
*
*   SYNOPSIS
*      void LStrCpy(char * to, char * from);
*
*   FUNCTION
*
*   INPUTS
*       to - 
*       from - 
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

void _DOpus_LStrCpy(struct DOpusIFace *Self, char *to, char *from)
{
	while((*to++ = *from++));
}

