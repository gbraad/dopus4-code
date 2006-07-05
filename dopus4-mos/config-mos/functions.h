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

//void KPrintF __ARGS((char *,...));

/* main.c */

int main(int, char **);
void quit(void);
void getconfig(void);
void giveconfig(void);
void readconfig(void);
void configmsg(int);
void fixcstuff(struct ConfigStuff *);
void cstufffix(struct ConfigStuff *);
void cleanconfigscreen(void);
void showconfigscreen(int);
void initsidegads(STRPTR *, int, int);
void inittickgads(struct ConfigGadget *, int, int);
void removetickgads(void);
int processtickgad(struct ConfigGadget *, int, int, int);
struct ConfigUndo *makeundo(int);
void copygadgets(struct dopusgadgetbanks *, struct dopusgadgetbanks *, struct DOpusRemember **);
void copyfiletypes(struct dopusfiletype *, struct dopusfiletype **, struct DOpusRemember **);
void doundo(struct ConfigUndo *, int);
void freegadgets(struct dopusgadgetbanks *);
void freegadgetbank(struct dopusgadgetbanks *, int);
void freefunction(struct dopusfunction *);
void freenewfunction(struct newdopusfunction *);
char *getcopy(STRPTR, int, struct DOpusRemember **);
void makestring(BYTE *buf, ...);
int getstring(STRPTR, STRPTR, int, int);
int request(STRPTR);
int do_request(STRPTR, STRPTR, STRPTR);
void doglassimage(struct Gadget *);
void freestring(STRPTR);
void doscreentitle(STRPTR);
struct IntuiMessage *getintuimsg(void);
struct TextFont *getfont(STRPTR, int *, int);
void load_palette(struct Screen *, ULONG *, int);
void get_palette(struct Screen *, ULONG *, int);
void open_screen(void);
void close_screen(void);
void get_colour_table(void);
void free_colour_table(struct ColorMap *);
void loadrgb4(struct Screen *, USHORT *, int);
unsigned char getkeyshortcut(const char *str);

/* main1.c */

int dooperationconfig(void);
int dosystemconfig(void);
void getnextgadget(struct Gadget *);
int fixformlist(int);
void fixdisplaylengths(int);
int system_requester(char *, char *, struct Gadget *, LONG);
void system_makeiconlist(int);
void system_showicon(STRPTR, int);
void get_language_list(struct DOpusRemember **);
void quicksort(char **, int, int);
void sortswap(char **, int, int);

/* main2.c */

int dogadgetconfig(void);
void showgadgets(struct dopusgadgetbanks *, int);
void showgadgetname(struct newdopusfunction *, struct Gadget *);
int makenewbank(int);
struct dopusgadgetbanks *lastbank(struct dopusgadgetbanks *);
void dogadgetinfo(STRPTR);
int domenuconfig(void);
void showmenus(int);
void showmenuname(struct newdopusfunction *, struct Gadget *);
void showmenutext(STRPTR, int, int, int, int);
void clearmenu(struct newdopusfunction *);
int doinitmenutext(int);
void sortmenustrip(int);
void do_menuscreen_title(int);

/* main3.c */

int editfunction(struct dopusfunction *func, int, APTR);
void setuplist(struct DOpusListView *, int, int);
void makefunclist(STRPTR, STRPTR *, STRPTR);
void makedispfunc(STRPTR, char, STRPTR);
char *compilefunclist(STRPTR *, STRPTR, struct DOpusRemember **);
void freefunclist(STRPTR *);
void docolourbox(struct RastPort *, int, int, int, int);
void docoloursel(struct RastPort *, int, int, int);
void drawcompbox(struct RastPort *, int, int, int, int);
void showkey(USHORT, USHORT, int, int, int, int);
void showfuncob(struct RastPort *, STRPTR, int, int, int, int, int);
void makeselflags(int, STRPTR);
int getselflags(STRPTR);
void checkmutflags(STRPTR, int);
void editgadsoff(void);
void editgadson(int);
void deleteentry(STRPTR *, STRPTR, STRPTR *, int);
void endedit(void);
void setupforedit(int, int);
void checkswapgad(void);
int funcrequester(int, STRPTR, STRPTR);
void makenewentry(STRPTR *, STRPTR, STRPTR *, int);
void setupcolourbox(struct RastPort *, int, int, int, int);
void getfiletypefunc(struct dopusfunction *, struct dopusfiletype *, int);
void makefiletypefunc(struct dopusfunction *, struct dopusfiletype *, int);
void fixeditfunction(struct dopusfunction *, STRPTR, STRPTR *, STRPTR, int, int, APTR, int);
void setupeditdisplay(struct dopusfunction *, int, APTR, int, STRPTR *, STRPTR *, STRPTR, STRPTR, int);
void erasefunction(struct dopusfunction *, STRPTR *, STRPTR *, STRPTR);
int fixfunctypelist(struct dopusfiletype *);
void add_appobject(int);
void rem_appobject(int);
void do_gad_label(STRPTR, int, int);

/* main4.c */

int dodriveconfig(void);
void showdrives(void);
void showdrivename(struct dopusfunction *, struct Gadget *);
int doinitdrivetext(int);
int docolourgad(struct RastPort *, int, int, int, int, STRPTR, int *, int *, int);
void driveeditoff(void);
void deletedrive(struct dopusfunction *);
void sortdrivebank(int);

/* main5.c */

int dofiletypeconfig(void);
char **makefiletypelist(struct DOpusRemember **);
void showtypelist(STRPTR *);
int doinitfiletypetext(int);
void filetypetitle(struct dopusfiletype *);
int editfiletype(struct dopusfiletype *, struct DOpusRemember **, int);
int editclass(struct fileclass *, int);
void addfiletype(struct dopusfiletype *);
void readfileclasses(void);
int importfileclasses(void);
int savefileclasses(void);
int addfileclass(STRPTR, STRPTR, STRPTR);
void freefileclasses(void);
void removefileclass(struct fileclass *);
char **makeclasslist(struct DOpusRemember **);
int readline(STRPTR, int, STRPTR, int);
int editfileclass(struct fileclass *, int);
void makeeditclasslist(struct fileclass *, STRPTR *, STRPTR);
void dispclasslist(STRPTR *, STRPTR, STRPTR *);
void seteditclassgads(int);
void removeclassentry(STRPTR *, STRPTR, int);
void insertnewclass(STRPTR *, STRPTR, int, STRPTR *, STRPTR, int);
void endclassedit(int, STRPTR *, STRPTR, STRPTR *);
void makeclassrecog(struct fileclass *, STRPTR *, STRPTR);
void checkclassswap(void);
void showclassop(int);
struct fileclass *getfileclasslist(int);
void draw_file_view(void);
void free_file_view(void);
void load_file_view(void);
void file_view_text(STRPTR, int);
void show_file_view(void);
void show_view_number(int, int);
void cursor_fileview(int);

/* screen.c */

int doscreenconfig(void);
void makescreenedit(int);
void removescreenedit(int);

/* screen_colors.c */

void showcolourvals(int, int);
void updatecolourvals(int, int);
void showscreencolours(int, UBYTE **, UBYTE **, int);
void solidblock(struct RastPort *, STRPTR, int, int, int, int, int, int, int);
void showcolourscycle(int, int, int);

/* screen_screenmode.c */

int initscreenmodes(void);
int addscreenmode(STRPTR, UWORD, UWORD, UWORD, UWORD, UWORD, UWORD, UWORD, ULONG);
struct ScreenMode *showdisplaydesc(void);
void fixmodegads(struct ScreenMode *);
void fixdefaultgads(struct ScreenMode *);
void sortscreenmodes(int, int);
struct ScreenMode *getscreenmode(int);

/* screen_fonts.c */

int initfontlist(int, int, int);
void sortfontlist(struct AvailFonts *, int, int);
void dofontdemo(STRPTR, int);

/* screen_palette.c */

void showpalettegad(int);
void scalecol(ULONG *);
void scalecolup(ULONG *);

/* screen_arrows.c */

void showupdownslider(void);
void showleftrightslider(void);
void showstringgadget(void);
void showarrowborders(int, int);
void drawarrowrec(int);
void fixarrowgads(int);

/* screen_sliders.c */

void show_slider_setup(int, int, int);

/* main7.c */

void busy(void);
void unbusy(void);
void dosave(int);
int doload(int, int);
void copyconfigonly(struct Config *, struct Config *);
int dolistwindow(STRPTR, int, int, STRPTR *, int, STRPTR, int *);
void copytoclip(struct dopusfunction *, STRPTR *, STRPTR, STRPTR);
int pasteclip(struct dopusfunction *, STRPTR *, STRPTR, STRPTR *, STRPTR);
void makefilebuffer(STRPTR);
char *strstri(STRPTR, STRPTR);

/* main8.c */

int dohotkeysconfig(void);
void makehotkeylist(void);
int doedithotkey(struct dopushotkey *);
void loadpalette(USHORT *, ULONG *, int);
void loadrgbtable(struct ViewPort *, ULONG *, int, int);
void drawcornerimage(struct RastPort *, int, int, int, int);
int fixrmbimage(struct RMBGadget *, struct Gadget *, int, int, int);
void doradiobuttons(void);
void fix_gadget_border(struct Gadget *);
void select_gadget(struct Gadget *, int);
void fix_gadget_positions(void);
void fix_gad_pos(struct Gadget *, short *);

/* main9.c */

void readhelp(void);
void makehelpname(STRPTR);
void doconfighelp(void);
void dohelpmsg(STRPTR);
void load_clips(void);
void save_clips(void);
int readfile(STRPTR, STRPTR *, int *);
void init_strings(void);
void setup_list_window(struct NewWindow *, struct DOpusListView *, struct Gadget *, int);

/* strings.c */

void read_strings(void);

/* paint_mode.c */

int get_paint_colours(int *, int *, int);
void do3dbox(int, int, int, int);
void fix_slider(struct Gadget *);
struct Window *openwindow(struct NewWindow *);

/* swapmem.c */
void SwapMem(char *, char *, int);
