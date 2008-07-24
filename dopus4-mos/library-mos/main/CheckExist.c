/*
 *  $VER: CheckExist.c $Revision$ (01-Feb-2006)
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

int _DOpus_CheckExist(char *name, int *size)
{
	int a = 0;
	BPTR lock;
	struct Process *myproc;
	APTR wsave;

	myproc = (struct Process *)FindTask(NULL);
	wsave = myproc->pr_WindowPtr;
	myproc->pr_WindowPtr = (APTR) - 1;
	if((lock = Lock(name, ACCESS_READ)))
	{
		struct FileInfoBlock fib;

		if (SysBase->LibNode.lib_Version >= 51)
		{
			Examine64(lock, &fib, NULL);
		}
		else
		{
			Examine(lock, &fib);
			fib.fib_Size64 = fib.fib_Size;
		}

		if(size)
			*size = fib.fib_Size64;

		a = fib.fib_DirEntryType;

		UnLock(lock);
	}
	myproc->pr_WindowPtr = wsave;

	return(a);
}
