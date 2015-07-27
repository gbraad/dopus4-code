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

#include "dopus.h"

USHORT projecticon_data[] =
{
	0xffff, 0xffff, 0xfff8, 0x0000, 0xd555, 0x5555, 0x5556, 0x0000,
	0xd555, 0x5555, 0x5555, 0x8000, 0xdfff, 0xffff, 0x5555, 0x6000,
	0xd000, 0x0001, 0x5555, 0x5000, 0xd000, 0xc001, 0x5555, 0x5000,
	0xd001, 0xe001,	0x5555, 0x5000, 0xd003, 0x3001, 0x5555, 0x5000,
	0xd006, 0x1801, 0x7fff, 0x5000, 0xd00c, 0x0c01,	0x7fff, 0x5000,
	0xd018, 0x0601, 0x7fff, 0x5000,	0xd03f, 0xff01, 0x000f, 0x5000,
	0xd000, 0x0001,	0x7fff, 0x5000, 0xd000, 0x0001, 0x01ff, 0x5000,
	0xdfff, 0xffff, 0x7fff, 0x5000, 0xd555, 0x55e0,	0x0003, 0x5000,
	0xd555, 0x55ff, 0xffff, 0x5000,	0xd555, 0x55e0, 0x003f, 0x5000,
	0x3555, 0x55ff,	0xffff, 0x5000, 0x0d55, 0x55ff, 0xffff, 0x5000,
	0x0355, 0x5555, 0x5555, 0x5000, 0x0000, 0x0000,	0x0000, 0x0000,
	0x0000, 0x0000, 0x0004, 0x0000,	0x0000, 0x0000, 0x0001, 0x0000,
	0x0000, 0x0000,	0x0000, 0x4000, 0x0000, 0x0000, 0x0000, 0x1000,
	0x0000, 0x0000, 0x0000, 0x0800, 0x0000, 0xe000,	0x0000, 0x0c00,
	0x0001, 0xf000, 0x0000, 0x0c00,	0x0003, 0xb800, 0x0000, 0x0c00,
	0x0007, 0x1c00,	0x0000, 0x0c00, 0x000e, 0x0e00, 0x0000, 0x0c00,
	0x001c, 0x0700, 0x0000, 0x0c00, 0x003f, 0xff80,	0x7ff0, 0x0c00,
	0x001f, 0xff80, 0x0000, 0x0c00,	0x0000, 0x0000, 0x7e00, 0x0c00,
	0x0000, 0x0000,	0x0000, 0x0c00, 0x0000, 0x001f, 0xfffc, 0x0c00,
	0x0000, 0x0000, 0x0000, 0x0c00, 0x0000, 0x001f,	0xffc0, 0x0c00,
	0x4000, 0x0000, 0x0000, 0x0c00,	0x1000, 0x0000, 0x0000, 0x0c00,
	0x0400, 0x0000,	0x0000, 0x0c00, 0x01ff, 0xffff, 0xffff, 0xfc00
}
, toolicon_data[] =
{
	0xffff, 0xffff, 0xffff, 0xf800, 0xd555, 0x5555, 0x5555, 0x5000,
	0xd555, 0x5555, 0x5555, 0x5000, 0xd555, 0x5555,	0x5555, 0x5000,
	0xd555, 0x5555, 0x5555, 0x5000,	0xd555, 0x5555, 0x5555, 0x5000,
	0xd405, 0x5000,	0x1555, 0x5000, 0xd405, 0x4000, 0x0155, 0x5000,
	0xd400, 0x0000, 0x0055, 0x5000, 0xd400, 0x0000,	0x0015, 0x5000,
	0xd400, 0x0000, 0x0005, 0x5000,	0xd400, 0x0000, 0x0005, 0x5000,
	0xd400, 0x0000,	0x0001, 0x5000, 0xd405, 0x4000, 0x1541, 0x5000,
	0xd405, 0x4000, 0x1550, 0x5000, 0xd555, 0x5000,	0x5554, 0x5000,
	0xd555, 0x5401, 0x5555, 0x5000,	0xd555, 0x5401, 0x5555, 0x5000,
	0xd555, 0x5401,	0x5555, 0x5000, 0xd555, 0x5401, 0x5555, 0x5000,
	0xd555, 0x5555, 0x5555, 0x5000, 0x8000, 0x0000,	0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0400,	0x0000, 0x0000, 0x0000, 0x0c00,
	0x0000, 0x0000,	0x0000, 0x0c00, 0x0000, 0x0000, 0x0000, 0x0c00,
	0x0000, 0x0000, 0x0000, 0x0c00, 0x0000, 0x0000,	0x0000, 0x0c00,
	0x03f0, 0x0fff, 0xe000, 0x0c00,	0x0208, 0x3000, 0x1c00, 0x0c00,
	0x0207, 0xc000,	0x0380, 0x0c00, 0x0200, 0x0000, 0x0060, 0x0c00,
	0x0200, 0x0000, 0x0010, 0x0c00, 0x0200, 0x0000, 0x0008, 0x0c00,
	0x0207, 0xc000, 0x1fc4, 0x0c00, 0x0208, 0x2000, 0x2032, 0x0c00,
	0x03f0, 0x1800, 0xc00d, 0x0c00, 0x0000, 0x0603, 0x0003, 0x0c00,
	0x0000, 0x0202, 0x0000, 0x0c00, 0x0000, 0x0202, 0x0000, 0x0c00,
	0x0000, 0x0202, 0x0000, 0x0c00, 0x0000, 0x03fe, 0x0000, 0x0c00,
	0x0000, 0x0000, 0x0000, 0x0c00, 0x7fff, 0xffff, 0xffff, 0xfc00
}
, drawericon_1_data[] =
{
	0x3fff, 0xffff, 0xffff, 0xf800, 0x3000, 0x0000, 0x0000, 0x1800,
	0x33ff, 0xffff, 0xffff, 0xf800, 0x37ff, 0xffff,	0xffff, 0xd800,
	0x37ff, 0xf600, 0xdfff, 0xd800,	0x37ff, 0xf62a, 0xdfff, 0xd800,
	0x37ff, 0xf7ff,	0xdfff, 0xd800, 0x37ff, 0xf800, 0x3fff, 0xd800,
	0x37ff, 0xffff, 0xffff, 0xd800, 0x3600, 0x0000,	0x0000, 0x1800,
	0x3fff, 0xffff, 0xffff, 0xf800,	0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000,	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0fff, 0xffff,	0xffff, 0xe400,
	0x0c00, 0x0000, 0x0000, 0x0400,	0x0800, 0x0000, 0x0000, 0x2400,
	0x0800, 0x09ff, 0x2000, 0x2400, 0x0800, 0x09d5, 0x2000, 0x2400,
	0x0800, 0x0800, 0x2000, 0x2400, 0x0800, 0x07ff, 0xc000, 0x2400,
	0x0800, 0x0000, 0x0000, 0x2400, 0x09ff, 0xffff, 0xffff, 0xe400,
	0x0000, 0x0000, 0x0000, 0x0400, 0x0fff, 0xffff, 0xffff, 0xfc00,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
}
, drawericon_2_data[] =
{
	0x3fff, 0xffff, 0xffff, 0xf800, 0x3000, 0x0000, 0x0000, 0x1800,
	0x22aa, 0xaaaa, 0xaaaa, 0xa800, 0x0555, 0x5555,	0x5555, 0x5000,
	0x0000, 0x0000, 0x0000, 0x0000,	0x3fff, 0xffff, 0xffff, 0xf800,
	0x3fff, 0xffff,	0xffff, 0xf800, 0x3fff, 0xf600, 0xdfff, 0xf800,
	0x3fff, 0xf62a, 0xdfff, 0xf800, 0x3fff, 0xf7ff,	0xdfff, 0xf800,
	0x3fff, 0xf800, 0x3fff, 0xf800,	0x3fff, 0xffff, 0xffff, 0xf800,
	0x0000, 0x0000,	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0fff, 0xffff,	0xffff, 0xe400,
	0x1aaa, 0xaaaa, 0xaaaa, 0xb400,	0x3555, 0x5555, 0x5555, 0x5c00,
	0x7fff, 0xffff, 0xffff, 0xfc00, 0x4000, 0x0000, 0x0000, 0x0400,
	0x4000, 0x0000, 0x0000, 0x0600, 0x4000, 0x09ff, 0x2000, 0x0600,
	0x4000, 0x09d5, 0x2000, 0x0680, 0x4000, 0x0800, 0x2000, 0x0680,
	0x4000, 0x07ff, 0xc000, 0x0680, 0x4000, 0x0000, 0x0000, 0x0680,
	0x7fff, 0xffff, 0xffff, 0xfe80, 0x02aa, 0xaaaa, 0xaaaa, 0xaa80
}
, trashcanicon_1_data[] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,	0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000,	0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000,	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x03ff, 0xfff5, 0x5500, 0x0000, 0x07ff, 0xffff,	0xaa80, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000,	0x03ff, 0xff55, 0x5400, 0x0000,
	0x03ff, 0xffff,	0xaa80, 0x0000, 0x038f, 0xfc7d, 0x4200, 0x0000,
	0x0317, 0xf8be, 0x8880, 0x0000, 0x0337, 0xf9bf,	0x9100, 0x0000,
	0x0313, 0xf9bf, 0x1000, 0x0000,	0x019b, 0xf9be, 0x9100, 0x0000,
	0x019b, 0xf93f,	0x1200, 0x0000, 0x019b, 0xf9be, 0x9100, 0x0000,
	0x019b, 0xf93f, 0x2200, 0x0000, 0x019b, 0xf9be,	0x2500, 0x0000,
	0x0189, 0xf93f, 0x2200, 0x0000,	0x01cd, 0xf9be, 0x2400, 0x0000,
	0x00c9, 0xf93d,	0x2a00, 0x0000, 0x00cd, 0xf9be, 0x4400, 0x0000,
	0x00c7, 0xf9fe, 0x4a00, 0x0000, 0x00e3, 0xfe75,	0x1400, 0x0000,
	0x007f, 0xffea, 0xa800, 0x0000,	0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000,	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,	0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000,	0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x3fe0,	0x0000, 0x0000, 0x0000, 0xe038, 0x0000, 0x0000,
	0x07ff, 0xffff, 0xffc0, 0x0000, 0x0fff, 0xffff,	0xffe0, 0x0000,
	0x1fff, 0xffff, 0xfff0, 0x0000,	0x1fff, 0xffff, 0xfff0, 0x0000,
	0x0fff, 0xffff,	0xffe0, 0x0000, 0x0fff, 0xffff, 0xffe0, 0x0000,
	0x0fff, 0xffff, 0xffe0, 0x0000, 0x0fff, 0xffff,	0xffe0, 0x0000,
	0x0fff, 0xffff, 0xffe0, 0x0000,	0x0fff, 0xffff, 0xffe0, 0x0000,
	0x07ff, 0xffff,	0xffe0, 0x0000, 0x07ff, 0xffff, 0xffc0, 0x0000,
	0x07ff, 0xffff, 0xffc0, 0x0000, 0x07ff, 0xffff, 0xffc0, 0x0000,
	0x07ff, 0xffff, 0xffc0, 0x0000, 0x07ff, 0xffff, 0xffc0, 0x0000,
	0x07ff, 0xffff, 0xffc0, 0x0000, 0x03ff, 0xffff, 0xff80, 0x0000,
	0x03ff, 0xffff, 0xff80, 0x0000, 0x03ff, 0xffff, 0xff80, 0x0000,
	0x03ff, 0xffff, 0xfff8, 0x0000, 0x03ff, 0xffff, 0xffff, 0xe000,
	0x01ff, 0xffff, 0xffff, 0xe000, 0x000f, 0xffff, 0xfffc, 0x0000
}
, trashcanicon_2_data[] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0003, 0xffff, 0x8000, 0x0000, 0x00fc, 0x0000,	0x7e00, 0x0000,
	0x0702, 0xaaaa, 0x81c0, 0x0000,	0x0855, 0x6ab5, 0x5420, 0x0000,
	0x02aa, 0xaaaa,	0xabc0, 0x0000, 0x00fd, 0x5555, 0x7e00, 0x0000,
	0x0003, 0xffff, 0x8000, 0x0000, 0x0000, 0x0000,	0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000,	0x03ff, 0xff55, 0x5400, 0x0000,
	0x03ff, 0xffff,	0xaa80, 0x0000, 0x038f, 0xfc7d, 0x4200, 0x0000,
	0x0317, 0xf8be, 0x8880, 0x0000, 0x0337, 0xf9bf,	0x9100, 0x0000,
	0x0313, 0xf9bf, 0x1000, 0x0000,	0x019b, 0xf9be, 0x9100, 0x0000,
	0x019b, 0xf93f,	0x1200, 0x0000, 0x019b, 0xf9be, 0x9100, 0x0000,
	0x019b, 0xf93f, 0x2200, 0x0000, 0x019b, 0xf9be,	0x2500, 0x0000,
	0x0189, 0xf93f, 0x2200, 0x0000,	0x01cd, 0xf9be, 0x2400, 0x0000,
	0x00c9, 0xf93d,	0x2a00, 0x0000, 0x00cd, 0xf9be, 0x4400, 0x0000,
	0x00c7, 0xf9fe, 0x4a00, 0x0000, 0x00e3, 0xfe75,	0x1400, 0x0000,
	0x007f, 0xffea, 0xa800, 0x0000,	0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000,	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0003, 0xffff, 0x8000, 0x0000, 0x00ff, 0xffff,	0xfe00, 0x0000,
	0x07ff, 0xffff, 0xffc0, 0x0000,	0x0fff, 0xffff, 0xffe0, 0x0000,
	0x1fff, 0xffff,	0xfff0, 0x0000, 0x1fff, 0xffff, 0xfff0, 0x0000,
	0x0fff, 0xffff, 0xffe0, 0x0000, 0x00ff, 0xffff,	0xfe00, 0x0000,
	0x0003, 0xffff, 0x8000, 0x0000,	0x1fff, 0xffff, 0xfff0, 0x0000,
	0x0fff, 0xffff,	0xffe0, 0x0000, 0x0fff, 0xffff, 0xffe0, 0x0000,
	0x0fff, 0xffff, 0xffe0, 0x0000, 0x0fff, 0xffff,	0xffe0, 0x0000,
	0x0fff, 0xffff, 0xffe0, 0x0000,	0x0fff, 0xffff, 0xffe0, 0x0000,
	0x07ff, 0xffff,	0xffe0, 0x0000, 0x07ff, 0xffff, 0xffc0, 0x0000,
	0x07ff, 0xffff, 0xffc0, 0x0000, 0x07ff, 0xffff, 0xffc0, 0x0000,
	0x07ff, 0xffff, 0xffc0, 0x0000, 0x07ff, 0xffff, 0xffc0, 0x0000,
	0x07ff, 0xffff, 0xffc0, 0x0000, 0x03ff, 0xffff, 0xff80, 0x0000,
	0x03ff, 0xffff, 0xff80, 0x0000, 0x03ff, 0xffff, 0xff80, 0x0000,
	0x03ff, 0xffff, 0xfff8, 0x0000, 0x03ff, 0xffff, 0xffff, 0xe000,
	0x01ff, 0xffff, 0xffff, 0xe000, 0x000f, 0xffff, 0xfffc, 0x0000
};

struct Image projecticon_image =
{
	0, 0, 54, 22, 2, projecticon_data, 3, 0, NULL
}
, toolicon_image =
{
0, 0, 54, 22, 2, toolicon_data, 3, 0, NULL
}
, drawericon_1_image =
{
0, 0, 57, 14, 2, drawericon_1_data, 3, 0, NULL
}
, drawericon_2_image =
{
0, 0, 57, 14, 2, drawericon_2_data, 3, 0, NULL
}
, trashcanicon_1_image =
{
0, 0, 51, 31, 2, trashcanicon_1_data, 3, 0, NULL
}
, trashcanicon_2_image =
{
0, 0, 51, 31, 2, trashcanicon_2_data, 3, 0, NULL
};

struct DrawerData drawericon_drawer =
{
	{ 50, 50, 400, 100, 1, 0, 0, 0x240027f, NULL, NULL, NULL, NULL, NULL, 90, 40, 0xffff, 0xffff, WBENCHSCREEN },
	0, 0, 0, 0
}
, trashcanicon_drawer =
{
	{ 50, 50, 400, 100, 1, 0, 0, 0x240027f, NULL, NULL, NULL, NULL, NULL, 90, 40, 0xffff, 0xffff, WBENCHSCREEN },
	0, 0, 0, 0
};

struct DiskObject projecticon_icon =
{
	WB_DISKMAGIC,
	WB_DISKVERSION,
	{ NULL, 0, 0, 54, 23, GFLG_GADGIMAGE | GADGBACKFILL, GACT_RELVERIFY, GTYP_BOOLGADGET, (APTR) & projecticon_image, NULL, NULL, 0, NULL, 0, NULL },
	WBPROJECT,
	(char *)"",
	NULL,
	NO_ICON_POSITION,
	NO_ICON_POSITION,
	NULL,
	NULL,
	4096
}
, toolicon_icon =
{
	WB_DISKMAGIC,
	WB_DISKVERSION,
	{ NULL, 0, 0, 54, 23, GFLG_GADGIMAGE | GADGBACKFILL, GACT_RELVERIFY, GTYP_BOOLGADGET, (APTR) & toolicon_image, NULL, NULL, 0, NULL, 0, NULL },
	WBTOOL,
	NULL,
	NULL,
	NO_ICON_POSITION,
	NO_ICON_POSITION,
	NULL,
	NULL,
	4096
}
, drawericon_icon =
{
	WB_DISKMAGIC,
	WB_DISKVERSION,
	{ NULL, 0, 0, 57, 14, GFLG_GADGIMAGE | GFLG_GADGHIMAGE, GACT_RELVERIFY | GACT_IMMEDIATE, GTYP_BOOLGADGET, (APTR) & drawericon_1_image, (APTR) & drawericon_2_image, NULL, 0, NULL, 0, NULL },
	WBDRAWER,
	NULL,
	NULL,
	NO_ICON_POSITION,
	NO_ICON_POSITION,
	&drawericon_drawer,
	NULL,
	0
}
, trashcanicon_icon =
{
	WB_DISKMAGIC,
	WB_DISKVERSION,
	{ NULL, 0, 0, 51, 31, GFLG_GADGIMAGE | GFLG_GADGHIMAGE, GACT_RELVERIFY | GACT_IMMEDIATE, GTYP_BOOLGADGET, (APTR) & trashcanicon_1_image, (APTR) & trashcanicon_2_image, NULL, 0, NULL, 0, NULL },
	WBGARBAGE,
	NULL,
	NULL,
	NO_ICON_POSITION,
	NO_ICON_POSITION,
	&trashcanicon_drawer,
	NULL,
	0
}
, *icontable[4] =
{
	&drawericon_icon,
	&toolicon_icon,
	&projecticon_icon,
	&trashcanicon_icon
};
