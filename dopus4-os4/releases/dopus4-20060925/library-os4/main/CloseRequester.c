/*
 *  $VER: CloseRequester.c $Revision$ (01-Feb-2006)
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
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/CloseRequester ******************************************
*
*   NAME
*      CloseRequester -- Description
*
*   SYNOPSIS
*      void CloseRequester(struct RequesterBase * reqbase);
*
*   FUNCTION
*
*   INPUTS
*       reqbase - 
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

void _DOpus_CloseRequester(struct DOpusIFace *Self, struct RequesterBase *reqbase)
{
	if(reqbase->rb_window)
	{
		IIntuition->CloseWindow(reqbase->rb_window);
		reqbase->rb_window = NULL;
	}
	if((reqbase->rb_privateflags & RBP_CLOSEFONT) && reqbase->rb_font)
		IGraphics->CloseFont(reqbase->rb_font);

	Self->LFreeRemember(&reqbase->rb_memory);
}
