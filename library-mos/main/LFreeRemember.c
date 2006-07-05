/*
 *  $VER: LFreeRemember.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/LFreeRemember ******************************************
*
*   NAME
*      LFreeRemember -- Description
*
*   SYNOPSIS
*      void LFreeRemember(struct DOpusRemember ** key);
*
*   FUNCTION
*
*   INPUTS
*       key - 
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

void _DOpus_LFreeRemember(APTR *pool)
{
	if (*pool)
	{
		DeletePool(*pool);
		*pool = NULL;
	}
}

