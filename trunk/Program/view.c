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

#include "dopus.h"
#include "view.h"
#include "searchdata.h"
#include <newmouse.h>
//#include <proto/powerpacker.h>
#include <proto/xfdmaster.h>

static struct TagItem wintags[2] =
 {
  { WA_AutoAdjust, TRUE },
  { TAG_END }
 };

static struct ExtNewWindow
  viewwin={
    0,0,0,0,
    255,255,
    IDCMP_RAWKEY|IDCMP_VANILLAKEY|IDCMP_MOUSEBUTTONS|IDCMP_GADGETUP|
      IDCMP_GADGETDOWN|IDCMP_INACTIVEWINDOW|IDCMP_ACTIVEWINDOW|IDCMP_MOUSEMOVE,
    WFLG_BORDERLESS|WFLG_RMBTRAP,
    NULL,NULL,NULL,NULL,NULL,0,0,0,0,CUSTOMSCREEN,
    wintags},
  ansiread_win={
    0,0,0,0,
    255,255,
    0,
    WFLG_BORDERLESS|WFLG_BACKDROP,
    NULL,NULL,NULL,NULL,NULL,
    0,0,0,0,CUSTOMSCREEN,
    wintags};

struct ViewMessage {
  char *filename;
  char *name;
  int function;
  char *initialsearch;
  struct ViewData *viewdata;
  BOOL deleteonexit;
};

int viewfile(filename,name,function,initialsearch,viewdata,wait,noftype)
char *filename,*name;
int function;
char *initialsearch;
struct ViewData *viewdata;
int wait,noftype;
{
  struct ArbiterLaunch launch;
  struct ViewMessage *view_message;
  struct DOpusRemember *memkey=NULL;
  int flags;

  if (!noftype) {
    switch (function) {
      case FUNC_HEXREAD:
        if (checkfiletypefunc(filename,FTFUNC_HEXREAD)) return(1);
        break;
      case FUNC_ANSIREAD:
        if (checkfiletypefunc(filename,FTFUNC_ANSIREAD)) return(1);
        break;
      default:
        if (checkfiletypefunc(filename,FTFUNC_READ)) return(1);
        break;
    }
  }

  if ((view_message=LAllocRemember(&memkey,sizeof(struct ViewMessage),MEMF_CLEAR)) &&
    copy_string(filename,&view_message->filename,&memkey) &&
    copy_string(name,&view_message->name,&memkey) &&
    copy_string(initialsearch,&view_message->initialsearch,&memkey)) {

D(bug("view message:\t%lx (%ld bytes)\n",view_message,sizeof(struct ViewMessage)));
D(bug("viewmsg filename: %lx (%s)\n",view_message->filename,filename?filename:"<NULL>"));
D(bug("viewmsg name:\t%lx (%s)\n",view_message->name,name?name:"<NULL>"));
D(bug("viewmsg initsearch:\t%lx (%s)\n",view_message->initialsearch,initialsearch?initialsearch:"<NULL>"));
    launch.launch_code=(void *)view_file_process;
    launch.launch_name="dopus_view";
    launch.launch_memory=memkey;
    launch.data=(APTR)view_message;

    view_message->function=function;
    view_message->viewdata=viewdata;
    if (dopus_curwin[data_active_window]->flags & DWF_ARCHIVE) view_message->deleteonexit = TRUE;

    if (wait || viewdata) flags=ARB_WAIT;
    else flags=0;
    return(arbiter_command(ARBITER_LAUNCH,(APTR)&launch,flags));
  }

  LFreeRemember(&memkey);
  return(0);
}

void __saveds view_file_process()
{
  int a,b,c,in,/*err,*/size,/*old_file_size,old_buffer_size,*/fsize,retcode=100;
  short
    scroll_speed,
    scroll_pos_1,
    scroll_pos_2,
    scroll_pos_3,
    scroll_pos_4,
    scroll_pos_5,
    scroll_pos_6,
    scroll_pos_7;
  ULONG class;
  USHORT code,qual,gadgetid;
  char buf[60];
  struct ConUnit *view_console_unit=NULL;
  struct ViewData *vdata = NULL;
  struct Gadget *gadget;
  struct IntuiMessage *msg;
  struct Process *my_process;
  struct ArbiterMessage *my_startup_message;
  struct ViewMessage *view_msg;
  char *filename,*name;
  int function;
  char *initialsearch;
  struct ViewData *viewdata;
  struct MsgPort *view_port;
  struct IOStdReq *view_req=NULL;
//  UBYTE key_matrix[13];
  char portname[20],titlebuf[300];

  my_process=(struct Process *)FindTask(NULL);
  WaitPort(&my_process->pr_MsgPort);
  my_startup_message=(struct ArbiterMessage *)GetMsg(&my_process->pr_MsgPort);

  view_msg=(struct ViewMessage *)(my_startup_message->data);
  filename=view_msg->filename;
  name=view_msg->name;
  function=view_msg->function;
  initialsearch=view_msg->initialsearch;
  viewdata=view_msg->viewdata;

  Forbid();
  for (a=0;;a++) {
    lsprintf(portname,"DOPUS_VIEW%ld",a);
    if (!(FindPort(portname))) break;
  }
  Permit();

  if (!(view_port=LCreatePort(portname,0))) goto view_end;
  if (view_req=(struct IOStdReq *)LCreateExtIO(view_port,sizeof(struct IOStdReq))) {
    if (OpenDevice("keyboard.device",0,(struct IORequest *)view_req,0)) {
      LDeleteExtIO((struct IORequest *)view_req);
      view_req=NULL;
      goto view_end;
    }
  }

  if (CheckExist(filename,&size)>=0) {
    retcode=-2;
    goto view_end;
  }
  if (size<1) {
    retcode=-3;
    goto view_end;
  }

  if (viewdata)
   {
    vdata=viewdata;
D(bug("external vdata: %lX\n",vdata));
   }
  else {
    if (!(vdata=AllocMem(sizeof(struct ViewData),MEMF_CLEAR))) {
      retcode=-4;
      goto view_end;
    }
D(bug("internal vdata: %lX (%ld bytes)\n",vdata,sizeof(struct ViewData)));
  }
  strcpy(vdata->view_port_name,portname);
  vdata->view_port=view_port;
  vdata->view_file_size=size;

  view_clearsearch(vdata);

  vdata->view_search_string[0]=
    vdata->view_scroll_dir=
    vdata->view_scroll=
    vdata->view_last_line=
    vdata->view_last_charpos=0;
  vdata->view_last_char=NULL;
  vdata->view_first_hilite=
    vdata->view_current_hilite=NULL;
  vdata->view_search_flags=search_flags;

  if (vdata->view_window) {
    Forbid();
    vdata->view_window->UserPort->mp_SigTask=(void *)my_process;
    Permit();
  }
  else {
    short scr_pens[ARB_PEN_LASTPEN];

    vdata->view_colour_table[PEN_BACKGROUND]=0;
    vdata->view_colour_table[PEN_SHADOW]=config->gadgetbotcol;
    vdata->view_colour_table[PEN_SHINE]=config->gadgettopcol;
    vdata->view_colour_table[PEN_TEXT]=1;
    vdata->view_colour_table[PEN_TEXTBACKGROUND]=0;
    vdata->view_colour_table[VIEWPEN_STATUSTEXT]=config->statusfg;
    vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND]=config->statusbg;
    vdata->view_colour_table[VIEWPEN_LAST_COLOUR]=-1;

    if (config->statusbg==0) {
      scr_pens[ARB_PEN_DETAIL]=config->statusbg;
      scr_pens[ARB_PEN_BLOCK]=config->statusfg;
    }
    else {
      scr_pens[ARB_PEN_DETAIL]=config->statusfg;
      scr_pens[ARB_PEN_BLOCK]=config->statusbg;
    }

    if ( !( config->viewbits & VIEWBITS_INWINDOW ) ) if (!(vdata->view_screen= //HUX
      open_subprocess_screen(globstring[STR_TEXT_VIEWER_TITLE],
         scr_font[FONT_TEXT],&vdata->view_memory,
        /*(system_version2)?*/NULL/*:scr_pens*/)) ||
        !(vdata->view_font=OpenFont(vdata->view_screen->Font))) {
      retcode=-4;
      goto view_end;
    }

    if ( config->viewbits & VIEWBITS_INWINDOW ) //HUX
     {
      vdata->view_font = scr_font[FONT_TEXT];
// HUX: begin
      if ( config->viewtext_topleftx < 0 ) viewwin.LeftEdge = 0;
      else viewwin.LeftEdge = config->viewtext_topleftx;

      if ( config->viewtext_toplefty < 0 ) viewwin.TopEdge = 0;
      else viewwin.TopEdge = config->viewtext_toplefty;

      if ( config->viewtext_width <= 0 ) viewwin.Width = Window->WScreen->Width + config->viewtext_width; // The value is negative;//MainScreen->Width
      else viewwin.Width = config->viewtext_width;
      if ( viewwin.Width < 50 ) viewwin.Width = 50;

      if ( config->viewtext_height <= 0 ) viewwin.Height = Window->WScreen->Height + config->viewtext_height; // The value is negative //MainScreen->Height
      else viewwin.Height = config->viewtext_height;
      if ( viewwin.Height < 50 ) viewwin.Height = 50;
// HUX: end
      viewwin.Flags |= WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_NOCAREREFRESH;
      viewwin.Flags &= ~(WFLG_BORDERLESS /*| WFLG_RMBTRAP*/);
      viewwin.IDCMPFlags |= IDCMP_CLOSEWINDOW;
//      if (vdata->view_vis_info.vi_flags&VISF_WINDOW) viewwin.Screen = (struct Screen *)vdata->view_window;
//      else viewwin.Screen = LockPubScreen(NULL);
            viewwin.Screen = Window->WScreen;// HUX
     }
    else
     {
// HUX: begin
          viewwin.Flags = WFLG_BORDERLESS | WFLG_RMBTRAP;
      viewwin.IDCMPFlags = IDCMP_RAWKEY | IDCMP_VANILLAKEY |
          IDCMP_MOUSEBUTTONS | IDCMP_GADGETUP | IDCMP_GADGETDOWN |
          IDCMP_INACTIVEWINDOW | IDCMP_ACTIVEWINDOW |
          IDCMP_MOUSEMOVE;
// HUX: end
      viewwin.TopEdge=vdata->view_screen->BarHeight+2;
      viewwin.Width=vdata->view_screen->Width;
      viewwin.Height=vdata->view_screen->Height-(vdata->view_screen->BarHeight+2);
      viewwin.Screen=vdata->view_screen;
     }
    viewwin.BlockPen=vdata->view_colour_table[PEN_SHINE];

    if (!(vdata->view_gadgets=LAllocRemember(&vdata->view_memory,
        sizeof(struct Gadget)*VIEW_GADGET_COUNT,MEMF_CLEAR)) ||
      !(vdata->view_window=OpenWindowTags(&viewwin,WA_AutoAdjust,TRUE,TAG_END))) {
//      if (viewonwb) /*if (!(vdata->view_vis_info.vi_flags&VISF_WINDOW))*/ UnlockPubScreen(NULL,viewwin.Screen);
//      else CloseScreen(vdata->view_screen);
      if ( !( config->viewbits & VIEWBITS_INWINDOW ) ) CloseScreen(vdata->view_screen); //HUX
      LFreeRemember(&vdata->view_memory);
      if (!viewdata) FreeMem(vdata,sizeof(struct ViewData));
      retcode=-4;
      goto view_end;
    }
D(bug("view gads: %lX (%ld bytes)\n",vdata->view_gadgets,sizeof(struct Gadget)*VIEW_GADGET_COUNT));

    vdata->view_screen = vdata->view_window->WScreen;
    vdata->view_rastport=vdata->view_window->RPort;

    if ( config->viewbits & VIEWBITS_INWINDOW ) //HUX
     {
//      UnlockPubScreen(NULL,vdata->view_screen);
      SetFont(vdata->view_rastport,vdata->view_font);
     }

    vdata->view_vis_info.vi_fg=vdata->view_colour_table[VIEWPEN_STATUSTEXT];
    vdata->view_vis_info.vi_bg=vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND];
    vdata->view_vis_info.vi_shine=vdata->view_colour_table[PEN_SHINE];
    vdata->view_vis_info.vi_shadow=vdata->view_colour_table[PEN_SHADOW];

    vdata->view_vis_info.vi_stringcol[0]=vdata->view_colour_table[PEN_TEXT];
    vdata->view_vis_info.vi_stringcol[1]=vdata->view_colour_table[PEN_TEXTBACKGROUND];
    vdata->view_vis_info.vi_activestringcol[0]=vdata->view_colour_table[PEN_TEXT];
    vdata->view_vis_info.vi_activestringcol[1]=vdata->view_colour_table[PEN_TEXTBACKGROUND];

    if (vdata->view_window->UserData=LAllocRemember(&vdata->view_memory,SEARCH_COLOURS,0)) {
D(bug("view userdata: %lX (%ld bytes)\n",vdata->view_window->UserData,SEARCH_COLOURS));
      vdata->view_window->UserData[SEARCH_COL_FG]=
        vdata->view_colour_table[VIEWPEN_STATUSTEXT];
      vdata->view_window->UserData[SEARCH_COL_BG]=
        vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND];
      vdata->view_window->UserData[SEARCH_COL_SHINE]=
        vdata->view_colour_table[PEN_SHINE];
      vdata->view_window->UserData[SEARCH_COL_SHADOW]=
        vdata->view_colour_table[PEN_SHADOW];
      vdata->view_window->UserData[SEARCH_COL_STRINGFG]=
        vdata->view_colour_table[PEN_TEXT];
      vdata->view_window->UserData[SEARCH_COL_STRINGBG]=
        vdata->view_colour_table[PEN_TEXTBACKGROUND];
      vdata->view_window->UserData[SEARCH_COL_STRINGSELFG]=
        vdata->view_colour_table[PEN_TEXT];
      vdata->view_window->UserData[SEARCH_COL_STRINGSELBG]=
        vdata->view_colour_table[PEN_TEXTBACKGROUND];
    }

    vdata->view_vis_info.vi_font=vdata->view_font;
    if (vdata->view_vis_info.vi_flags&VISF_WINDOW)
      vdata->view_vis_info.vi_screen=(struct Screen *)vdata->view_window;
    else vdata->view_vis_info.vi_screen=vdata->view_screen;

    if (config->generalscreenflags&SCR_GENERAL_REQDRAG)
      vdata->view_vis_info.vi_flags|=VISF_BORDERS;

    vdata->view_vis_info.vi_language=config->language;

    vdata->view_status_bar_ypos=vdata->view_window->Height-(vdata->view_font->tf_YSize+1);
    if ( config->viewbits & VIEWBITS_INWINDOW ) vdata->view_status_bar_ypos -= vdata->view_window->BorderBottom; //HUX

    if ((config->viewbits&VIEWBITS_TEXTBORDERS) /*|| viewonwb*/) {

      vdata->view_display_left = 2;
      vdata->view_display_top  = 2;
      vdata->view_display_right = vdata->view_window->Width-20;
      if ( config->viewbits & VIEWBITS_INWINDOW ) //HUX
       {
        vdata->view_display_left  += vdata->view_window->BorderLeft;
        vdata->view_display_top   += vdata->view_window->BorderTop;
        vdata->view_display_right -= vdata->view_window->BorderRight;
       }
      vdata->view_display_bottom = vdata->view_status_bar_ypos-3;

      vdata->view_lines_per_screen=(vdata->view_display_bottom - vdata->view_display_top)/vdata->view_font->tf_YSize;

      Do3DBox(vdata->view_rastport,vdata->view_display_left,vdata->view_display_top-1,
        (vdata->view_display_right-vdata->view_display_left)+2,
        vdata->view_display_bottom-vdata->view_display_top+2,
        vdata->view_colour_table[PEN_SHINE],
        vdata->view_colour_table[PEN_SHADOW]);

      vdata->view_gadgets[VIEW_SCROLLGADGET].LeftEdge=vdata->view_display_right+8;
      vdata->view_gadgets[VIEW_SCROLLGADGET].TopEdge=vdata->view_display_top;
      vdata->view_gadgets[VIEW_SCROLLGADGET].Height=vdata->view_display_bottom-vdata->view_display_top;
      vdata->view_gadgets[VIEW_SCROLLGADGET].Width=8;
      vdata->view_gadgets[VIEW_SCROLLGADGET].Flags=GFLG_GADGHNONE;
      vdata->view_gadgets[VIEW_SCROLLGADGET].Activation=
        GACT_IMMEDIATE|GACT_RELVERIFY|GACT_FOLLOWMOUSE;
      vdata->view_gadgets[VIEW_SCROLLGADGET].GadgetType=GTYP_PROPGADGET;
      vdata->view_gadgets[VIEW_SCROLLGADGET].GadgetRender=
        (APTR)&vdata->view_prop_image;
      vdata->view_gadgets[VIEW_SCROLLGADGET].SpecialInfo=
        (APTR)&vdata->view_prop_info;
      vdata->view_gadgets[VIEW_SCROLLGADGET].GadgetID=VIEW_SCROLLGADGET;

      vdata->view_prop_info.Flags=AUTOKNOB|FREEVERT|PROPNEWLOOK|PROPBORDERLESS;
      vdata->view_prop_info.VertBody=MAXBODY;

      AddGList(vdata->view_window,&vdata->view_gadgets[VIEW_SCROLLGADGET],-1,1,NULL);
      RefreshGList(&vdata->view_gadgets[VIEW_SCROLLGADGET],vdata->view_window,NULL,1);
      Do3DBox(vdata->view_rastport,
        vdata->view_gadgets[VIEW_SCROLLGADGET].LeftEdge-2,
        vdata->view_gadgets[VIEW_SCROLLGADGET].TopEdge-1,
        vdata->view_gadgets[VIEW_SCROLLGADGET].Width+4,
        vdata->view_gadgets[VIEW_SCROLLGADGET].Height+2,
        vdata->view_colour_table[PEN_SHINE],
        vdata->view_colour_table[PEN_SHADOW]);

      vdata->view_scroll_bar=1;
    }
    else {
      vdata->view_display_left = vdata->view_display_top = 0;
      vdata->view_display_right = vdata->view_window->Width-1;
      if ( config->viewbits & VIEWBITS_INWINDOW ) //HUX
       {
        vdata->view_display_left  += vdata->view_window->BorderLeft;
        vdata->view_display_top   += vdata->view_window->BorderTop;
        vdata->view_display_right -= vdata->view_window->BorderRight;
       }
      vdata->view_lines_per_screen=(vdata->view_status_bar_ypos-vdata->view_display_top-1)/vdata->view_font->tf_YSize;

      vdata->view_display_bottom=vdata->view_display_top+
        (vdata->view_lines_per_screen*vdata->view_font->tf_YSize)-1;
    }

    vdata->view_display_height=vdata->view_lines_per_screen*vdata->view_font->tf_YSize;

    for (a=9;a<11;a++) {
      vdata->view_gadgets[a].TopEdge=vdata->view_status_bar_ypos;
      vdata->view_gadgets[a].Width=vdata->view_font->tf_XSize*((a==9)?6:4);
      vdata->view_gadgets[a].Height=vdata->view_font->tf_YSize;
      vdata->view_gadgets[a].Flags=GFLG_GADGHCOMP;
      vdata->view_gadgets[a].Activation=GACT_RELVERIFY;
      vdata->view_gadgets[a].GadgetType=GTYP_BOOLGADGET;
      vdata->view_gadgets[a].GadgetID=a;
    }

    vdata->view_gadgets[VIEW_JUMPTOLINE].NextGadget=&vdata->view_gadgets[VIEW_JUMPTOPERCENT];
    a = vdata->view_window->Width-( ( config->viewbits & VIEWBITS_INWINDOW )?vdata->view_window->BorderRight:0); //HUX
    vdata->view_gadgets[VIEW_JUMPTOLINE].LeftEdge=a-((vdata->view_font->tf_XSize*49)+14);
    vdata->view_gadgets[VIEW_JUMPTOPERCENT].LeftEdge=a-((vdata->view_font->tf_XSize*22)+14);

    view_setupscreen(vdata);
    AddGadgetBorders(&vdata->view_memory,
      vdata->view_gadgets,9,
      vdata->view_colour_table[PEN_SHINE],vdata->view_colour_table[PEN_SHADOW]);
    AddGList(vdata->view_window,vdata->view_gadgets,-1,11,NULL);
  }

  vdata->view_char_width=((vdata->view_display_right-vdata->view_display_left)+1)/
    vdata->view_font->tf_XSize;

  if (function==FUNC_HEXREAD) {
    vdata->view_char_width=(vdata->view_char_width>62)?62:vdata->view_char_width;
    vdata->view_max_line_length=16;
  }
  else vdata->view_max_line_length=vdata->view_char_width+1;

D(bug("charwidth: %ld\n",vdata->view_max_line_length));
  a=vdata->view_window->Height/8;
  scroll_pos_1=a;
  scroll_pos_2=a*2;
  scroll_pos_3=a*3;
  scroll_pos_4=a*4;
  scroll_pos_5=a*5;
  scroll_pos_6=a*6;
  scroll_pos_7=a*7;

  vdata->view_display_as_hex=(function==FUNC_HEXREAD);

//  view_busy(vdata);
  ActivateWindow(vdata->view_window);

  view_status_text(vdata,globstring[STR_READING_FILE]);
//tryload: // HUX this label is not used
  if (vdata->view_file_size>16) {
     if ((vdata->view_file_size&15)==0)
       vdata->view_buffer_size=vdata->view_file_size+16;
     else vdata->view_buffer_size=((vdata->view_file_size>>4)+1)<<4;

     for (a=0;a<2;a++) {
       if (vdata->view_text_buffer=AllocMem(vdata->view_buffer_size,MEMF_ANY))
         break;
       view_status_text(vdata,globstring[STR_NO_MEMORY_TO_DECRUNCH]);
       if ((vdata->view_buffer_size=AvailMem(MEMF_PUBLIC|MEMF_LARGEST))<16)
         break;
       if (vdata->view_buffer_size<=vdata->view_file_size)
         vdata->view_file_size=vdata->view_buffer_size;
     }
     if (!vdata->view_text_buffer) goto cleanup;
D(bug("view textbuf: %lX (%ld bytes)\n",vdata->view_text_buffer,vdata->view_buffer_size));
     if (!(in=Open(filename,MODE_OLDFILE))) goto cleanup;
     {
      char *bufpos;
      int chunk,done,stop=0;
      struct IntuiMessage *imsg;
      int class, code;

      bufpos = vdata->view_text_buffer;
      chunk = 64*1024;
      for(done = 0; (done < vdata->view_file_size) && !stop;)
       {
        if ((done + chunk) > vdata->view_file_size) chunk = vdata->view_file_size - done;
        fsize=Read(in,bufpos,chunk);
        while(imsg = (struct IntuiMessage *)GetMsg(vdata->view_window->UserPort))
         {
          class = imsg->Class;
          code = imsg->Code;
          ReplyMsg((struct Message *)imsg);
          if ((class == IDCMP_MOUSEBUTTONS) && (code == MENUDOWN))
           {
D(bug("*** USER BREAK ***\t%ld (0x%lx) bytes read\n",done+fsize,done+fsize));
//      Close(in);
//      goto cleanup;
      stop = 1;
           }
         }
        if (fsize <= 0) break;
        bufpos += fsize;
        done += fsize;
        fsize = done;
       }
     }
     if ((vdata->view_buffer_size!=vdata->view_file_size) ||
       ((fsize>-1) && (fsize!=vdata->view_file_size))) vdata->view_file_size=fsize;
     Close(in);
D(bug("view_file_size=%ld\n",fsize));
     if (fsize==-1) goto cleanup;
     view_busy(vdata);
     if (OpenXFDlib())
      {
       struct xfdBufferInfo *xfdbi;

       if (xfdbi = xfdAllocObject(XFDOBJ_BUFFERINFO))
        {
         xfdbi->xfdbi_SourceBuffer = vdata->view_text_buffer;
         xfdbi->xfdbi_SourceBufLen = vdata->view_file_size;
//         xfdbi->xfdbi_Flags = XFDFB_RECOGEXTERN;
         if (xfdRecogBuffer(xfdbi))
          {
           xfdbi->xfdbi_TargetBufMemType = MEMF_ANY;
           if (xfdDecrunchBuffer(xfdbi))
            {
             FreeMem(xfdbi->xfdbi_SourceBuffer,vdata->view_buffer_size);
             vdata->view_text_buffer = xfdbi->xfdbi_TargetBuffer;
             vdata->view_buffer_size = xfdbi->xfdbi_TargetBufLen;
             vdata->view_file_size   = xfdbi->xfdbi_TargetBufSaveLen;
            }
           else view_status_text(vdata,globstring[STR_NO_MEMORY_TO_DECRUNCH]);
          }
         xfdFreeObject(xfdbi);
        }
      }
     vdata->view_last_char=vdata->view_text_buffer+vdata->view_buffer_size-16;
     vdata->view_last_charpos=vdata->view_file_size-
       (vdata->view_last_char-vdata->view_text_buffer);
/*
    if (!PPBase ||
      (err=ppLoadData(filename,DECR_POINTER,MEMF_CLEAR,
        (UBYTE **)&vdata->view_text_buffer,&size,NULL))==PP_UNKNOWNPP)
      goto readnormal;

    old_file_size=vdata->view_file_size;
    old_buffer_size=vdata->view_buffer_size;

    vdata->view_file_size=vdata->view_buffer_size=size;

    if ((size%16)==0) size+=16;
    else size=((vdata->view_file_size/16)+1)*16;

    vdata->view_last_char=vdata->view_text_buffer+size-16;
    vdata->view_last_charpos=vdata->view_file_size-
      (vdata->view_last_char-vdata->view_text_buffer);

    if (err) {
      vdata->view_text_buffer=NULL;
      vdata->view_file_size=0;

      if (err==PP_NOMEMORY) {
        view_status_text(vdata,globstring[STR_NO_MEMORY_TO_DECRUNCH]);
        vdata->view_file_size=old_file_size;
        vdata->view_buffer_size=old_buffer_size;
        goto readnormal;
      }
      else if (err==PP_PASSERR) {
        view_status_text(vdata,globstring[STR_BAD_PASSWORD]);
        view_simplerequest(vdata,globstring[STR_BAD_PASSWORD],globstring[STR_CONTINUE],NULL);
        goto cleanup;
      }
      else {
readnormal:
        if (!(in=Open(filename,MODE_OLDFILE))) goto cleanup;
        if ((vdata->view_file_size%16)==0)
          vdata->view_buffer_size=vdata->view_file_size+16;
        else vdata->view_buffer_size=((vdata->view_file_size/16)+1)*16;

        for (a=0;a<2;a++) {
          if (vdata->view_text_buffer=AllocMem(vdata->view_buffer_size,MEMF_CLEAR))
            break;
          if ((vdata->view_buffer_size=AvailMem(MEMF_PUBLIC|MEMF_LARGEST))<16)
            break;
          if (vdata->view_buffer_size<=vdata->view_file_size)
            vdata->view_file_size=vdata->view_buffer_size;
        }
        if (!vdata->view_text_buffer) {
          Close(in);
          goto cleanup;
        }

        fsize=Read(in,vdata->view_text_buffer,vdata->view_file_size);

        if (vdata->view_buffer_size!=vdata->view_file_size &&
          fsize>-1 && fsize!=vdata->view_file_size) vdata->view_file_size=fsize;

        Close(in);
        if (fsize==-1) goto cleanup;
        vdata->view_last_char=vdata->view_text_buffer+vdata->view_buffer_size-16;
        vdata->view_last_charpos=vdata->view_file_size-
          (vdata->view_last_char-vdata->view_text_buffer);
      }
    }
*/
  }
  else {
    vdata->view_buffer_size=16;
    if (!(in=Open(filename,MODE_OLDFILE))) goto cleanup;
    if (!(vdata->view_text_buffer=AllocMem(16,MEMF_CLEAR))) {
      Close(in);
      goto cleanup;
    }
    vdata->view_file_size=Read(in,vdata->view_text_buffer,16);
    Close(in);
    vdata->view_last_char=vdata->view_text_buffer;
    vdata->view_last_charpos=vdata->view_file_size;
  }

  vdata->view_text_offset=
    vdata->view_old_offset=
    vdata->view_line_count=
    vdata->view_top_buffer_pos=
    vdata->view_bottom_buffer_pos=0;

  view_status_text(vdata,globstring[STR_COUNTING_LINES]);

  vdata->view_tab_size=config->tabsize;

  if (function==FUNC_SMARTREAD) {
    if ((vdata->view_line_count=smartcountlines(vdata))==-1) {
      vdata->view_display_as_hex=1;
      vdata->view_max_line_length=16;
      vdata->view_char_width=62;
    }
    else if (vdata->view_line_count==-2) function=FUNC_ANSIREAD;
//    else removetabs(vdata);
  }

  if (function==FUNC_ANSIREAD) {
    vdata->view_line_count=ansicountlines(vdata);
  }
  else {
    if (vdata->view_display_as_hex) {
//      vdata->view_file_size=vdata->view_buffer_size;
      vdata->view_line_count=vdata->view_file_size/16;
      if (vdata->view_file_size<16) {
        vdata->view_line_count=1;
      }
      else if (vdata->view_file_size%16!=0) {
        ++vdata->view_line_count;
      }
    }
    else {
      vdata->view_line_count=countlines(vdata);
    }
  }
  vdata->view_last_line=vdata->view_line_count-vdata->view_lines_per_screen;
  if (vdata->view_last_line<0) vdata->view_last_line=0;

  if (function==FUNC_ANSIREAD && !vdata->view_display_as_hex) {
    vdata->view_display_as_ansi=1;

    ansiread_win.LeftEdge = viewwin.LeftEdge;
    ansiread_win.TopEdge  = viewwin.TopEdge;
    ansiread_win.Width=vdata->view_char_width*vdata->view_font->tf_XSize;
    ansiread_win.Height=vdata->view_font->tf_YSize*6; //HUX was 3
/*
    if ( config->viewbits & VIEWBITS_INWINDOW ) //HUX
     {
      ansiread_win.Flags |= WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_NOCAREREFRESH;
      ansiread_win.Flags &= ~(WFLG_BORDERLESS | WFLG_RMBTRAP);
      ansiread_win.IDCMPFlags |= IDCMP_CLOSEWINDOW;
     }
     else // HUX
     {
      ansiread_win.Flags = WFLG_BORDERLESS | WFLG_BACKDROP;
        ansiread_win.IDCMPFlags &= ~IDCMP_CLOSEWINDOW;
     }
*/
    ansiread_win.Screen=vdata->view_screen;

    if (!(vdata->view_ansiread_window=OpenWindow(&ansiread_win)))
      vdata->view_display_as_ansi=0;
    else {
      WindowToBack(vdata->view_ansiread_window);
      SetFont(vdata->view_ansiread_window->RPort,vdata->view_font);
      vdata->view_console_request.io_Data=(APTR)vdata->view_ansiread_window;
      Delay(5);
    }

    if (vdata->view_display_as_ansi) {
      if (OpenDevice("console.device",0,(struct IORequest *)&vdata->view_console_request,0))
        vdata->view_display_as_ansi=0;
      else {
        view_console_unit=(struct ConUnit *)vdata->view_console_request.io_Unit;
        for (a=0;a<MAXTABS;a++) {
          view_console_unit->cu_TabStops[a]=a*config->tabsize;
        }
        view_console_unit->cu_TabStops[MAXTABS-1]=0xffff;
        lsprintf(buf,"\x9b\x30\x20\x70\x9b%ld\x74",vdata->view_lines_per_screen);
        view_print(vdata,buf,1,strlen(buf));
        vdata->view_max_line_length=255;
      }
    }
  }
  else vdata->view_display_as_ansi=0;

  view_status_text(vdata,name);
  vdata->view_file_name=name;
  vdata->view_path_name=filename;
  view_update_status(vdata);
  view_penrp(vdata);

  vdata->view_scroll_width=vdata->view_char_width*vdata->view_font->tf_XSize;
  view_displayall(vdata);
  retcode=1;

  if (config->viewbits&VIEWBITS_TEXTBORDERS) {
    FixSliderBody(vdata->view_window,&vdata->view_gadgets[VIEW_SCROLLGADGET],
      vdata->view_line_count,vdata->view_lines_per_screen,1);
    FixSliderPot(vdata->view_window,&vdata->view_gadgets[VIEW_SCROLLGADGET],
      0,vdata->view_line_count,vdata->view_lines_per_screen,1);
  }

  lsprintf(titlebuf,"%s - \"%s\"",globstring[STR_FILE],vdata->view_path_name);
  if ( config->viewbits & VIEWBITS_INWINDOW ) SetWindowTitles(vdata->view_window,titlebuf,"Directory Opus Reader"); //HUX
  else SetWindowTitles(vdata->view_window,(char *)-1,titlebuf);

  if ( !( config->viewbits & VIEWBITS_INWINDOW ) ) vdata->view_window->WLayer->Flags|=LAYERBACKDROP; //HUX

  view_unbusy(vdata);

  if (initialsearch) {
    strcpy(vdata->view_search_string,initialsearch);
    view_search(vdata,1);
  }

  FOREVER {
    while (msg=(struct IntuiMessage *)GetMsg(vdata->view_window->UserPort)) {
      class=msg->Class; code=msg->Code; qual=msg->Qualifier;
      gadget=(struct Gadget *)msg->IAddress;
      ReplyMsg((struct Message *)msg);

      switch (class) {
        case IDCMP_CLOSEWINDOW:
          retcode=-1;
          goto cleanup;
        case IDCMP_MOUSEMOVE:
          if (vdata->view_gadgets[VIEW_SCROLLGADGET].Flags&GFLG_SELECTED)
            view_fix_scroll_gadget(vdata);
          break;
        case IDCMP_MOUSEBUTTONS:
          if (code==SELECTDOWN) view_togglescroll(vdata);
          else if (code==MENUDOWN) {
            retcode=-1;
            goto cleanup;
          }
          break;
        case IDCMP_GADGETDOWN:
          if (vdata->view_scroll) {
            view_togglescroll(vdata);
            break;
          }
          gadgetid=gadget->GadgetID;
          switch (gadgetid) {
            case VIEW_SCROLLGADGET:
              view_fix_scroll_gadget(vdata);
              break;
            case VIEW_SCROLLUP:
            case VIEW_SCROLLDOWN:
              if (gadgetid==VIEW_SCROLLUP) a=view_lineup(vdata);
              else a=view_linedown(vdata);
              if (!a) break;
              Delay(5);
              FOREVER {
                if (msg=(struct IntuiMessage *)GetMsg(vdata->view_window->UserPort)) {
                  if (msg->Class==IDCMP_GADGETUP ||
                    (msg->Class==IDCMP_MOUSEBUTTONS && msg->Code==SELECTUP))
                    break;
                  ReplyMsg((struct Message *)msg); msg=NULL;
                }
                if (gadgetid==VIEW_SCROLLUP) a=view_lineup(vdata);
                else a=view_linedown(vdata);
                if (!a) break;
              }
              if (msg) ReplyMsg((struct Message *)msg);
              break;
          }
          break;
        case IDCMP_GADGETUP:
          if (vdata->view_scroll) {
            view_togglescroll(vdata);
            break;
          }
          gadgetid=gadget->GadgetID;
testgad:
          switch (gadgetid) {
            case VIEW_SCROLLGADGET:
              view_fix_scroll_gadget(vdata);
              break;
            case VIEW_PAGEUP:
              view_pageup(vdata);
              break;
            case VIEW_PAGEDOWN:
              view_pagedown(vdata);
              break;
            case VIEW_GOTOP:
              view_gotop(vdata);
              break;
            case VIEW_GOBOTTOM:
              view_gobottom(vdata);
              break;
            case VIEW_SEARCH:
              view_search(vdata,1);
              break;
            case VIEW_QUIT:
              goto cleanup;
              break;
            case VIEW_PRINT:
              view_checkprint(vdata,0);
              break;
            case VIEW_JUMPTOLINE:
            case VIEW_JUMPTOPERCENT:
              view_busy(vdata);
              buf[0]=0;
              if (gadgetid==VIEW_JUMPTOLINE) {
                a=10;
                b=STR_JUMP_TO_LINE;
              }
              else {
                a=4;
                b=STR_JUMP_TO_PERCENTAGE;
              }
              if (!(view_whatsit(vdata,globstring[b],a,buf)) || !buf[0]) {
                view_unbusy(vdata);
                break;
              }
              a=atoi(buf); if (a<0) a=0;
              if (gadgetid==VIEW_JUMPTOPERCENT)
                a=((a+1)*(vdata->view_last_line))/100;
              if (a>vdata->view_last_line) a=vdata->view_last_line;

              if (vdata->view_display_as_hex) {
                if (vdata->view_text_offset!=a) {
                  vdata->view_text_offset=a;
                  view_displayall(vdata);
                }
                view_unbusy(vdata);
                break;
              }

              if (vdata->view_text_offset!=a) {
                if (vdata->view_text_offset<a) {
                  if (a-vdata->view_text_offset<vdata->view_last_line-a) {
                    for (b=vdata->view_text_offset;b<a;b++) {
                      for (c=vdata->view_top_buffer_pos;c<vdata->view_file_size;c++)
                        if (vdata->view_text_buffer[c]==10) break;
                      vdata->view_top_buffer_pos=c+1;
                    }
                  }
                  else {
                    vdata->view_text_offset=vdata->view_last_line;
                    vdata->view_top_buffer_pos=vdata->view_file_size;
                    for (c=0;c<vdata->view_lines_per_screen;c++) {
                      for (b=vdata->view_top_buffer_pos-2;b>=0;b--)
                        if (vdata->view_text_buffer[b]==10) break;
                      vdata->view_top_buffer_pos=b+1;
                    }
                    for (b=a;b<vdata->view_text_offset;b++) {
                      for (c=vdata->view_top_buffer_pos-2;c>=0;c--)
                        if (vdata->view_text_buffer[c]==10) break;
                      vdata->view_top_buffer_pos=c+1;
                    }
                  }
                }
                else {
                  if (vdata->view_text_offset-a<a) {
                    for (b=a;b<vdata->view_text_offset;b++) {
                      for (c=vdata->view_top_buffer_pos-2;c>=0;c--)
                        if (vdata->view_text_buffer[c]==10) break;
                      vdata->view_top_buffer_pos=c+1;
                    }
                  }
                  else {
                    vdata->view_top_buffer_pos=0;
                    for (b=0;b<a;b++) {
                      for (c=vdata->view_top_buffer_pos;c<vdata->view_file_size;c++)
                        if (vdata->view_text_buffer[c]==10) break;
                      vdata->view_top_buffer_pos=c+1;
                    }
                  }
                }
                vdata->view_text_offset=a;
                view_displayall(vdata);
              }
              view_unbusy(vdata);
              break;
          }
          break;
        case IDCMP_VANILLAKEY:
D(bug("VANILLAKEY: %lx,%lx\n",code,qual));
          code = ToUpper(code);
          if (strchr(globstring[STR_VIEW_BUTTONS],code))
           {
            switch((ULONG)strchr(globstring[STR_VIEW_BUTTONS],code) - (ULONG)globstring[STR_VIEW_BUTTONS])
             {
              case 0:
                view_pageup(vdata);
                break;
              case 1:
                view_pagedown(vdata);
                break;
              case 2:
                view_gotop(vdata);
                break;
              case 3:
                view_gobottom(vdata);
                break;
              case 4:
                view_search(vdata,1);
                break;
              case 5:
                view_checkprint(vdata,0);
                break;
              case 6:
                goto cleanup;
             }
           }
          else switch (code)
           {
/* Code added by Pavel Cizek, 28. 3. 2000:
*   It enables to use backspace to goto one page up
*/
            case 0x08: // BackSpace
/* End of inserted code */
              view_pageup(vdata);
              break;
/* Code added by Pavel Cizek, 28. 3. 2000:
*   It enables to use space to goto one page down
*   The code for scroll toggle had to be omitted.
*/
            case ' ': // Space
/* End of inserted code */
              view_pagedown(vdata);
              break;
/* Code added by Pavel Cizek, 28. 3. 2000:
*   It enables to use '/' to invoke search window
*/
            case '/':
/* End of inserted code */
              view_search(vdata,1);
              break;
            case 0x1B: // ESC
              retcode=-1;
            case 'X':
              goto cleanup;
            case 'J':
              gadgetid=VIEW_JUMPTOLINE;
              goto testgad;
            case 'N':
              view_search(vdata,0);
              break;
            case 'C':
              view_checkprint(vdata,1);
              break;
            default:
              if (qual & IEQUALIFIER_NUMERICPAD) switch (code)
               {
                case '1': // End
            view_gobottom(vdata);
            break;
                case '7': // Home
            view_gotop(vdata);
            break;
                case '9': // PageUp
            view_pageup(vdata);
            break;
                case '3': // PageDown
            view_pagedown(vdata);
            break;
                case '8': // CrsrUp
            if (vdata->view_line_count<=vdata->view_lines_per_screen) break;
/*            view_readkeys(view_req,key_matrix);
            if (!(qual&IEQUALIFIER_REPEAT) ||
              key_matrix[9]&16 || key_matrix[7]&64)*/ {                             // 9,4 = 4C = UP_ARROW; 7,6 = 3E = NUM-8
              if (qual&(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)) view_pageup(vdata);
              else if (qual&IEQUALIFIER_CONTROL) view_gotop(vdata);
              else view_lineup(vdata);
            }
            break;
                case '2': // CrsrDown
            if (vdata->view_line_count<=vdata->view_lines_per_screen) break;
/*            view_readkeys(view_req,key_matrix);
            if (!(qual&IEQUALIFIER_REPEAT) ||
              key_matrix[9]&32 || key_matrix[3]&64)*/ {                             // 9,5 = 4D = DOWN_ARROW; 3,6 = 1E = NUM-2
              if (qual&(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)) view_pagedown(vdata);
              else if (qual&IEQUALIFIER_CONTROL) view_gobottom(vdata);
              else view_linedown(vdata);
            }
            break;
               }
              break;
               }
              break;
        case IDCMP_RAWKEY:
          if (vdata->view_scroll && code!=0x40) {
            view_togglescroll(vdata);
            break;
          }
D(bug("RAWKEY: %lx,%lx\n",code,qual));
          switch (code) {
/* Code deleted by Pavel Cizek, 28. 3. 2000:
 *   It was deactivated so that space can be used for Page Down command
            case 0xc0:
              view_togglescroll(vdata);
                   break;
 * End of deleted code
 */
            case CURSOR_UP:
            case NM_WHEEL_UP:
            case NM_WHEEL_LEFT:
              if (vdata->view_line_count<=vdata->view_lines_per_screen) break;
/*              view_readkeys(view_req,key_matrix);
              if (!(qual&IEQUALIFIER_REPEAT/ ||
                key_matrix[9]&16 || key_matrix[7]&64)*/ {                             // 9,4 = 4C = UP_ARROW; 7,6 = 3E = NUM-8
                if (qual&(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)) view_pageup(vdata);
                else if (qual&IEQUALIFIER_CONTROL) view_gotop(vdata);
                else view_lineup(vdata);
              }
              break;
            case CURSOR_DOWN:
            case NM_WHEEL_DOWN:
            case NM_WHEEL_RIGHT:
              if (vdata->view_line_count<=vdata->view_lines_per_screen) break;
/*              view_readkeys(view_req,key_matrix);
              if (!(qual&IEQUALIFIER_REPEAT) ||
                key_matrix[9]&32 || key_matrix[3]&64)*/ {                             // 9,5 = 4D = DOWN_ARROW; 3,6 = 1E = NUM-2
                if (qual&(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)) view_pagedown(vdata);
                else if (qual&IEQUALIFIER_CONTROL) view_gobottom(vdata);
                else view_linedown(vdata);
              }
              break;
          }
          break;
      }
    }
    if (IntuitionBase->ActiveWindow==vdata->view_window && vdata->view_scroll) {
      a=vdata->view_window->MouseY;
      if (a<scroll_pos_4) {
        if (a<scroll_pos_1) scroll_speed=1;
        else if (a<scroll_pos_2) scroll_speed=vdata->view_font->tf_YSize/4;
        else if (a<scroll_pos_3) scroll_speed=vdata->view_font->tf_YSize/2;
        else if (a<scroll_pos_4-10) scroll_speed=vdata->view_font->tf_YSize;
        else scroll_speed=0;

        if (scroll_speed>0 && vdata->view_line_count>vdata->view_lines_per_screen) {
          --vdata->view_text_offset;
          if (vdata->view_text_offset<0) {
            vdata->view_text_offset=0;
            view_togglescroll(vdata);
          }
          else {
            if (vdata->view_scroll_dir==1) view_display(vdata,scroll_speed,1);
            else view_display(vdata,scroll_speed,0);
          }
          vdata->view_scroll_dir=-1;
        }
      }
      else {
        if (a>scroll_pos_7) scroll_speed=1;
        else if (a>scroll_pos_6) scroll_speed=vdata->view_font->tf_YSize/4;
        else if (a>scroll_pos_5) scroll_speed=vdata->view_font->tf_YSize/2;
        else if (a>scroll_pos_4+10) scroll_speed=vdata->view_font->tf_YSize;
        else scroll_speed=0;
        if (scroll_speed>0 && vdata->view_line_count>vdata->view_lines_per_screen) {
          ++vdata->view_text_offset;
          vdata->view_scroll_dir=1;
          if (vdata->view_text_offset>vdata->view_last_line) {
            vdata->view_text_offset=vdata->view_last_line;
            view_togglescroll(vdata);
          }
          else view_display(vdata,scroll_speed,0);
        }
      }
      continue;
    }
     Wait(1<<vdata->view_window->UserPort->mp_SigBit);
  }

cleanup:
  if (vdata->view_text_buffer)
    FreeMem(vdata->view_text_buffer,vdata->view_buffer_size);
  if (view_console_unit)
    CloseDevice((struct IORequest *)&vdata->view_console_request);
  if (vdata->view_ansiread_window) {
    CloseWindow(vdata->view_ansiread_window);
    vdata->view_ansiread_window=NULL;
  }
  view_clearhilite(vdata,0);
  if (!viewdata) {
    cleanupviewfile(vdata);
    FreeMem(vdata,sizeof(struct ViewData));
  }
  else {
    SetBusyPointer(vdata->view_window);
    if ( !( config->viewbits & VIEWBITS_INWINDOW ) ) vdata->view_window->WLayer->Flags&=~LAYERBACKDROP; //HUX
  }

view_end:
  if (view_req) {
    CloseDevice((struct IORequest *)view_req);
    LDeleteExtIO((struct IORequest *)view_req);
  }
  if (view_port) LDeletePort(view_port);
  my_startup_message->command=retcode;
  if (view_msg->deleteonexit) DeleteFile(filename);
  Forbid();
  ReplyMsg((struct Message *)my_startup_message);
  return;
}

void cleanupviewfile(vdata)
struct ViewData *vdata;
{
  if ( config->viewbits & VIEWBITS_INWINDOW ) //HUX
   {
    if (vdata->view_window)
     {
      config->viewtext_topleftx = vdata->view_window->LeftEdge;
      config->viewtext_toplefty  = vdata->view_window->TopEdge;
      CloseWindow(vdata->view_window);
     }
   }
  else if (vdata->view_screen) {
    ScreenToBack(vdata->view_screen);
    if (vdata->view_window) CloseWindow(vdata->view_window);
    CloseScreen(vdata->view_screen);
    if (vdata->view_font) CloseFont(vdata->view_font);
  }
  LFreeRemember(&vdata->view_memory);
}

void view_display(vdata,scroll_speed,w)
struct ViewData *vdata;
int scroll_speed,w;
{
  int a,b,c,d;

  view_clearhilite(vdata,1);
  view_clearsearch(vdata);

  d=vdata->view_font->tf_YSize/scroll_speed;

  if (vdata->view_text_offset<vdata->view_old_offset) {
    --vdata->view_old_offset;
    a=0;
    while (a<vdata->view_font->tf_YSize) {
      if (a+d>vdata->view_font->tf_YSize) d=vdata->view_font->tf_YSize-a;
      view_doscroll(vdata,-d,vdata->view_scroll_width);
      a+=d;
    }
    if (vdata->view_display_as_hex) {
      view_print(vdata,&vdata->view_text_buffer[vdata->view_text_offset*16],0,16);
      view_update_status(vdata);
      return;
    }
    else {
      for (a=vdata->view_top_buffer_pos-2,b=0;a>=0;a--,b++) {
        if (vdata->view_text_buffer[a]==10 || b==vdata->view_max_line_length)
          break;
      }
      c=vdata->view_top_buffer_pos-a-1;
      vdata->view_top_buffer_pos=a+1;
      for (a=vdata->view_bottom_buffer_pos-2,b=0;a>=0;a--,b++) {
        if (vdata->view_text_buffer[a]==10 || b==vdata->view_max_line_length)
          break;
      }
      vdata->view_bottom_buffer_pos=a+1;
    }
    if (vdata->view_top_buffer_pos+c>vdata->view_file_size)
      c=vdata->view_file_size-vdata->view_top_buffer_pos;
    view_print(vdata,&vdata->view_text_buffer[vdata->view_top_buffer_pos],0,c);
  }
  else if (vdata->view_text_offset>vdata->view_old_offset) {
    ++vdata->view_old_offset;
    a=0;
    while (a<vdata->view_font->tf_YSize) {
      if (a+d>vdata->view_font->tf_YSize) d=vdata->view_font->tf_YSize-a;
      view_doscroll(vdata,d,vdata->view_scroll_width);
      a+=d;
    }
    if (!w && !vdata->view_display_as_hex) {
      for (a=vdata->view_top_buffer_pos,b=0;a<vdata->view_file_size;a++,b++) {
        if (vdata->view_text_buffer[a]==10 || b==vdata->view_max_line_length)
          break;
      }
      vdata->view_top_buffer_pos=a+1;
    }
    if (!vdata->view_display_as_hex) {
      for (a=vdata->view_bottom_buffer_pos,b=0;a<vdata->view_file_size;a++,b++) {
        if (vdata->view_text_buffer[a]==10 || b==vdata->view_max_line_length)
          break;
      }
      vdata->view_bottom_buffer_pos=a+1;
    }
    if (vdata->view_bottom_buffer_pos<vdata->view_file_size) {
      if (vdata->view_display_as_hex) {
        view_print(vdata,
          &vdata->view_text_buffer[(vdata->view_text_offset+vdata->view_lines_per_screen-1)*16],
          vdata->view_lines_per_screen-1,16);
      }
      else {
        for (a=vdata->view_bottom_buffer_pos,b=0;a<vdata->view_file_size;a++,b++) {
          if (vdata->view_text_buffer[a]==10 || b==vdata->view_max_line_length)
          break;
        }
        c=a-vdata->view_bottom_buffer_pos+1;
        if (vdata->view_bottom_buffer_pos+c>vdata->view_file_size)
          c=vdata->view_file_size-vdata->view_bottom_buffer_pos;
        view_print(vdata,
          &vdata->view_text_buffer[vdata->view_bottom_buffer_pos],
          vdata->view_lines_per_screen-1,c);
      }
    }
  }
  view_update_status(vdata);
}

void view_displayall(vdata)
struct ViewData *vdata;
{
  int a,c,d,f;

  SetAPen(vdata->view_rastport,vdata->view_colour_table[PEN_TEXTBACKGROUND]);
  RectFill(vdata->view_rastport,
    vdata->view_display_left,
    vdata->view_display_top,
    vdata->view_display_right,
    vdata->view_display_bottom);
  SetAPen(vdata->view_rastport,vdata->view_colour_table[PEN_TEXT]);

  view_clearhilite(vdata,0);
  view_clearsearch(vdata);

  f=d=vdata->view_bottom_buffer_pos=vdata->view_top_buffer_pos;

  if (vdata->view_display_as_hex) {
    d=vdata->view_text_offset*16;
    for (a=0;a<vdata->view_lines_per_screen;a++) {
      if (d<vdata->view_file_size)
        view_print(vdata,&vdata->view_text_buffer[d],a,16);
      d+=16;
    }
  }
  else {
#ifdef DEBUG
long long time1,time2;
//D(bug("tabsize: %ld\n",config->tabsize));
kinittimer();
kgettime(&time1);
#endif
    for (a=0;a<vdata->view_lines_per_screen;a++) {
      for (c=0;c<vdata->view_max_line_length;c++) {
        if (vdata->view_text_buffer[f++]==10) break;
        if (f > vdata->view_file_size) break;
//        ++f;
      }
//      ++f;
      if ((d+c+1)>vdata->view_file_size)
        c=vdata->view_file_size-d-1;
      if (d<vdata->view_file_size)
        view_print(vdata,&vdata->view_text_buffer[d],a,c+1);
      d=f;
      if (a<(vdata->view_lines_per_screen-1) )
        vdata->view_bottom_buffer_pos=d;
    }
#ifdef DEBUG
kgettime(&time2);
D(bug("Time: %ld ticks\n",(ULONG)(time2-time1)));
kremovetimer();
#endif
  }

  vdata->view_old_offset=vdata->view_text_offset;
  view_update_status(vdata);
}

void view_print(vdata,str,y,len)
struct ViewData *vdata;
char *str;
int y,len;
{
  unsigned char textbuf[300];

  if (vdata->view_display_as_ansi) {
    if (len>0) {
      if (str[len-1]=='\n') --len;
      vdata->view_console_request.io_Data=(APTR)"\x9b;0m\f";
      vdata->view_console_request.io_Length=5;
      vdata->view_console_request.io_Command=CMD_WRITE;
      DoIO((struct IORequest *)&vdata->view_console_request);

      vdata->view_console_request.io_Data=(APTR)str;
      vdata->view_console_request.io_Length=len;
      vdata->view_console_request.io_Command=CMD_WRITE;
      DoIO((struct IORequest *)&vdata->view_console_request);

      if (y>-1) {
        ClipBlit(vdata->view_ansiread_window->RPort,
          vdata->view_ansiread_window->BorderLeft, vdata->view_ansiread_window->BorderTop, // HUX
          vdata->view_rastport,
          vdata->view_display_left,
          vdata->view_display_top+(y*vdata->view_font->tf_YSize),
          vdata->view_ansiread_window->Width - vdata->view_ansiread_window->BorderLeft - vdata->view_ansiread_window->BorderRight, // HUX
          vdata->view_font->tf_YSize,0xc0);
      }
    }
  }
  else {
    Move(vdata->view_rastport,
      vdata->view_display_left,
      vdata->view_display_top+(y*vdata->view_font->tf_YSize)+vdata->view_font->tf_Baseline);

    if (vdata->view_display_as_hex) {
      view_makeuphex(vdata,str,textbuf,vdata->view_text_offset+y);
      len=vdata->view_char_width;
    }
    else {
      register unsigned short int i = 0, j, k;

      if (len>vdata->view_char_width) len=vdata->view_char_width;
      for(; i < len; i++, str++)
       {
        if (*str == 0x09)
         {
          k = i % (UWORD)config->tabsize;
          k = config->tabsize - k;
          for(j = 0; j < k; j++, i++) textbuf[i] = ' ';
          len += k - 1;
          i--;
         }
        else textbuf[i] = *str;
       }
      if (len>vdata->view_char_width) len=vdata->view_char_width;
//      CopyMem(str,(char *)textbuf,len);
      if (textbuf[len-1]==10) textbuf[len-1]=' ';
    }
    if (len>0) Text(vdata->view_rastport,(char *)textbuf,len);
  }
}

void view_update_status(vdata)
struct ViewData *vdata;
{
  char textbuf[80];
  int a;

  view_pensrp(vdata);
  Move(vdata->view_rastport,
    vdata->view_window->Width-((config->viewbits & VIEWBITS_INWINDOW)?vdata->view_window->BorderRight:0)-((vdata->view_font->tf_XSize*49)+14), //HUX
    vdata->view_status_bar_ypos+vdata->view_font->tf_Baseline);

  if (vdata->view_line_count<=vdata->view_lines_per_screen)
    a=100;
  else a=(vdata->view_text_offset*100)/vdata->view_last_line;

  lsprintf(textbuf,
    "%-6ld > %-6ld / %-6ld   %-3ld%%",vdata->view_text_offset,
    (vdata->view_text_offset+vdata->view_lines_per_screen>vdata->view_line_count)?
      vdata->view_line_count:vdata->view_text_offset+vdata->view_lines_per_screen,
    vdata->view_line_count,a);

  Text(vdata->view_rastport,textbuf,31);

  if (config->viewbits&VIEWBITS_TEXTBORDERS && vdata->view_scroll_bar)
    FixSliderPot(vdata->view_window,
      &vdata->view_gadgets[VIEW_SCROLLGADGET],
      vdata->view_text_offset,
      vdata->view_line_count,
      vdata->view_lines_per_screen,1);

  view_penrp(vdata);
}

void view_pensrp(vdata)
struct ViewData *vdata;
{
  SetAPen(vdata->view_rastport,vdata->view_colour_table[VIEWPEN_STATUSTEXT]);
  SetBPen(vdata->view_rastport,vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND]);
  SetDrMd(vdata->view_rastport,JAM2);
}

void view_penrp(vdata)
struct ViewData *vdata;
{
  SetAPen(vdata->view_rastport,vdata->view_colour_table[PEN_TEXT]);
  SetBPen(vdata->view_rastport,vdata->view_colour_table[PEN_TEXTBACKGROUND]);
  SetDrMd(vdata->view_rastport,JAM1);
}

void view_pageup(vdata)
struct ViewData *vdata;
{
  int a,b,c;

  if (vdata->view_text_offset==0) return;
  vdata->view_text_offset-=vdata->view_lines_per_screen;
  if (vdata->view_text_offset<0) {
    vdata->view_text_offset=0;
    vdata->view_top_buffer_pos=0;
  }
  else if (!vdata->view_display_as_hex) {
    for (c=0;c<vdata->view_lines_per_screen;c++) {
      for (a=vdata->view_top_buffer_pos-2,b=0;a>=0;a--,b++) {
        if (vdata->view_text_buffer[a]==10 || b==vdata->view_max_line_length)
          break;
      }
      vdata->view_top_buffer_pos=a+1;
    }
  }
  view_displayall(vdata);
}

void view_pagedown(vdata)
struct ViewData *vdata;
{
  int a,b,c;

  if (vdata->view_text_offset==vdata->view_last_line ||
    vdata->view_line_count<=vdata->view_lines_per_screen) return;

  vdata->view_text_offset+=vdata->view_lines_per_screen;

  if (vdata->view_text_offset>vdata->view_last_line) {
    vdata->view_text_offset=vdata->view_last_line;

    if (!vdata->view_display_as_hex) {
      vdata->view_top_buffer_pos=vdata->view_file_size;
      for (c=0;c<vdata->view_lines_per_screen;c++) {
        for (a=vdata->view_top_buffer_pos-2,b=0;a>=0;a--,b++) {
          if (vdata->view_text_buffer[a]==10 || b==vdata->view_max_line_length)
            break;
        }
        vdata->view_top_buffer_pos=a+1;
      }
    }
  }
  else {
    if (!vdata->view_display_as_hex) {
      vdata->view_top_buffer_pos=vdata->view_bottom_buffer_pos;
      for (a=vdata->view_top_buffer_pos,b=0;a<vdata->view_file_size;a++,b++) {
        if (vdata->view_text_buffer[a]==10 || b==vdata->view_max_line_length) // Hit!
          break;
      }
      vdata->view_top_buffer_pos=a+1;
    }
  }
  view_displayall(vdata);
}

void view_gotop(vdata)
struct ViewData *vdata;
{
  if (vdata->view_text_offset) {
    vdata->view_text_offset=0;
    vdata->view_top_buffer_pos=0;
    view_displayall(vdata);
  }
}

void view_gobottom(vdata)
struct ViewData *vdata;
{
  int a,b,c;

  if (vdata->view_text_offset!=vdata->view_last_line &&
    vdata->view_line_count>vdata->view_lines_per_screen) {

    vdata->view_text_offset=vdata->view_last_line;

    if (!vdata->view_display_as_hex) {
      vdata->view_top_buffer_pos=vdata->view_file_size;

      for (c=0;c<vdata->view_lines_per_screen;c++) {
        for (a=vdata->view_top_buffer_pos-2,b=0;a>=0;a--,b++) {
          if (vdata->view_text_buffer[a]==10 || b==vdata->view_max_line_length)
            break;
        }
        vdata->view_top_buffer_pos=a+1;
      }
    }
    view_displayall(vdata);
  }
}

void view_search(vdata,ask)
struct ViewData *vdata;
int ask;
{
  int a,off,bpos,c,d,e,f,mlen;
  unsigned char temp[128];

  status_haveaborted=0;
  view_clearhilite(vdata,1);

  if (!vdata->view_search_string[0]) ask=1;
  strcpy(temp,vdata->view_search_string);
  view_busy(vdata);

  if (ask) {
    if (!(get_search_data(vdata->view_search_string,&vdata->view_search_flags,
      vdata->view_window,vdata->view_font))) {
      view_unbusy(vdata);
      return;
    }
  }

  if (!vdata->view_search_string[0]) {
    view_unbusy(vdata);
    return;
  }

  if (LStrCmpI(temp,vdata->view_search_string)) view_clearsearch(vdata);

  if (vdata->view_display_as_hex) {
    if (vdata->view_search_offset!=-1) {
      a=(vdata->view_search_offset*16)+vdata->view_search_charoffset;
      off=vdata->view_search_offset;
    }
    else {
      a=vdata->view_text_offset*16;
      off=vdata->view_text_offset;
    }
  }
  else {
    if (vdata->view_search_charoffset!=-1) {
      a=vdata->view_search_charoffset;
      off=vdata->view_search_offset;
    }
    else {
      a=vdata->view_top_buffer_pos;
      off=vdata->view_text_offset;
    }
  }
dosearch:
  if ((typesearch(0,vdata->view_search_string,
    vdata->view_search_flags,&vdata->view_text_buffer[a],
    vdata->view_file_size-a))>-1) {

    if (vdata->view_display_as_hex) {
      bpos=search_found_position-vdata->view_text_buffer;
      off=bpos/16; c=bpos%16;
      mlen=search_found_size;
      if (vdata->view_pick_charoffset!=bpos) {
        if (!(vdata->view_text_offset==vdata->view_last_line ||
          vdata->view_line_count<vdata->view_lines_per_screen ||
          vdata->view_text_offset>vdata->view_last_line ||
          off<vdata->view_text_offset+vdata->view_lines_per_screen)) {

          vdata->view_text_offset=
            vdata->view_search_offset=off;

          if (vdata->view_text_offset>vdata->view_last_line)
            view_gobottom(vdata);
          else view_displayall(vdata);
        }
        else if (vdata->view_text_offset>vdata->view_last_line) {
          view_gobottom(vdata);
        }

        d=(10+(c*2)+(c/4))*vdata->view_font->tf_XSize;
        if ((f=c+mlen)>16) f=16;
        e=(10+(f*2)+(f/4))*vdata->view_font->tf_XSize;
        if ((f%4)==0) e-=vdata->view_font->tf_XSize;

        for (a=0;a<2;a++) {
          view_viewhilite(vdata,
            d,(off-vdata->view_text_offset)*vdata->view_font->tf_YSize,e-1,
            (off-vdata->view_text_offset)*vdata->view_font->tf_YSize+(vdata->view_font->tf_YSize-1));
          d=(46+c)*vdata->view_font->tf_XSize;
          e=(46+f)*vdata->view_font->tf_XSize;
        }
        vdata->view_pick_offset=-1;
        vdata->view_pick_charoffset=bpos;
        vdata->view_search_offset=off;
        vdata->view_search_charoffset=c+1;
      }
      else {
        a=bpos+1;
        goto dosearch;
      }
    }
    else {
      off+=search_found_lines;
      bpos=search_last_line_pos+a;
      if (vdata->view_pick_offset!=bpos && vdata->view_pick_charoffset!=off) {
        if (vdata->view_text_offset==vdata->view_last_line ||
          vdata->view_line_count<vdata->view_lines_per_screen ||
          off<vdata->view_text_offset+vdata->view_lines_per_screen ||
          off>vdata->view_last_line) {

          if (off>vdata->view_last_line) view_gobottom(vdata);

          vdata->view_search_charoffset=bpos;
          vdata->view_search_offset=off;

          view_viewhilite(vdata,
            0,(off-vdata->view_text_offset)*vdata->view_font->tf_YSize,
            vdata->view_char_width*vdata->view_font->tf_XSize,
            (off-vdata->view_text_offset)*vdata->view_font->tf_YSize+(vdata->view_font->tf_YSize-1));
        }
        else {
          vdata->view_top_buffer_pos=bpos;
          vdata->view_text_offset=off;
          view_displayall(vdata);
          view_viewhilite(vdata,
            0,0,
            vdata->view_char_width*vdata->view_font->tf_XSize,
            vdata->view_font->tf_YSize-1);
        }
        vdata->view_pick_offset=bpos;
        vdata->view_pick_charoffset=off;
      }
      else {
        for (a=bpos;a<vdata->view_file_size;a++) {
          if (vdata->view_text_buffer[a]==10)
            break;
        }
        ++a; ++off;
        goto dosearch;
      }
    }
  }
  else {
    view_clearsearch(vdata);
    view_simplerequest(vdata,globstring[STR_STRING_NOT_FOUND],globstring[STR_CONTINUE],NULL);
  }
  view_unbusy(vdata);
  return;
}

void view_busy(vdata)
struct ViewData *vdata;
{
  if (!vdata->view_window->Pointer) SetBusyPointer(vdata->view_window);
  if (!vdata->view_window->FirstRequest) {
    InitRequester(&vdata->view_busy_request);
    Request(&vdata->view_busy_request,vdata->view_window);
  }
}

void view_unbusy(vdata)
struct ViewData *vdata;
{
  ClearPointer(vdata->view_window);
  if (vdata->view_window->FirstRequest)
    EndRequest(&vdata->view_busy_request,vdata->view_window);
}

void view_doscroll(vdata,dy,w)
struct ViewData *vdata;
int dy,w;
{
  if (!vdata->view_display_as_ansi)
    SetWrMsk(vdata->view_rastport,vdata->view_colour_table[PEN_TEXT]);
  ScrollRaster(vdata->view_rastport,0,dy,
    vdata->view_display_left,
    vdata->view_display_top,
    vdata->view_display_left+w,
    vdata->view_display_top+vdata->view_display_height-1);
  if (!vdata->view_display_as_ansi)
    SetWrMsk(vdata->view_rastport,0xff);
}

int view_lineup(vdata)
struct ViewData *vdata;
{
  if (vdata->view_text_offset==0) return(0);
  --vdata->view_text_offset;
  if (vdata->view_scroll_dir==1)
    view_display(vdata,1,1);
  else view_display(vdata,1,0);
  vdata->view_scroll_dir=-1;
  return(1);
}

int view_linedown(vdata)
struct ViewData *vdata;
{
  if (vdata->view_text_offset>=vdata->view_last_line) return(0);
  ++vdata->view_text_offset;
  vdata->view_scroll_dir=1;
  view_display(vdata,1,0);
  return(1);
}

void view_status_text(vdata,str)
struct ViewData *vdata;
char *str;
{
  SetAPen(vdata->view_rastport,vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND]);
  RectFill(vdata->view_rastport,
    vdata->view_display_left,
    vdata->view_status_bar_ypos,
    vdata->view_window->Width-((config->viewbits & VIEWBITS_INWINDOW)?vdata->view_window->BorderRight:0)-((vdata->view_font->tf_XSize*18)+3), //HUX
    vdata->view_window->Height-2-((config->viewbits & VIEWBITS_INWINDOW) ? vdata->view_window->BorderBottom : 0)); //HUX

  view_pensrp(vdata);

  if (str) {
    Move(vdata->view_rastport,vdata->view_display_left,vdata->view_status_bar_ypos+vdata->view_font->tf_Baseline);
    Text(vdata->view_rastport,str,strlen(str));
  }
}

void view_printtext(vdata,state)
struct ViewData *vdata;
int state;
{
  int out,a,tb;
  char temp[60],*str;
  unsigned char buf1[80];

  status_haveaborted=0;

  if (state==0 && !vdata->view_display_as_hex) {
    struct DOpusArgsList print_args;

    print_args.single_file=vdata->view_path_name;
    print_args.file_window=-1;
    print_args.file_list=NULL;
    print_args.last_select=NULL;

    dopus_print(0,&print_args,0,vdata->view_port_name,vdata);
    return;
  }

  if (!(out=Open("PRT:",MODE_NEWFILE))) {
    view_simplerequest(vdata,globstring[STR_CANT_OPEN_PRINTER],globstring[STR_CONTINUE],NULL);
    return;
  }
  if (!vdata->view_display_as_hex) {
    char *print_ptr;
    int print_size;

    if (vdata->view_bottom_buffer_pos>=vdata->view_file_size) {
      print_size=vdata->view_file_size;
      print_ptr=vdata->view_text_buffer;
    }
    else {
      for (a=vdata->view_bottom_buffer_pos;a<vdata->view_file_size;a++) {
        if (vdata->view_text_buffer[a]==10) break;
      }
      print_size=a-vdata->view_top_buffer_pos+1;
      print_ptr=&vdata->view_text_buffer[vdata->view_top_buffer_pos];
    }

    if ((Write(out,print_ptr,print_size))<print_size) {
      Close(out);
      lsprintf(temp,globstring[STR_ERROR_PRINTING_FILE],IoErr());
      view_simplerequest(vdata,temp,globstring[STR_CONTINUE],NULL);
      return;
    }
  }
  else {
    if (state==0) {
      str=vdata->view_text_buffer;
      for (a=0;a<vdata->view_line_count;a++) {
        view_makeuphex(vdata,str,buf1,a);
        if ((Write(out,(char *)buf1,63))<63) {
          Close(out);
          lsprintf(temp,globstring[STR_ERROR_PRINTING_FILE],IoErr());
          view_simplerequest(vdata,temp,globstring[STR_CONTINUE],NULL);
          return;
        }
        str+=16;
        if (status_haveaborted) break;
      }
    }
    else {
      str=&vdata->view_text_buffer[vdata->view_text_offset*16];
      tb=vdata->view_text_offset*16;
      for (a=0;a<vdata->view_lines_per_screen;a++) {
        view_makeuphex(vdata,str,buf1,vdata->view_text_offset+a);
        if ((Write(out,(char *)buf1,63))<63) {
          Close(out);
          lsprintf(temp,globstring[STR_ERROR_PRINTING_FILE],IoErr());
          view_simplerequest(vdata,temp,globstring[STR_CONTINUE],NULL);
          return;
        }
        str+=16; tb+=16;
        if (tb>vdata->view_file_size || status_haveaborted) break;
      }
    }
  }
  Close(out);
  return;
}

void view_checkprint(vdata,code)
struct ViewData *vdata;
int code;
{
  view_busy(vdata);
  if ((code==0 && !vdata->view_display_as_hex) ||
    (view_simplerequest(vdata,globstring[STR_READY_PRINTER],
      str_okaystring,str_cancelstring,NULL))) view_printtext(vdata,code);
  view_unbusy(vdata);
}

void view_makeuphex(vdata,hex,textbuf,line)
struct ViewData *vdata;
char *hex;
unsigned char *textbuf;
int line;
{
  unsigned char buf2[20];
//  unsigned int buf3[20];
  int at,b,c,e;

  if (vdata->view_last_char && hex>=vdata->view_last_char)
    b=vdata->view_last_charpos;
  else b=16;
  for (at=0;at<b;at++) {
    if (!(_isprint(hex[at]))) buf2[at]='.';
    else buf2[at]=hex[at];
//    buf3[at]=(unsigned int)((unsigned char)hex[at]);
  }
  if (vdata->view_last_char && hex>=vdata->view_last_char) {
    for (at=b;at<16;at++) {
      buf2[at]=' ';
//      buf3[at]=0;
    }
    e=b/4; c=10+(b*2)+e;
  }
  else c=-1;
  buf2[16]=0;
/*  lsprintf((char *)textbuf,
    "%08lx: %02lx%02lx%02lx%02lx %02lx%02lx%02lx%02lx %02lx%02lx%02lx%02lx \
%02lx%02lx%02lx%02lx %s\n",line*16,
    buf3[0],buf3[1],buf3[2],buf3[3],buf3[4],buf3[5],buf3[6],buf3[7],
    buf3[8],buf3[9],buf3[10],buf3[11],buf3[12],buf3[13],buf3[14],buf3[15],
    buf2);*/
  lsprintf((char *)textbuf,
    "%08lx: %08lx %08lx %08lx %08lx %s\n",line*16,
    ((long *)hex)[0],((long *)hex)[1],((long *)hex)[2],((long *)hex)[3],buf2);
  if (c>-1) {
    for (b=c;b<46;b++)
      textbuf[b]=' ';
  }
}

void view_togglescroll(vdata)
struct ViewData *vdata;
{
  if (vdata->view_scroll) {
    vdata->view_scroll=0;
    ClearPointer(vdata->view_window);
  }
  else if (vdata->view_line_count>vdata->view_lines_per_screen) {
    vdata->view_scroll=1;
    setnullpointer(vdata->view_window);
  }
}

void view_setupscreen(vdata)
struct ViewData *vdata;
{
  int a,width;

  width = vdata->view_window->Width-((config->viewbits & VIEWBITS_INWINDOW)?vdata->view_window->BorderRight:0); //HUX
  SetAPen(vdata->view_rastport,vdata->view_colour_table[PEN_TEXTBACKGROUND]);
  RectFill(vdata->view_rastport,
    vdata->view_display_left,
    vdata->view_display_top,
    vdata->view_display_right,
    vdata->view_display_bottom);

  Do3DBox(vdata->view_rastport,
    vdata->view_display_left,
    vdata->view_status_bar_ypos,
    width-((vdata->view_font->tf_XSize*18)+4),
    vdata->view_font->tf_YSize,
    vdata->view_colour_table[PEN_SHINE],
    vdata->view_colour_table[PEN_SHADOW]);

  view_status_text(vdata,NULL);
  SetDrMd(vdata->view_rastport,JAM2);

  for (a=0;a<9;a++) {
    vdata->view_gadgets[a].NextGadget=&vdata->view_gadgets[a+1];
    vdata->view_gadgets[a].LeftEdge=(width-(vdata->view_font->tf_XSize*18))+(a*vdata->view_font->tf_XSize*2);
    vdata->view_gadgets[a].TopEdge=vdata->view_status_bar_ypos-1;
    vdata->view_gadgets[a].Width=vdata->view_font->tf_XSize*2;
    vdata->view_gadgets[a].Height=vdata->view_font->tf_YSize+2;
    vdata->view_gadgets[a].Flags=GFLG_GADGHCOMP;
    vdata->view_gadgets[a].Activation=(a<2)?GACT_IMMEDIATE|GACT_RELVERIFY:GACT_RELVERIFY;
    vdata->view_gadgets[a].GadgetType=GTYP_BOOLGADGET;
    vdata->view_gadgets[a].GadgetID=a;

    Do3DBox(vdata->view_rastport,
      vdata->view_gadgets[a].LeftEdge+2,
      vdata->view_status_bar_ypos,
      (vdata->view_font->tf_XSize*2)-4,vdata->view_font->tf_YSize,
      vdata->view_colour_table[PEN_SHINE],
      vdata->view_colour_table[PEN_SHADOW]);

    SetAPen(vdata->view_rastport,vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND]);
    RectFill(vdata->view_rastport,
      vdata->view_gadgets[a].LeftEdge+2,
      vdata->view_status_bar_ypos,
      vdata->view_gadgets[a].LeftEdge+vdata->view_gadgets[a].Width-3,
      vdata->view_status_bar_ypos+vdata->view_font->tf_YSize-1);
    SetAPen(vdata->view_rastport,vdata->view_colour_table[VIEWPEN_STATUSTEXT]);
    SetBPen(vdata->view_rastport,vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND]);

    if (a>1) {
      Move(vdata->view_rastport,
        vdata->view_gadgets[a].LeftEdge+
          ((vdata->view_gadgets[a].Width-vdata->view_font->tf_XSize)/2),
        vdata->view_status_bar_ypos+vdata->view_font->tf_Baseline);
      Text(vdata->view_rastport,&globstring[STR_VIEW_BUTTONS][a-2],1);
    }
  }
  for (a=0;a<2;a++) {
    DoArrow(vdata->view_rastport,
      vdata->view_gadgets[a].LeftEdge+2,
      vdata->view_status_bar_ypos+1,
      (vdata->view_font->tf_XSize*2)-4,vdata->view_font->tf_YSize-2,
      vdata->view_colour_table[VIEWPEN_STATUSTEXT],
      vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND],a);
  }
  SetFont(vdata->view_rastport,vdata->view_font);
}

void view_viewhilite(vdata,x,y,x1,y1)
struct ViewData *vdata;
int x,y,x1,y1;
{
  struct viewhilite *hi;

  if (!(hi=AllocMem(sizeof(struct viewhilite),MEMF_CLEAR))) return;
D(bug("view hilite: %lX (%ld bytes)\n",hi,sizeof(struct viewhilite)));
  if (vdata->view_current_hilite) vdata->view_current_hilite->next=hi;
  else vdata->view_first_hilite=hi;
  vdata->view_current_hilite=hi;
  hi->x=x+vdata->view_display_left;
  hi->y=y+vdata->view_display_top;
  hi->x1=x1+vdata->view_display_left;
  hi->y1=y1+vdata->view_display_top;
  SetDrMd(vdata->view_rastport,COMPLEMENT);
  RectFill(vdata->view_rastport,hi->x,hi->y,hi->x1,hi->y1);
  SetDrMd(vdata->view_rastport,JAM1);
}

void view_clearhilite(vdata,show)
struct ViewData *vdata;
int show;
{
  struct viewhilite *hi,*next;

  hi=vdata->view_first_hilite;
  if (show) SetDrMd(vdata->view_rastport,COMPLEMENT);
  while (hi) {
    next=hi->next;
    if (show) RectFill(vdata->view_rastport,hi->x,hi->y,hi->x1,hi->y1);
    FreeMem(hi,sizeof(struct viewhilite));
    hi=next;
  }
  vdata->view_first_hilite=vdata->view_current_hilite=NULL;
  if (show) SetDrMd(vdata->view_rastport,JAM1);
}

void view_fix_scroll_gadget(vdata)
struct ViewData *vdata;
{
  int a,b,c,d;

  if ((a=GetSliderPos(&vdata->view_gadgets[VIEW_SCROLLGADGET],
    vdata->view_line_count,
    vdata->view_lines_per_screen))==vdata->view_text_offset) return;

  d=vdata->view_text_offset-a;
  vdata->view_scroll_bar=0;

  if (d>0 && d<vdata->view_lines_per_screen/2) {
    while (d--) view_lineup(vdata);
  }
  else if (d<0 && d>-(vdata->view_lines_per_screen/2)) {
    while (d++) view_linedown(vdata);
  }
  else if (vdata->view_display_as_hex) {
    vdata->view_text_offset=a;
    if (vdata->view_text_offset>vdata->view_last_line)
      vdata->view_text_offset=vdata->view_last_line;
    view_displayall(vdata);
  }
  else {
    SetBusyPointer(vdata->view_window);
    if (a>vdata->view_last_line) a=vdata->view_last_line;
    if (vdata->view_text_offset<a) {
      if (a-vdata->view_text_offset<vdata->view_last_line-a) {
        for (b=vdata->view_text_offset;b<a;b++) {
          for (c=vdata->view_top_buffer_pos;c<vdata->view_file_size;c++) {
            if (vdata->view_text_buffer[c]==10)
              break;
          }
          vdata->view_top_buffer_pos=c+1;
        }
      }
      else {
        vdata->view_text_offset=vdata->view_last_line;
        vdata->view_top_buffer_pos=vdata->view_file_size;
        for (c=0;c<vdata->view_lines_per_screen;c++) {
          for (b=vdata->view_top_buffer_pos-2;b>=0;b--) {
            if (vdata->view_text_buffer[b]==10)
              break;
          }
          vdata->view_top_buffer_pos=b+1;
        }
        for (b=a;b<vdata->view_text_offset;b++) {
          for (c=vdata->view_top_buffer_pos-2;c>=0;c--) {
            if (vdata->view_text_buffer[c]==10)
              break;
          }
          vdata->view_top_buffer_pos=c+1;
        }
      }
    }
    else {
      if (vdata->view_text_offset-a<a) {
        for (b=a;b<vdata->view_text_offset;b++) {
          for (c=vdata->view_top_buffer_pos-2;c>=0;c--) {
            if (vdata->view_text_buffer[c]==10) break;
          }
          vdata->view_top_buffer_pos=c+1;
        }
      }
      else {
        vdata->view_top_buffer_pos=0;
        for (b=0;b<a;b++) {
          for (c=vdata->view_top_buffer_pos;c<vdata->view_file_size;c++) {
            if (vdata->view_text_buffer[c]==10)
              break;
          }
          vdata->view_top_buffer_pos=c+1;
        }
      }
    }
    vdata->view_text_offset=a;
    view_displayall(vdata);
    ClearPointer(vdata->view_window);
  }
  vdata->view_scroll_bar=1;
}

void view_clearsearch(vdata)
struct ViewData *vdata;
{
  vdata->view_search_offset=
    vdata->view_search_charoffset=
    vdata->view_pick_offset=
    vdata->view_pick_charoffset=-1;
}
/*
void view_readkeys(req,keys)
struct IOStdReq *req;
APTR keys;
{
  if (req) {
    req->io_Length=13;
    req->io_Data=(APTR)keys;
    req->io_Command=KBD_READMATRIX;
    DoIO((struct IORequest *)req);
  }
}
*/
int view_simplerequest(struct ViewData *vdata,char *txt,...)
{
  char *gads[4],*cancelgad=NULL,*gad;
  int a/*=1*/,r,rets[3],num;
  va_list ap;
  struct DOpusSimpleRequest request;

  va_start(ap,txt); r=1; num=0;
  for (a=0;a<3;a++) {
    if (!(gad=(char *)va_arg(ap,char *))) break;
    if (a==1) cancelgad=gad;
    else {
      gads[num]=gad;
      rets[num++]=r++;
    }
  }
  if (cancelgad) {
    gads[num]=cancelgad;
    rets[num]=0;
    a=num+1;
  }
  for (;a<4;a++) gads[a]=NULL;

  request.hi=vdata->view_colour_table[PEN_SHINE];
  request.lo=vdata->view_colour_table[PEN_SHADOW];
  request.fg=vdata->view_colour_table[VIEWPEN_STATUSTEXT];
  request.bg=vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND];

  request.strbuf=NULL;

  return(dorequest(&request,txt,gads,rets,vdata->view_window));
}

int view_whatsit(vdata,txt,max,buffer)
struct ViewData *vdata;
char *txt;
int max;
char *buffer;
{
  char *gads[3];
  int rets[2];
  struct DOpusSimpleRequest request;

  gads[0]=str_okaystring; rets[0]=1;
  gads[1]=str_cancelstring; rets[1]=0;
  gads[2]=NULL;

  request.hi=vdata->view_colour_table[PEN_SHINE];
  request.lo=vdata->view_colour_table[PEN_SHADOW];
  request.fg=vdata->view_colour_table[VIEWPEN_STATUSTEXT];
  request.bg=vdata->view_colour_table[VIEWPEN_STATUSBACKGROUND];

  request.strbuf=buffer;
  request.strlen=max;
  request.flags=0;
  return(dorequest(&request,txt,gads,rets,vdata->view_window));
}
