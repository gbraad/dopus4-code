/*
 *  $VER: LMatchPattern.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/LMatchPattern ******************************************
*
*   NAME
*      LMatchPattern -- Description
*
*   SYNOPSIS
*      int LMatchPattern(char * parsepat, char * str);
*
*   FUNCTION
*
*   INPUTS
*       parsepat - 
*       str - 
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

int _DOpus_LMatchPattern(struct DOpusIFace *Self, char *parsepat, char *str)
{
	return(DoMatchPattern(parsepat, str, 0));
}
