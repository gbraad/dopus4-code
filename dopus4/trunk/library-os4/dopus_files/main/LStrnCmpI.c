/*
 *  $VER: LStrnCmpI.c $Revision$ (01-Feb-2006)
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
#include <proto/utility.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/LStrnCmpI ******************************************
*
*   NAME
*      LStrnCmpI -- Description
*
*   SYNOPSIS
*      int LStrnCmpI(char * s1, char * s2, int len);
*
*   FUNCTION
*
*   INPUTS
*       s1 - 
*       s2 - 
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

int _DOpus_LStrnCmpI(struct DOpusIFace *Self, char *s1, char *s2, int len)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *UtilityBase = IExec->OpenLibrary("utility.library", 50L);
	struct UtilityIFace *IUtility = (struct UtilityIFace *)IExec->GetInterface(UtilityBase, "main", 1, NULL);
	char c1, c2, diff;

	while(1)
	{
		c1 = IUtility->ToUpper(*s1++);
		c2 = IUtility->ToUpper(*s2++);

		diff = c1 - c2;

		if(diff > 0)
			return 1;
		else if(diff < 0)
			return -1;
		else if((c1 == 0) || (len == 0))
			break;
		len--;
	}
	return 0;
}

