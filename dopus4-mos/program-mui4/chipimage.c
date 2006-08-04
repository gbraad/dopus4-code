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

 All users of Directory Opus 4 (including versions distributed
 under the GPL) are entitled to upgrade to the latest version of
 Directory Opus version 5 at a reduced price. Please see
 http://www.gpsoft.com.au for more information.

 The release of Directory Opus 4 under the GPL in NO WAY affects
 the existing commercial status of Directory Opus 5.
*/

#include <exec/types.h>

UWORD uniconifydata[] =
{
	0x3f80, 0xa0a0, 0x2080, 0xa0a0, 0x3f80, 0x8020, 0x0000, 0x8020,
	0x0000, 0xaaa0,	0x0000, 0x1f00, 0x1f00, 0x1f00, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000,	0xffe0, 0x8020, 0xffe0, 0x8020,
	0x8020, 0x8020, 0x8020, 0x8020, 0x8020, 0xffe0,	0x0000, 0x7fc0,
	0x0000, 0x7fc0, 0x7fc0, 0x7fc0, 0x7fc0, 0x7fc0, 0x7fc0, 0x0000
};

UWORD pageflip_data1[] =
{
	0x4000, 0xa000, 0x9000, 0x8800, 0xfc00
};

UWORD pageflip_data2[] =
{
	0x4000, 0x6000, 0x7000
};

UWORD null_pointer[] =
{
	0, 0, 0, 0, 0, 0
};

BYTE beepwave[] =
{
	0, 30, 60, 95, 127, 95, 60, 30, 0, -30, -60, -95, -127, -95, -60, -30
};
