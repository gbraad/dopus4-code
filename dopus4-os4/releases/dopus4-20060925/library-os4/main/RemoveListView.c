/*
 *  $VER: RemoveListView.c $Revision$ (01-Feb-2006)
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
#include <proto/intuition.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/RemoveListView ******************************************
*
*   NAME
*      RemoveListView -- Description
*
*   SYNOPSIS
*      int RemoveListView(struct DOpusListView * listview, int count);
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

int _DOpus_RemoveListView(struct DOpusIFace *Self, struct DOpusListView *view, int count)
{
	int realcount = 0;

	while(view && realcount < count)
	{
		if(view->gadadd)
			IIntuition->RemoveGList(view->window, view->listgads, view->gadadd);
		Self->LFreeRemember(&view->key);
		view = view->next;
		++realcount;
	}

	return(realcount);
}
