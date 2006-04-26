/*
 *  $VER: Assign.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/Assign ******************************************
*
*   NAME
*      Assign -- Description
*
*   SYNOPSIS
*      int Assign(char * name, char * dir);
*
*   FUNCTION
*
*   INPUTS
*       name - 
*       dir - 
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

int _DOpus_Assign(struct DOpusIFace *Self, char *name, char *dir)
{
	BPTR lock;
	char nname[FILEBUF_SIZE];
	int len,found;

	Self->LStrCpy(nname,name);
	if((len = strlen(nname)) == 0)
		return(ASSIGN_NODEV);
	if(nname[len - 1] == ':')
		nname[len - 1] = 0;

	found = IDOS->AssignLock(nname, 0L) ? ASSIGN_OK : ASSIGN_NODEV;

	if(!dir || !dir[0])
		return(found);

	if((lock = IDOS->Lock(dir, ACCESS_READ)))
	{
		if(IDOS->AssignLock(nname, lock))
			return(ASSIGN_OK);

		return(ASSIGN_NODEV);
	}
	return(ASSIGN_NODEV);
}
