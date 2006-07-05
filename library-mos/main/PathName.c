/*
 *  $VER: PathName.c $Revision$ (01-Feb-2006)
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

int _DOpus_PathName(BPTR lock, char *buf, int len)
{
	struct PathList2 *list = NULL, *temp, *first = NULL;
	struct FileInfoBlock finfo;
	BPTR templock;
	struct DOpusRemember *key = NULL;
	int a, it;

	buf[0] = it = 0;
	do
	{
		Examine(lock, &finfo);
		if(!(temp = (struct PathList2 *)LAllocRemember(&key, sizeof(struct PathList2), MEMF_CLEAR)))
			break;
		if(!first)
			first = temp;
		temp->last = list;
		list = temp;
		strcpy(list->path, finfo.fib_FileName);
		templock = lock;
		lock = ParentDir(lock);
		if(it)
			UnLock(templock);
		++it;
	}
	while(lock);

	a = 0;

	while(list)
	{
 
		StrConcat(buf, list->path, len);
		if(a == 0)
			StrConcat(buf, ":", len);
		else if(list->last)
			StrConcat(buf, "/", len);
		list = list->last;
		++a;
	}
	LFreeRemember(&key);

	return((int)strlen(buf));
}
