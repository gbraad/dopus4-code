/*
 *  $VER: LStrnCat.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/LStrnCat ******************************************
*
*   NAME
*      LStrnCat -- Description
*
*   SYNOPSIS
*      void LStrnCat(char * s1, char * s2, int len);
*
*   FUNCTION
*
*   INPUTS
*       s1 - 
*       s2 - 
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

void _DOpus_LStrnCat(struct DOpusIFace *Self, char *s1, char *s2, int len)
{
	while (*s1++);
	for(s1--;len;len--)
	{
		*s1++ = *s2++;
		if (!(*s2))
			break;
	}
	*s1=0;
}

