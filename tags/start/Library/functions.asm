; Directory Opus 4
; Original GPL release version 4.12
; Copyright 1993-2000 Jonathan Potter
; 
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
; 
; All users of Directory Opus 4 (including versions distributed
; under the GPL) are entitled to upgrade to the latest version of
; Directory Opus version 5 at a reduced price. Please see
; http://www.gpsoft.com.au for more information.
; 
; The release of Directory Opus 4 under the GPL in NO WAY affects
; the existing commercial status of Directory Opus 5.


    SECTION .text

    INCLUDE "exec/types.i"
    INCLUDE "exec/initializers.i"
    INCLUDE "exec/libraries.i"
    INCLUDE "exec/lists.i"
    INCLUDE "exec/alerts.i"
    INCLUDE "exec/resident.i"
    INCLUDE "exec/ports.i"
    INCLUDE "exec/io.i"
    INCLUDE "libraries/dos.i"
    INCLUDE "graphics/rastport.i"

    INCLUDE "asmsupp.i"
    INCLUDE "dopusbase.i"

    XREF _IntuitionBase
    XREF _GfxBase
    XREF _UtilityBase

    XDEF endcode
    XDEF _GetWBScreen
    XDEF _Seed
    XDEF _Random
    XDEF _StrToUpper
    XDEF _StrToLower
    XDEF _DisableGadget
    XDEF _EnableGadget
    XDEF _GhostGadget
    XDEF _SwapMem
    XDEF _LSprintf
    XDEF LSprintf
;    XDEF _LCreateExtIO
;    XDEF _LCreatePort
;    XDEF _LDeleteExtIO
;    XDEF _LDeletePort
    XDEF _LToUpper
    XDEF _LToLower
    XDEF _LStrCat
    XDEF _LStrnCat
    XDEF _LStrCpy
    XDEF _LStrnCpy
    XDEF _LStrCmpI
    XDEF _LStrnCmpI
    XDEF _LStrCmp
    XDEF _LStrnCmp

    
wbname
    dc.b 'Workbench',0
rand
    ds.l 2
ditherdata
    dc.w $8888,$2222

_GetWBScreen:
    movem.l a2/a3/a5/a6,-(sp)
    move.l _IntuitionBase,a6
;    move.w 20(a6),d0
;    cmp.w #36,d0
;    blt getwb13
    move.l a0,a2
    lea wbname(pc),a0
    jsr _LVOLockPubScreen(a6)
    move.l d0,a3
    move.l d0,a0
    move.l a2,a1
    move.l #346,d0
    move.l 4,a6
    jsr _LVOCopyMem(a6)
    move.l a5,a6
    move.l #0,a0
    move.l a3,a1
    move.l _IntuitionBase,a6
    jsr _LVOUnlockPubScreen(a6)
;    bra endgetscr
;getwb13:
;    move.l #346,d0
;    moveq.l #1,d1
;    move.l #0,a1
;    jsr _LVOGetScreenData(a6)
;endgetscr:
    movem.l (sp)+,a2/a3/a5/a6
    rts

_Seed:
    move.l d0,d1
    not.l d1
    movem.l d0/d1,rand
longrnd:
  movem.l d2-d3,-(sp) 
    movem.l rand,d0/d1
    andi.b #$0e,d0
    ori.b #$20,d0
    move.l d0,d2
    move.l d1,d3
    add.l d2,d2 
    addx.l d3,d3
    add.l d2,d0
    addx.l d3,d1
    swap d3
    swap d2
    move.w d2,d3
    clr.w d2
    add.l d2,d0
    addx.l d3,d1
    movem.l d0/d1,rand
    move.l d1,d0
    movem.l (sp)+,d2-d3
    rts

_Random:
    move.w d2,-(sp)
    move.w d0,d2
    beq.s skip
    bsr.s longrnd
    clr.w d0
    swap d0
    divu.w d2,d0
    clr.w d0
    swap d0
skip:
    move.w (sp)+,d2
    rts

_StrToUpper:
    move.b (a0)+,d0
    bsr _LToUpper
    move.b d0,(a1)+
    bne.s _StrToUpper
    rts

_StrToLower:
    move.b (a0)+,d0
    bsr _LToLower
    move.b d0,(a1)+
    bne.s _StrToLower
    rts

_DisableGadget:
    btst.b #0,12(a0)
    bne disabled
    bset.b #0,12(a0)
    bsr _GhostGadget
disabled:
    rts

_EnableGadget:
    btst.b #0,12(a0)
    beq enabled
    bclr.b #0,12(a0)
    bsr _GhostGadget
enabled:
    rts

_GhostGadget:
    movem.l a2/a3/a6/d2-d5,-(sp)
    move.b 28(a1),-(sp)
    move.l a0,a2
    move.l a1,a3
    move.l d0,d4
    move.l d1,d5

    moveq.l #2,d0   
    move.l db_GfxBase(a6),a6
    jsr _LVOSetDrMd(a6)

    move.l a3,a1    
    lea ditherdata(pc),a0
    move.l a0,8(a1)
    move.b #1,29(a1)

    move.l d4,d0
    add.w 4(a2),d0
    move.l d5,d1
    add.w 6(a2),d1
    moveq.l #0,d2
    move.w 4(a2),d2
    add.w 8(a2),d2
    subq.l #1,d2
    sub.l d4,d2
    moveq.l #0,d3
    move.w 6(a2),d3
    add.w 10(a2),d3
    subq.l #1,d3
    sub.l d5,d3
    jsr _LVORectFill(a6)

    move.l a3,a1    
    move.l #0,8(a1)
    move.b #0,29(a1)

    moveq.l #0,d0
    move.b (sp)+,d0
    jsr _LVOSetDrMd(a6)
    movem.l (sp)+,a2/a3/a6/d2-d5
    rts

_DrawRadioButton:
    movem.l a2-a4/a6/d2-d7,-(sp)

    move.l db_GfxBase(a6),a6
    move.l a0,a2
    move.l d0,d6
    move.l d1,d7

    move.l a2,a1
    move.l d4,d0
    jsr _LVOSetAPen(a6)

    move.l a2,a1
    move.l d6,d0
    move.l d0,a3
    move.l d7,d1
    subq.l #1,d1
    move.l d1,a4
    jsr _LVOMove(a6)

    move.l a2,a1
    move.l d6,d0
    add.l d2,d0
    addq.l #1,d0
    move.l a4,d1
    subq.l #2,d1
    jsr _LVODraw(a6)

    move.l a2,a1
    move.l a3,d0
    move.l a4,d1
    jsr _LVOMove(a6)

    move.l a2,a1
    move.l a3,d0
    move.l d7,d1
    add.l d3,d1
    jsr _LVODraw(a6)

    move.l a2,a1
    move.l d6,d0
    subq.l #1,d0
    move.l d0,a3
    move.l a4,d1
    jsr _LVOMove(a6)

    move.l a2,a1
    move.l a3,d0
    move.l d7,d1
    add.l d3,d1
    move.l d1,a4
    subq.l #1,d1
    jsr _LVODraw(a6)

    move.l a2,a1
    move.l d5,d0
    jsr _LVOSetAPen(a6)

    move.l a2,a1
    move.l a3,d0
    move.l a4,d1
    jsr _LVOMove(a6)

    move.l a2,a1    
    move.l d6,d0
    add.l d2,d0
    move.l d0,a3
    addq.l #1,d0
    move.l a4,d1
    jsr _LVODraw(a6)

    move.l a2,a1
    move.l a3,d0
    addq.l #1,d0
    move.l d7,d1
    subq.l #1,d1
    jsr _LVODraw(a6)

    move.l a2,a1
    move.l a3,d0
    move.l a4,d1
    jsr _LVOMove(a6)

    move.l a2,a1
    move.l a3,d0
    move.l d7,d1
    jsr _LVODraw(a6)

    movem.l (sp)+,a2-a4/a6/d2-d7
    rts

_SwapMem:
    move.b (a0),d1
    move.b (a1),(a0)+
    move.b d1,(a1)+
    subq.l #1,d0
    bne.s _SwapMem
    rts

_LSprintf:
LSprintf:
    movem.l a2/a3/a6,-(sp)
    move.l 4*4(sp),a3
    move.l 5*4(sp),a0
    lea.l 6*4(sp),a1
    lea.l stuffChar(pc),a2
    move.l 4,a6
    jsr _LVORawDoFmt(a6)
    movem.l (sp)+,a2/a3/a6
    rts

stuffChar:
    move.b d0,(a3)+
    rts

;LCreateExtIO:
;    movem.l a6/d2/d3,-(sp)
;    move.l a0,d2
;    moveq.l #0,d3
;    move.w d0,d3
;    beq.s endextio
;    move.l #196608,d1
;    move.l 4,a6
;    jsr _LVOAllocMem(a6)
;    tst.l d0
;    beq.s endextio
;    move.l d0,a0
;    move.b #NT_MESSAGE,LN_TYPE(a0)
;    move.w d3,MN_LENGTH(a0)
;    move.l d2,MN_REPLYPORT(a0)
;    move.l a0,d0
;    movem.l (sp)+,a6/d2/d3
;    rts
;endextio:
;    movem.l (sp)+,a6/d2/d3
;    moveq.l #0,d0
;    rts

;LCreatePort:
;    movem.l a2/a3/a6/d2/d3,-(sp)
;    move.l a0,a2
;    move.l d0,d2
;    move.l #-1,d0
;    move.l 4,a6
;    jsr _LVOAllocSignal(a6)
;    cmp.b #-1,d0
;    beq endport
;    move.b d0,d3
;    move.l #MP_SIZE,d0
;    move.l #196608,d1
;    move.l 4,a6
;    jsr _LVOAllocMem(a6)
;    tst.l d0
;    beq.s endport1
;    move.l d0,a3
;    move.l a2,LN_NAME(a3)
;    move.b d2,LN_PRI(a3)
;    move.b #NT_MSGPORT,LN_TYPE(a3)
;    move.b #PA_SIGNAL,MP_FLAGS(a3)
;    move.b d3,MP_SIGBIT(a3)
;    move.l #0,a1
;    move.l 4,a6
;    jsr _LVOFindTask(a6)
;    move.l d0,MP_SIGTASK(a3)
;    cmp.l #0,a2
;    beq.s noname
;    move.l a3,a1
;    move.l 4,a6
;    jsr _LVOAddPort(a6)
;    bra.s endgotport
;noname:
;    lea MP_MSGLIST(a3),a0
;    move.l a0,LH_HEAD(a0)
;    addq.l #4,LH_HEAD(a0)
;    clr.l LH_TAIL(a0)
;    move.l a0,LH_TAILPRED(a0)
;endgotport:
;    move.l a3,d0
;    movem.l (sp)+,a2/a3/a6/d2/d3
;    rts
;endport1:
;    move.l 4,a6
;    jsr _LVOFreeSignal(a6)
;endport:
;    movem.l (sp)+,a2/a3/a6/d2/d3
;    rts

;LDeleteExtIO:
;    move.l a6,-(sp)
;    move.b #-1,LN_TYPE(a0)
;    move.l #-1,MN_REPLYPORT(a0)
;    move.l #-1,IO_DEVICE(a0)
;    move.l a0,a1
;    moveq.l #0,d0
;    move.w MN_LENGTH(a0),d0
;    move.l 4,a6
;    jsr _LVOFreeMem(a6)
;    move.l (sp)+,a6
;    rts

;LDeletePort:
;    movem.l a2/a6,-(sp)
;    move.l a0,a2
;    move.l LN_NAME(a2),d0
;    tst.l d0
;    beq.s nonamefree
;    move.l a0,a1
;    move.l 4,a6
;    jsr _LVORemPort(a6)
;nonamefree:
;    move.l #-1,MP_SIGTASK(a2)
;    lea MP_MSGLIST(a2),a0
;    move.l #-1,LH_HEAD(a0)
;    moveq.l #0,d0
;    move.b MP_SIGBIT(a2),d0
;    move.l 4,a6
;    jsr _LVOFreeSignal(a6)
;    move.l a2,a1
;    move.l #MP_SIZE,d0
;    move.l 4,a6
;    jsr _LVOFreeMem(a6)
;    movem.l (sp)+,a2/a6
;    rts

_LToUpper:
;   cmp.b #'a'-1,d0
;   bls tu_done
;   cmp.b #'z',d0
;   bhi tu_done
;   sub.b #'a'-'A',d0
;tu_done:
    movem.l a0/a1/d1,-(sp)
    move.l _UtilityBase,a6
    jsr _LVOToUpper(a6)
    movem.l (sp)+,a0/a1/d1
    rts

_LToLower:
;   cmp.b #'A'-1,d0
;   bls tl_done
;   cmp.b #'Z',d0
;   bhi tl_done
;   add.b #'a'-'A',d0
;tl_done:
    movem.l a0/a1/d1,-(sp)
    move.l _UtilityBase,a6
    jsr _LVOToLower(a6)
    movem.l (sp)+,a0/a1/d1
    rts

_LStrCat:
    move.l #$ffff,d0
_LStrnCat:
    tst.b (a0)+
    bne.s   _LStrnCat
    sub.l #1,a0
    subq.l #1,d0
catloop:
    move.b (a1)+,(a0)+
    dbeq d0,catloop
    beq.s endcat
    clr.b (a0)
endcat:
    rts

_LStrCpy:
    move.b (a1)+,(a0)+
    bne.s   _LStrCpy
    rts

_LStrnCpy:
    move.b (a1)+,(a0)+
    beq.s endstrncpy
    dbeq d0,_LStrnCpy
    move.b #0,-(a0)
endstrncpy:
    rts

_LStrCmpI:
    move.l #$7fff,d0
_LStrnCmpI:
    move.l d2,-(sp)
    move.l d0,d2
    subq.l #1,d2
    bmi equal
cmploop:
    move.b (a0)+,d0
    ext.l d0
    bsr _LToUpper
    move.b d0,d1
    move.b (a1)+,d0
    ext.l d0
    bsr _LToUpper
    cmp.b d0,d1
    blt less
    bgt greater
    tst.b d1
    dbeq d2,cmploop
equal:
    moveq.l #0,d0
    move.l (sp)+,d2
    rts
greater:
    moveq.l #1,d0
    move.l (sp)+,d2
    rts
less:
    move.l #-1,d0
    move.l (sp)+,d2
    rts

_LStrCmp:
    move.l #$7fff,d0
_LStrnCmp:
    subq.l #1,d0
    bmi equal1
    moveq.l #0,d1
cmploop1:
    move.b (a0)+,d1
    cmp.b (a1)+,d1
    blt less1
    bgt greater1
    tst.b d1
    dbeq d0,cmploop1
equal1:
    moveq.l #0,d0
    rts
greater1:
    moveq.l #1,d0
    rts
less1:
    move.l #-1,d0
    rts

endcode:
    END
