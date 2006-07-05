/*
 *  $VER: LFreeRemEntry.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/LFreeRemEntry ******************************************
*
*   NAME
*      LFreeRemEntry -- Description
*
*   SYNOPSIS
*      void LFreeRemEntry(struct DOpusRemember ** key, char * pointer);
*
*   FUNCTION
*
*   INPUTS
*       key - 
*       pointer - 
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

void _DOpus_LFreeRemEntry(APTR *pool, char *pointer)
{
	if (*pool)
	{
		FreeVecPooled(*pool, pointer);
	}
}

