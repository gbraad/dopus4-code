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

static struct NewGadget ng[6] = {
	{0,0,13,6,NULL,NULL,INSTALL_DEVLIST,0,NULL,NULL},
    {14,0,0,0,NULL,NULL,INSTALL_FFS,PLACETEXT_RIGHT,NULL,NULL},
    {0,7,41,1,NULL,NULL,INSTALL_INFO,0,NULL,NULL},
    {0,8,13,1,NULL,NULL,INSTALL_INSTALL,PLACETEXT_IN,NULL,NULL},
    {14,8,13,1,NULL,NULL,INSTALL_NOBOOT,PLACETEXT_IN,NULL,NULL},
    {28,8,13,1,NULL,NULL,INSTALL_CANCEL,PLACETEXT_IN,NULL,NULL},
};
/*
struct TagItem
    install_device_list[]={
        {RO_Type,OBJECT_LISTVIEW},
        {RO_ListViewID,0},
        {RO_LeftFine,2},
        {RO_Width,10},
        {RO_Height,6},
        {RO_HeightFine,30},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},

    install_ffs_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_CHECK},
        {RO_GadgetID,INSTALL_FFS},
        {RO_Left,14},
        {RO_TextNum,STR_FORMAT_FFS},
        {RO_TextPos,TEXTPOS_RIGHT},
        {RO_ChkCenter,TRUE},
        {TAG_END,0}},

    install_install_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_BOOLEAN},
        {RO_GadgetID,INSTALL_INSTALL},
        {RO_Top,8},
        {RO_TopFine,37},
        {RO_Width,13},
        {RO_Height,1},
        {RO_HeightFine,4},
        {RO_TextNum,STR_INSTALL_INSTALL},
        {RO_TextPos,TEXTPOS_CENTER},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},
    install_noboot_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_BOOLEAN},
        {RO_GadgetID,INSTALL_NOBOOT},
        {RO_Left,14},
        {RO_Top,8},
        {RO_TopFine,37},
        {RO_Width,13},
        {RO_Height,1},
        {RO_HeightFine,4},
        {RO_TextNum,STR_INSTALL_NOBOOT},
        {RO_TextPos,TEXTPOS_CENTER},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},
    install_cancel_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_BOOLEAN},
        {RO_GadgetID,INSTALL_CANCEL},
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

    install_info_box[]={
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

   *install_gadgets[]={
       install_ffs_gadget,
       install_install_gadget,
       install_noboot_gadget,
       install_cancel_gadget,
       NULL};
*/
/* prototypes */
void show_install_info(struct RequesterBase *reqbase,struct Gadget /*Object_Border*/ *border,char *name);
/* end of prototypes */

void diskop_install(/*vis,*/argc,argv)
//struct VisInfo *vis;
int argc;
char *argv[];
{
   struct Window *window;
   struct IntuiMessage *msg;
//   Object_Border *infobox;
   struct DOpusListView *devicelist/*,*view*/;
//    struct Gadget *gadlist;
    ULONG class/*,mask*/;
    USHORT code,gadgetid;
    APTR iaddress;
    int /*gadcount=0,*/ignorechange=0,a,b;
    int start=-1/*,prefs.ffs=0*/,startnoboot=0;
    unsigned char keys[4];

    struct Gadget *gtgads[6], *gtglist=NULL;
    APTR gt_vi;
    struct TextFont *tf;
    struct TextAttr ta;
    struct List gt_devlist;
    struct installP prefs = {0};

    req.rb_width=41;
    req.rb_height=9;
    req.rb_widthfine=16;
    req.rb_heightfine=54;
    req.rb_title=string_table[STR_WINDOW_INSTALL];

//    mask = (SysBase->LibNode.lib_Version<36) ? 1 : 0;

    if (!(window=OpenRequester(&req)) ||
//        !(gadlist=addreqgadgets(&req,install_gadgets,mask,&gadcount)) ||
//        !(infobox=(Object_Border *)AddRequesterObject(&req,install_info_box)) ||
//        !(devicelist=(struct DOpusListView *)AddRequesterObject(&req,install_device_list)) ||
        !(devicelist=(struct DOpusListView *)LAllocRemember(&req.rb_memory,sizeof(struct DOpusListView),MEMF_ANY|MEMF_CLEAR)) ||
        !(devicelist->items=get_device_list(&(req.rb_memory),"DF0:"))) {
        CloseRequester(&req);
        return;
    }

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

    tf = window->RPort->Font;
    ta.ta_Name = tf->tf_Message.mn_Node.ln_Name;
    ta.ta_YSize = tf->tf_YSize;
    ta.ta_Style = tf->tf_Style;
    ta.ta_Flags = tf->tf_Flags;
    gt_vi = GetVisualInfoA(window->WScreen,NULL);
    for (a=0; a<6; a++)
     {
      ng[a].ng_VisualInfo = gt_vi;
      ng[a].ng_TextAttr = &ta;
      ng[a].ng_LeftEdge *= tf->tf_XSize;
      ng[a].ng_LeftEdge += window->BorderLeft + 8;
      ng[a].ng_TopEdge *= tf->tf_YSize;
      ng[a].ng_TopEdge += window->BorderTop + 8;
      ng[a].ng_Width *= tf->tf_XSize;
      ng[a].ng_Height *= tf->tf_YSize;
      ng[a].ng_Height += 4;
     }
    ng[0].ng_Height += 26;
    ng[2].ng_TopEdge += 24;
    ng[3].ng_TopEdge = ng[4].ng_TopEdge = ng[5].ng_TopEdge += 33;

D(for (a=0; a<6; a++) bug("Gadget %ld: %ld/%ld/%ld/%ld\n",a,ng[a].ng_LeftEdge,ng[a].ng_TopEdge,ng[a].ng_Width,ng[a].ng_Height));

    ng[1].ng_GadgetText = string_table[STR_FORMAT_FFS];
    ng[3].ng_GadgetText = string_table[STR_INSTALL_INSTALL];
    ng[4].ng_GadgetText = string_table[STR_INSTALL_NOBOOT];
    ng[5].ng_GadgetText = string_table[STR_FORMAT_EXIT];

//    fix_listview(&req,devicelist);
//    get_env("install",gadlist,gadcount,devicelist);
    prefs.srclist = devicelist;
    get_env((union DD_Prefs *)&prefs,PREFS_INSTALL);

    for (a=0;a<argc;a++) {
        if (LStrCmpI(argv[a],"ffs")==0 /*&& !mask*/) prefs.ffs=1;
        else if (LStrCmpI(argv[a],"noboot")==0) startnoboot=1;
        else {
            if (start==-1) {
                for (b=0;devicelist->items[b];b++) {
                    if (LStrCmpI(argv[a],devicelist->items[b])==0) {
                        start=b;
                        devicelist->itemselected=b;
                        break;
                    }
                }
            }
        }
    }
D(bug("itemselected = %ld\n",devicelist->itemselected));
/*
    if (!(AddListView(devicelist,1))) {
        CloseRequester(&req);
        return;
    }
    show_sel_item(devicelist);

    if (start>-1) {
        if (prefs.ffs) gadlist->Flags|=GFLG_SELECTED;
        else gadlist->Flags&=~GFLG_SELECTED;
    }
*/
    CreateContext(&gtglist);
    gtgads[0] = CreateGadget(LISTVIEW_KIND,gtglist,&ng[0],
    				GTLV_Labels, (Tag)&gt_devlist,
                    GTLV_Selected, devicelist->itemselected,
                    GTLV_ShowSelected, NULL,
                    TAG_END);
    gtgads[1] = CreateGadget(CHECKBOX_KIND,gtgads[0],&ng[1],
                    GTCB_Checked, prefs.ffs,
//                    GA_Disabled, mask,
                    TAG_MORE, (Tag)commonGTtags);
    gtgads[2] = CreateGadget(TEXT_KIND,gtgads[1],&ng[2],
                    GTTX_Border, TRUE,
                    GTTX_Justification, GTJ_CENTER,
                    GTTX_FrontPen, req.rb_fg,
                    GTTX_BackPen,  req.rb_bg,
                    TAG_END);
    for (a=3; a<6; a++) gtgads[a] = CreateGadgetA(BUTTON_KIND,gtgads[a-1],&ng[a],commonGTtags);

    AddGList(window,gtglist,-1,-1,NULL);
    RefreshGList(gtglist,window,NULL,-1);
//    RefreshRequesterObject(&req,NULL);
//    RefreshGList(gadlist,window,NULL,gadcount);
    GT_RefreshWindow(window,NULL);
    ModifyIDCMP(window,IDCMP_CLOSEWINDOW|IDCMP_DISKINSERTED|IDCMP_DISKREMOVED|IDCMP_VANILLAKEY |
                       LISTVIEWIDCMP|CHECKBOXIDCMP|BUTTONIDCMP|TEXTIDCMP);

    show_install_info(&req,gtgads[2]/*infobox*/,devicelist->items[devicelist->itemselected]);

    if (start>-1) {
        do_install(&req,gtgads[2]/*infobox*/,
            devicelist->items[devicelist->itemselected],
            startnoboot,prefs.ffs);
//        RemoveListView(devicelist,1);
        CloseRequester(&req);
        FreeGadgets(gtglist);
        FreeVisualInfo(gt_vi);
        return;
    }

    keys[0] = getkeyshortcut(string_table[STR_FORMAT_FFS]);
    keys[1] = getkeyshortcut(string_table[STR_INSTALL_INSTALL]);
    keys[2] = getkeyshortcut(string_table[STR_INSTALL_NOBOOT]);
    keys[3] = getkeyshortcut(string_table[STR_FORMAT_EXIT]);

    FOREVER {
        while ((msg=(struct IntuiMessage *)GT_GetIMsg(window->UserPort))) {
/*            if ((view=(struct DOpusListView *)ListViewIDCMP(devicelist,msg))==
                (struct DOpusListView *)-1)*/ {
                class=msg->Class;
                code=msg->Code;
                iaddress=msg->IAddress;
                ReplyMsg((struct Message *)msg);

                switch (class) {
                    case IDCMP_DISKINSERTED:
                    case IDCMP_DISKREMOVED:
                        if (ignorechange) ignorechange=0;
                        else {
                            show_install_info(&req,gtgads[2]/*infobox*/,devicelist->items[devicelist->itemselected]);
                        }
                        break;

                    case IDCMP_VANILLAKEY:  
                        code=LToLower(code);
                        if (code == keys[0]) {
/*
                                if (!mask) {
                                    gadlist->Flags^=GFLG_SELECTED;
                                    RefreshGList(gadlist,window,NULL,1);
                                }
*/
                              prefs.ffs = !prefs.ffs;
                              GT_SetGadgetAttrs(gtgads[1],window,NULL,GTCB_Checked,prefs.ffs,TAG_END);
                        }
                        else if ((code == keys[1]) || (code == keys[2]) || (code == '\r')) {
                          if (do_install(&req,gtgads[2]/*infobox*/,
                              devicelist->items[devicelist->itemselected],
                              code == keys[2],(/*!mask &&*/ prefs.ffs /*gadlist->Flags&GFLG_SELECTED*/)))
                              ignorechange=1;
                        }
                        else if ((code == keys[3]) || (code == 0x1b)) {
//                                set_env("install",gadlist,gadcount,devicelist);
                                set_env((union DD_Prefs *)&prefs,PREFS_INSTALL);
//                                RemoveListView(devicelist,1);
                                CloseRequester(&req);
                                FreeGadgets(gtglist);
                                FreeVisualInfo(gt_vi);
                                return;
                        }
                        break;

//                    case IDCMP_GADGETDOWN:
                    case IDCMP_GADGETUP:
                        gadgetid=((struct Gadget *)iaddress)->GadgetID;
                    case IDCMP_CLOSEWINDOW:
                        if (class==IDCMP_CLOSEWINDOW) gadgetid=INSTALL_CANCEL;

                        switch (gadgetid) {
                            case INSTALL_CANCEL:
//                                set_env("install",gadlist,gadcount,devicelist);
                                set_env((union DD_Prefs *)&prefs,PREFS_INSTALL);
//                                RemoveListView(devicelist,1);
                                CloseRequester(&req);
                                FreeGadgets(gtglist);
                                FreeVisualInfo(gt_vi);
                                return;

                            case INSTALL_FFS:
                                prefs.ffs = code;
//D(bug("FFS %s\n",code?"ON":"OFF"));
                                break;

                            case INSTALL_INSTALL:
                            case INSTALL_NOBOOT:
                              if (do_install(&req,gtgads[2]/*infobox*/,
                                  devicelist->items[devicelist->itemselected],
                                  (gadgetid==INSTALL_NOBOOT),(/*!mask &&*/ prefs.ffs/*gadlist->Flags&GFLG_SELECTED*/)))
                                  ignorechange=1;
                                break;

                            case INSTALL_DEVLIST:
                              show_install_info(&req,gtgads[2]/*infobox*/,devicelist->items[devicelist->itemselected=code]);
                              break;
                        }
                        break;
                }
            }
/*
            else if (view) {
                show_install_info(&req,infobox,view->items[view->itemselected]);
            }
*/
        }
        Wait(1<<window->UserPort->mp_SigBit);
    }
}

void show_install_info(reqbase,border,name)
struct RequesterBase *reqbase;
/*Object_Border*/struct Gadget *border;
char *name;
{
    struct DeviceHandle handle;
    ULONG *buffer,tracksize,dostype;
    char infobuf[80];
    int a,sum,lastsum,ffs;

D(bug("show_install_info: %s\n",name));
    border_text(reqbase,border,NULL);
//    GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,NULL,TAG_END);
    strcpy(infobuf,string_table[STR_INSTALL_NOINFO]);

    if (open_device(name,&handle)) {
        handle.device_req->iotd_Req.io_Command=TD_CHANGESTATE;
        DoIO((struct IORequest *)handle.device_req);
        if (handle.device_req->iotd_Req.io_Actual)
            lsprintf(infobuf,string_table[STR_NODISKPRESENT],name);
        else {
            tracksize=(handle.dosenvec->de_SizeBlock<<2)*
                handle.dosenvec->de_Surfaces*handle.dosenvec->de_BlocksPerTrack,
            drive_motor(handle.device_req,1);
            if ((buffer=(ULONG *)AllocMem(TD_SECTOR*2,handle.dosenvec->de_BufMemType))) {
                handle.device_req->iotd_Req.io_Command=CMD_READ;
                handle.device_req->iotd_Req.io_Data=(APTR)buffer;
                handle.device_req->iotd_Req.io_Offset=handle.dosenvec->de_LowCyl*tracksize;
                handle.device_req->iotd_Req.io_Length=TD_SECTOR*2;
                if (!(DoIO((struct IORequest *)handle.device_req))) {
                    dostype=buffer[0];
                    for (a=0,sum=0;a<TD_SECTOR>>1;a++) {
                        lastsum=sum;
                        sum+=buffer[a];
                        if (lastsum>sum) ++sum;
                    }
                    if (sum!=0) strcpy(infobuf,string_table[STR_INSTALL_NOTBOOTABLE]);
                    else {
                        if (dostype&1) ffs=1;
                        else ffs=0;

                        if (install_compare_block(buffer,bootblock_13,13)) {
                            lsprintf(infobuf,string_table[STR_INSTALL_NORMAL],
                                "1.3",(ffs)?"FFS":"OFS");
                        }
                        else if (install_compare_block(buffer,bootblock_20,24)) {
                            lsprintf(infobuf,string_table[STR_INSTALL_NORMAL],
                                "2.0+",(ffs)?"FFS":"OFS");
                            if ((dostype&0xfffffffe)==ID_INTER_DOS_DISK)
                                strcat(infobuf," (Inter)");
                            else if ((dostype&0xfffffffe)==ID_FASTDIR_DOS_DISK)
                                strcat(infobuf," (Cache)");
                        }
                        else strcpy(infobuf,string_table[STR_INSTALL_NONSTANDARD]);
                    }
                }
                FreeMem(buffer,TD_SECTOR*2);
            }
        }
        drive_motor(handle.device_req,0);
    }
    close_device(&handle);
    border_text(reqbase,border,infobuf);
//    GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)infobuf,TAG_END);
}

int install_compare_block(block,testblock,size)
ULONG *block,*testblock,size;
{
    int a;

    for (a=3;a<size;a++)
        if (block[a]!=testblock[a])
            return(0);
    return(1);
}

int do_install(reqbase,border,device,noboot,forceffs)
struct RequesterBase *reqbase;
/*Object_Border*/struct Gadget *border;
char *device;
int noboot,forceffs;
{
    struct DeviceHandle handle;
    ULONG dostype;
    ULONG *buffer,tracksize;
    BPTR lock;
    struct InfoData __aligned data;
    int a,err=STR_SUCCESS,sum,lastsum;
    struct Requester busyreq;

    InitRequester(&busyreq);
    busyreq.Flags=NOISYREQ;
    Request(&busyreq,reqbase->rb_window);
    SetBusyPointer(reqbase->rb_window);

    if (open_device(device,&handle)) {
        border_text(reqbase,border,string_table[STR_CHECKING_DESTINATION]);
//        GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)string_table[STR_CHECKING_DESTINATION],TAG_END);
        drive_motor(handle.device_req,1);
        tracksize=(handle.dosenvec->de_SizeBlock<<2)*
            handle.dosenvec->de_Surfaces*handle.dosenvec->de_BlocksPerTrack;

        if (check_disk(reqbase,handle.device_req,device,1)) {
            if ((lock=Lock(device,ACCESS_READ))) {
                Info(lock,&data);
                UnLock(lock);

                dostype=data.id_DiskType;

                border_text(reqbase,border,string_table[STR_INSTALL_INSTALLINGDISK]);
//                GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)string_table[STR_INSTALL_INSTALLINGDISK],TAG_END);

                if ((dostype&~(0xff))==0x444f5300) {
                    if (forceffs) dostype|=1;
                    if ((buffer=(ULONG *)AllocMem(TD_SECTOR*2,handle.dosenvec->de_BufMemType|MEMF_CLEAR))) {

                        if (noboot) buffer[0]=dostype;
                        else {
/*                            if (SysBase->LibNode.lib_Version<36)
                                CopyMem((char *)bootblock_13,(char *)buffer,sizeof(bootblock_13));
                            else*/ CopyMem((char *)bootblock_20,(char *)buffer,sizeof(bootblock_20));

                            buffer[0]=dostype;

                            for (a=0,sum=0;a<TD_SECTOR>>1;a++) {
                                lastsum=sum;
                                sum+=buffer[a];
                                if (lastsum>sum) ++sum;
                            }

                            buffer[1]=~sum;
                        }

                        inhibit_drive(device,DOSTRUE);

                        FOREVER {
                            handle.device_req->iotd_Req.io_Command=CMD_WRITE;
                            handle.device_req->iotd_Req.io_Data=(APTR)buffer;
                            handle.device_req->iotd_Req.io_Offset=handle.dosenvec->de_LowCyl*tracksize;
                            handle.device_req->iotd_Req.io_Length=TD_SECTOR*2;

                            if (!(DoIO((struct IORequest *)handle.device_req))) {
                                handle.device_req->iotd_Req.io_Command=CMD_UPDATE;
                                if (!(DoIO((struct IORequest *)handle.device_req))) break;
                            }

                            border_text(reqbase,border,string_table[STR_INSTALL_ERRORWRITING]);
//                            GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)string_table[STR_INSTALL_ERRORWRITING],TAG_END);
                            if (!(check_error(reqbase,string_table[STR_INSTALL_ERRORWRITING],STR_RETRY))) {
                                err=-1;
                                break;
                            }
                        }

                        inhibit_drive(device,FALSE);
                        FreeMem(buffer,TD_SECTOR*2);
                    }
                    else err=STR_MEMORY_ERROR;
                }
                else err=STR_INVALID_DISK;
            }
            else err=STR_INVALID_DISK;
        }
        else err=STR_ABORTED;

        drive_motor(handle.device_req,0);
    }
    else err=STR_NODEVICE_ERROR;

    if (err>-1) border_text(reqbase,border,string_table[err]);
//                GT_SetGadgetAttrs(border,reqbase->rb_window,NULL,GTTX_Text,(Tag)string_table[err],TAG_END);

    close_device(&handle);

    EndRequest(&busyreq,reqbase->rb_window);
    ClearPointer(reqbase->rb_window);
    return((err==STR_SUCCESS));
}
