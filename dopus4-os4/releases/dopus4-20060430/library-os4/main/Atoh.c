/*
 *  $VER: Atoh.c $Revision$ (01-Feb-2006)
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
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/Atoh ******************************************
*
*   NAME
*      Atoh -- Description
*
*   SYNOPSIS
*      int Atoh(char * str, int len);
*
*   FUNCTION
*
*   INPUTS
*       str - 
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

int _DOpus_Atoh(struct DOpusIFace *Self, char *str, int len)
{
	int a, c, d, e, f;

	c = e = 0;
	for(a = 0; ; a++)
	{
		if(!str[a] || !((str[a] >= '0' && str[a] <= '9') || (str[a] >= 'a' && str[a] <= 'f') || (str[a] >= 'A' && str[a] <= 'F')))
			break;
		if((++e) == len)
			break;
	}
	if(e == 0)
		return(0);
	f = 1;
	for(a = 1; a < e; a++)
		f *= 16;
	for(a = 0; a < e; a++)
	{
		if(str[a] >= '0' && str[a] <= '9')
			d = str[a] - '0';
		else if(str[a] >= 'A' && str[a] <= 'F')
			d = 10 + (str[a] - 'A');
		else if(str[a] >= 'a' && str[a] <= 'f')
			d = 10 + (str[a] - 'a');
		c += (d * f);
		f /= 16;
	}
	return(c);
}
