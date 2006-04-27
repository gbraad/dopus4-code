/*
 *  $VER: GetCheckImage.c $Revision$ (01-Feb-2006)
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
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

#include "extras.h"

/****** dopus/main/GetCheckImage ******************************************
*
*   NAME
*      GetCheckImage -- Description
*
*   SYNOPSIS
*      struct Image * GetCheckImage(UBYTE fg, UBYTE bg, int pen, 
*          struct DOpusRemember ** key);
*
*   FUNCTION
*
*   INPUTS
*       fg - 
*       bg - 
*       pen - 
*       key - 
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

extern USHORT pdb_check[];

struct Image * _DOpus_GetCheckImage(struct DOpusIFace *Self, UBYTE fg, UBYTE bg, int pen, struct DOpusRemember **key)
{
	struct Image *image;
	int a, b, depth;
	struct BitMap tbm;
	struct RastPort trp;

	b = (fg > bg) ? fg : bg;
	depth = 2;
	for(a = 0; a < 8; a++)
		if(b & (1 << a))
			depth = a + 1;

	if(!(image = (struct Image *)get_image_data(key, 13, 7, depth, &tbm, &trp)))
		return(NULL);

	IGraphics->SetDrMd(&trp, JAM1);
	IGraphics->SetAPen(&trp, bg);
	IGraphics->RectFill(&trp, 0, 0, 12, 6);
	if(pen)
	{
		IGraphics->SetAPen(&trp, fg);
		IGraphics->BltTemplate(/*(char *)DOpusBase->*/pdb_check, 0, 2, &trp, 0, 0, 13, 7);
	}

	image->LeftEdge = 7;
	image->TopEdge = 2;

	return(image);
}
