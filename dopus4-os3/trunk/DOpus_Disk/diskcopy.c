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

static struct NewGadget ng[8] = {
    {0,1,13,6,(APTR)STR_DISKCOPY_FROM,NULL,DISKCOPY_SRCDEV,PLACETEXT_ABOVE,NULL,NULL},
    {36,1,13,6,(APTR)STR_DISKCOPY_TO,NULL,DISKCOPY_DSTDEV,PLACETEXT_ABOVE,NULL,NULL},
    {14,4,0,0,(APTR)STR_FORMAT_VERIFY,NULL,DISKCOPY_VERIFY,PLACETEXT_RIGHT,NULL,NULL},
    {14,6,0,0,(APTR)STR_BUMP_NAMES,NULL,DISKCOPY_BUMPNAMES,PLACETEXT_RIGHT,NULL,NULL},
    {0,7,46,1,(APTR)~0,NULL,DISKCOPY_INFO,0,NULL,NULL},
    {0,8,14,1,(APTR)STR_DISKCOPY_DISKCOPY,NULL,DISKCOPY_DISKCOPY,PLACETEXT_IN,NULL,NULL},
    {16,8,14,1,(APTR)STR_DISKCOPY_CHECK,NULL,DISKCOPY_CHECK,PLACETEXT_IN,NULL,NULL},
    {32,8,14,1,(APTR)STR_FORMAT_EXIT,NULL,DISKCOPY_CANCEL,PLACETEXT_IN,NULL,NULL},
};
/*
struct TagItem
    diskcopy_source_list[]={
        {RO_Type,OBJECT_LISTVIEW},
        {RO_ListViewID,0},
        {RO_Top,1},
        {RO_LeftFine,2},
        {RO_Width,10},
        {RO_Height,6},
        {RO_HeightFine,30},
        {RO_TextNum,STR_DISKCOPY_FROM},
        {RO_TextPos,TEXTPOS_ABOVE},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},
    diskcopy_destination_list[]={
        {RO_Type,OBJECT_LISTVIEW},
        {RO_ListViewID,1},
        {RO_Top,1},
        {RO_Left,33},
        {RO_LeftFine,-18},
        {RO_Width,10},
        {RO_Height,6},
        {RO_HeightFine,30},
        {RO_TextNum,STR_DISKCOPY_TO},
        {RO_TextPos,TEXTPOS_ABOVE},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},

    diskcopy_verify_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_CHECK},
        {RO_GadgetID,DISKCOPY_VERIFY},
        {RO_Left,14},
        {RO_Top,4},
        {RO_TextNum,STR_FORMAT_VERIFY},
        {RO_TextPos,TEXTPOS_RIGHT},
        {RO_LeftFine,-2},
        {RO_BoolOn,TRUE},
        {TAG_END,0}},

    diskcopy_bumpnames_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_CHECK},
        {RO_GadgetID,DISKCOPY_BUMPNAMES},
        {RO_Left,14},
        {RO_Top,6},
        {RO_TextNum,STR_BUMP_NAMES},
        {RO_TextPos,TEXTPOS_RIGHT},
        {RO_LeftFine,-2},
        {RO_BoolOn,TRUE},
        {TAG_END,0}},

    diskcopy_info_box[]={
        {RO_Type,OBJECT_BORDER},
        {RO_BorderType,BORDER_RECESSED},
        {RO_LeftFine,2},
        {RO_Top,7},
        {RO_TopFine,36},
        {RO_Width,43},
        {RO_WidthFine,-4},
        {RO_Height,1},
        {RO_HeightFine,4},
        {TAG_END,0}},

    diskcopy_diskcopy_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_BOOLEAN},
        {RO_GadgetID,DISKCOPY_DISKCOPY},
        {RO_Top,8},
        {RO_TopFine,45},
        {RO_Width,13},
        {RO_Height,1},
        {RO_HeightFine,4},
        {RO_TextNum,STR_DISKCOPY_DISKCOPY},
        {RO_TextPos,TEXTPOS_CENTER},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},

    diskcopy_check_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_BOOLEAN},
        {RO_GadgetID,DISKCOPY_CHECK},
        {RO_Left,15},
        {RO_Top,8},
        {RO_TopFine,45},
        {RO_Width,13},
        {RO_Height,1},
        {RO_HeightFine,4},
        {RO_TextNum,STR_DISKCOPY_CHECK},
        {RO_TextPos,TEXTPOS_CENTER},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},

    diskcopy_cancel_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_BOOLEAN},
        {RO_GadgetID,DISKCOPY_CANCEL},
        {RO_Left,30},
        {RO_Top,8},
        {RO_TopFine,45},
        {RO_Width,13},
        {RO_Height,1},
        {RO_HeightFine,4},
        {RO_TextNum,STR_FORMAT_EXIT},
        {RO_TextPos,TEXTPOS_CENTER},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},

    *diskcopy_gadgets[]={
        diskcopy_verify_gadget,
        diskcopy_bumpnames_gadget,
        diskcopy_diskcopy_gadget,
        diskcopy_check_gadget,
        diskcopy_cancel_gadget,
        NULL};
*/

void diskop_diskcopy(/*vis,*/portname,argc,argv)
//struct VisInfo *vis;
char *portname;
int argc;
char *argv[];
{
    struct Window *window;
    struct IntuiMessage *msg;
//    Object_Border *infobox;
    struct DOpusListView *sourcelist,*destinationlist/*,*view*/;
//    struct Gadget *gadlist;
    ULONG class;
    USHORT code,gadgetid;
    APTR iaddress;
    int a,b,/*gadcount=0,*/ignorechange=0;
    int source=-1,dest=-1,/*startverify=0,bumpnames=0,*/oldsel;
    int startcheck=0;
    struct DOpusRemember *destkey=NULL;
    unsigned char keys[5];

    struct Gadget *gtgads[8], *gtglist=NULL;
    APTR gt_vi;
    struct TextFont *tf;
    struct TextAttr ta;
    struct List gt_devlist1, gt_devlist2;
    struct Node *node;
	struct diskcopyP prefs;

    req.rb_width=46;
    req.rb_height=9;
    req.rb_widthfine=16;
    req.rb_heightfine=61;
    req.rb_title=string_table[STR_WINDOW_DISKCOPY];

    if (!(window=OpenRequester(&req)) ||
//        !(gadlist=addreqgadgets(&req,diskcopy_gadgets,0,&gadcount)) ||
//        !(infobox=(Object_Border *)AddRequesterObject(&req,diskcopy_info_box)) ||
//        !(sourcelist=(struct DOpusListView *)AddRequesterObject(&req,diskcopy_source_list)) ||
        !(sourcelist=(struct DOpusListView *)LAllocRemember(&req.rb_memory,sizeof(struct DOpusListView),MEMF_ANY|MEMF_CLEAR)) ||
        !(sourcelist->items=get_device_list(&req.rb_memory,NULL)) ||
//        !(destinationlist=(struct DOpusListView *)AddRequesterObject(&req,diskcopy_destination_list))) {
        !(destinationlist=(struct DOpusListView *)LAllocRemember(&req.rb_memory,sizeof(struct DOpusListView),MEMF_ANY|MEMF_CLEAR))) {
        CloseRequester(&req);
        return;
    }

    tf = window->RPort->Font;
    ta.ta_Name = tf->tf_Message.mn_Node.ln_Name;
    ta.ta_YSize = tf->tf_YSize;
    ta.ta_Style = tf->tf_Style;
    ta.ta_Flags = tf->tf_Flags;
    gt_vi = GetVisualInfoA(window->WScreen,NULL);
    for (a=0; a<8; a++)
     {
      ng[a].ng_VisualInfo = gt_vi;
      if (ng[a].ng_GadgetText != (UBYTE *)~0) ng[a].ng_GadgetText = string_table[(ULONG)(ng[a].ng_GadgetText)];
      else ng[a].ng_GadgetText = NULL;
      ng[a].ng_TextAttr = &ta;
      ng[a].ng_LeftEdge *= tf->tf_XSize;
      ng[a].ng_LeftEdge += window->BorderLeft + 8;
      ng[a].ng_TopEdge *= tf->tf_YSize;
      ng[a].ng_TopEdge += window->BorderTop + 8;
      ng[a].ng_Width *= tf->tf_XSize;
      ng[a].ng_Height *= tf->tf_YSize;
      ng[a].ng_Height += 4;
     }
    ng[0].ng_Height = ng[1].ng_Height += 26;
    ng[1].ng_LeftEdge -= 18;
    ng[4].ng_TopEdge += 36;
    ng[5].ng_TopEdge = ng[6].ng_TopEdge = ng[7].ng_TopEdge += 45;

D(for (a=0; a<8; a++) bug("Gadget %ld: %ld/%ld/%ld/%ld\n",a,ng[a].ng_LeftEdge,ng[a].ng_TopEdge,ng[a].ng_Width,ng[a].ng_Height));

//    fix_listview(&req,sourcelist);
    NewList(&gt_devlist1);
    for (a=0; sourcelist->items[a]; a++)
     {
      if ((node = LAllocRemember(&req.rb_memory,sizeof(struct Node),MEMF_ANY)))
       {
        node->ln_Name = sourcelist->items[a];
        AddTail(&gt_devlist1,node);
       }
     }
//    fix_listview(&req,destinationlist);
    NewList(&gt_devlist2);

    select_device(sourcelist,NULL);
//    get_env("diskcopy",gadlist,gadcount,sourcelist);
    prefs.srclist = sourcelist;
    prefs.dstlist = destinationlist;
    get_env((union DD_Prefs *)&prefs,PREFS_DISKCOPY);

    oldsel=sourcelist->itemselected;

    for (a=0;a<argc;a++) {
        if (LStrCmpI(argv[a],"verify")==0) prefs.verify=1;
        else if (LStrnCmpI(argv[a],"bump",4)==0) prefs.bump=1;
        else if (LStrnCmpI(argv[a],"check",5)==0) startcheck=1;
        else {
            if (source==-1) {
                for (b=0;sourcelist->items[b];b++) {
                    if (LStrCmpI(argv[a],sourcelist->items[b])==0) {
                        sourcelist->itemselected = source = b;
                        destinationlist->items = get_device_list(&destkey,sourcelist->items[b]);
                        break;
                    }
                }
            }
            else if (dest==-1 && destinationlist->items) {
                for (b=0;destinationlist->items[b];b++) {
                    if (LStrCmpI(argv[a],destinationlist->items[b])==0) {
                        destinationlist->itemselected = dest = b;
                        break;
                    }
                }
            }
        }
    }

    if (source>-1 && dest==-1) {
        LFreeRemember(&destkey);
        destinationlist->items=NULL;
        sourcelist->itemselected=oldsel;
        prefs.verify=prefs.bump=0;
    }

    if (!destinationlist->items) {
        destinationlist->items=get_device_list(&destkey,sourcelist->items[sourcelist->itemselected]);
        select_device(destinationlist,sourcelist->items[sourcelist->itemselected]);
//        sourcelist->next=destinationlist;
        if (source==-1) get_env((union DD_Prefs *)&prefs,PREFS_DISKCOPY/*"diskcopy",gadlist,gadcount,sourcelist*/);
    }
//    else sourcelist->next=destinationlist;
    for (a=0; destinationlist->items[a]; a++)
     {
      if ((node = LAllocRemember(&destkey,sizeof(struct Node),MEMF_ANY)))
       {
        node->ln_Name = destinationlist->items[a];
        AddTail(&gt_devlist2,node);
       }
     }
/*
    if (!(AddListView(sourcelist,2))) {
        CloseRequester(&req);
        LFreeRemember(&destkey);
        return;
    }
    show_sel_item(sourcelist);

    if (prefs.verify) gadlist->Flags|=GFLG_SELECTED;
    else if (dest>-1) gadlist->Flags&=~GFLG_SELECTED;

    if (prefs.bump) gadlist->NextGadget->Flags|=GFLG_SELECTED;
    else if (dest>-1) gadlist->NextGadget->Flags&=~GFLG_SELECTED;
*/
    keys[0] = getkeyshortcut(string_table[STR_FORMAT_VERIFY]);
    keys[1] = getkeyshortcut(string_table[STR_BUMP_NAMES]);
    keys[2] = getkeyshortcut(string_table[STR_DISKCOPY_DISKCOPY]);
    keys[3] = getkeyshortcut(string_table[STR_DISKCOPY_CHECK]);
    keys[4] = getkeyshortcut(string_table[STR_FORMAT_EXIT]);

    CreateContext(&gtglist);
    gtgads[0] = CreateGadget(LISTVIEW_KIND,gtglist,&ng[0],
    				GTLV_Labels, (Tag)&gt_devlist1,
                    GTLV_Selected, sourcelist->itemselected,
                    GTLV_ShowSelected, NULL,
                    TAG_END);
    gtgads[1] = CreateGadget(LISTVIEW_KIND,gtgads[0],&ng[1],
    				GTLV_Labels, (Tag)&gt_devlist2,
                    GTLV_Selected, destinationlist->itemselected,
                    GTLV_ShowSelected, NULL,
                    TAG_END);
    gtgads[2] = CreateGadget(CHECKBOX_KIND,gtgads[1],&ng[2],
                    GTCB_Checked, prefs.verify,
                    TAG_MORE, (Tag)commonGTtags);
    gtgads[3] = CreateGadget(CHECKBOX_KIND,gtgads[2],&ng[3],
                    GTCB_Checked, prefs.bump,
                    TAG_MORE, (Tag)commonGTtags);
    gtgads[4] = CreateGadget(TEXT_KIND,gtgads[3],&ng[4],
                    GTTX_Border, TRUE,
                    GTTX_Justification, GTJ_CENTER,
                    GTTX_FrontPen, req.rb_fg,
                    GTTX_BackPen,  req.rb_bg,
                    TAG_END);
    for (a=5; a<8; a++) gtgads[a] = CreateGadgetA(BUTTON_KIND,gtgads[a-1],&ng[a],commonGTtags);

    AddGList(window,gtglist,-1,-1,NULL);
    RefreshGList(gtglist,window,NULL,-1);
//    RefreshRequesterObject(&req,NULL);
//    RefreshGList(gadlist,window,NULL,gadcount);
    GT_RefreshWindow(window,NULL);
    ModifyIDCMP(window,IDCMP_CLOSEWINDOW|IDCMP_DISKINSERTED|IDCMP_DISKREMOVED|IDCMP_VANILLAKEY |
                       LISTVIEWIDCMP|CHECKBOXIDCMP|BUTTONIDCMP|TEXTIDCMP);

    show_diskcopy_info(&req,gtgads[4]/*infobox*/,sourcelist->items[sourcelist->itemselected]);

    if (dest>-1) {
        if (do_diskcopy(&req,
            gtgads[4]/*infobox*/,
            &prefs,
//            sourcelist->items[sourcelist->itemselected],
//            destinationlist->items[destinationlist->itemselected],
//            prefs.verify/*(gadlist->Flags&GFLG_SELECTED)*/,
//            prefs.bump/*(gadlist->NextGadget->Flags&GFLG_SELECTED)*/,
            startcheck)) {
            dopus_message(DOPUSMSG_UPDATEDRIVE,
                (APTR)destinationlist->items[destinationlist->itemselected],
                portname);
        }
//        RemoveListView(sourcelist,2);
        CloseRequester(&req);
        FreeGadgets(gtglist);
        LFreeRemember(&destkey);
        FreeVisualInfo(gt_vi);
        return;
    }

    FOREVER {
        while ((msg=(struct IntuiMessage *)GT_GetIMsg(window->UserPort))) {
/*            if ((view=(struct DOpusListView *)ListViewIDCMP(sourcelist,msg))==
                (struct DOpusListView *)-1) */{
                class=msg->Class;
                code=msg->Code;
                iaddress=msg->IAddress;
                ReplyMsg((struct Message *)msg);

                switch (class) {
                    case IDCMP_DISKINSERTED:
                    case IDCMP_DISKREMOVED:
                        if (ignorechange) ignorechange=0;
                        else {
                            show_diskcopy_info(&req,gtgads[4]/*infobox*/,sourcelist->items[sourcelist->itemselected]);
                        }
                        break;

                    case IDCMP_VANILLAKEY:  
                        code=LToLower(code);
                        if (code == keys[0]) {
                                prefs.verify = !prefs.verify;
                                GT_SetGadgetAttrs(gtgads[2],window,NULL,GTCB_Checked,prefs.verify,TAG_END);
//                                gadlist->Flags^=GFLG_SELECTED;
//                                RefreshGList(gadlist,window,NULL,1);
                        }
                        else if (code == keys[1]) {
                                prefs.bump = !prefs.bump;
                                GT_SetGadgetAttrs(gtgads[3],window,NULL,GTCB_Checked,prefs.bump,TAG_END);
//                                gadlist->NextGadget->Flags^=GFLG_SELECTED;
//                                RefreshGList(gadlist,window,NULL,-1);
                        }
                        else if ((code == keys[2]) || (code == keys[3]) || (code == '\r')) {
                                if (do_diskcopy(&req,
                                    gtgads[4]/*infobox*/,
                                    &prefs,
//                                    sourcelist->items[sourcelist->itemselected],
//                                    destinationlist->items[destinationlist->itemselected],
//                                    (gadlist->Flags&GFLG_SELECTED),
//                                    (gadlist->NextGadget->Flags&GFLG_SELECTED),
                                    code == keys[3])) {
                                    ignorechange=1;
                                    dopus_message(DOPUSMSG_UPDATEDRIVE,
                                        (APTR)destinationlist->items[destinationlist->itemselected],
                                        portname);
                                }
                        }
                        else if ((code == keys[4]) || (code == 0x1b)) {
//                                set_env("diskcopy",gadlist,gadcount,sourcelist);
                                set_env((union DD_Prefs *)&prefs,PREFS_DISKCOPY);
//                                RemoveListView(sourcelist,2);
                                CloseRequester(&req);
                                FreeGadgets(gtglist);
                                LFreeRemember(&destkey);
                                FreeVisualInfo(gt_vi);
                                return;
                        }
                        break;

//                    case IDCMP_GADGETDOWN:
                    case IDCMP_GADGETUP:
                        gadgetid=((struct Gadget *)iaddress)->GadgetID;
                    case IDCMP_CLOSEWINDOW:
                        if (class==IDCMP_CLOSEWINDOW) gadgetid=DISKCOPY_CANCEL;

                        switch (gadgetid) {
                            case DISKCOPY_CANCEL:
//                                set_env("diskcopy",gadlist,gadcount,sourcelist);
                                set_env((union DD_Prefs *)&prefs,PREFS_DISKCOPY);
//                                RemoveListView(sourcelist,2);
                                CloseRequester(&req);
                                FreeGadgets(gtglist);
                                LFreeRemember(&destkey);
                                FreeVisualInfo(gt_vi);
                                return;

                            case DISKCOPY_VERIFY:
                                prefs.verify = code;
                                break;
                            case DISKCOPY_BUMPNAMES:
                                prefs.bump = code;
                                break;
                            case DISKCOPY_DISKCOPY:
                            case DISKCOPY_CHECK:
                                if (do_diskcopy(&req,
                                    gtgads[4]/*infobox*/,
                                    &prefs,
//                                    sourcelist->items[sourcelist->itemselected],
//                                    destinationlist->items[destinationlist->itemselected],
//                                    prefs.verify/*(gadlist->Flags&GFLG_SELECTED)*/,
//                                    prefs.bump/*(gadlist->NextGadget->Flags&GFLG_SELECTED)*/,
                                    (gadgetid==DISKCOPY_CHECK))) {
                                    ignorechange=1;
                                    dopus_message(DOPUSMSG_UPDATEDRIVE,
                                        (APTR)destinationlist->items[destinationlist->itemselected],
                                        portname);
                                }
                                break;

                            case DISKCOPY_SRCDEV:
                                GT_SetGadgetAttrs(gtgads[1],window,NULL,GTLV_Labels,~0,TAG_END);
                                while (RemTail(&gt_devlist2));
                                LFreeRemember(&destkey);
                                sourcelist->itemselected = code;
                                destinationlist->items=get_device_list(&destkey,sourcelist->items[code]);
                                for (a=0; destinationlist->items[a]; a++)
                                 {
                                  if ((node = LAllocRemember(&destkey,sizeof(struct Node),MEMF_ANY)))
                                   {
                                    node->ln_Name = destinationlist->items[a];
                                    AddTail(&gt_devlist2,node);
                                   }
                                 }
                                select_device(destinationlist,sourcelist->items[code]);
                                GT_SetGadgetAttrs(gtgads[1],window,NULL,
                                      GTLV_Labels,   (Tag)&gt_devlist2,
                                      GTLV_Selected, destinationlist->itemselected,
                                      TAG_END);
//                                RefreshListView(destinationlist,1);
                                show_diskcopy_info(&req,gtgads[4]/*infobox*/,sourcelist->items[code]);
                                break;
                            case DISKCOPY_DSTDEV:
                                destinationlist->itemselected = code;
                                break;
                        }
                        break;
                }
            }
/*
            else if (view) {
                switch (view->listid) {
                    case 0:
                        LFreeRemember(&destkey);
                        destinationlist->items=get_device_list(&destkey,view->items[view->itemselected]);
                        select_device(destinationlist,view->items[view->itemselected]);
                        RefreshListView(destinationlist,1);
                        show_diskcopy_info(&req,infobox,sourcelist->items[sourcelist->itemselected]);
                        break;
                }
            }
*/
        }
        Wait(1<<window->UserPort->mp_SigBit);
    }
}

void show_diskcopy_info(reqbase,border,name)
struct RequesterBase *reqbase;
/*Object_Border*/struct Gadget *border;
char *name;
{
    char infobuf[60],sizebuf[20],*ptr;
    BPTR lock;
    struct FileInfoBlock __aligned fib;
    struct InfoData __aligned info;
    struct Process *myproc;
    APTR wsave;

    myproc=(struct Process *)FindTask(NULL);
    wsave=myproc->pr_WindowPtr;
    myproc->pr_WindowPtr=(APTR)-1;

D(bug("show_diskcopy_info(%s)\n",name));
    border_text(reqbase,border,NULL);
//    GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)NULL,TAG_END);

    if (name) {
        if ((lock=Lock(name,ACCESS_READ))) {
            Info(lock,&info);
            Examine(lock,&fib);
            UnLock(lock);

            getsizestring(sizebuf,((long long)info.id_NumBlocksUsed)*info.id_BytesPerBlock);
            lsprintf(infobuf,string_table[STR_DISKCOPY_INFODISPLAY],fib.fib_FileName,sizebuf);

//            border_text(reqbase,border,infobuf);
            ptr = infobuf;
        }
        else ptr = string_table[STR_DISKCOPY_NODISK]; //border_text(reqbase,border,string_table[STR_DISKCOPY_NODISK]);
        border_text(reqbase,border,ptr);
//        GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)ptr,TAG_END);
    }
    myproc->pr_WindowPtr=wsave;
}

int do_diskcopy(struct RequesterBase *reqbase,/*Object_Border*/struct Gadget *border,struct diskcopyP *prefs/*,char *source,char *dest,int verify,int bump*/,int check)
{
    struct IOExtTD *device_req[2];
    struct MsgPort *device_port[2];
    struct DeviceNode *source_node,*dest_node;
    struct FileSysStartupMsg *startup;
    struct DosEnvec *dosenvec;
    struct Requester busyreq;
    ULONG tracksize,lowtrack[2],numtracks,memtype,deviceunit[2],deviceflags[2],track;
    ULONG trackcount,trackmod,curtrack,offset;
    char devicename[2][40],infobuf[80];
    int a,b,drives,abort=0,err=0,cmpsize,txt=-1;
    char *buffer=NULL,*dev_table[2];
    ULONG *verifybuffer=NULL,*cmpbuffer;
    struct DOpusRemember *memkey=NULL;
    char *source, *dest;
    int verify,bump;

    source = prefs->srclist->items[prefs->srclist->itemselected];
    dest = prefs->dstlist->items[prefs->dstlist->itemselected];
    verify = prefs->verify;
    bump = prefs->bump;

    if (!source || !dest ||
        !(source_node=find_device(source)) ||
        !(dest_node=find_device(dest))) return(0);

    if (check) verify=1;
    if (LStrCmpI(source,dest)==0) drives=1;
    else drives=2;

    dev_table[0]=source; dev_table[1]=dest;

    for (a=0;a<2;a++) {
        device_port[a]=NULL;
        device_req[a]=NULL;
    }

    startup=(struct FileSysStartupMsg *)BADDR(source_node->dn_Startup);
    BtoCStr((BPTR)startup->fssm_Device,devicename[0],40);
    deviceunit[0]=startup->fssm_Unit;
    deviceflags[0]=startup->fssm_Flags;

    dosenvec=(struct DosEnvec *)BADDR(startup->fssm_Environ);
    tracksize=(dosenvec->de_SizeBlock<<2)*dosenvec->de_Surfaces*dosenvec->de_BlocksPerTrack;
    lowtrack[0]=dosenvec->de_LowCyl;
    numtracks=dosenvec->de_HighCyl-dosenvec->de_LowCyl+1;
    memtype=dosenvec->de_BufMemType;

    Forbid();

    if (drives==2) trackcount=1;
    else {
        if ((trackcount=AvailMem(memtype|MEMF_LARGEST)/tracksize)>1) {
            if (numtracks%trackcount) {
                trackmod=(numtracks/trackcount)+1;
                trackcount=numtracks/trackmod;
            }
        }
    }

    if (trackcount>0) buffer=LAllocRemember(&memkey,tracksize*trackcount,memtype);

    Permit();

    if (!buffer) {
        border_text(reqbase,border,string_table[STR_MEMORY_ERROR]);
//        GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)string_table[STR_MEMORY_ERROR],TAG_END);
        return(0);
    }

    cmpbuffer=(ULONG *)buffer;

    if (verify) {
        verifybuffer=(ULONG *)LAllocRemember(&memkey,tracksize,memtype);
        cmpsize=tracksize>>2;
    }
    if (check && !verifybuffer) {
        border_text(reqbase,border,string_table[STR_MEMORY_ERROR]);
//        GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)string_table[STR_MEMORY_ERROR],TAG_END);
        return(0);
    }

    startup=(struct FileSysStartupMsg *)BADDR(dest_node->dn_Startup);
    BtoCStr((BPTR)startup->fssm_Device,devicename[1],40);
    deviceunit[1]=startup->fssm_Unit;
    deviceflags[1]=startup->fssm_Flags;

    dosenvec=(struct DosEnvec *)BADDR(startup->fssm_Environ);
    lowtrack[1]=dosenvec->de_LowCyl;

    for (a=0;a<2;a++) {
        if (!(device_port[a]=LCreatePort(NULL,0)) ||
            !(device_req[a]=(struct IOExtTD *)
            LCreateExtIO(device_port[a],sizeof(struct IOExtTD)))) break;
        if (OpenDevice(devicename[a],deviceunit[a],
            (struct IORequest *)device_req[a],deviceflags[a])) {
            LDeleteExtIO((struct IORequest *)device_req[a]);
            device_req[a]=NULL;
            break;
        }
    }

    if (a==2) {
        InitRequester(&busyreq);
        busyreq.Flags=NOISYREQ;
        Request(&busyreq,reqbase->rb_window);
        SetBusyPointer(reqbase->rb_window);

        if (drives==2) {
            border_text(reqbase,border,string_table[STR_CHECKING_DISKS]);
//            GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)string_table[STR_CHECKING_DISKS],TAG_END);

            for (a=0;a<2;a++) {
                if (!(check_disk(reqbase,device_req[a],dev_table[a],(check)?0:a))) break;
                if (a==1 && !check && !(check_blank_disk(reqbase,dev_table[a],
                    string_table[STR_DISK_NOT_BLANK_DISKCOPY])))
                    break;
            }
        }
        else a=2;

        if (a==2) {
            inhibit_drive(source,DOSTRUE);
            if (drives==2) inhibit_drive(dest,DOSTRUE);

            for (a=0;a<drives;a++) drive_motor(device_req[a],1);

            for (track=0;track<numtracks;track+=trackcount) {

                offset=(lowtrack[0]+track)*tracksize;

                if (drives==1) {
                    lsprintf(infobuf,string_table[STR_DISKCOPY_INSERTSOURCE],source);
                    border_text(reqbase,border,infobuf);
//                    GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)infobuf,TAG_END);
                    drive_motor(device_req[0],0);
                    if (!(check_error(reqbase,infobuf,STR_PROCEED)) ||
                        !(check_disk(reqbase,device_req[0],source,0))) {
                        err=ERROR_ABORTED;
                        break;
                    }
                    drive_motor(device_req[0],1);
                }

                for (curtrack=0;curtrack<trackcount;curtrack++) {

                    lsprintf(infobuf,string_table[STR_DISKCOPY_READING],
                        track+curtrack,numtracks-curtrack-track-1);
                    border_text(reqbase,border,infobuf);
//                    GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)infobuf,TAG_END);

                    if (check_abort(reqbase->rb_window)) {
                        abort=1;
                        err=ERROR_ABORTED;
                        break;
                    }

                    FOREVER {
                        device_req[0]->iotd_Req.io_Command=CMD_READ;
                        device_req[0]->iotd_Req.io_Data=(APTR)&buffer[curtrack*tracksize];
                        device_req[0]->iotd_Req.io_Offset=offset;
                        device_req[0]->iotd_Req.io_Length=tracksize;

                        if (!(DoIO((struct IORequest *)device_req[0]))) break;

                        lsprintf(infobuf,string_table[STR_DISKCOPY_READERROR],track+curtrack);
                        border_text(reqbase,border,infobuf);
//                        GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)infobuf,TAG_END);
                        if (!(check_error(reqbase,infobuf,STR_RETRY))) {
                            abort=1;
                            err=ERROR_FAILED;
                            break;
                        }
                    }

                    if (abort) break;
                    offset+=tracksize;
                }

                if (abort) break;

                offset=(lowtrack[1]+track)*tracksize;

                if (drives==1) {
                    lsprintf(infobuf,string_table[STR_DISKCOPY_INSERTDEST],dest);
                    border_text(reqbase,border,infobuf);
//                    GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)infobuf,TAG_END);
                    drive_motor(device_req[0],0);
                    if (!(check_error(reqbase,infobuf,STR_PROCEED)) ||
                        !(check_disk(reqbase,device_req[1],dest,1))) {
                        err=ERROR_ABORTED;
                        abort=1;
                        break;
                    }
                    drive_motor(device_req[0],1);
                }

                for (curtrack=0;curtrack<trackcount;curtrack++) {

                    if (!check) {
                        lsprintf(infobuf,string_table[STR_DISKCOPY_WRITING],
                            track+curtrack,numtracks-curtrack-track-1);
                        border_text(reqbase,border,infobuf);
//                        GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)infobuf,TAG_END);

                        if (check_abort(reqbase->rb_window)) {
                            abort=1;
                            err=ERROR_ABORTED;
                            break;
                        }

                        FOREVER {
                            device_req[1]->iotd_Req.io_Command=TD_FORMAT;
                            device_req[1]->iotd_Req.io_Data=(APTR)&buffer[curtrack*tracksize];
                            device_req[1]->iotd_Req.io_Offset=offset;
                            device_req[1]->iotd_Req.io_Length=tracksize;

                            if (!(DoIO((struct IORequest *)device_req[1]))) {
                                if (!verifybuffer) break;
                                device_req[1]->iotd_Req.io_Command=CMD_UPDATE;
                                if (!(DoIO((struct IORequest *)device_req[1]))) break;
                            }

                            lsprintf(infobuf,string_table[STR_DISKCOPY_WRITEERROR],track+curtrack);
                            border_text(reqbase,border,infobuf);
//                            GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)infobuf,TAG_END);
                            if (!(check_error(reqbase,infobuf,STR_RETRY))) {
                                abort=1;
                                err=ERROR_FAILED;
                                break;
                            }
                        }

                        if (abort) break;
                    }

                    if (verifybuffer) {
                        lsprintf(infobuf,string_table[STR_FORMAT_VERIFYING],
                            track+curtrack,numtracks-curtrack-track-1);
                        border_text(reqbase,border,infobuf);
//                        GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)infobuf,TAG_END);

                        FOREVER {
                            device_req[1]->iotd_Req.io_Command=CMD_READ;
                            device_req[1]->iotd_Req.io_Data=(APTR)verifybuffer;
                            device_req[1]->iotd_Req.io_Offset=offset;
                            device_req[1]->iotd_Req.io_Length=tracksize;

                            if (!(DoIO((struct IORequest *)device_req[1]))) {
                                for (a=0,b=curtrack*cmpsize;a<cmpsize;a++,b++) {
                                    if (verifybuffer[a]!=cmpbuffer[b]) break;
                                }
                                if (a==cmpsize) break;
                                lsprintf(infobuf,string_table[STR_FORMAT_VERIFYERROR],track+curtrack);
                                err=ERROR_VERIFY;
                            }
                            else {
                                lsprintf(infobuf,string_table[STR_DISKCOPY_READERROR],track+curtrack);
                                err=ERROR_FAILED;
                            }

                            border_text(reqbase,border,infobuf);
//                            GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)infobuf,TAG_END);
                            if (!(check_error(reqbase,infobuf,STR_RETRY))) {
                                abort=1;
                                break;
                            }
                            err=0;
                        }

                        if (abort) break;
                    }

                    offset+=tracksize;
                }

            }

            for (a=0;a<drives;a++) drive_motor(device_req[a],0);

            switch (err) {
                case 0:
                    if (bump && !check) {
                        border_text(reqbase,border,string_table[STR_BUMPING_NAME]);
//                        GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)string_table[STR_BUMPING_NAME],TAG_END);
                        inhibit_drive(dest,FALSE);
                        bump_disk_name(dest);
                    }
                    txt=STR_SUCCESS;
                    break;
                case ERROR_ABORTED: txt=STR_ABORTED; break;
                case ERROR_FAILED: txt=STR_FAILED_ERROR; break;
                case ERROR_VERIFY: txt=-1; break;
            }

            inhibit_drive(source,FALSE);
            if (drives==2) inhibit_drive(dest,FALSE);
        }
        else txt=STR_ABORTED;

        EndRequest(&busyreq,reqbase->rb_window);
        ClearPointer(reqbase->rb_window);
    }
    else txt=STR_NODEVICE_ERROR;

    if (txt>-1) border_text(reqbase,border,string_table[txt]);
//        GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)string_table[txt],TAG_END);

    for (a=0;a<2;a++) {
        if (device_req[a]) {
            CloseDevice((struct IORequest *)device_req[a]);
            LDeleteExtIO((struct IORequest *)device_req[a]);
        }
        if (device_port[a]) LDeletePort(device_port[a]);
    }

    LFreeRemember(&memkey);
    return((txt==STR_SUCCESS));
}

void bump_disk_name(disk)
char *disk;
{
    if (IconBase) {
        struct Process *myproc;
        APTR wsave;
        BPTR lock;

        myproc=(struct Process *)FindTask(NULL);
        wsave=myproc->pr_WindowPtr;
        myproc->pr_WindowPtr=(APTR)-1;

        if ((lock=Lock(disk,ACCESS_READ))) {
            struct FileInfoBlock __aligned info;
            char namebuf[FILEBUF_SIZE];
            struct MsgPort *port;

            Examine(lock,&info);
            UnLock(lock);

            BumpRevision(namebuf,info.fib_FileName);

            if ((port=(struct MsgPort *)DeviceProc(disk))) {
                char __aligned bstr_name[FILEBUF_SIZE+4];
                ULONG arg;

                bstr_name[0]=strlen(namebuf);
                strcpy(&bstr_name[1],namebuf);
                arg=(ULONG)bstr_name>>2;
                SendPacket(port,ACTION_RENAME_DISK,&arg,1);
            }
        }
        myproc->pr_WindowPtr=wsave;
    }
}
