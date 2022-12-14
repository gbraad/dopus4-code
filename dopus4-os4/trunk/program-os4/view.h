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

#ifndef DOPUS_VIEW_H
#define DOPUS_VIEW_H

#ifdef __GNUC__
	#ifdef __PPC__
		#pragma pack(2)
	#endif
#elif defined(__VBCC__)
	#pragma amiga-align
#endif

#define PN_SIZE 20  /* Size of portname[] */

struct ViewNode
{
	struct Node vnode;
	STRPTR filename;
	STRPTR name;
	int function;
	int wait;
	STRPTR initialsearch;
	BOOL deleteonexit;
	STRPTR processname;
};

struct ViewData
{
	int64 view_file_size;
//	int view_file_size;
	int view_max_line_length;
	int view_tab_size;

	STRPTR view_text_buffer;
	int64 view_buffer_size;
//	int view_buffer_size;

	STRPTR view_file_name;
	STRPTR view_path_name;

	int view_line_count;

	int view_text_offset;
	int view_old_offset;
	int view_top_buffer_pos;
	int view_bottom_buffer_pos;

	int view_lines_per_screen;
	int view_last_line;
	int view_last_charpos;
	STRPTR view_last_char;

	int view_scroll;
	int view_scroll_dir;
	int view_scroll_width;
	int view_char_width;
	int view_display_as_hex;
	int view_display_as_ansi;
	int view_status_bar_ypos;

	char view_search_string[80];
	int view_search_flags;
	int view_search_offset;
	int view_search_charoffset;
	int view_pick_offset;
	int view_pick_charoffset;

	struct Screen *view_screen;
	struct Window *view_window;
	struct RastPort *view_rastport;
	struct Window *view_ansiread_window;
	struct Gadget *view_gadgets;
	struct TextFont *view_font;

	int view_display_left;
	int view_display_top;
	int view_display_right;
	int view_display_bottom;
	int view_display_height;

	APTR view_GTvi;

	struct viewhilite *view_first_hilite;
	struct viewhilite *view_current_hilite;

	struct IOStdReq view_console_request;
	struct PropInfo view_prop_info;
	struct Image view_prop_image;

	char view_port_name[PN_SIZE];
	struct MsgPort *view_port;

	int16 view_colour_table[8];
	struct VisInfo view_vis_info;
};

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack()
   #endif
#elif defined(__VBCC__)
   #pragma default-align
#endif

#endif /* DOPUS_VIEW_H */

