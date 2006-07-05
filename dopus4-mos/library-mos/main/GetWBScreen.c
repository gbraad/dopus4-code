/*
 *  $VER: GetWBScreen.c $Revision$ (01-Feb-2006)
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
#include <proto/intuition.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/GetWBScreen ******************************************
*
*   NAME
*      GetWBScreen -- Description
*
*   SYNOPSIS
*      void GetWBScreen(struct Screen * scrbuf);
*
*   FUNCTION
*
*   INPUTS
*       scrbuf - 
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

void _DOpus_GetWBScreen(struct Screen *scrbuf)
{
	struct Screen *scr = NULL;

	if((scr = LockPubScreen(NULL)))
	{
		CopyMem(scr, scrbuf, sizeof(struct Screen));
		UnlockPubScreen(NULL, scr);
	}

	return;
}

