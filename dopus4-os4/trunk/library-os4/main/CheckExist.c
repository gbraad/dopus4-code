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
*      int CheckExist(const char * name, int * size);
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

int _DOpus_CheckExist(struct DOpusIFace *Self, const char *name, int *size)
{
	int a = 0;
//	BPTR lock;
	APTR wsave;
	struct ExamineData *data;

	wsave = IDOS->SetProcWindow((APTR)-1L);

	if((data = IDOS->ExamineObjectTags(EX_StringName, name, TAG_END)))
	{
		if(size)
		{
			*size = (int)data->FileSize;
		}
		if(EXD_IS_FILE(data))
		{
			a = -1;
		}
		else if (EXD_IS_DIRECTORY(data))
		{
			a = 1;
		}
		IDOS->FreeDosObject(DOS_EXAMINEDATA,data);
	}

/*	if((lock = IDOS->Lock(name, ACCESS_READ)))
	{
		struct FileInfoBlock *fib = IDOS->AllocDosObjectTags(DOS_FIB, NULL);
		IDOS->Examine(lock, fib);

		if(size)
			*size = fib->fib_Size;

		a = fib->fib_DirEntryType;

		IDOS->UnLock(lock);
		IDOS->FreeDosObject(DOS_FIB, fib);
	}*/

	IDOS->SetProcWindow(wsave);

	return(a);
}
