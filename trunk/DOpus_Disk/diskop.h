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

#ifndef DISKOP_H
#define DISKOP_H

#include <debug.h>
//#include <fctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/filehandler.h>
#include <intuition/intuitionbase.h>
#include <intuition/sghooks.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <devices/trackdisk.h>
#include <datatypes/datatypesclass.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#include <proto/gadtools.h>

//#include "dopusbase.h"
//#include "dopuspragmas.h"
//#include "requesters.h"
//#include "stringdata.h"

#include <proto/dopus.h>
#include <dopus/dopusmessage.h>

//#include "functions.h"
#include "diskstrings.h"

extern struct DOpusBase *DOpusBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Library *IconBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;

extern struct RequesterBase req;

#define ERROR_FAILED  -1
#define ERROR_MEMORY  -2
#define ERROR_BITMAP  -3
#define ERROR_VERIFY  -4
#define ERROR_ABORTED -5

struct BitmapBlock {
    ULONG Checksum;
    ULONG Bitmap[127];
};

struct BitmapExtension {
    ULONG BitmapPointers[127];
    ULONG   BitmapExtension;
};

struct RootDirectory {
    ULONG   PrimaryType;
    ULONG   HeaderKey;
    ULONG   WhoKnows0;
    ULONG   HashTableSize;
    ULONG   WhoKnows1;
    ULONG   Checksum;
    ULONG HashTable[72];
    ULONG   BitmapFlag;
    ULONG   BitmapPointers[25];
    ULONG   BitmapExtension;
    struct DateStamp LastRootChange;
    UBYTE   DiskName[32];
    ULONG   WhoKnows2[2];
    struct DateStamp LastDiskChange;
    struct DateStamp CreationDate;
    ULONG   NextHash;
    ULONG   ParentDir;
    ULONG   WhoKnows3;
    ULONG   SecondaryType;
};

union DD_Prefs {
    struct diskcopyP {
        BOOL verify, bump;
        struct DOpusListView *srclist,*dstlist;
    } diskcopy;
    struct formatP {
        char name[32];
        ULONG flags;
        struct DOpusListView *srclist;
        UBYTE pfsdeldir, pfsfnsize;
    } format;
    struct installP {
        BOOL ffs;
        struct DOpusListView *srclist;
    } install;
};

enum {
    PREFS_DISKCOPY,
    PREFS_FORMAT,
    PREFS_INSTALL };

enum {
    DISKCOPY_VERIFY,
    DISKCOPY_BUMPNAMES,
    DISKCOPY_SRCDEV,
    DISKCOPY_DSTDEV,
    DISKCOPY_INFO,
    DISKCOPY_DISKCOPY,
    DISKCOPY_CHECK,
    DISKCOPY_CANCEL };

enum {
    INSTALL_FFS,
    INSTALL_INSTALL,
    INSTALL_NOBOOT,
    INSTALL_CANCEL,
    INSTALL_DEVLIST,
    INSTALL_INFO };

enum {
    FORMAT_NAME,
    FORMAT_FFS,
    FORMAT_INTERNATIONAL,
    FORMAT_CACHING,
    FORMAT_TRASHCAN,
    FORMAT_VERIFY,
    FORMAT_DEV,
    FORMAT_INFO,
    FORMAT_FORMAT,
    FORMAT_QUICKFORMAT,
    FORMAT_CANCEL,
    FORMAT_SFS_CASE,
    FORMAT_SFS_SHOWREC,
    FORMAT_SFS_NOREC,
    FORMAT_PFS_DELDIR,
    FORMAT_PFS_FNSIZE,
    FORMAT_GAD_LAST
};

#define FORMATFLAG_FFS             (1<<0)
#define FORMATFLAG_INTERNATIONAL   (1<<1)
#define FORMATFLAG_CACHING         (1<<2)
#define FORMATFLAG_TRASHCAN        (1<<3)
#define FORMATFLAG_VERIFY          (1<<4)
#define FORMATFLAG_SFS_CASE        (1<<5)
#define FORMATFLAG_SFS_SHOWREC     (1<<6)
#define FORMATFLAG_SFS_NOREC       (1<<7)

extern ULONG BitTable[32];
/*
extern struct TagItem format_name_gadget[];
extern struct TagItem format_ffs_gadget[];
extern struct TagItem format_international_gadget[];
extern struct TagItem format_caching_gadget[];
extern struct TagItem format_trashcan_gadget[];
extern struct TagItem format_verify_gadget[];
extern struct TagItem format_format_gadget[];
extern struct TagItem format_quickformat_gadget[];
extern struct TagItem format_cancel_gadget[];

extern struct TagItem diskcopy_verify_gadget[];
extern struct TagItem diskcopy_bumpnames_gadget[];
extern struct TagItem diskcopy_diskcopy_gadget[];
extern struct TagItem diskcopy_cancel_gadget[];
extern struct TagItem diskcopy_source_list[];
extern struct TagItem diskcopy_destination_list[];

extern struct TagItem install_ffs_gadget[];
extern struct TagItem install_install_gadget[];
extern struct TagItem install_noboot_gadget[];
extern struct TagItem install_cancel_gadget[];
*/
extern struct TagItem commonGTtags[];

extern struct DiskObject trashcanicon_icon;

extern ULONG bootblock_13[13],bootblock_20[24];

struct DeviceHandle {
    struct MsgPort *device_port;
    struct IOExtTD *device_req;
    struct FileSysStartupMsg *startup;
    struct DosEnvec *dosenvec;
};

extern struct DefaultString default_strings[];

#define STRING_VERSION 1

/* diskop.c */

void get_vis_info(struct VisInfo *,char *);
void sort_device_list(char **);
int dopus_message(int,APTR,char *);
int like_devices(struct DeviceNode *,struct DeviceNode *);
void fill_out_req(struct RequesterBase *,struct VisInfo *);
char **get_device_list(struct DOpusRemember **,char *);
void select_device(struct DOpusListView *,char *);
void border_text(struct RequesterBase *,struct Gadget /*Object_Border*/ *,char *);
struct DeviceNode *find_device(char *);
int check_error(struct RequesterBase *,char *,int);
int check_abort(struct Window *);
int check_disk(struct RequesterBase *,struct IOExtTD *,char *,int);
int check_blank_disk(struct RequesterBase *,char *,char *);
void drive_motor(struct IOExtTD *,int);
void inhibit_drive(char *,ULONG);
ULONG do_checksum(ULONG *);
int open_device(char *,struct DeviceHandle *);
void close_device(struct DeviceHandle *);
void get_env(union DD_Prefs *prefs,int id);
void set_env(union DD_Prefs *prefs,int id);
unsigned char getkeyshortcut(const char *);
void getsizestring(char *,long long);
void getfloatstr(double f,char *buf);

/* diskcopy.c */
void diskop_diskcopy(/*struct VisInfo *,*/char *,int,char *[]);
void show_diskcopy_info(struct RequesterBase *,/*Object_Border*/struct Gadget *,char *);
void bump_disk_name(char *);
int do_diskcopy(struct RequesterBase *,/*Object_Border*/struct Gadget *,struct diskcopyP */*char *,char *,int,int*/,int);

/* format.c */
void diskop_format(/*struct VisInfo *,*/char *,int,char *[]);
int do_format(struct RequesterBase *,/*Object_Border*/struct Gadget *,char *,struct formatP * /*char *,ULONG*/,char);
int do_raw_format(struct RequesterBase *,/*Object_Border*/struct Gadget *,struct IOExtTD *,ULONG,ULONG,ULONG,ULONG,ULONG);
void show_device_info(struct RequesterBase *reqbase,struct Gadget /*Object_Border*/ *border,char *name);
void write_trashcan(/*struct RequesterBase *reqbase,Object_Border *border,*/char *device);

/* install.c */

void diskop_install(/*struct VisInfo *,*/int, char *[]);
int do_install(struct RequesterBase *,/*Object_Border*/struct Gadget *,char *,int,int);
int install_compare_block(ULONG *,ULONG *,ULONG);

/* lsprintf.asm */
void lsprintf(const char *,...);

#endif /* DISKOP_H */

