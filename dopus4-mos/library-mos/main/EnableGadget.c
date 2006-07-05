/*
 *  $VER: EnableGadget.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/EnableGadget ******************************************
*
*   NAME
*      EnableGadget -- Description
*
*   SYNOPSIS
*      void EnableGadget(struct Gadget * gad, struct RastPort * rp, 
*          int xoff, int yoff);
*
*   FUNCTION
*
*   INPUTS
*       gad - 
*       rp - 
*       xoff - 
*       yoff - 
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

void _DOpus_EnableGadget(struct Gadget *gad, struct RastPort *rp, int xoff, int yoff)
{
	if(gad->Flags & GFLG_DISABLED)
	{
		gad->Flags &= ~GFLG_DISABLED;
		GhostGadget(gad, rp, xoff, yoff);
	}
}

