/*
 *  $VER: Random.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/Random ******************************************
*
*   NAME
*      Random -- Description
*
*   SYNOPSIS
*      int Random(int limit);
*
*   FUNCTION
*
*   INPUTS
*       limit - 
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

int _DOpus_Random(struct DOpusIFace *Self, int limit)
{
	return(RangeRand(limit+1));
}

