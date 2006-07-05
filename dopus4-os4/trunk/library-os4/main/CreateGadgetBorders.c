/*
 *  $VER: CreateGadgetBorders.c $Revision$ (01-Feb-2006)
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

#include "extras.h"

/****** dopus/main/CreateGadgetBorders ******************************************
*
*   NAME
*      CreateGadgetBorders -- Description
*
*   SYNOPSIS
*      void CreateGadgetBorders(struct DOpusRemember ** key, int w, int h, 
*          struct Border ** selborder, struct Border ** unselborder, 
*          int dogear, int shine, int shadow);
*
*   FUNCTION
*
*   INPUTS
*       key - 
*       w - 
*       h - 
*       selborder - 
*       unselborder - 
*       dogear - 
*       shine - 
*       shadow - 
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

void _DOpus_CreateGadgetBorders(struct DOpusIFace *Self, struct DOpusRemember **key, int w, int h, struct Border **selborder, struct Border **unselborder, int dogear, int shine, int shadow)
{
	short *shine_data, *shadow_data;

	*selborder = NULL;
	*unselborder = NULL;

	if((shine_data = Self->LAllocRemember(key, 10 * sizeof(short), MEMF_CLEAR)) && (shadow_data = Self->LAllocRemember(key, 10 * sizeof(short), MEMF_CLEAR)))
	{
		fill_out_border_data(shine_data, shadow_data, w - 1, h - 1);
		if(dogear)
		{
			shine_data[0] = w - 6;
			shadow_data[5] = 4;
			shadow_data[7] = 4;
		}

		if ((*selborder = Self->LAllocRemember(key, sizeof(struct Border) * 2, MEMF_CLEAR)) && (*unselborder = Self->LAllocRemember(key, sizeof(struct Border) * 2, MEMF_CLEAR)))
		{
			fill_out_border(*selborder, shadow, shine, shine_data, shadow_data);
			fill_out_border(*unselborder, shine, shadow, shine_data, shadow_data);
		}
	}
	return;
}
