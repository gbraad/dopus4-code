/* extras.h 2006-02-05 */

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

extern char **string_table;

/* From borders.c */

void fill_out_border_data(short *shine, short *shadow, int w, int h);
void fill_out_border(struct Border *border, int fpen, int bpen, short *shine, short *shadow);

/* END From borders.c */

/* From dopus_stuff.c */

char *getstringcopy(char *str);
void freestring(char *str);
int writestring(int file, char *string);
int readline(char *buf, int pos, char *buf1, int size);
void AssignDrive(struct ConfigStuff *cstuff, int drv, char *name, char *path);
void linkinnewfiletype(struct ConfigStuff *cstuff, struct dopusfiletype *temp);

/* END From dopus_stuff.c */

/* From gui.c */

int makeusstring(char *from, char *to, int *uspos, int size);
void ShowRMBGadName(struct RastPort *rp, struct RMBGadget *gad, int a);
void HighlightRMBGad(struct RastPort *rp, struct RMBGadget *gad, int state);


/* END From gui.c */

/* From function.c */

void SwapMem(char *src, char *dst, int size);
void stuffChar(char c, char *buf);


/* changed by Joachim 'zerohero' Birging 2006-02-02 */
void LSprintfA(char *buf, char *fmt, void *args);
void VARARGS68K LSprintf(char *buf, char *fmt, ...);

/* END changed by Joachim 'zerohero' Birging 2006-02-02 */
/* END From functions.c */

/* From imagery.c */

struct Image *get_image_data(struct DOpusRemember **key, int width, int height, int depth, struct BitMap *bm, struct RastPort *rp);

/* END From imagery.c */

/* From language.c */

//char **string_table;

/* END From language.c */

/* From listview.c */

//const char nullstring[] = "                                                                                                                                ";

void dohilite(struct DOpusListView *view, int a);
void savepens(struct DOpusListView *view);
void restorepens(struct DOpusListView *view);
void DisplayView(struct DOpusListView *view);
int scroll_view(struct DOpusListView *view, int offset, int *histate, int oldoffset);
int view_valid(struct DOpusListView *view, int itemnum);

/* END From listview.c */

/* From wildcard.c */

int DoMatchPattern(char *pat, char *str, int cas);
void DoParsePattern(STRPTR pat, STRPTR patbuf, int cas);

/* END From wildcard.c */

/* END extras.h */
