/*
 *  $VER: SearchPathList.c $Revision$ (01-Feb-2006)
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

const static char *pathlists[7] = { "Workbench", "Initial CLI", "Shell Process", "New CLI", "AmigaShell", "New_WShell", "Background CLI" };

int _DOpus_SearchPathList(struct DOpusIFace *Self, char *name, char *buffer, int size)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *DOSBase = IExec->OpenLibrary("dos.library", 50L);
	struct DOSIFace *IDOS = (struct DOSIFace *)IExec->GetInterface(DOSBase, "main", 1, NULL);
	struct Process *proc, *myproc;
	struct CommandLineInterface *cli;
	struct PathList *wext;
	BPTR lock, lock1;
	APTR wsave;
	int a, pass = 0;

	myproc = (struct Process *)IExec->FindTask(0);
	wsave = myproc->pr_WindowPtr;
	myproc->pr_WindowPtr = (APTR)-1;

	if((lock = IDOS->Lock(name, ACCESS_READ)))
	{
		IDOS->UnLock(lock);
		myproc->pr_WindowPtr = wsave;
		Self->LStrCpy(buffer, name);
		return(1);
	}
    
	IExec->Forbid();
	proc = myproc;
	for(a = 0; a < 8; a++)
	{
		if(proc)
		{
			if((cli = (struct CommandLineInterface *)BADDR(proc->pr_CLI)))
			{
				for (wext = (struct PathList *)BADDR(cli->cli_CommandDir); wext; wext = (struct PathList *)BADDR(wext->nextPath))
				{
					if ((lock1 = IDOS->DupLock(wext->pathLock)))
					{
						Self->PathName(lock1, buffer, size);
						IDOS->UnLock(lock1);
						Self->TackOn(buffer, name, size);
						if((lock = IDOS->Lock(buffer, ACCESS_READ)))
						{
							pass = 1;
							IDOS->UnLock(lock);
							break;
						}
					}
				}
			}
		}
		if(pass)
			break;
		if(a < 7)
			proc = (struct Process *)IExec->FindTask((char *)pathlists[a]);
	}
	IExec->Permit();
	myproc->pr_WindowPtr = wsave;
	return(pass);
}
