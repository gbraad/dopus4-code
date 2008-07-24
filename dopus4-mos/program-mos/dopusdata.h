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

#ifndef DOPUS_DATA
#define DOPUS_DATA

/* SAS Detach information */

//extern long _stack;                              /* Stack size needed */
extern long _priority;		/* Default task priority */
extern long _BackGroundIO;	/* Background IO off */
extern char *_procname;		/* Task name */

/* Library bases */

extern struct Library *DOpusBase;			/* dopus.library */

/* Intuition objects */

extern struct IntuiMessage *IMsg;	/* Main IntuiMessage pointer */
extern struct Screen *MainScreen;	/* Main screen */
extern struct Screen *iffscreen;	/* IFF display screen */
extern struct Screen *fontscreen;	/* Font display screen */
extern struct Screen *blankscreen;	/* Blank screen for IFF viewer */
extern struct Window *Window;		/* Main window */
extern struct Image *iconifyimage;	/* Iconify Image for main window */
extern struct Gadget *iconifygadget;	/* Iconify Gagdet for main window */
extern struct DrawInfo *DRI;
extern struct Window *fontwindow;	/* Window for font display */
extern struct Window *ansiread_window;	/* Window for rendering ANSI text */
extern struct Window *reqoverride;	/* Pointer for SimpleRequest window */
extern struct PopupMenu *sortmenu;	/* Sort pop-up menu */

/* Graphics objects */

//extern struct ViewPort *main_vp;                 /* Main screen ViewPort */
extern struct RastPort *main_rp;	/* Main window RastPort */
extern struct RastPort *bar_rp;	/* Screen BarLayer RastPort */
extern struct RastPort dir_rp[2];	/* Directory window RastPorts */

extern struct TextFont *scr_font[NUMFONTS];	/* Fonts used in screen display */

/* Exec objects */

extern struct Process *main_proc;	/* Main Process */
extern struct Task *hotkey_task;	/* HotKey Task */
extern struct Task *clock_task;	/* Clock Task */

extern struct MsgPort *count_port;	/* Port to establish run count */
extern struct MsgPort *general_port;	/* General Message Port */
extern struct MsgPort *appmsg_port;	/* AppObject Message Port */
extern struct MsgPort *arexx_port;	/* ARexx Port */

extern struct MsgPort *hotkeymsg_port;	/* Messages to the hotkey task */
extern struct MsgPort *clockmsg_port;	/* Messages to the clock task */

//extern struct IOStdReq *keyboard_req;            /* Keyboard IO Request block */
extern struct IOStdReq *input_req;	/* Input IO Request block */

/* DOS objects */

extern BPTR ramdisk_lock;	/* Lock on RAM: */
extern APTR windowptr_save;	/* Saved Window Pointer */
extern LONG nil_file_handle;	/* Handle on the NIL: device */

extern BPTR old_pr_cis;		/* Old pr_CIS pointer */
extern BPTR old_pr_cos;		/* Old pr_COS pointer */
extern void *old_pr_consoletask;	/* Old pr_ConsoleTask pointer */

/* General objects */

extern struct DOpusRemember *general_key;	/* General memory key */
extern struct DOpusRemember *help_key;	/* Memory key for help files */
extern struct DOpusRemember *filetype_key;	/* Memory key for filetype list */
extern struct DOpusRemember *menu_key;	/* Memory key for menus */

extern USHORT *nullpalette;	/* Pointer to a black palette */

extern struct Locale *locale;

/* Directory Opus Internal objects */

extern struct Directory *last_selected_entry;	/* The last entry selected */
extern struct Help *dopus_firsthelp;	/* The first help message */
extern struct dopusfiletype *dopus_firsttype;	/* The first filetype */
extern struct dopusgadgetbanks *dopus_firstgadbank;	/* The first gadget bank */
extern struct dopusgadgetbanks *dopus_curgadbank;	/* The current gadget bank */
extern struct dopushotkey *dopus_firsthotkey;	/* The first hotkey */
extern struct dopushotkey *dopus_globalhotkey;	/* The last hotkey function */

extern struct DirectoryWindow *dopus_curwin[2];	/* Currently displayed buffer */
extern struct DirectoryWindow *dopus_firstwin[2];	/* First buffer in each window */
extern struct DirectoryWindow *dopus_specialwin[2];	/* Special buffer for each window */
extern struct DirectoryWindow *dopus_oldwin[2];	/* Last buffer displayed */

extern void *dir_memory_pool;	/* Memory pool for allocations */


/* String data */

extern char str_config_basename[256];	/* Configuration file path name */
extern char str_config_file[256];	/* Configuration file */
extern char str_dopusrt_path[256];	/* DOpusRT path name */

extern char str_select_info[128];	/* Status bar text for selection info */
extern char *str_select_pattern[4];	/* Four different Select patterns */
extern char *str_arexx_command;	/* Entered ARexx command string */
extern char *str_space_string;	/* 255 spaces in a string */
extern char str_hunt_name[80];	/* File name to hunt for */
extern char str_hunt_name_parsed[164];	/* Hunt file parsed */
extern char str_search_string[80];	/* String to search for */
extern char str_filter[40];	/* File filter for searching */
extern char str_filter_parsed[84];	/* File filter parsed */
extern char str_arcorgname[FILEBUF_SIZE];

extern char str_version_string[30];	/* Current version string */
extern char str_arexx_portname[30];	/* ARexx Port name */

//extern UBYTE var_key_matrix[13];                 /* Keyboard matrix data area */
extern char *str_last_statustext;	/* Last status text displayed */

extern char *str_last_rexx_result;	/* Last ARexx result */

extern const char *str_protbits;

/* Various data */

extern BYTE status_iconified;	/* Current iconified status */
extern BYTE status_configuring;	/* Configuring status */
extern BYTE status_justabort;	/* Have we just aborted? */
extern BYTE status_haveaborted;	/* The abort sequence has been hit */
extern BYTE status_rexxabort;	/* Abort flag for ARexx scripts */
extern BYTE status_previousabort;	/* Did we abort a while ago ? */
extern BYTE status_publicscreen;	/* Are we on a public screen? */
extern ULONG status_flags;	/* Various status bits */

extern BYTE data_active_window;	/* Active directory window */
extern UBYTE data_buffer_count[2];	/* Number of buffers in each window */
extern BYTE data_gadgetrow_offset;	/* Gadget row offset */
extern SHORT data_drive_offset;	/* Current drive offset */
extern ULONG data_colorclock;	/* Color clock value for this system */

extern ULONG time_current_sec;	/* Current Second (from Intuition) */
extern ULONG time_current_micro;	/* Current Micro (from Intuition) */
extern ULONG time_previous_sec;	/* Previous Second (from Intuition) */
extern ULONG time_previous_micro;	/* Previous Micro (from Intuition) */

extern int system_dopus_runcount;	/* Run count of this copy */

extern USHORT scr_gadget_rows;	/* Number of button rows */
extern USHORT scr_gadget_bank_count;	/* Number of button banks */

extern USHORT scr_min_width;	/* Minimum window width */
extern USHORT scr_min_height;	/* Minimum window height */

extern int askeach;		/* Should replace requester be displayed */
extern int autoskip;		/* Should files be automatically owerwritten */

/* Configuration data */

extern struct Config *config;	/* Configuration structure */
extern BPTR configopus_segment;	/* Segment pointer for ConfigOpus */
extern BPTR external_mod_segment[3];	/* External module segments */
extern BYTE config_changed;	/* Configuration has been changed */

extern const char *config_replyport_basename;

/* Disk and DOS operations data */

extern UBYTE disk_change_state;	/* Lower bits indicate change state */
extern UQUAD dos_global_bytecount;	/* Global byte count from recursive op */
extern UQUAD dos_global_copiedbytes;	/* Bytes copied in a recursive op */
extern UQUAD dos_global_deletedbytes;	/* Bytes copied in a recursive op */
extern ULONG dos_global_blocksneeded;	/* Number of blocks needed to copy */
extern ULONG dos_global_files;	/* Number of files found in recursive op */

extern struct DateStamp dos_copy_date;	/* DateStamp of file last copied */
extern ULONG dos_copy_protection;	/* Protection of file last copied */
extern char dos_copy_comment[80];	/* Comment of file last copied */

extern BOOL glob_unprotect_all;	/* Unprotect all files */

extern BOOL global_swap_window;	/* Active window changed */

extern struct Directory dos_global_entry;	/* Global single file entry */

extern struct NotifyRequest *dos_notify_req[2];	/* Notify requests for both windows */
extern char dos_notify_names[2][1024];	/* Notify requests name buffers */

extern struct recpath *rec_firstpath;	/* Directory path list pointer */
extern struct DOpusRemember *rec_pathkey;	/* Memory key for dir list */

extern char *icon_type_names[];	/* Names of different icon types */

extern struct DOpusRemember *recurse_dir_key;	/* Memory key for RecursiveDirectory's */
extern struct RecursiveDirectory *first_recurse;	/* First recursive directory pointer */
extern int recurse_max_depth;	/* Maximum depth reached */

extern struct path_complete completion[2];	/* Path-name completion data */

/* DateTime stuff */

extern struct StringData stringdata;	/* String data */


/* Gadgets from tasks.c */

extern struct Gadget abortopgad;
extern struct Gadget closegadget_gad;
extern struct Gadget upfrontgadget_gad;
extern struct Gadget downbackgadget_gad;

extern struct DiskObject *user_appicon;	// HUX: User selected app icon

#endif
