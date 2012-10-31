/****************************************************************

   This file was created automatically by `FlexCat 2.4'
   from "../catalogs/DOpus4_Icon.cd".

   Do NOT edit by hand!

****************************************************************/

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

#ifndef ICON_STRINGS
#define ICON_STRINGS

enum {
    STR_REMAP_COLORS=0,
    STR_SAVE=1,
    STR_SKIP=2,
    STR_CANCEL=3,
    STR_DELETE=4,
    STR_NEW=5,
    STR_TOOLTYPES=6,
    STR_COMMENT=7,
    STR_DEFAULT_TOOL=8,
    STR_VALIDATING=9,
    STR_WRITE_PROTECTED=10,
    STR_READ_WRITE=11,
    STR_SCRIPT=12,
    STR_ARCHIVED=13,
    STR_READABLE=14,
    STR_WRITABLE=15,
    STR_EXECUTABLE=16,
    STR_DELETABLE=17,
    STR_BLOCKS=18,
    STR_USED=19,
    STR_FREE=20,
    STR_BLOCK_SIZE=21,
    STR_CREATED=22,
    STR_LAST_CHANGED=23,
    STR_BYTES=24,
    STR_STACK=25,
    STR_DISK=26,
    STR_DRAWER=27,
    STR_TOOL=28,
    STR_PROJECT=29,
    STR_TRASH=30,
    STR_DEVICE=31,
    STR_KICK=32,
    STR_APPICON=33,
    STR_ICON=34,

    STR_STRING_COUNT};

enum {
  PROTECT_S = 0, PROTECT_A, PROTECT_R, PROTECT_W, PROTECT_E, PROTECT_D,
  KEY_SKIP, KEY_CANCEL, KEY_SAVE, KEY_NEW, KEY_DELETE, KEY_COMMENT,
  KEY_REMAP, KEY_STACK, KEY_DEFTOOL
};

extern char **string_table;

#endif
