/*
 *  $VER: RefreshListView.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/RefreshListView ******************************************
*
*   NAME
*      RefreshListView -- Description
*
*   SYNOPSIS
*      int RefreshListView(struct DOpusListView * listview, int count);
*
*   FUNCTION
*
*   INPUTS
*       listview - 
*       count - 
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

int _DOpus_RefreshListView(struct DOpusIFace *Self, struct DOpusListView *view, int count)
{
	int realcount = 0;

	while (view && realcount < count)
	{
		if(view->items)
			for(view->count = 0; view->items[view->count] && (!(view->flags & DLVF_ENDNL) || view->items[view->count][0]); view->count++);
		else
			view->count = 0;
		if(view->selectarray)
		{
			if(view->selected = (char *)Self->LAllocRemember(&view->key, view->count, MEMF_CLEAR))
				IExec->CopyMem(view->selectarray, view->selected, view->count);
		}
		if(view->topitem > (view->count - view->lines))
			view->topitem = view->count - view->lines;
		if(view->topitem < 0)
			view->topitem = 0;
		view->chk = view->itemselected;
		Self->FixSliderBody(view->window, &view->listgads[0], view->count, view->lines, 0);
		Self->FixSliderPot(view->window, &view->listgads[0], view->topitem, view->count, view->lines, 2);
		view->oldoffset = -1;
		DisplayView(Self, view);
//		DisplayView(view);
		view = view->next;
		++realcount;
	}
	return (realcount);
}

