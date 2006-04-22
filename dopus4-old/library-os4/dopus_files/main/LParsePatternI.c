/*
 *  $VER: LParsePatternI.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/LParsePatternI ******************************************
*
*   NAME
*      LParsePatternI -- Description
*
*   SYNOPSIS
*      void LParsePatternI(char * pat, char * parsepat);
*
*   FUNCTION
*
*   INPUTS
*       pat - 
*       parsepat - 
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

int _DOpus_LParsePatternI(struct DOpusIFace *Self, char *pat, char *parsepat)
{
	return(DoMatchPattern(pat, parsepat, 1));
}
