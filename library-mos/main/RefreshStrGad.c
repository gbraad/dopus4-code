/*
 *  $VER: RefreshStrGad.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/RefreshStrGad ******************************************
*
*   NAME
*      RefreshStrGad -- Description
*
*   SYNOPSIS
*      void RefreshStrGad(struct Gadget * gadget, struct Window * window);
*
*   FUNCTION
*
*   INPUTS
*       gadget - 
*       window - 
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

void _DOpus_RefreshStrGad(struct Gadget *gad, struct Window *win)
{
	((struct StringInfo *)(gad->SpecialInfo))->BufferPos = 0;

	gad->Flags |= GFLG_GADGHNONE;
	RefreshGList(gad, win, NULL, 1);
	gad->Flags &= ~GFLG_GADGHNONE;

	return;
}

