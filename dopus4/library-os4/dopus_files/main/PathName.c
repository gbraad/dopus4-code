/*
 *  $VER: PathName.c $Revision$ (01-Feb-2006)
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


#include <string.h>
#include <exec/exec.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/PathName ******************************************
*
*   NAME
*      PathName -- Description
*
*   SYNOPSIS
*      int PathName(BPTR lock, char * buffer, int len);
*
*   FUNCTION
*
*   INPUTS
*       lock - 
*       buffer - 
*       len - 
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

struct PathList2
{
	struct PathList2 *last;
	char path[FILEBUF_SIZE];
};

int _DOpus_PathName(struct DOpusIFace *Self, BPTR lock, char *buf, int len)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *DOSBase = IExec->OpenLibrary("dos.library", 50L);
	struct DOSIFace *IDOS = (struct DOSIFace *)IExec->GetInterface(DOSBase, "main", 1, NULL);
	struct PathList2 *list = NULL, *temp, *first = NULL;
	struct FileInfoBlock *finfo = IDOS->AllocDosObjectTags(DOS_FIB, NULL);
	BPTR templock;
	struct DOpusRemember *key = NULL;
	int a, it;

	buf[0] = it = 0;
	do
	{
		IDOS->Examine(lock, finfo);
		if(!(temp = (struct PathList2 *)Self->LAllocRemember(&key, sizeof(struct PathList2), MEMF_CLEAR)))
			break;
		if(!first)
			first = temp;
		temp->last = list;
		list = temp;
		Self->LStrCpy(list->path, finfo->fib_FileName);
		templock = lock;
		lock = IDOS->ParentDir(lock);
		if(it)
			IDOS->UnLock(templock);
		++it;
	}
	while(lock);

	a = 0;

	while(list)
	{
 
		Self->StrConcat(buf, list->path, len);
		if(a == 0)
			Self->StrConcat(buf, ":", len);
		else if(list->last)
			Self->StrConcat(buf, "/", len);
		list = list->last;
		++a;
	}
	Self->LFreeRemember(&key);
	IDOS->FreeDosObject(DOS_FIB, finfo);
	return((int)strlen(buf));
}
