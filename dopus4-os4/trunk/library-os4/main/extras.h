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

/* The "From" comment lines indicate which OS3 DirectoryOpus4 library
 * source file contained the functions declared below the comment line.
*/


enum
{
	STR_CONFIG_TOO_OLD=0,
	STR_CONTINUE=1,
	STR_REQUEST=2,
	STR_TRY_AGAIN=3,
	STR_CANCEL=4,
	STR_CANNOT_FIND_FILE_REQ=5,
	STR_FILEREQ_FILE=6,
	STR_FILEREQ_DRAWER=7,
	STR_FILEREQ_ACCEPT=8,
	STR_FILEREQ_PARENT=9,
	STR_FILEREQ_DRIVES=10,
	STR_FILEREQ_DIR=11,
	STR_FILEREQ_NAME=12,
	STR_DEV_DEV=13,
	STR_DEV_ASN=14,
	STR_DEV_VOL=15,
	STR_DEV_DFR=16,
	STR_DEV_NBD=17,
	STR_SELECT_DIR=18,
	STR_SELECT_FILE=19,

	STR_STRING_COUNT
};

extern char **string_table; // From language.c

/* Same as in program-os4/main12.c ***********************/

void * doAllocVec(uint32 byteSize, uint32 attributes);

/* From borders.c ****************************************/

void fill_out_border_data(short *shine, short *shadow, int w, int h);
void fill_out_border(struct Border *border, int fpen, int bpen, short *shine, short *shadow);

/* From dopus_stuff.c ************************************/

char *getstringcopy(char *str);
void freestring(char *str);
int writestring(int file, char *string);
int readline(char *buf, int pos, char *buf1, int size);
void AssignDrive(struct ConfigStuff *cstuff, int drv, char *name, char *path);
void linkinnewfiletype(struct ConfigStuff *cstuff, struct dopusfiletype *temp);

/* From gui.c ********************************************/

int makeusstring(char *from, char *to, int *uspos, int size);
void ShowRMBGadName(struct DOpusIFace *IDOpus, struct RastPort *rp, struct RMBGadget *gad, int a);
void HighlightRMBGad(struct RastPort *rp, struct RMBGadget *gad, int state);

/* From function.c ***************************************/

void SwapMem(char *src, char *dst, int size);

/* From imagery.c ****************************************/

struct Image *get_image_data(struct DOpusIFace *IDOpus, struct DOpusRemember **key, int width, int height, int depth, struct BitMap *bm, struct RastPort *rp);

/* From listview.c ***************************************/

void dohilite(struct DOpusListView *view, int a);
void savepens(struct DOpusListView *view);
void restorepens(struct DOpusListView *view);
void DisplayView(struct DOpusIFace *IDOpus, struct DOpusListView *view);
int scroll_view(struct DOpusIFace *IDOpus, struct DOpusListView *view, int offset, int *histate, int oldoffset);
int view_valid(struct DOpusListView *view, int itemnum);

/* END extras.h */
