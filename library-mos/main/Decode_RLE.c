/*
 *  $VER: Decode_RLE.c $Revision$ (01-Feb-2006)
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
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/Decode_RLE ******************************************
*
*   NAME
*      Decode_RLE -- Description
*
*   SYNOPSIS
*      void Decode_RLE(char * source, char * dest, int size);
*
*   FUNCTION
*
*   INPUTS
*       source - 
*       dest - 
*       size - 
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

void _DOpus_Decode_RLE(char *source, char *dest, int size)
{
	int a;
	BYTE copy, count;

	for(a = 0; a < size;)
	{
		if((count = source[a++]) >= 0)
		{
			copy = count + 1;
			while(copy--)
				*dest++ = source[a++];
		}
		else if(count != -128)
		{
			copy = 1 - count;
			while(copy--)
				*dest++ = source[a];
			++a;
		}
	}
}
