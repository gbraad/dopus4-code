/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 distributed under the GPL may be entitled
to upgrade to the latest versions of Directory Opus at a reduced price.
Please see http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include "config.h"

void SwapMem(char *src, char *dst, int size)
{
	char tmp;

	for(; size; size--)
	{
		tmp = *dst;
		*dst++ = *src;
		*src++ = tmp;
	}
}

/************************** doAllocVec **************************
*
*   NAME
*      doAllocVec -- allocate memory and track the size
*
*   SYNOPSIS
*      void * doAllocVec(uint32 byteSize, uint32 attributes);
*
*   FUNCTION
*      Allocates specified amount of memory with the exec AllocVecTags()
*      function and returns a pointer to the memory allocated or NULL
*      for failure. Attributes are currently ignored. Memory returned is
*      type MEMF_SHARED and cleared to 0.
*
*   INPUTS
*      byteSize - Amount of memory to be allocated in bytes.
*      attributes - Currently ignored.
*
*   RESULT
*      A pointer to memory allocated or NULL for failure.
*
******************************************************************
*/

void *doAllocVec(uint32 byteSize, uint32 attributes)
{
	APTR memory;

	memory = IExec->AllocVecTags(byteSize,
	                             AVT_Type, MEMF_SHARED,
	                             AVT_ClearWithValue, 0,
	                             TAG_END);

	return(memory);
}

