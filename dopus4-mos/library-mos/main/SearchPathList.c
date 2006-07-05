/*
 *  $VER: SearchPathList.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/SearchPathList ******************************************
*
*   NAME
*      SearchPathList -- Description
*
*   SYNOPSIS
*      int SearchPathList(char * name, char * buffer, int size);
*
*   FUNCTION
*
*   INPUTS
*       name - 
*       buffer - 
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

struct PathList
{
	BPTR nextPath;
	BPTR pathLock;
};

STATIC CONST CONST_STRPTR pathlists[4] = { "Workbench", "Shell Process", "New_WShell", "Background CLI" };

int _DOpus_SearchPathList(char *name, char *buffer, int size)
{
	struct Process *proc, *myproc;
	struct CommandLineInterface *cli;
	struct PathList *wext;
	BPTR lock, lock1;
	APTR wsave;
	int a, pass = 0;

	myproc = (struct Process *)FindTask(0);
	wsave = myproc->pr_WindowPtr;
	myproc->pr_WindowPtr = (APTR)-1;

	if((lock = Lock(name, ACCESS_READ)))
	{
		UnLock(lock);
		myproc->pr_WindowPtr = wsave;
		strcpy(buffer, name);
		return(1);
	}
    
	Forbid();
	proc = myproc;
	for(a = 0; a < 5; a++)
	{
		if(proc)
		{
			if((cli = (struct CommandLineInterface *)BADDR(proc->pr_CLI)))
			{
				for (wext = (struct PathList *)BADDR(cli->cli_CommandDir); wext; wext = (struct PathList *)BADDR(wext->nextPath))
				{
					if ((lock1 = DupLock(wext->pathLock)))
					{
						PathName(lock1, buffer, size);
						UnLock(lock1);
						TackOn(buffer, name, size);
						if((lock = Lock(buffer, ACCESS_READ)))
						{
							pass = 1;
							UnLock(lock);
							break;
						}
					}
				}
			}
		}
		if(pass)
			break;
		if(a < 4)
			proc = (struct Process *)FindTask((char *)pathlists[a]);
	}
	Permit();
	myproc->pr_WindowPtr = wsave;

	return(pass);
}
