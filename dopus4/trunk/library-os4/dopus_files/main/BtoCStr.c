/*
 *  $VER: BtoCStr.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/BtoCStr ******************************************
*
*   NAME
*      BtoCStr -- Description
*
*   SYNOPSIS
*      void BtoCStr(BSTR bstr, char * cstr, int len);
*
*   FUNCTION
*
*   INPUTS
*       bstr - 
*       cstr - 
*       len - 
*
*   RESULT
*       This function does not return a result
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

void _DOpus_BtoCStr(struct DOpusIFace *Self, BSTR bstr, char *cstr, int len)
{
	char *c = (char *)(bstr<<2);
	short i = 0;

	len--;
	i = *((UBYTE *)c);
	c++;
	if(i < len)
		len = i;
	for(i = 0; i < len; i++)
		*(cstr++)=*(c++);
	*cstr=0;

}

