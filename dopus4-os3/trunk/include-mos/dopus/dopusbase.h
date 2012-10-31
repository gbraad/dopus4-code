#ifndef  DOPUSLIB_H
#define  DOPUSLIB_H





#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif
struct DOpusBase {
    struct Library LibNode;
    UBYTE Flags;
    UBYTE pad;
    struct ExecBase *ExecBase;
    struct DosLibrary *DOSBase;
    struct IntuitionBase *IntuitionBase;
    struct GfxBase *GfxBase;
    struct LayersBase *LayersBase;
    
    ULONG pdb_cycletop;
    ULONG pdb_cyclebot;
    ULONG pdb_check;
    ULONG pad1;
    ULONG pad2;
    ULONG pad3;
    ULONG pad4;
    ULONG pdb_Flags;
    ULONG SegList;
} __attribute__((packed));



#define DIRBUF_SIZE  256
#define FILEBUF_SIZE 108 // 32
struct DOpusFileReq {
    char *title;            
    char *dirbuf,*filebuf;  
    struct Window *window;  
    int x,y;                
    int lines;              
    ULONG flags;            
    struct DOpusRemember
        *filearraykey;        
    char **filearray;       
} __attribute__((packed));



#define DFRB_DIRREQ 0 
#define DFRB_MULTI  1 
#define DFRB_SAVE   2 
#define DFRB_FONT   3 

#define DFRF_DIRREQ (1<<DFRB_DIRREQ)
#define DFRF_MULTI  (1<<DFRB_MULTI)
#define DFRF_SAVE   (1<<DFRB_SAVE)
#define DFRF_FONT   (1<<DFRB_FONT)



#define RETURN_OKAY    1
#define RETURN_CANCEL  0
#define ERROR_NOMEM   -1



#define LOCK_DIFFERENT   -1
#define LOCK_SAME         0
#define LOCK_SAME_VOLUME  1



#define ASSIGN_OK     0
#define ASSIGN_NODEV  1
#define ASSIGN_NOMEM  2
#define ASSIGN_CANCEL 3
struct DOpusDateTime {
    struct DateStamp dat_Stamp; 
    UBYTE dat_Format;           
    UBYTE dat_Flags;            
    char * __attribute__((aligned(2))) dat_StrDay;           
    char * __attribute__((aligned(2))) dat_StrDate;          
    char * __attribute__((aligned(2))) dat_StrTime;          
//    char **custom_weekdays;     
//    char **custom_shortweekdays;
//    char **custom_months;       
//    char **custom_shortmonths;  
//    char **custom_special_days; 
} __attribute__((packed));

#define DDTB_SUBST  0 
#define DDTB_FUTURE 1 

#define DDTB_CUSTOM 6 
#define DDTB_12HOUR 7 

#define DDTF_SUBST  (1<<DDTB_SUBST)
#define DDTF_FUTURE (1<<DDTB_FUTURE)

#define DDTF_CUSTOM (1<<DDTB_CUSTOM)
#define DDTF_12HOUR (1<<DDTB_12HOUR)

#define FORMAT_DOS 0 
#define FORMAT_INT 1 
#define FORMAT_USA 2 
#define FORMAT_CDN 3 
#define FORMAT_AUS FORMAT_CDN
struct DOpusListView {
    ULONG listid;          
    struct Window *window; 
    int x,y;               
    int w,h;               
    char *title;           
    char **items;          
    char *selectarray;     
                           
    ULONG flags;           
    int sliderwidth;       
    int slidercol;         
    int sliderbgcol;       
    int textcol;           
    int boxhi,boxlo;       
    int arrowfg,arrowbg;   
    int itemfg,itembg;     
    int topitem;           
    int itemselected;      
    struct DOpusListView
        *next;               

    

    int oldoffset,count,lines,columns;
    struct DOpusRemember *key;
    struct Gadget *listgads;
    struct Image *listimage;
    struct PropInfo *listprop;
    struct Region *listregion;
    int gadadd,fw,fh,fb,xo,yo,mx,my,chk,ty;
    char ofg,obg,odm;
    char * __attribute__((aligned(2))) selected;
} __attribute__((packed));



#define DLVB_MULTI  0 
#define DLVB_LEAVE  1 
#define DLVB_CHECK  2 
#define DLVB_TOGGLE 3 
#define DLVB_SLOW   4 
#define DLVB_DUMB   5 
#define DLVB_TTOP   6 
#define DLVB_ENDNL  7 
#define DLVB_HIREC  8 

#define DLVF_MULTI  (1<<DLVB_MULTI)
#define DLVF_LEAVE  (1<<DLVB_LEAVE)
#define DLVF_CHECK  (1<<DLVB_CHECK)
#define DLVF_TOGGLE (1<<DLVB_TOGGLE)
#define DLVF_SLOW   (1<<DLVB_SLOW)
#define DLVF_DUMB   (1<<DLVB_DUMB)
#define DLVF_TTOP   (1<<DLVB_TTOP)
#define DLVF_ENDNL  (1<<DLVB_ENDNL)
#define DLVF_HIREC  (1<<DLVB_HIREC)



#define LVARRAY_SELECTED     1   
#define LVARRAY_DISABLED     2   
struct DOpusRemember {
    struct DOpusRemember *next,*current;
    ULONG size;
} __attribute__((packed));
struct RMBGadgetText {
    char *text;                    
    int x,y;                       
    int w,h;                       
    int fg,bg;                     
} __attribute__((packed));
struct RMBGadget {
    struct RMBGadget *next;        
    int x,y;                       
    int w,h;                       
    int flags;                     
    int id;                        
    struct RMBGadgetText txt[2];   
    struct Border *high_bord[2];   
} __attribute__((packed));

#define RGB_ALTTEXT 0  
#define RGB_ALTBORD 1  

#define RGF_ALTTEXT (1<<RGB_ALTTEXT)
#define RGF_ALTBORD (1<<RGB_ALTBORD)



#define GAD_NONE  -1
#define GAD_CYCLE 1
#define GAD_CHECK 2
#define GAD_RADIO 3
#define GAD_GLASS 4
struct DOpusSimpleRequest {
    char *text;                 
    char **gads;                
    int *rets;                  
    int hi,lo;                  
    int fg,bg;                  
    char *strbuf;               
    int strlen;                 
    int flags;                  
    struct TextFont *font;      
    int value;                  
    char *title;                
} __attribute__((packed));

#define SRB_LONGINT   0  
#define SRB_CENTJUST  1  
#define SRB_RIGHTJUST 2  
#define SRB_GLASS     3  
#define SRB_DIRGLASS  4  
#define SRB_BORDERS   5  
#define SRB_NOCENTER  6  
#define SRB_RECESSHI  7  
#define SRB_EXTEND    8  

#define SRF_LONGINT   (1<<SRB_LONGINT)
#define SRF_CENTJUST  (1<<SRB_CENTJUST)
#define SRF_RIGHTJUST (1<<SRB_RIGHTJUST)
#define SRF_GLASS     (1<<SRB_GLASS)
#define SRF_DIRGLASS  (1<<SRB_DIRGLASS)
#define SRF_BORDERS   (1<<SRB_BORDERS)
#define SRF_NOCENTER  (1<<SRB_NOCENTER)
#define SRF_RECESSHI  (1<<SRB_RECESSHI)
#define SRF_EXTEND    (1<<SRB_EXTEND)

#define SYSFILE_COMMAND    0
#define SYSFILE_DATA       1
#define SYSFILE_LIBRARY    2
#define SYSFILE_REXX       3
#define SYSFILE_MODULE     4
#define SYSFILE_REQUESTER  5

#define ERROR_NOT_CONFIG -1

#endif 

