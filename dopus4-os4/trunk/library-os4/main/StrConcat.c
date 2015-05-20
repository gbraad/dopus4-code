/*
 *  $VER: StrConcat.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/StrConcat ******************************************
*
*   NAME
*      StrConcat -- Description
*
*   SYNOPSIS
*      int StrConcat(char * s1, char * s2, int len);
*
*   FUNCTION
*
*   INPUTS
*       s1 - 
*       s2 - 
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

int _DOpus_StrConcat(struct DOpusIFace *Self, char *s1, char *s2, int len)
{
	int a, b;

	a = strlen(s2);
	b = strlen(s1);
	--len;

	if(a + b < len)
	{
		strlcpy(&s1[b], s2, a);
		s1[b + a] = 0;
		return(1);
	}

	if(len > b)
		strlcpy(&s1[b], s2, len - b);

	s1[len] = 0;
	return(0);
}

