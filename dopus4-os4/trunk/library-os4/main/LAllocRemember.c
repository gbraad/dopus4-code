/*
 *  $VER: LAllocRemember.c $Revision$ (01-Feb-2006)
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

struct DOpusPool
{
	APTR fast;
	APTR chip;
};

void * _DOpus_LAllocRemember(struct DOpusIFace *Self, struct DOpusRemember **key, ULONG size, ULONG type)
{
	APTR *pool;
	ULONG *mem;

	if(*key == NULL)
	{
		*key = IExec->AllocVecTags(sizeof(struct DOpusPool),
		                           AVT_Type, MEMF_SHARED,
		                           AVT_ClearWithValue, 0, TAG_END);
//		*key = IExec->AllocMem(sizeof(struct DOpusPool), MEMF_ANY|MEMF_CLEAR);
		if(*key == NULL)
		{
			return (0);
		}
	}
	pool = &((struct DOpusPool *)*key)->fast;

	if(*pool == NULL)
		*pool = IExec->AllocSysObjectTags(ASOT_MEMPOOL, ASOPOOL_MFlags, type | MEMF_CLEAR, ASOPOOL_Puddle, 1024, ASOPOOL_Threshold, 1024, TAG_DONE);

	size += sizeof(ULONG);

	if(*pool)
	{
		if((mem = IExec->AllocPooled(*pool, size)))
		{
			mem[0] = size;
			return(mem+1);
		}
	}

	return(0);
}

