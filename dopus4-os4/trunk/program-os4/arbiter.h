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

#ifndef DOPUS_ARBITER
#define DOPUS_ARBITER

#ifdef __GNUC__
	#ifdef __PPC__
		#pragma pack(2)
	#endif
#elif defined(__VBCC__)
	#pragma amiga-align
#endif

struct ArbiterMessage
{
	struct Message msg;
	uint32 command;
	int32 current;
	int32 total;
	int64 file_current;
	int64 file_total;
	APTR data;
	char copy_file_indicator;
};

enum
{
	ARBITER_REMOVE,

	ARBITER_PROGRESS_OPEN,
	ARBITER_PROGRESS_CLOSE,
	ARBITER_PROGRESS_INCREASE,
	ARBITER_PROGRESS_UPDATE
};

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack()
   #endif
#elif defined(__VBCC__)
   #pragma default-align
#endif

#endif /* DOPUS_ARBITER */

