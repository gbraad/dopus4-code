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

#ifndef DO_PRINT_H
#define DO_PRINT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <intuition/intuitionbase.h>
#include <intuition/sghooks.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <devices/printer.h>
#include <devices/prtbase.h>
#include <datatypes/datatypesclass.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#include <proto/wb.h>

#include <proto/dopus.h>
#include <dopus/dopusmessage.h>
#include <dopus/debug.h>
#include "printstrings.h"

#ifdef __GNUC__
	#ifdef __PPC__
		#pragma pack(2)
	#endif
#elif defined(__VBCC__)
	#pragma amiga-align
#endif

#define PRINTFLAG_EJECT      1
#define PRINTFLAG_FILE       2
#define PRINTFLAG_IGNORE     4
#define PRINTFLAG_ADDCR      8
#define PRINTFLAG_EJECTFIRST 16
#define PRINTFLAG_INIT       32

#define HEADFOOTFLAG_TITLE   1
#define HEADFOOTFLAG_DATE    2
#define HEADFOOTFLAG_PAGE    4

#define PITCH_PICA           0
#define PITCH_ELITE          1
#define PITCH_FINE           2

#define PITCH_COUNT          3

#define STYLE_NORMAL         0
#define STYLE_BOLD           1
#define STYLE_ITALICS        2
#define STYLE_UNDERLINED     3
#define STYLE_DOUBLESTRIKE   4
#define STYLE_SHADOW         5

#define STYLE_COUNT          6

#define QUALITY_DRAFT        0
#define QUALITY_NLQ          1

#define HEADER               0
#define FOOTER               1

#define STR_LEN4 4
#define STR_LEN3 3
#define HEADFOOT_SIZE 40

typedef struct HeadFootData
{
	char text_style;
	char headfoot_flags;
	char headfoot_title[HEADFOOT_SIZE];
} HeaderFooter;

typedef struct MyPrinterData
{
	USHORT top_margin;
	USHORT bottom_margin;
	USHORT left_margin;
	USHORT right_margin;
	char tab_size;
	char print_pitch;
	char text_quality;
	char print_flags;
	char output_file[PATHBUF_SIZE];
	HeaderFooter headfoot[2];
} PrintData;

#define DOUBLESTRIKE_OFF "\x1b[3\"z"
#define SHADOW_OFF       "\x1b[5\"z"

extern struct DefaultString default_strings[];

#define STRING_VERSION 1

#ifdef __GNUC__
	#ifdef __PPC__
		#pragma pack(2)
	#endif
#elif defined(__VBCC__)
	#pragma amiga-align
#endif

/* print.c */
void activate_next_gadget(struct Gadget *gadget, struct Window *window);
void get_print_env(PrintData *printdata);
void set_print_env(PrintData *printdata);
int dopus_message(int cmd, APTR data, char *portname);
struct Gadget *addreqgadgets(struct RequesterBase *reqbase, struct TagItem **gadgets, struct Gadget **gadgetlist);
int get_file_byrequest(struct Gadget *gadget, struct Window *window, int save);
int check_error(struct RequesterBase *reqbase, char *str, int abort);
void fill_out_req(struct RequesterBase *req, struct VisInfo *vis);
/* printdir.c */
void do_printdir(struct VisInfo *vis, char *portname, int wind);
/* printreq.c */
void do_print(struct VisInfo *vis, char *portname, struct DOpusArgsList *argslist, int argc, char **argv);
/* doprint.c */
int printfile(struct RequesterBase *reqbase, char *filename, PrintData *printdata, struct Requester *requester);
int check_print_abort(struct RequesterBase *);

#endif /* DO_PRINT_H */

