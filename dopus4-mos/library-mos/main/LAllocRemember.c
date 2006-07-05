/*
 *  $VER: LAllocRemember.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/LAllocRemember ******************************************
*
*   NAME
*      LAllocRemember -- Description
*
*   SYNOPSIS
*      void * LAllocRemember(struct DOpusRemember ** key, ULONG size, 
*          ULONG type);
*
*   FUNCTION
*
*   INPUTS
*       key - 
*       size - 
*       type - 
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

void * _DOpus_LAllocRemember(APTR *pool, ULONG size, ULONG type)
{
	ULONG *mem;

	if(*pool == NULL)
		*pool = CreatePool(type | MEMF_CLEAR, 2048, 1024);

	if(*pool)
	{
		if((mem = AllocVecPooled(*pool, size)))
		{
			return(mem);
		}
	}

	return(0);
}

