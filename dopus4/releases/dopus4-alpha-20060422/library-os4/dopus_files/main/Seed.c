/*
 *  $VER: Seed.c $Revision$ (01-Feb-2006)
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
#include <clib/alib_protos.h>

/****** dopus/main/Seed ******************************************
*
*   NAME
*      Seed -- Description
*
*   SYNOPSIS
*      void Seed(int seed);
*
*   FUNCTION
*
*   INPUTS
*       seed - 
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

extern ULONG RangeSeed;

void _DOpus_Seed(struct DOpusIFace *Self, int seed)
{
	RangeSeed = FastRand(seed);
}

