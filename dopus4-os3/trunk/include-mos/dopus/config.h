#ifndef DOPUS_CONFIG_H
#define DOPUS_CONFIG_H



#define CONFIG_VERSION         10022
#define CONFIG_MAGIC          0xFACE

#define OLD_CONFIG_VERSION      9999
#define CONFIG_LESS_DODRIVES   10001
#define CONFIG_OLD_FILETYPE    10002
#define CONFIG_CHANGE_DOSREQ   10003
#define CONFIG_CHANGE_BUFCOUNT 10004
#define CONFIG_CHANGE_FILETYPE 10009
#define CONFIG_NEW_FUNCTIONS   10010
#define CONFIG_CHANGE_PALETTE  10012
#define CONFIG_CHANGE_ARROWS   10013
#define CONFIG_CHANGE_FTYPE    10014
#define CONFIG_CHANGE_EXTERN   10015

#define NEW_CONFIG_VERSION     10016
#define CONFIG_CHANGE_DISPLAY  10017
#define CONFIG_CHANGE_CONFIGXY 10018
#define CONFIG_CHANGE_DIMENS   10019
#define CONFIG_NEW_SLIDERS     10020
#define CONFIG_USERAW0_HOTKEY  10021
#define CONFIG_BIG_DISPLEN     10022

#define FTYC_MATCH        1
#define FTYC_MATCHNAME    2
#define FTYC_MATCHBITS    3
#define FTYC_MATCHCOMMENT 4
#define FTYC_MATCHSIZE    5
#define FTYC_MATCHDATE    6
#define FTYC_MOVETO       7
#define FTYC_MOVE         8
#define FTYC_SEARCHFOR    9
#define FTYC_MATCHI      10
#define FTYC_OR         253
#define FTYC_AND        254
#define FTYC_ENDSECTION 255

#define DATE_DOS   1
#define DATE_INT   2
#define DATE_USA   4
#define DATE_AUS   8
#define DATE_SUBST 16
#define DATE_12HOUR 32

#define SCRCLOCK_MEMORY    1
#define SCRCLOCK_CPU       2
#define SCRCLOCK_DATE      4
#define SCRCLOCK_TIME      8
#define SCRCLOCK_BYTES     64
#define SCRCLOCK_C_AND_F   128

#define ICON_MEMORY    1
#define ICON_CPU       2
#define ICON_DATE      4
#define ICON_TIME      8
#define ICON_NOWINDOW  16
#define ICON_APPICON   32

#define DISPLAY_NAME 0
#define DISPLAY_SIZE 1
#define DISPLAY_PROTECT 2
#define DISPLAY_DATE 3
#define DISPLAY_COMMENT 4
#define DISPLAY_FILETYPE 5
#define DISPLAY_OWNER    6
#define DISPLAY_GROUP    7
#define DISPLAY_NETPROT  8
#define DISPLAY_EXT 9
#define DISPLAY_LAST 9

#define FLAG_OUTWIND      1
#define FLAG_OUTFILE      2
#define FLAG_WB2F         4
#define FLAG_DOPUSF       8
#define FLAG_ASYNC       16
#define FLAG_CDSOURCE    32
#define FLAG_CDDEST      64
#define FLAG_DOALL      128
#define FLAG_RECURSE    256
#define FLAG_RELOAD     512
#define FLAG_ICONIFY   1024
#define FLAG_NOQUOTE   2048
#define FLAG_SCANSRCE  4096
#define FLAG_SCANDEST  8192
#define FLAG_SHELLUP  16384
#define FLAG_DOPUSUP  32768

#define MENUCOUNT 100
#define GADCOUNT 84
#define DRIVECOUNT 32
#define ARCHIVECOUNT 6
#define NUMFONTS 16

#define OLDDRIVECOUNT 24

#define FILETYPE_FUNCNUM   16

#define FTFUNC_AUTOFUNC1   0
#define FTFUNC_AUTOFUNC2   1
#define FTFUNC_DOUBLECLICK 2
#define FTFUNC_CLICKMCLICK 3
#define FTFUNC_ANSIREAD    4
#define FTFUNC_AUTOFUNC3   5
#define FTFUNC_HEXREAD     6
#define FTFUNC_LOOPPLAY    7
#define FTFUNC_PLAY        8
#define FTFUNC_AUTOFUNC4   9
#define FTFUNC_READ        10
#define FTFUNC_SHOW        11
#define FTFUNC_MMBCLICK    12


#define MAX_DISPLAYLENGTH 4096
struct olddopusgadget {
    char name[10];
    int __attribute__((aligned(2))) which,stack;
    unsigned char __attribute__((aligned(2))) key,qual;
    char type,pri,delay;
    UBYTE fpen,bpen;
    char pad[3];
    char * __attribute__((aligned(2))) function;
} __attribute__((packed));
struct dopusfunction {
    char name[16];
    int which,stack;
    unsigned char key,qual;
    char type,pri,delay;
    UBYTE fpen,bpen;
    char pad;
    char * __attribute__((aligned(2))) function;
} __attribute__((packed));
struct newdopusfunction {
    char *name;
    int pad2[3];
    int which,stack;
    unsigned char key,qual;
    char type,pri,delay;
    UBYTE fpen,bpen;
    char pad;
    char * __attribute__((aligned(2))) function;
} __attribute__((packed));
struct dopusdrive {
    char name[10];
    char path[33];
    unsigned char __attribute__((aligned(2))) key,qual;
    UBYTE fpen,bpen;
    char pad;
	char __pad__;
} __attribute__((packed));
struct olddopusfiletype {
    struct dopusfiletype *next;
    char type[40];
    char filepat[60];
    char recogchars[100];
    char actionstring[4][60];
    int which[4],stack[4];
    char pri[4],delay[4];
    char and;
    char pad[3];
    char *function[4];
} __attribute__((packed));
struct dopusfiletype {
    struct dopusfiletype *next;
    char type[32];
    char typeid[8];
    char actionstring[FILETYPE_FUNCNUM][40];
    int which[FILETYPE_FUNCNUM],stack[FILETYPE_FUNCNUM];
    char pri[FILETYPE_FUNCNUM],delay[FILETYPE_FUNCNUM];
    unsigned char* __attribute__((aligned(2))) recognition;
    char * __attribute__((aligned(2))) function[FILETYPE_FUNCNUM];
    char * __attribute__((aligned(2))) iconpath;
} __attribute__((packed));
struct wr_dopusfiletype {
    struct dopusfiletype *next;
    char type[40];
    char actionstring[FILETYPE_FUNCNUM][40];
    int which[FILETYPE_FUNCNUM],stack[FILETYPE_FUNCNUM];
    char pri[FILETYPE_FUNCNUM],delay[FILETYPE_FUNCNUM];
} __attribute__((packed));
struct dopusgadgetbanks {
    struct newdopusfunction gadgets[GADCOUNT];
    struct dopusgadgetbanks* __attribute__((aligned(2))) next;
} __attribute__((packed));
struct dopushotkey {
    struct dopushotkey *next;
    UWORD code,qualifier;
    char name[40];
    struct dopusfunction func;
} __attribute__((packed));
struct Config {
    USHORT version;
    USHORT magic;

    UBYTE copyflags;
    UBYTE deleteflags;
    UBYTE errorflags;
    UBYTE generalflags;
    UBYTE iconflags;
    UBYTE existflags;
    UBYTE sepflags;
    UBYTE sortflags;
    UBYTE dynamicflags; // JRZ
    char sortmethod[2];

    UBYTE hotkeyflags;

    char menutit[5][16];
    struct newdopusfunction menu[MENUCOUNT];

    struct dopusfunction __attribute__((aligned(2))) drive[DRIVECOUNT];

    char outputcmd[80],output[80];
    int __attribute__((aligned(2))) gadgetrows;

    char separatemethod[2];

    char language[30];

    char displaypos[2][16];
    UBYTE old_displaylength[2][16];

    char pubscreen_name[80];

    USHORT __attribute__((aligned(2))) Palette[16];
    UBYTE gadgettopcol,gadgetbotcol;
    UBYTE statusfg,statusbg;
    UBYTE filesfg,filesbg,filesselfg,filesselbg;
    UBYTE dirsfg,dirsbg,dirsselfg,dirsselbg;
    UBYTE clockfg,clockbg;
    UBYTE requestfg,requestbg;
    UBYTE disknamefg,disknamebg,disknameselfg,disknameselbg;
    UBYTE slidercol,arrowfg,arrowbg,littlegadfg,littlegadbg;

    char pad3;

    char scrdepth;
    char screenflags;
    int __attribute__((aligned(2))) screenmode;
    int __attribute__((aligned(2))) scrw,scrh;
    char pad3a[40];
    char arrowpos[3];

    char pad4; //HUX - now unused

    char startupscript[80];
    char dirflags;
    unsigned char __attribute__((aligned(2))) bufcount;

    UBYTE listerdisplayflags[2]; //HUX - was char pad5[2]

    char autodirs[2][70]; // JRZ - was: 30
//    char pad5a[80];     // JRZ - assigned to autodirs
    UWORD __attribute__((aligned(2))) hotkeycode,hotkeyqual;

    char toolicon[80],projecticon[80],drawericon[80],defaulttool[80];
    char priority;
    unsigned char __attribute__((aligned(2))) showdelay,viewbits,fadetime,tabsize; // JRZ

    char pad7[2];

    char hiddenbit;
    char showpat[40],hidepat[40];
    char showpatparsed[40],hidepatparsed[40];
    char icontype,scrclktype,showfree;

    char pad8;

    short __attribute__((aligned(2))) iconx,icony;
    short __attribute__((aligned(2))) wbwinx,wbwiny;

    char configreturnscript[80];

    char fontsizes[NUMFONTS];
    char fontbufs[NUMFONTS][40];

    char uniconscript[80];
    UBYTE sliderbgcol;

    char pad_foo;

    short __attribute__((aligned(2))) scr_winx,scr_winy;
    short __attribute__((aligned(2))) scr_winw,scr_winh;

        short __attribute__((aligned(2))) viewtext_topleftx, viewtext_toplefty; //HUX
        short __attribute__((aligned(2))) viewtext_width, viewtext_height; //HUX
    char morepadding[223]; //HUX was 231

    char old_displaypos[2][8];
    char dateformat,addiconflags;
    UBYTE stringfgcol,stringbgcol;
    char namelength[2];
    char sliderwidth,sliderheight;
    char formatflags;
    short __attribute__((aligned(2))) iconbutx,iconbuty;
    char stringheight;
    UBYTE stringselfgcol,stringselbgcol;
    char generalscreenflags;

    struct Rectangle __attribute__((aligned(2))) scrollborders[2];

    char old2_displaylength[2][8];

    char shellstartup[30];

    WORD __attribute__((aligned(2))) windowdelta;

    UWORD __attribute__((aligned(2))) displaylength[2][16]; // JRZ
    char pad9a[396-64];         // 64 bytes assigned to displaylength

    int __attribute__((aligned(2))) loadexternal;

    ULONG __attribute__((aligned(2))) new_palette[48];

    char arrowsize[3];

    char slider_pos;

    short __attribute__((aligned(2))) config_x;
    short __attribute__((aligned(2))) config_y;

    char pad10[1414];
} __attribute__((packed));
struct ConfigStuff {
    struct Config *config;
    struct DOpusRemember *typekey;
    struct dopusfiletype *firsttype;
    struct dopusgadgetbanks *firstbank,*curbank;
    struct dopushotkey *firsthotkey;
} __attribute__((packed));

#define MODE_WORKBENCHUSE    1
#define MODE_WORKBENCHCLONE  2

#define SCRFLAGS_DEFWIDTH   1
#define SCRFLAGS_DEFHEIGHT  2
#define SCRFLAGS_HALFHEIGHT 4

#ifndef DOPUS_CONFIGFLAGS
#include <dopus/configflags.h>
#endif

#endif 
