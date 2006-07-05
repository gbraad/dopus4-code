/*
 *  $VER: GetButtonImage.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/GetButtonImage ******************************************
*
*   NAME
*      GetButtonImage -- Description
*
*   SYNOPSIS
*      struct Image * GetButtonImage(int w, int h, int fg, int bg, int fpen, 
*          int bpen, struct DOpusRemember ** key);
*
*   FUNCTION
*
*   INPUTS
*       w - 
*       h - 
*       fg - 
*       bg - 
*       fpen - 
*       bpen - 
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

struct Image * _DOpus_GetButtonImage(int w, int h, int fg, int bg, int fpen, int bpen, struct DOpusRemember **key)
{
	struct Image *image;
	short a, b, depth;
	struct BitMap tbm;
	struct RastPort trp;

	b = (fg > bg) ? fg : bg;
	if(fpen > b)
		b = fpen;
	if(bpen > b)
		b = bpen;

	depth = 2;
	w += 4;
	for(a = 0; a < 8; a++)
		if(b & (1 << a))
			depth = a + 1;

	if(!(image = get_image_data(key, w, h, depth, &tbm, &trp)))
		return(NULL);

	SetAPen(&trp, bpen);
	RectFill(&trp, 0, 0, w - 1, h - 1);
	DrawRadioButton(&trp, 2, 1, w - 4, h, (fpen != bpen) ? bg : fg, (fpen != bpen) ? fg : bg);
	if((w > 10) && (h > 4) && (fpen != bpen))
	{
		SetAPen(&trp, fpen);
		RectFill(&trp, 4, 2, w - 7, h - 3);
		SetAPen(&trp, bpen);
		WritePixel(&trp, 4, 2);
		WritePixel(&trp, 4, h - 3);
		WritePixel(&trp, w - 7, 2);
		WritePixel(&trp, w - 7, h - 3);
	}

	image->LeftEdge = -2;
	image->TopEdge = -1;

	return(image);
}
