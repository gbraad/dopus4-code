

#define FILEBASE 1
#define FILENAME 1000
#define DRAWERNAME 1001
#define FILENAMEUP 1002
#define FILENAMEDOWN 1003
#define FILENAMEPOS 1004
#define OKAY 1005
#define CANCEL 1006
#define DRIVES 1007
#define PARENT 1008

#define INTERRUPT 2000
struct direntry {
    struct direntry *last,*next;
    int type,select;
    char name[FILEBUF_SIZE];
    char size[10];
} __attribute__((packed));
struct FileReqData {
    int *filetype;
    struct Window *fr_Window;
    struct FileInfoBlock *finfo;
    struct direntry *firstfile,*firstdir;
    struct DOpusRemember *filekey;
    struct DOpusFileReq *freq;
    int prevsec,prevmic;
    int fileentries,fileoffset,oldfileoffset;
    int version2,flags,fh,fw,fb,width,ww,sfh,sfw,sfy,fy;
    struct PropInfo filenameprop;
    struct Image filenameimage;
    struct StringInfo filenamesinfo,drawernamesinfo;
    struct Gadget reqgads[9];
    struct IntuiText reqtext[6];
    struct NewWindow reqwin;
} __attribute__((packed));

#define ERROR_CANCEL 0
#define ERROR_NOMEM -1

#define DFRB_DIRREQ 0
#define DFRB_MULTI  1
#define DFRB_SAVE   2

#define DFRF_DIRREQ (1<<DFRB_DIRREQ)
#define DFRF_MULTI  (1<<DFRB_MULTI)
#define DFRF_SAVE   (1<<DFRB_SAVE)

