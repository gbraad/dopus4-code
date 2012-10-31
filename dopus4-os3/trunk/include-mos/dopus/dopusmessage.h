#ifndef DOPUS_MESSAGE_H
#define DOPUS_MESSAGE_H
struct NetworkStuff {
    char *owner;
    char *group;
    char net_protbuf[10];
} __attribute__((packed));
struct Directory {
    struct Directory *last,*next;
    char name[FILEBUF_SIZE];
    int __attribute__((aligned(2))) type;
#ifdef __GNUC__
    long long __attribute__((aligned(2))) size;
#else
    int __attribute__((aligned(2))) size;
#endif
    int __attribute__((aligned(2))) subtype;
    int __attribute__((aligned(2))) protection;
    char * __attribute__((aligned(2))) comment;
    char * __attribute__((aligned(2))) dispstr;
    char protbuf[12],datebuf[LEN_DATSTRING+12];
    int __attribute__((aligned(2))) selected;
    char * __attribute__((aligned(2))) description;
    int __attribute__((aligned(2))) userdata,userdata2;
    struct DateStamp __attribute__((aligned(2))) date;
    UWORD __attribute__((aligned(2))) owner_id,group_id;
    struct NetworkStuff* __attribute__((aligned(2))) network;
    char * __attribute__((aligned(2))) extension;
} __attribute__((packed));

#define DOPUSMSG_GETVIS         1
#define DOPUSMSG_UPDATEDRIVE    2
#define DOPUSMSG_GETNEXTFILE    3
#define DOPUSMSG_UNSELECTFILE   4
#define DOPUSMSG_GETPRINTDIR    5
struct DOpusMessage {
    struct Message msg;
    int command;
    APTR data;
} __attribute__((packed));
struct VisInfo {
    char vi_fg,vi_bg;
    char vi_shine,vi_shadow;
    struct TextFont *vi_font;
    struct Screen *vi_screen;
    char vi_stringcol[2];
    char vi_activestringcol[2];
    ULONG vi_flags;
    char *vi_language;
} __attribute__((packed));

#define VISF_8POINTFONT 1
#define VISF_WINDOW     2
#define VISF_BORDERS    (1<<16)
struct DOpusArgsList {
    char *single_file;
    int   file_window;
    APTR  file_list;
    APTR  last_select;
    char *file_data;
} __attribute__((packed));
struct PrintDirData {
    int win;
    char titlebuf[512];
    struct Directory *entry;
    int onlysel;
    int namelen;
} __attribute__((packed));
struct DirWindowPars {
    char top_name[FILEBUF_SIZE];
    int __attribute__((aligned(2))) offset;
    int __attribute__((aligned(2))) hoffset;
    char * __attribute__((aligned(2))) reselection_list;
    ULONG __attribute__((aligned(2))) reselection_size;
    UBYTE reselection_win;
    struct DirectoryWindow* __attribute__((aligned(2))) reselection_dir;
} __attribute__((packed));

#define CONFIG_GET_CONFIG 1
#define CONFIG_HERES_CONFIG 2
#define CONFIG_ALL_DONE 3
#define CONFIG_NEW_HOTKEY 4
#define CONFIG_HOTKEYS_CHANGE 5
struct dopusconfigmsg {
    struct Message msg;
    int command;
    char *buffer;
} __attribute__((packed));
struct configconfig {
    struct Config *config;
    struct dopusfiletype *firsttype;
    struct DOpusRemember *typekey;
    struct dopusgadgetbanks *firstbank;
    int changed;
    char configname[256];
    struct Window *Window;
    struct Screen *Screen;
    struct dopushotkey *firsthotkey;
} __attribute__((packed));

#endif 
