/*
 *  $VER: LFreeRemEntry.c $Revision$ (01-Feb-2006)
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

struct DOpusPool
{
	APTR fast;
	APTR chip;
};

void _DOpus_LFreeRemEntry(struct DOpusIFace *Self, struct DOpusRemember **key, char *pointer)
{
	struct DOpusPool *pool;
	ULONG *ptr = (ULONG *)pointer;

	if((*key) == NULL)
		return;
	if(IExec->TypeOfMem(pointer) & MEMF_CHIP)
	{
		pool = ((struct DOpusPool *)(*key))->chip;
	}
	else
	{
		pool = ((struct DOpusPool *)(*key))->fast;
	}

	if(pool)
	{
		IExec->FreePooled(pool, ptr - 1, *(ptr - 1));
	}
}

