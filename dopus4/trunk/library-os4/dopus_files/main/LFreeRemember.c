/*
 *  $VER: LFreeRemember.c $Revision$ (01-Feb-2006)
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

struct DOpusPool
{
	APTR fast;
	APTR chip;
};

void _DOpus_LFreeRemember(struct DOpusIFace *Self, struct DOpusRemember **key)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct DOpusPool *pool = (struct DOpusPool *)(*key);
	if(*key)
	{
		if(pool->fast)
			IExec->FreeSysObject(ASOT_MEMPOOL, pool->fast);
//			IExec->DeletePool(pool->fast);
//		if(pool->chip)
//			IExec->DeletePool(pool->chip);
		IExec->FreeMem(*key, sizeof(struct DOpusPool));
		*key = NULL;
	}
}

