/*
 *  $VER: DoCycleGadget.c $Revision$ (01-Feb-2006)
 *
 *  This file is part of dopus.
 *
 * $Id$
 *
 * $Log$
 *
 *
 */


#include <string.h>
#include <exec/exec.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/DoCycleGadget ******************************************
*
*   NAME
*      DoCycleGadget -- Description
*
*   SYNOPSIS
*      void DoCycleGadget(struct Gadget * gadget, struct Window * window, 
*          char ** choices, int selection);
*
*   FUNCTION
*
*   INPUTS
*       gadget - 
*       window - 
*       choices - 
*       selection - 
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

void _DOpus_DoCycleGadget(struct Gadget *gad, struct Window *window, char **choices, int select)
{
	int op, a;
	struct RastPort *rp;

	rp = window->RPort;
	op = rp->FgPen;
	SetAPen(rp, rp->BgPen);
	RectFill(rp, gad->LeftEdge + 22, gad->TopEdge + 1, gad->LeftEdge + gad->Width - 3, gad->TopEdge + gad->Height - 2);
	SetAPen(rp, op);
	if(choices && choices[select])
	{
		Move(rp, gad->LeftEdge + 20 + ((gad->Width - 22 - ((a = strlen(choices[select])) * rp->Font->tf_XSize)) / 2), gad->TopEdge + rp->Font->tf_Baseline + ((gad->Height - rp->Font->tf_YSize) / 2));
		Text(rp, choices[select], a);
	}
}

