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

#ifndef DOPUS_INCLUDE
#define DOPUS_INCLUDE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <dos/exall.h>
#include <intuition/intuitionbase.h>
#include <intuition/imageclass.h>
#include <intuition/sghooks.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <graphics/rpattr.h>
#include <libraries/commodities.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <libraries/application.h>

#include <devices/audio.h>
#include <devices/input.h>
#include <devices/keyboard.h>
#include <devices/trackdisk.h>
#include <devices/printer.h>
#include <devices/prtbase.h>
#include <devices/conunit.h>

#include <hardware/intbits.h>

#include <datatypes/datatypesclass.h>
#include <datatypes/animationclass.h>
#include <datatypes/pictureclass.h>
#include <datatypes/soundclass.h>


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/datatypes.h>
#include <proto/diskfont.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/gadtools.h>
#include <proto/layers.h>
#include <proto/locale.h>
#include <proto/commodities.h>
#include <proto/wb.h>
#include <proto/rexxsyslib.h>
#include <proto/timer.h>
#include <proto/utility.h>
#include <proto/xadmaster.h>
#include <proto/Picasso96API.h>
#include <proto/application.h>
#include <proto/pm.h>

#include <proto/dopus.h>

#include <dopus/dopusmessage.h>
#include <dopus/configflags.h>

#include "arbiter.h"
#include "dopusstructures.h"
#include "dopusflags.h"
#include "dopusiff.h"
#include "dopusfunctions.h"
#include "dopusdata.h"
#include "dopusstrings.h"
#include "screendata.h"
#include "launchexternal.h"
#include "view.h"

#include "dopusproto.h"

#define APPWINID     1

#define TASK_QUIT             999
#define TASK_HOTKEY_WAKEUP     33
#define PROGRESS_OPEN         100
#define PROGRESS_UPDATE       101
#define PROGRESS_CLOSE        102
#define PROGRESS_INCREASE     103
#define HOTKEY_HOTKEYCHANGE   120
#define HOTKEY_NEWHOTKEYS     121
#define HOTKEY_KILLHOTKEYS    122
#define CLOCK_ACTIVE          130

#define VALID_QUALIFIERS (IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND | IEQUALIFIER_CONTROL | IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT | IEQUALIFIER_LALT | IEQUALIFIER_RALT)

#define IEQUALIFIER_ANYSHIFT (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)

#define ICON_IDCMP (IDCMP_CLOSEWINDOW | IDCMP_MOUSEBUTTONS | IDCMP_DISKINSERTED | IDCMP_DISKREMOVED | IDCMP_GADGETUP | IDCMP_GADGETDOWN | IDCMP_ACTIVEWINDOW | IDCMP_INACTIVEWINDOW | IDCMP_MOUSEMOVE | IDCMP_MENUPICK)


#define XAD_WRITEOVER 1
#define XAD_NOABS     2
#define XAD_MAKETREE  4

#define ENTRY_FILE              -1
#define ENTRY_DEVICE             0
#define ENTRY_DIRECTORY          1
#define ENTRY_CUSTOM           999
#define CUSTOMENTRY_DIRTREE      1
#define CUSTOMENTRY_BUFFERLIST   2
#define CUSTOMENTRY_USER         3

#define CUSTENTRY_CANSELECT      0x00010000

#define USERENTRY_ERROR         -1
#define USERENTRY_FUNCTION       0
#define USERENTRY_DOUBLECLICK    1
#define USERENTRY_CLICKMCLICK    2

#define ENTRYTYPE(t) (( t == ENTRY_CUSTOM) ? ENTRY_CUSTOM : ((t == 0) ? ENTRY_DEVICE : (( t < 0) ? ENTRY_FILE : ENTRY_DIRECTORY)))

#define MAXDISPLAYLENGTH  300

#define isonlyword(c) (!c || c==10 || c==13 || isspace(c) || ispunct(c))

#define MENUCOUNT 100
#define GADCOUNT 84
#define DRIVECOUNT 32
#define NUMFONTS 16
#define USEDRIVECOUNT 30
#define FILETYPE_FUNCNUM   16

#define DRIVE1 200
#define DRIVE2 201
#define DRIVE3 202
#define DRIVE4 203
#define DRIVE5 204
#define DRIVE6 205

#define ARROWS_INSIDE   0
#define ARROWS_OUTSIDE  1
#define ARROWS_EACHSIDE 2

#define ABSI(n) ((n<0)?-n:n)

#define MODE_WORKBENCHUSE         1
#define MODE_WORKBENCHCLONE       2
#define MODE_PUBLICSCREEN         3

#define STATUS_HELP         1	/* Help mode is active */
#define STATUS_IANSCRAP     2	/* Various things for Ian */
#define STATUS_ISINBUTTONS  4	/* Is current iconified to buttons */
#define STATUS_DONEREXX     8	/* Done the initial ARexx script */
#define STATUS_BUSY         16	/* Busy */
#define STATUS_AUDIOLED     32	/* AudioLed is off */
#define STATUS_GLOBALFILE   64	/* File is a global one */
#define STATUS_VERIFYFAIL   128	/* Verify requester was cancelled */
#define STATUS_FROMHOTKEY   256	/* File is run from hotkey */
#define STATUS_IANSCRAP2    512	/* More of Ian's crap :-) */
#define STATUS_SQUAREPIXEL  1024	/* Screen has square pixels */
#define STATUS_NEWLOOK      2048	/* New-look sliders */

extern struct DiskObject *icontable[4];

#define MAXMENULENGTH 64

extern USHORT pageflip_data1[5], pageflip_data2[3]; //, null_pointer[6];

extern char beepwave[16];

extern struct Image appicon_image;

#endif
