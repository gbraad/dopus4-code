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

#include "diskop.h"

#define DISKTYPE_UNKNOWN 	0
#define DISKTYPE_DOS 		1
#define DISKTYPE_SFS 		2
#define DISKTYPE_PFS 		3
#define DISKTYPE_FAT		4

#define GADGETS_NONE	0
#define GADGETS_FFS		(1<<0)
#define GADGETS_SFS		(1<<1)
#define GADGETS_PFS		(1<<2)
#define GADGETS_ALL		(GADGETS_FFS | GADGETS_SFS | GADGETS_PFS)

/* SFS specific definitions */

#define SFS_PACKET_BASE         (0xf00000)
#define ACTION_SFS_FORMAT       (SFS_PACKET_BASE + 100 + 3)

#define ASFBASE            (TAG_USER)
#define ASF_NAME           (ASFBASE + 1)
#define ASF_NORECYCLED     (ASFBASE + 2)
#define ASF_CASESENSITIVE  (ASFBASE + 3)
#define ASF_SHOWRECYCLED   (ASFBASE + 4)

/* PFS specific definitions */

#define ACTION_IS_PFS2	2211
#define ACTION_SET_DELDIR  2221
#define ACTION_SET_FNSIZE  2222

#define ID_PFS_DISK     (0x50465301L)   /* 'PFS\1' */
#define ID_PFS2_DISK    (0x50465302L)   /* 'PFS\2' */
#define ID_PFS3_DISK    (0x50465303L)   /* 'PFS\3' */
#define ID_MUPFS_DISK   (0x6d755046L)   /* 'muPF'  */
#define ID_AFS_DISK     (0x41465301L)   /* 'AFS\1' */


static struct NewGadget ng[] = {
	{0,0,13,6,NULL,NULL,FORMAT_DEV,0,NULL,NULL},
    {22,0,22,1,NULL,NULL,FORMAT_NAME,PLACETEXT_LEFT,NULL,NULL},
    {15,2,0,0,NULL,NULL,FORMAT_FFS,PLACETEXT_RIGHT,NULL,NULL},
    {15,3,0,0,NULL,NULL,FORMAT_INTERNATIONAL,PLACETEXT_RIGHT,NULL,NULL},
    {15,4,0,0,NULL,NULL,FORMAT_CACHING,PLACETEXT_RIGHT,NULL,NULL},
    {15,5,0,0,NULL,NULL,FORMAT_TRASHCAN,PLACETEXT_RIGHT,NULL,NULL},
    {15,6,0,0,NULL,NULL,FORMAT_VERIFY,PLACETEXT_RIGHT,NULL,NULL},
    {0,7,44,1,NULL,NULL,FORMAT_INFO,0,NULL,NULL},
    {0,8,14,1,NULL,NULL,FORMAT_FORMAT,PLACETEXT_IN,NULL,NULL},
    {15,8,14,1,NULL,NULL,FORMAT_QUICKFORMAT,PLACETEXT_IN,NULL,NULL},
    {30,8,14,1,NULL,NULL,FORMAT_CANCEL,PLACETEXT_IN,NULL,NULL},
    {15,2,0,0,NULL,NULL,FORMAT_SFS_CASE,PLACETEXT_RIGHT,NULL,NULL},
    {15,3,0,0,NULL,NULL,FORMAT_SFS_SHOWREC,PLACETEXT_RIGHT,NULL,NULL},
    {15,4,0,0,NULL,NULL,FORMAT_SFS_NOREC,PLACETEXT_RIGHT,NULL,NULL},
    {19,2,6,1,NULL,NULL,FORMAT_PFS_DELDIR,PLACETEXT_RIGHT,NULL,NULL},
    {19,3,6,1,NULL,NULL,FORMAT_PFS_FNSIZE,PLACETEXT_RIGHT,NULL,NULL},
};
/*
struct TagItem
    format_name_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_STRING},
        {RO_GadgetID,FORMAT_NAME},
        {RO_Left,19},
        {RO_Width,21},
        {RO_Height,1},
        {RO_TextNum,STR_FORMAT_NAME},
        {RO_TextPos,TEXTPOS_LEFT},
        {RO_StringLen,31},
        {TAG_END,0}},
    format_ffs_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_CHECK},
        {RO_GadgetID,FORMAT_FFS},
        {RO_Left,14},
        {RO_Top,2},
        {RO_TextNum,STR_FORMAT_FFS},
        {RO_TextPos,TEXTPOS_RIGHT},
        {TAG_END,0}},
    format_international_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_CHECK},
        {RO_GadgetID,FORMAT_INTERNATIONAL},
        {RO_Left,14},
        {RO_Top,3},
        {RO_TopFine,5},
        {RO_TextNum,STR_FORMAT_INTERNATIONAL},
        {RO_TextPos,TEXTPOS_RIGHT},
        {TAG_END,0}},
    format_caching_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_CHECK},
        {RO_GadgetID,FORMAT_CACHING},
        {RO_Left,14},
        {RO_Top,4},
        {RO_TopFine,10},
        {RO_TextNum,STR_FORMAT_CACHING},
        {RO_TextPos,TEXTPOS_RIGHT},
        {TAG_END,0}},
    format_trashcan_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_CHECK},
        {RO_GadgetID,FORMAT_TRASHCAN},
        {RO_Left,14},
        {RO_Top,5},
        {RO_TopFine,15},
        {RO_TextNum,STR_FORMAT_TRASHCAN},
        {RO_TextPos,TEXTPOS_RIGHT},
        {TAG_END,0}},
    format_verify_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_CHECK},
        {RO_GadgetID,FORMAT_VERIFY},
        {RO_Left,14},
        {RO_Top,6},
        {RO_TopFine,20},
        {RO_TextNum,STR_FORMAT_VERIFY},
        {RO_TextPos,TEXTPOS_RIGHT},
        {RO_BoolOn,TRUE},
        {TAG_END,0}},
    format_format_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_BOOLEAN},
        {RO_GadgetID,FORMAT_FORMAT},
        {RO_Top,8},
        {RO_TopFine,37},
        {RO_Width,13},
        {RO_Height,1},
        {RO_HeightFine,4},
        {RO_TextNum,STR_FORMAT_FORMAT},
        {RO_TextPos,TEXTPOS_CENTER},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},
    format_quickformat_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_BOOLEAN},
        {RO_GadgetID,FORMAT_QUICKFORMAT},
        {RO_Left,14},
        {RO_Top,8},
        {RO_TopFine,37},
        {RO_Width,13},
        {RO_Height,1},
        {RO_HeightFine,4},
        {RO_TextNum,STR_FORMAT_QUICKFORMAT},
        {RO_TextPos,TEXTPOS_CENTER},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},
    format_cancel_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_BOOLEAN},
        {RO_GadgetID,FORMAT_CANCEL},
        {RO_Left,28},
        {RO_Top,8},
        {RO_TopFine,37},
        {RO_Width,13},
        {RO_Height,1},
        {RO_HeightFine,4},
        {RO_TextNum,STR_FORMAT_EXIT},
        {RO_TextPos,TEXTPOS_CENTER},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},

    format_device_list[]={
        {RO_Type,OBJECT_LISTVIEW},
        {RO_ListViewID,0},
        {RO_LeftFine,2},
        {RO_Width,10},
        {RO_Height,6},
        {RO_HeightFine,30},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},

    format_info_box[]={
        {RO_Type,OBJECT_BORDER},
        {RO_BorderType,BORDER_RECESSED},
        {RO_LeftFine,2},
        {RO_Top,7},
        {RO_TopFine,28},
        {RO_Width,41},
        {RO_WidthFine,-4},
        {RO_Height,1},
        {RO_HeightFine,4},
        {TAG_END,0}},
        
    *format_gadgets[]={
        format_name_gadget,
        format_ffs_gadget,
        format_international_gadget,
        format_caching_gadget,
        format_trashcan_gadget,
        format_verify_gadget,
        format_format_gadget,
        format_quickformat_gadget,
        format_cancel_gadget,
        NULL};
*/
static const char stringmap[] =
 { ~0, STR_FORMAT_NAME, STR_FORMAT_FFS, STR_FORMAT_INTERNATIONAL, STR_FORMAT_CACHING,
   STR_FORMAT_TRASHCAN, STR_FORMAT_VERIFY, ~0,
   STR_FORMAT_FORMAT, STR_FORMAT_QUICKFORMAT, STR_FORMAT_EXIT,
   STR_FORMAT_SFS_CASE, STR_FORMAT_SFS_SHOWREC, STR_FORMAT_SFS_NOREC,
   STR_FORMAT_PFS_DELDIR, STR_FORMAT_PFS_FNSIZE,
 };

int identifyFS(ULONG disktype)
 {
  if ((disktype>>8) == 'DOS')
   {
D(bug("DOS disk\n"));
    return DISKTYPE_DOS;
   }
  else if ((disktype>>8) == 'SFS')
   {
D(bug("SFS disk\n"));
    return DISKTYPE_SFS;
   }
  else if ((disktype>>8) == 'PFS')
   {
D(bug("PFS disk\n"));
    return DISKTYPE_PFS;
   }
  else
   {
D(bug("unsupported disk type: 0x%08lx\n",disktype));
    return DISKTYPE_UNKNOWN;
   }
 }

void dynamicgads(struct Window *window,struct Gadget *gads[],UBYTE bgpen,int mode)
 {
  RemoveGList(window,gads[10],-1);
  if (mode != GADGETS_ALL)
   {
    SetAPen(window->RPort,bgpen);
    RectFill(window->RPort,ng[0].ng_LeftEdge+ng[0].ng_Width,gads[2]->TopEdge,window->Width-window->BorderRight-1,gads[5]->TopEdge-1);
   }
  AddGadget(window,gads[10],-1);

  if (mode & GADGETS_FFS) AddGList(window,gads[2],-1,3,NULL);
  if (mode & GADGETS_SFS) AddGList(window,gads[11],-1,3,NULL);
  if (mode & GADGETS_PFS) AddGList(window,gads[14],-1,4,NULL);
 }

void dynamicgadsbydevice(struct Window *window, struct Gadget *gads[], UBYTE bgpen, char *device)
 {
  char *c;
  int mode;

  if ((c = strchr(device,':')))
   {
    c++;
    switch (identifyFS(*((int *)c)))
     {
      case DISKTYPE_DOS:
        mode = GADGETS_FFS;
        break;
      case DISKTYPE_SFS:
        mode = GADGETS_SFS;
        break;
      case DISKTYPE_PFS:
        mode = GADGETS_PFS;
        break;
      case DISKTYPE_UNKNOWN:
        mode = GADGETS_NONE;
        break;
     }
    dynamicgads(window,gads,bgpen,mode);
   }
 }

void diskop_format(/*vis,*/portname,argc,argv)
//struct VisInfo *vis;
char *portname;
int argc;
char *argv[];
{
    struct Window *window;
    struct IntuiMessage *msg;
//    Object_Border *infobox;
    struct DOpusListView *devicelist/*,*view*/;
//    struct Gadget *gadlist,*gad,*gad_international=NULL,*gad_caching=NULL;
    ULONG class/*,mask,flags*/;
    USHORT code,gadgetid;
    APTR iaddress;
//    struct StringExtend stringex;
//    char *diskname;
    int a,b,/*gadcount=0,*/ignorechange=0;
    int start=-1,quick=0/*,startflags=FORMATFLAG_TRASHCAN*/;
    unsigned char keys[FORMAT_GAD_LAST-2];

    struct Gadget *gtgads[FORMAT_GAD_LAST], *gtglist=NULL;
    APTR gt_vi;
    struct TextFont *tf;
    struct TextAttr ta;
    struct List gt_devlist;
    struct formatP prefs;

    prefs.pfsdeldir=2;
    prefs.pfsfnsize=31;

    req.rb_width=44;
    req.rb_height=9;
    req.rb_widthfine=16;
    req.rb_heightfine=54;
    req.rb_title=string_table[STR_WINDOW_FORMAT];

/*
    for (a=0;a<2;a++) {
        stringex.Pens[a]=vis->vi_stringcol[a];
        stringex.ActivePens[a]=vis->vi_activestringcol[a];
    }
    stringex.InitialModes=0;
    stringex.EditHook=NULL;
    stringex.WorkBuffer=NULL;
    for (a=0;a<4;a++) stringex.Reserved[a]=0;

    if (SysBase->LibNode.lib_Version<39) mask=12;
    else mask=0;
*/
    if (!(window=OpenRequester(&req)) ||
//        !(gadlist=addreqgadgets(&req,format_gadgets,mask,&gadcount)) ||
//        !(infobox=(Object_Border *)AddRequesterObject(&req,format_info_box)) ||
//        !(devicelist=(struct DOpusListView *)AddRequesterObject(&req,format_device_list)) ||
        !(devicelist=(struct DOpusListView *)LAllocRemember(&req.rb_memory,sizeof(struct DOpusListView),MEMF_ANY|MEMF_CLEAR)) ||
        !(devicelist->items=get_device_list(&req.rb_memory,NULL))) {
        CloseRequester(&req);
        return;
    }

//    fix_listview(&req,devicelist);
//    select_device(devicelist,NULL);

//    diskname=((struct StringInfo *)gadlist->SpecialInfo)->Buffer;
    strcpy(prefs.name,string_table[STR_FORMAT_EMPTY]);

//    get_env("format",gadlist,gadcount,devicelist);
    prefs.srclist = devicelist;
    prefs.flags = FORMATFLAG_TRASHCAN;
    get_env((union DD_Prefs *)&prefs,PREFS_FORMAT);
D(bug("format flags: %lx,default device:%s(%ld)\n",prefs.flags,devicelist->items[devicelist->itemselected],devicelist->itemselected));
/*
    flags=0;
    gad=gadlist;
    for (a=0;a<gadcount;a++) {
        if (gad->Activation&GACT_TOGGLESELECT)
            if (gad->Flags&GFLG_SELECTED) flags|=1<<(gad->GadgetID-1);
        gad->Flags&=~GFLG_DISABLED;
        gad=gad->NextGadget;
    }
    if (SysBase->LibNode.lib_Version>38) {
        gad_international=gadlist->NextGadget->NextGadget;
        gad_caching=gad_international->NextGadget;
    }
*/
    for (a=0;a<argc;a++) {
        if (LStrCmpI(argv[a],"quick")==0) quick=1;
        else if (LStrCmpI(argv[a],"ffs")==0) prefs.flags|=FORMATFLAG_FFS;
        else if (LStrCmpI(argv[a],"cache")==0) prefs.flags|=FORMATFLAG_CACHING;
        else if (LStrnCmpI(argv[a],"inter",5)==0) prefs.flags|=FORMATFLAG_INTERNATIONAL;
        else if (LStrCmpI(argv[a],"noicons")==0) prefs.flags&=~FORMATFLAG_TRASHCAN;
        else if (LStrCmpI(argv[a],"verify")==0) prefs.flags|=FORMATFLAG_VERIFY;
        else {
            if (start==-1) {
                for (b=0;devicelist->items[b];b++) {
                    if (LStrCmpI(argv[a],devicelist->items[b])==0) {
                        devicelist->itemselected=start=b;
                        break;
                    }
                }
            }
            else LStrnCpy(prefs.name,argv[a],30);
        }
    }

    for (a=0; devicelist->items[a];a++)
     {
      struct DeviceNode *devnode;
      ULONG dostype = ID_NO_DISK_PRESENT;
      BPTR lock = Lock(devicelist->items[a],ACCESS_READ);

      if (lock)
       {
        __aligned struct InfoData id;

        if (Info(lock,&id)) dostype = id.id_DiskType;
        UnLock(lock);
       }
      if ((devnode=find_device(devicelist->items[a])))
       {
        struct DosEnvec *dosenvec = (struct DosEnvec *)BADDR(((struct FileSysStartupMsg *)BADDR(devnode->dn_Startup))->fssm_Environ);

        if (dosenvec->de_TableSize >= DE_DOSTYPE) dostype = dosenvec->de_DosType;
       }
      if (dostype != ID_NO_DISK_PRESENT)
       {
        b = strlen(devicelist->items[a]);
        devicelist->items[a][b++] = dostype >> 24;
        devicelist->items[a][b++] = (dostype >> 16) & 0xFF;
        devicelist->items[a][b++] = (dostype >> 8) & 0xFF;
        devicelist->items[a][b] = (dostype) & 0xFF;
        if (devicelist->items[a][b] < 10) devicelist->items[a][b] += '0';
        devicelist->items[a][++b] = 0;
       }
     }
/*
    if (start>-1) {
        gad=gadlist;
        for (a=0;a<gadcount;a++) {
            if (startflags&(1<<(gad->GadgetID-1))) gad->Flags|=GFLG_SELECTED;
            else gad->Flags&=~GFLG_SELECTED;
            gad=gad->NextGadget;
        }
    }
*/
    tf = window->RPort->Font;
    ta.ta_Name = tf->tf_Message.mn_Node.ln_Name;
    ta.ta_YSize = tf->tf_YSize;
    ta.ta_Style = tf->tf_Style;
    ta.ta_Flags = tf->tf_Flags;
    gt_vi = GetVisualInfoA(window->WScreen,NULL);
    for (a=0; a<FORMAT_GAD_LAST; a++)
     {
      ng[a].ng_VisualInfo = gt_vi;
      ng[a].ng_TextAttr = &ta;
      if (stringmap[a] != ~0) ng[a].ng_GadgetText = string_table[stringmap[a]];
      ng[a].ng_LeftEdge *= tf->tf_XSize;
      ng[a].ng_LeftEdge += window->BorderLeft + 8;
      ng[a].ng_TopEdge *= tf->tf_YSize;
      ng[a].ng_TopEdge += window->BorderTop + 8;
      ng[a].ng_Width *= tf->tf_XSize;
      ng[a].ng_Height *= tf->tf_YSize;
      ng[a].ng_Height += 4;
     }
    ng[0].ng_Height += 30;
    ng[12].ng_TopEdge = ng[3].ng_TopEdge += 5;
    ng[14].ng_TopEdge += 5;
    ng[13].ng_TopEdge = ng[4].ng_TopEdge += 10;
    ng[15].ng_TopEdge += 15;
    ng[5].ng_TopEdge += 15;
    ng[6].ng_TopEdge += 20;
    ng[7].ng_TopEdge += 28;
    ng[8].ng_TopEdge = ng[9].ng_TopEdge = ng[10].ng_TopEdge += 37;

D(for (a=0; a<FORMAT_GAD_LAST; a++) bug("Gadget %ld: %ld/%ld/%ld/%ld/%s\n",a,ng[a].ng_LeftEdge,ng[a].ng_TopEdge,ng[a].ng_Width,ng[a].ng_Height,ng[a].ng_GadgetText?ng[a].ng_GadgetText:"<no name>"));

    NewList(&gt_devlist);
    {
     struct Node *node;
     for (a=0; devicelist->items[a]; a++)
      {
       if ((node = LAllocRemember(&req.rb_memory,sizeof(struct Node),MEMF_ANY)))
        {
         node->ln_Name = devicelist->items[a];
         AddTail(&gt_devlist,node);
        }
      }
    }

    CreateContext(&gtglist);
    gtgads[0] = CreateGadget(LISTVIEW_KIND,gtglist,&ng[0],
    				GTLV_Labels, (Tag)&gt_devlist,
                    GTLV_Selected, devicelist->itemselected,
                    GTLV_MakeVisible, devicelist->itemselected,
                    GTLV_ShowSelected, NULL,
                    TAG_END);
    gtgads[1] = CreateGadget(STRING_KIND,gtgads[0],&ng[1],
                    GTST_String, (Tag)prefs.name,
                    GTST_MaxChars, 30,
                    TAG_MORE, (Tag)commonGTtags);
    gtgads[5] = CreateGadget(CHECKBOX_KIND,gtgads[1],&ng[5],
                    GTCB_Checked, prefs.flags & FORMATFLAG_TRASHCAN,
                    TAG_MORE, (Tag)commonGTtags);
    gtgads[6] = CreateGadget(CHECKBOX_KIND,gtgads[5],&ng[6],
                    GTCB_Checked, prefs.flags & FORMATFLAG_VERIFY,
                    TAG_MORE, (Tag)commonGTtags);
    gtgads[7] = CreateGadget(TEXT_KIND,gtgads[6],&ng[7],
                    GTTX_Border, TRUE,
                    GTTX_Justification, GTJ_CENTER,
                    GTTX_FrontPen, req.rb_fg,
                    GTTX_BackPen,  req.rb_bg,
                    TAG_END);
    for (a=8; a<11; a++) gtgads[a] = CreateGadgetA(BUTTON_KIND,gtgads[a-1],&ng[a],commonGTtags);
/* FFS specific options */
    gtgads[2] = CreateGadget(CHECKBOX_KIND,gtgads[10],&ng[2],
                    GTCB_Checked, prefs.flags & FORMATFLAG_FFS,
                    TAG_MORE, (Tag)commonGTtags);
    gtgads[3] = CreateGadget(CHECKBOX_KIND,gtgads[2],&ng[3],
                    GTCB_Checked, prefs.flags & (FORMATFLAG_INTERNATIONAL|FORMATFLAG_CACHING),
                    GA_Disabled, prefs.flags & FORMATFLAG_CACHING,
                    TAG_MORE, (Tag)commonGTtags);
    gtgads[4] = CreateGadget(CHECKBOX_KIND,gtgads[3],&ng[4],
                    GTCB_Checked, prefs.flags & FORMATFLAG_CACHING,
                    TAG_MORE, (Tag)commonGTtags);
/* SFS specific options */
    gtgads[11] = CreateGadget(CHECKBOX_KIND,gtgads[4],&ng[11],
                    GTCB_Checked, prefs.flags & FORMATFLAG_SFS_CASE,
                    TAG_MORE, (Tag)commonGTtags);
    gtgads[12] = CreateGadget(CHECKBOX_KIND,gtgads[11],&ng[12],
                    GTCB_Checked, prefs.flags & FORMATFLAG_SFS_SHOWREC,
                    GA_Disabled, prefs.flags & FORMATFLAG_SFS_NOREC,
                    TAG_MORE, (Tag)commonGTtags);
    gtgads[13] = CreateGadget(CHECKBOX_KIND,gtgads[12],&ng[13],
                    GTCB_Checked, prefs.flags & FORMATFLAG_SFS_NOREC,
                    TAG_MORE, (Tag)commonGTtags);
/* PFS specific options */
    gtgads[14] = CreateGadget(SLIDER_KIND,gtgads[13],&ng[14],
                    GTSL_Min, 0,
                    GTSL_Max, 32,
                    GTSL_Level, prefs.pfsdeldir,
                    GTSL_LevelFormat, "%3ld",
                    GTSL_MaxLevelLen, 3,
                    TAG_MORE, (Tag)commonGTtags);
    gtgads[15] = CreateGadget(SLIDER_KIND,gtgads[14],&ng[15],
                    GTSL_Min, 31,
                    GTSL_Max, 107,
                    GTSL_Level, prefs.pfsfnsize,
                    GTSL_LevelFormat, "%3ld",
                    GTSL_MaxLevelLen, 3,
                    TAG_MORE, (Tag)commonGTtags);

    gtgads[15] = gtgads[14]->NextGadget;
    gtgads[14] = gtgads[13]->NextGadget;

D({struct Gadget *g; for(g=gtglist;g;g=g->NextGadget) bug("gad %lx - %ld\n",g,g->GadgetID);});
D(for (a=0; a<FORMAT_GAD_LAST; a++) bug("gtgad[%ld] = %lx\n",a,gtgads[a]));

    AddGList(window,gtglist,-1,-1,NULL);
    dynamicgadsbydevice(window,gtgads,req.rb_bg,devicelist->items[devicelist->itemselected]);
    RefreshGList(gtglist,window,NULL,-1);
//    RefreshRequesterObject(&req,NULL);
//    RefreshGList(gadlist,window,NULL,gadcount);
    GT_RefreshWindow(window,NULL);
    ModifyIDCMP(window,IDCMP_REFRESHWINDOW|IDCMP_CLOSEWINDOW|IDCMP_DISKINSERTED|IDCMP_DISKREMOVED|IDCMP_VANILLAKEY |
                       STRINGIDCMP|LISTVIEWIDCMP|CHECKBOXIDCMP|BUTTONIDCMP|TEXTIDCMP);
/*
    if (flags&FORMATFLAG_CACHING && gad_international) {
        flags|=FORMATFLAG_INTERNATIONAL;
        gad_international->Flags|=GFLG_SELECTED;
        RefreshGList(gad_international,window,NULL,1);
        DisableGadget(gad_international,window->RPort,0,0);
    }

    if (!(AddListView(devicelist,1))) {
        CloseRequester(&req);
        return;
    }
    show_sel_item(devicelist);
*/
    show_device_info(&req,gtgads[7]/*infobox*/,devicelist->items[devicelist->itemselected]);

    if (start>-1) {
        if (do_format(&req,
            gtgads[7]/*infobox*/,
            devicelist->items[start],
            &prefs/*.name,
            prefs.flags*/,
            quick))
            dopus_message(DOPUSMSG_UPDATEDRIVE,(APTR)devicelist->items[start],portname);
//        RemoveListView(devicelist,1);
        AddGList(window,gtgads[11],-1,-1,NULL);
        CloseRequester(&req);
        FreeGadgets(gtglist);
        FreeVisualInfo(gt_vi);
        return;
    }

    for(a = 0, b = 0; a < (FORMAT_GAD_LAST-2); b++)
     {
      if (stringmap[b] != ~0) keys[a++] = getkeyshortcut(string_table[stringmap[b]]);
     }

    FOREVER {
        while ((msg=(struct IntuiMessage *)GT_GetIMsg(window->UserPort))) {
/*            if ((view=(struct DOpusListView *)ListViewIDCMP(devicelist,msg))==
                (struct DOpusListView *)-1)*/ {
                class=msg->Class;
                code=msg->Code;
                iaddress=msg->IAddress;
                ReplyMsg((struct Message *)msg);

                switch (class) {
                    case IDCMP_REFRESHWINDOW:
D(bug("window refresh!\n"));
                        GT_BeginRefresh(window);
                        GT_EndRefresh(window,TRUE);
                        break;
                    case IDCMP_DISKINSERTED:
                    case IDCMP_DISKREMOVED:
                        if (ignorechange) ignorechange=0;
                        else {
                            show_device_info(&req,gtgads[7]/*infobox*/,devicelist->items[devicelist->itemselected]);
                        }
                        break;

                    case IDCMP_VANILLAKEY:  
//                        a=0;
                        code=LToLower(code);
                        if (code == keys[0]) {
                                ActivateStrGad(gtgads[1]/*gadlist*/,window);
                        }
                        else if (code == keys[1]) {
//                                a=1;
                                prefs.flags^=FORMATFLAG_FFS;
                                GT_SetGadgetAttrs(gtgads[2],window,NULL,GTCB_Checked,prefs.flags & FORMATFLAG_FFS,TAG_END);
                        }
                        else if (code == keys[2]) {
//                                if (SysBase->LibNode.lib_Version>38) {
//                                    a=2;
                                if (!(prefs.flags & FORMATFLAG_CACHING))
                                {
                                  prefs.flags^=FORMATFLAG_INTERNATIONAL;
                                  GT_SetGadgetAttrs(gtgads[3],window,NULL,GTCB_Checked,prefs.flags & FORMATFLAG_INTERNATIONAL,TAG_END);
                                }
                        }
                        else if (code == keys[3]) {
//                                if (SysBase->LibNode.lib_Version>38) {
//                                    a=3;
                                    prefs.flags^=FORMATFLAG_CACHING;
                                GT_SetGadgetAttrs(gtgads[4],window,NULL,GTCB_Checked,prefs.flags & FORMATFLAG_CACHING,TAG_END);
                                if (prefs.flags & FORMATFLAG_CACHING)
                                 {
                                  prefs.flags |= FORMATFLAG_INTERNATIONAL;
                                  GT_SetGadgetAttrs(gtgads[3],window,NULL,GTCB_Checked,TRUE,GA_Disabled,TRUE,TAG_END);
                                 }
                                else
                                  GT_SetGadgetAttrs(gtgads[3],window,NULL,GA_Disabled,FALSE,TAG_END);
//                                }
                        }
                        else if (code == keys[4]) {
//                                if (SysBase->LibNode.lib_Version>38) a=4;
//                                else a=2;
                                prefs.flags^=FORMATFLAG_TRASHCAN;
                                GT_SetGadgetAttrs(gtgads[5],window,NULL,GTCB_Checked,prefs.flags & FORMATFLAG_TRASHCAN,TAG_END);
                        }
                        else if (code == keys[5]) {
//                                if (SysBase->LibNode.lib_Version>38) a=5;
//                                else a=3;
                                prefs.flags^=FORMATFLAG_VERIFY;
                                GT_SetGadgetAttrs(gtgads[6],window,NULL,GTCB_Checked,prefs.flags & FORMATFLAG_VERIFY,TAG_END);
                        }
                        else if ((code == '\r') || (code == keys[6]) || (code == keys[7])) {
                                strcpy(((struct StringInfo *)gtgads[1]->SpecialInfo)->Buffer,prefs.name);
                                if (do_format(&req,
                                    gtgads[7]/*infobox*/,
                                    devicelist->items[devicelist->itemselected],
                                    &prefs/*.name,
                                    prefs.flags*/,
                                    (code == keys[7]))) {
                                    ignorechange=1;
                                    dopus_message(DOPUSMSG_UPDATEDRIVE,
                                        (APTR)devicelist->items[devicelist->itemselected],
                                        portname);
                                }
                        }
                        else if ((code == keys[8]) || (code == 0x1B)) {
//                                set_env("format",gadlist,gadcount,devicelist);
                                set_env((union DD_Prefs *)&prefs,PREFS_FORMAT);
//                                RemoveListView(devicelist,1);
                                dynamicgads(window,gtgads,0,GADGETS_ALL);
                                CloseRequester(&req);
                                FreeGadgets(gtglist);
                                FreeVisualInfo(gt_vi);
                                return;
                        }
                        else if (code == keys[9]) {
                                prefs.flags^=FORMATFLAG_SFS_CASE;
                                GT_SetGadgetAttrs(gtgads[11],window,NULL,GTCB_Checked,prefs.flags & FORMATFLAG_SFS_CASE,TAG_END);
                        }
                        else if (code == keys[10]) {
                                if (!(prefs.flags & FORMATFLAG_SFS_SHOWREC))
                                {
                                  prefs.flags^=FORMATFLAG_SFS_SHOWREC;
                                  GT_SetGadgetAttrs(gtgads[12],window,NULL,GTCB_Checked,prefs.flags & FORMATFLAG_SFS_SHOWREC,TAG_END);
                                }
                        }
                        else if (code == keys[11]) {
                                prefs.flags^=FORMATFLAG_SFS_NOREC;
                                GT_SetGadgetAttrs(gtgads[13],window,NULL,GTCB_Checked,prefs.flags & FORMATFLAG_SFS_NOREC,TAG_END);
                                GT_SetGadgetAttrs(gtgads[12],window,NULL,GA_Disabled,(prefs.flags & FORMATFLAG_SFS_NOREC)?TRUE:FALSE,TAG_END);
                        }
/*
                        if (a) {
                            gad=gadlist;
                            while (a-- && gad->NextGadget) gad=gad->NextGadget;
                            gad->Flags^=GFLG_SELECTED;
                            RefreshGList(gad,window,NULL,1);
                            if (gad==gad_international) {
                                gad_caching->Flags&=~GFLG_SELECTED;
                                RefreshGList(gad_caching,window,NULL,1);
                                flags&=~FORMATFLAG_CACHING;
                            }
                            else if (gad==gad_caching) {
                                gad_international->Flags&=~GFLG_SELECTED;
                                RefreshGList(gad_international,window,NULL,1);
                                flags&=~FORMATFLAG_INTERNATIONAL;
                            }
                        }
*/
                        break;

                    case IDCMP_GADGETDOWN:
                    case IDCMP_GADGETUP:
                        gadgetid=((struct Gadget *)iaddress)->GadgetID;
                    case IDCMP_CLOSEWINDOW:
                        if (class==IDCMP_CLOSEWINDOW) gadgetid=FORMAT_CANCEL;

                        switch (gadgetid) {
                            case FORMAT_NAME:
                                strcpy(prefs.name,((struct StringInfo *)gtgads[1]->SpecialInfo)->Buffer);
                                break;
                            case FORMAT_FFS:
                                prefs.flags^=FORMATFLAG_FFS;
                                break;

                            case FORMAT_INTERNATIONAL:
                                prefs.flags^=FORMATFLAG_INTERNATIONAL;
                                prefs.flags&=~FORMATFLAG_CACHING;
//                                gad_caching->Flags&=~GFLG_SELECTED;
//                                RefreshGList(gad_caching,window,NULL,1);
                                break;

                            case FORMAT_CACHING:
                                prefs.flags^=FORMATFLAG_CACHING;
                                if (prefs.flags&FORMATFLAG_CACHING) {
                                    prefs.flags|=FORMATFLAG_INTERNATIONAL;
//                                    gad_international->Flags|=GFLG_SELECTED;
//                                    RefreshGList(gad_international,window,NULL,1);
//                                    DisableGadget(gad_international,window->RPort,0,0);
                                    GT_SetGadgetAttrs(gtgads[3],window,NULL,GTCB_Checked,TRUE,GA_Disabled,TRUE,TAG_END);
                                }
                                else
                                  GT_SetGadgetAttrs(gtgads[3],window,NULL,GA_Disabled,FALSE,TAG_END);
//                                else EnableGadget(gad_international,window->RPort,0,0);
                                break;

                            case FORMAT_TRASHCAN:
                                prefs.flags^=FORMATFLAG_TRASHCAN;
                                break;

                            case FORMAT_VERIFY:
                                prefs.flags^=FORMATFLAG_VERIFY;
                                break;

                            case FORMAT_SFS_CASE:
                                prefs.flags^=FORMATFLAG_SFS_CASE;
                                break;

                            case FORMAT_SFS_SHOWREC:
                                prefs.flags^=FORMATFLAG_SFS_SHOWREC;
                                break;

                            case FORMAT_SFS_NOREC:
                                prefs.flags^=FORMATFLAG_SFS_NOREC;
                                GT_SetGadgetAttrs(gtgads[12],window,NULL,GA_Disabled,(prefs.flags & FORMATFLAG_SFS_NOREC)?TRUE:FALSE,TAG_END);
                                break;

                            case FORMAT_PFS_DELDIR:
                                prefs.pfsdeldir=code;
                                break;

                            case FORMAT_PFS_FNSIZE:
                                prefs.pfsfnsize=code;
                                break;

                            case FORMAT_CANCEL:
//                                set_env("format",gadlist,gadcount,devicelist);
                                set_env((union DD_Prefs *)&prefs,PREFS_FORMAT);
//                                RemoveListView(devicelist,1);
                                dynamicgads(window,gtgads,0,GADGETS_ALL);
                                CloseRequester(&req);
                                FreeGadgets(gtglist);
                                FreeVisualInfo(gt_vi);
                                return;

                            case FORMAT_FORMAT:
                            case FORMAT_QUICKFORMAT:
                                strcpy(((struct StringInfo *)gtgads[1]->SpecialInfo)->Buffer,prefs.name);
                                if (do_format(&req,
                                    gtgads[7]/*infobox*/,
                                    devicelist->items[devicelist->itemselected],
                                    &prefs/*diskname,
                                    prefs.flags*/,
                                    (gadgetid==FORMAT_QUICKFORMAT))) {
                                    ignorechange=1;
                                    dopus_message(DOPUSMSG_UPDATEDRIVE,
                                        (APTR)devicelist->items[devicelist->itemselected],
                                        portname);
                                }
                                break;

                            case FORMAT_DEV: {
                                show_device_info(&req,gtgads[7]/*infobox*/,devicelist->items[devicelist->itemselected=code]);
//                                RemoveGList(window,gtgads[10],-1);
//                                SetAPen(window->RPort,req.rb_bg);
//                                RectFill(window->RPort,gtgads[2]->LeftEdge,gtgads[2]->TopEdge,window->Width-window->BorderRight-1,gtgads[5]->TopEdge-1);
//                                AddGadget(window,gtgads[10],-1);
                                dynamicgadsbydevice(window,gtgads,req.rb_bg,devicelist->items[code]);
                                RefreshGList(gtglist,window,NULL,-1);
                                GT_RefreshWindow(window,NULL);
                                }
                                break;
                        }
                        break;
                }
            }
//            else if (view) show_device_info(&req,infobox,view->items[view->itemselected]);
        }
        Wait(1<<window->UserPort->mp_SigBit);
    }
}

void show_device_info(reqbase,border,name)
struct RequesterBase *reqbase;
/*Object_Border*/struct Gadget *border;
char *name;
{
    struct DeviceNode *devnode;
    struct DosEnvec *dosenvec;
    int tracks,tracksize;
    long long size;
    char infobuf[60],sizebuf[20];

    border_text(reqbase,border,NULL);

    if (!name || !(devnode=find_device(name))) return;

    dosenvec=(struct DosEnvec *)
        BADDR(((struct FileSysStartupMsg *)BADDR(devnode->dn_Startup))->fssm_Environ);

    tracks=dosenvec->de_HighCyl-dosenvec->de_LowCyl+1;
    tracksize=(dosenvec->de_BlocksPerTrack*dosenvec->de_Surfaces)*(dosenvec->de_SizeBlock*4);
    size=((long long)tracks)*tracksize;

    getsizestring(sizebuf,size);
    lsprintf(infobuf,string_table[STR_FORMAT_INFODISPLAY],tracks,tracksize,sizebuf);

    border_text(reqbase,border,infobuf);
}

int do_format(reqbase,border,device,prefs/*name,flags*/,quick)
struct RequesterBase *reqbase;
/*Object_Border*/struct Gadget *border;
char *device/*,*name*/;
struct formatP *prefs;
//ULONG flags;
char quick;
{
    ULONG dostype;
    struct DeviceHandle handle;
    int suc=1,txt=STR_FAILED_ERROR;
    struct Requester busyreq;
    char *name = prefs->name;
    ULONG flags = prefs->flags;

    if (!(open_device(device,&handle))) txt=STR_NODEVICE_ERROR;
    else {
        InitRequester(&busyreq);
        busyreq.Flags=NOISYREQ;
        Request(&busyreq,reqbase->rb_window);
        SetBusyPointer(reqbase->rb_window);

        border_text(reqbase,border,string_table[STR_CHECKING_DESTINATION]);

        if (check_disk(reqbase,handle.device_req,device,1)) {
            if (check_blank_disk(reqbase,device,string_table[STR_DISK_NOT_BLANK_FORMAT])) {

                suc=0;

                inhibit_drive(device,DOSTRUE);
                drive_motor(handle.device_req,1);

                if (!quick) {
                    suc=do_raw_format(reqbase,border,
                        handle.device_req,
                        (handle.dosenvec->de_SizeBlock<<2)*
                            handle.dosenvec->de_Surfaces*handle.dosenvec->de_BlocksPerTrack,
                        handle.dosenvec->de_LowCyl,
                        handle.dosenvec->de_HighCyl-handle.dosenvec->de_LowCyl+1,
                        handle.dosenvec->de_BufMemType,
                        flags&FORMATFLAG_VERIFY);
                }

                if (!suc) {
                    suc = ERROR_FAILED;

                    border_text(reqbase,border,string_table[STR_FORMAT_INITIALISING]);

//                    if (DOSBase->dl_lib.lib_Version>36)
                    {
                        char *c;

                        if ((c = strchr(device,':')))
                         {
                          c++;
                          switch (identifyFS(*((int *)c)))
                           {
                            case DISKTYPE_DOS:
                              if (flags&FORMATFLAG_INTERNATIONAL) {
                                  if (flags&FORMATFLAG_FFS) dostype=ID_INTER_FFS_DISK;
                                  else dostype=ID_INTER_DOS_DISK;
                              }
                              else if (flags&FORMATFLAG_CACHING) {
                                  if (flags&FORMATFLAG_FFS) dostype=ID_FASTDIR_FFS_DISK;
                                  else dostype=ID_FASTDIR_DOS_DISK;
                              }
                              else if (flags&FORMATFLAG_FFS) dostype=ID_FFS_DISK;
                              else dostype=ID_DOS_DISK;

                              if (Format(device,name,dostype)) suc=0;
                              break;
                            case DISKTYPE_SFS:
                             {
                              struct TagItem tags[5];
                              struct TagItem *tag=tags;
                              struct DeviceNode *devnode;

                              tag->ti_Tag=ASF_NAME;
                              tag->ti_Data=(ULONG)name;
                              tag++;

                              if(flags&FORMATFLAG_SFS_CASE) {
                                tag->ti_Tag=ASF_CASESENSITIVE;
                                tag->ti_Data=TRUE;
                                tag++;
                              }

                              if(flags&FORMATFLAG_SFS_NOREC) {
                                tag->ti_Tag=ASF_NORECYCLED;
                                tag->ti_Data=TRUE;
                                tag++;
                              }

                              if(flags&FORMATFLAG_SFS_SHOWREC) {
                                tag->ti_Tag=ASF_SHOWRECYCLED;
                                tag->ti_Data=TRUE;
                                tag++;
                              }

                              tag->ti_Tag=TAG_END;
                              tag->ti_Data=0;

                              if ((devnode = find_device(device)))
                               {
                                if (DoPkt(devnode->dn_Task, ACTION_SFS_FORMAT, (LONG)&tags,0,0,0,0)==DOSFALSE) suc=0;
                               }
                              break;
                             }
                            case DISKTYPE_PFS:
                             {
                              if (Format(device,name,ID_PFS3_DISK))
                               {
                                struct DeviceNode *devnode;

                                if ((devnode = find_device(device)))
                                  if (DoPkt(devnode->dn_Task, ACTION_SET_DELDIR, ID_PFS2_DISK,prefs->pfsdeldir,0,0,0)==DOSFALSE)
                                    if (DoPkt(devnode->dn_Task, ACTION_SET_FNSIZE, ID_PFS2_DISK,prefs->pfsfnsize,0,0,0)==DOSFALSE)
                                      suc=0;
                               }
                              break;
                             }
                            case DISKTYPE_UNKNOWN:
                              dostype = ((*c++)<<24);
                              dostype |= ((*c++)<<16);
                              dostype |= ((*c++)<<8);
                              if (*c <= '9') dostype |= *c - '0';
                              else dostype |= *c;
D(bug("formatting with unsupported dostype %lx\n",dostype));
                              if (Format(device,name,dostype)) suc=0;
                              break;
                           }
                         }
                    }
/*
                    else {
                        suc=do_initialise(handle.device_req,name,dostype,
                            handle.dosenvec->de_LowCyl*
                                handle.dosenvec->de_BlocksPerTrack*
                                handle.dosenvec->de_Surfaces,
                            (handle.dosenvec->de_HighCyl-handle.dosenvec->de_LowCyl+1)*
                                handle.dosenvec->de_BlocksPerTrack*handle.dosenvec->de_Surfaces,
                            handle.dosenvec->de_Reserved,
                            handle.dosenvec->de_BufMemType,
                            flags);
                    }
*/
                }

                drive_motor(handle.device_req,0);
                inhibit_drive(device,FALSE);

                switch (suc) {
                    case 0:
                        if (flags&FORMATFLAG_TRASHCAN) {
//                            if (SysBase->LibNode.lib_Version<36) Delay(150);
                            GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)string_table[STR_FORMAT_MAKINGTRASHCAN],TAG_END);
                            write_trashcan(/*reqbase,border,*/device);
                        }
                        txt=STR_SUCCESS;
                        break;
                    case ERROR_FAILED: txt=STR_FAILED_ERROR; break;
                    case ERROR_MEMORY: txt=STR_MEMORY_ERROR; break;
                    case ERROR_BITMAP: txt=STR_BITMAP_ERROR; break;
                    case ERROR_ABORTED: txt=STR_ABORTED; break;
                    case ERROR_VERIFY: txt=-1; break;
                    default: txt=STR_DEVICE_ERROR; break;
                }
            }
            else txt=STR_ABORTED;
        }
        else txt=STR_ABORTED;

        EndRequest(&busyreq,reqbase->rb_window);
        ClearPointer(reqbase->rb_window);
    }

    close_device(&handle);
    if (txt>-1) border_text(reqbase,border,string_table[txt]);
D(bug("do_format result: %s\n",(txt>-1)?string_table[txt]:"OK"));
    return((txt==STR_SUCCESS));
}
/*
do_initialise(device_req,name,dostype,firstblock,numblocks,reserved,memtype,flags)
struct IOExtTD *device_req;
char *name;
ULONG dostype;
ULONG firstblock,numblocks,reserved;
ULONG memtype;
ULONG flags;
{
    ULONG *bitmap,bitmapsize;
    char *buffer;
    struct BitmapBlock *bitmapblock;
    struct BitmapExtension *bitmapextension;
    struct RootDirectory *rootdirectory;
    struct DOpusRemember *key=NULL;
    LONG error,a;
    LONG bitmapcount,bitmapblocks;
    LONG count,root;
    LONG extensioncount,extensionblocks=0;

    bitmapsize=(numblocks-reserved+31)/32;

    if (!(buffer=LAllocRemember(&key,TD_SECTOR,memtype|MEMF_PUBLIC|MEMF_CLEAR)))
        return(ERROR_MEMORY);

    *(ULONG *)buffer=dostype;

    for (a=0;a<2;a++) {
        if (error=do_writeblock(device_req,buffer,firstblock+a)) {
            LFreeRemember(&key);
            return(error);
        }
        *(ULONG *)buffer=0;
    }

    if (!(bitmap=(ULONG *)LAllocRemember(&key,sizeof(ULONG)*bitmapsize,MEMF_PUBLIC))) {
        LFreeRemember(&key);
        return(ERROR_MEMORY);
    }

    for (a=0;a<bitmapsize;a++) bitmap[a]=~0;

    count=bitmapblocks=(bitmapsize+126)/127;

    if (count>25) {
        if (!(flags&FORMATFLAG_FFS)) {
            LFreeRemember(&key);
            return(ERROR_BITMAP);
        }
        count-=25;

        do {
            ++extensionblocks;

            if (count>127) count-=127;
            else count=0;
        }
        while (count);
    }

    root=numblocks>>1;
    rootdirectory=(struct RootDirectory *)buffer;
    for (a=0;a<TD_SECTOR;a++) buffer[a]=0;

    rootdirectory->PrimaryType=2;
    rootdirectory->HashTableSize=128-56;
    rootdirectory->BitmapFlag=DOSTRUE;
    DateStamp(&rootdirectory->LastRootChange);

    if ((a=strlen(name))>31) a=31;

    rootdirectory->DiskName[0]=a;
    CopyMem(name,&rootdirectory->DiskName[1],a);

    DateStamp(&rootdirectory->LastDiskChange);
    DateStamp(&rootdirectory->CreationDate);

    rootdirectory->SecondaryType=1;

    for (a=0;a<bitmapblocks;a++) {
        if (a==25) break;
        rootdirectory->BitmapPointers[a]=root+firstblock+a+1;
    }

    if (bitmapblocks>25) rootdirectory->BitmapExtension=root+firstblock+26;
    rootdirectory->Checksum=-do_checksum((ULONG *)rootdirectory);

    if (error=do_writeblock(device_req,rootdirectory,root+firstblock)) {
        LFreeRemember(&key);
        return(error);
    }

    for (a=root-reserved;a<root-reserved+bitmapblocks+extensionblocks+1;a++)
        bitmap[a/32]&=BitTable[a%32];

    bitmapblock=(struct BitmapBlock *)buffer;

    for (a=0;a<bitmapblocks;a++) {
        CopyMem(&bitmap[a*127],&bitmapblock->Bitmap[0],sizeof(ULONG)*127);
        bitmapblock->Checksum=0;
        bitmapblock->Checksum=-do_checksum((ULONG *)bitmapblock);
        if (error=do_writeblock(device_req,bitmapblock,root+firstblock+a+1)) {
            LFreeRemember(&key);
            return(error);
        }
    }

    if (extensionblocks) {
        bitmapextension=(struct BitmapExtension *)buffer;

        bitmapcount=root+firstblock+1;
        extensioncount=bitmapcount+bitmapblocks;
        bitmapblocks-=25;

        while (bitmapblocks) {
            for (a=0;a<TD_SECTOR;a++) buffer[a]=0;
            for (a=0;a<(bitmapblocks<127?bitmapblocks:127);a++)
                bitmapextension->BitmapPointers[a]=bitmapcount++;
            if (bitmapblocks>127) bitmapblocks-=127;
            else bitmapblocks=0;

            if (bitmapblocks) bitmapextension->BitmapExtension=extensioncount+1;
            if (error=do_writeblock(device_req,bitmapextension,extensioncount++)) {
                LFreeRemember(&key);
                return(error);
            }
        }
    }

    LFreeRemember(&key);
    return(0);
}
*/
int do_raw_format(reqbase,border,device_req,tracksize,lowtrack,numtracks,memtype,verify)
struct RequesterBase *reqbase;
/*Object_Border*/struct Gadget *border;
struct IOExtTD *device_req;
ULONG tracksize,lowtrack,numtracks;
ULONG memtype;
ULONG verify;
{
    ULONG *trackbuffer,*verifybuffer;
    struct DOpusRemember *key=NULL;
    ULONG offset;
    int track,a,cmpsize,ret;
    char infobuf[80];

    if (!(trackbuffer=(ULONG *)LAllocRemember(&key,tracksize,memtype|MEMF_CLEAR)))
        return(ERROR_MEMORY);
    if (verify) {
        verifybuffer=(ULONG *)LAllocRemember(&key,tracksize,memtype);
    }
    else verifybuffer=NULL;

    cmpsize=tracksize>>2;
    offset=lowtrack*tracksize;

    for (track=0;track<numtracks;track++) {
        lsprintf(infobuf,string_table[STR_FORMAT_FORMATTING],track,numtracks-track-1);
        border_text(reqbase,border,infobuf);

        FOREVER {
            if (check_abort(reqbase->rb_window)) {
                LFreeRemember(&key);
                return(ERROR_ABORTED);
            }

            device_req->iotd_Req.io_Command=TD_FORMAT;
            device_req->iotd_Req.io_Data=(APTR)trackbuffer;
            device_req->iotd_Req.io_Offset=offset;
            device_req->iotd_Req.io_Length=tracksize;

            if (!(DoIO((struct IORequest *)device_req))) {
                if (!verifybuffer) break;
                device_req->iotd_Req.io_Command=CMD_UPDATE;
                if (!(DoIO((struct IORequest *)device_req))) break;
            }

            lsprintf(infobuf,string_table[STR_FORMAT_FORMATERROR],track);
            border_text(reqbase,border,infobuf);
            if (!(check_error(reqbase,infobuf,STR_RETRY))) {
                LFreeRemember(&key);
                return(ERROR_FAILED);
            }
        }

        if (verifybuffer) {
            lsprintf(infobuf,string_table[STR_FORMAT_VERIFYING],track,numtracks-track-1);
            border_text(reqbase,border,infobuf);

            FOREVER {
                if (check_abort(reqbase->rb_window)) {
                    LFreeRemember(&key);
                    return(ERROR_ABORTED);
                }

                device_req->iotd_Req.io_Command=CMD_READ;
                device_req->iotd_Req.io_Data=(APTR)verifybuffer;
                device_req->iotd_Req.io_Offset=offset;
                device_req->iotd_Req.io_Length=tracksize;

                if (!(DoIO((struct IORequest *)device_req))) {
                    for (a=0;a<cmpsize;a++) {
                        if (verifybuffer[a]!=0) break;
                    }
                    if (a==cmpsize) break;
                    lsprintf(infobuf,string_table[STR_FORMAT_VERIFYERROR],track);
                    ret=ERROR_VERIFY;
                }
                else {
                    lsprintf(infobuf,string_table[STR_FORMAT_FORMATERROR],track);
                    ret=ERROR_FAILED;
                }

                border_text(reqbase,border,infobuf);
                if (!(check_error(reqbase,infobuf,STR_RETRY))) {
                    LFreeRemember(&key);
                    return(ret);
                }
            }
        }

        offset+=tracksize;
    }
    LFreeRemember(&key);
    return(0);
}

void write_trashcan(/*reqbase,border,*/device)
//struct RequesterBase *reqbase;
//Object_Border *border;
char *device;
{
    struct DiskObject *trashcan;
    BPTR lock;
    char name[80];

    if (!IconBase) return;

D(bug("device: %s\n",device));
//    border_text(reqbase,border,string_table[STR_FORMAT_MAKINGTRASHCAN]);

    strcpy(name,device);
    strchr(name,':')[1]=0;
    strcat(name,"Trashcan");
//    lsprintf(name,"%sTrashcan",device);
D(bug("path: %s\n",name));
    if (!(lock=CreateDir(name))) return;
    UnLock(lock);

    if (//IconBase->lib_Version>36 &&
        (trashcan=GetDefDiskObject(WBGARBAGE))) {
        PutDiskObject(name,trashcan);
        FreeDiskObject(trashcan);
    }
    else PutDiskObject(name,&trashcanicon_icon);
}
