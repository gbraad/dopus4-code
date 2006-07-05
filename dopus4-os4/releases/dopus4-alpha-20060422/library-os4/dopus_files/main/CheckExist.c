/*
 *  $VER: CheckExist.c $Revision$ (01-Feb-2006)
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
#include <proto/dos.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/CheckExist ******************************************
*
*   NAME
*      CheckExist -- Description
*
*   SYNOPSIS
*      int CheckExist(char * name, int * size);
*
*   FUNCTION
*
*   INPUTS
*       name - 
*       size - 
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

int _DOpus_CheckExist(struct DOpusIFace *Self, char *name, int *size)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *DOSBase = IExec->OpenLibrary("dos.library", 50L);
	struct DOSIFace *IDOS = (struct DOSIFace *)IExec->GetInterface(DOSBase, "main", 1, NULL);
	int a = 0;
	BPTR lock;
	struct Process *myproc;
	APTR wsave;

	myproc = (struct Process *)IExec->FindTask(NULL);
	wsave = myproc->pr_WindowPtr;
	myproc->pr_WindowPtr = (APTR) - 1;
	if((lock = IDOS->Lock(name, ACCESS_READ)))
	{
		struct FileInfoBlock *fib = IDOS->AllocDosObjectTags(DOS_FIB, NULL);
		IDOS->Examine(lock, fib);

		if(size)
			*size = fib->fib_Size;

		a = fib->fib_DirEntryType;

		IDOS->UnLock(lock);
		IDOS->FreeDosObject(DOS_FIB, fib);
	}
	myproc->pr_WindowPtr = wsave;

	return(a);
}
