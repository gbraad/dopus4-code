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

    INCLUDE "asmsupp.i"
    INCLUDE "dopusbase.i"

    XDEF _Do3DBox
    XDEF _Do3DStringBox
    XDEF _Do3DCycleBox
    XDEF _DrawCheckMark
    XDEF _HiliteGad

_Do3DBox:
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
    subq.l #2,d0
    move.l d0,a3
    move.l d7,d1
    subq.l #1,d1
    move.l d1,a4
    jsr _LVOMove(a6)

    move.l a2,a1
    move.l d6,d0
    add.l d2,d0
    move.l a4,d1
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

_Do3DStringBox:
    movem.l a0/d0-d3,-(sp)
    subq.l #2,d0
    subq.l #1,d1
    addq.l #4,d2
    addq.l #2,d3
    bsr _Do3DBox
    movem.l (sp)+,a0/d0-d3

    movem.l a2-a4/a6/d2-d7,-(sp)

    move.l db_GfxBase(a6),a6
    move.l a0,a2
    move.l d0,d6
    move.l d1,d7

    move.l a2,a1
    move.l d5,d0
    jsr _LVOSetAPen(a6)

    move.l a2,a1
    move.l d6,d0
    subq.l #2,d0
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
    jsr _LVODraw(a6)

    move.l a2,a1
    move.l a3,d0
    move.l a4,d1
    jsr _LVOMove(a6)

    move.l a2,a1
    move.l a3,d0
    move.l d7,d1
    add.l d3,d1
    addq.l #1,d1
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
    move.l d4,d0
    jsr _LVOSetAPen(a6)

    move.l a2,a1
    move.l a3,d0
    move.l a4,d1
    jsr _LVOMove(a6)

    move.l a2,a1
    move.l d6,d0
    add.l d2,d0
    addq.l #1,d0
    move.l d0,a3
    move.l a4,d1
    jsr _LVODraw(a6)

    move.l a2,a1
    move.l a3,d0
    move.l d7,d1
    subq.l #1,d1
    jsr _LVODraw(a6)

    move.l a2,a1
    sub.l #1,a3
    move.l a3,d0
    move.l a4,d1
    jsr _LVOMove(a6)

    move.l a2,a1
    move.l a3,d0
    move.l d7,d1
    jsr _LVODraw(a6)

    movem.l (sp)+,a2-a4/a6/d2-d7
    rts

_Do3DCycleBox:
    movem.l a0/d0/d1,-(sp)
    bsr _Do3DBox
    movem.l (sp)+,a0/d0/d1

    movem.l a2-a6/d2-d7,-(sp)

    move.l a6,a5
    move.l db_GfxBase(a6),a6
    move.l a0,a2
    move.l d0,d6
    move.l d1,d7

    move.l a2,a1
    move.l d5,d0
    jsr _LVOSetAPen(a6)

    move.l a2,a1
    moveq.l #0,d0
    move.b 28(a1),d0
    move.l d0,-(sp)
    moveq.l #0,d0
    jsr _LVOSetDrMd(a6)

    movem.l d0-d5,-(sp)
    move.l pdb_cycletop(a5),d0
    tst.l d0
    beq.s nocycletopdraw
    move.l d0,a0
    move.l d6,d2
    addq.l #4,d2
    move.l d3,a3
    move.l d7,d3
    addq.l #1,d3
    sub.l #12,a3
    add.l d3,a3
    add.l #7,a3
    moveq.l #0,d0
    moveq.l #2,d1
    move.l a2,a1
    moveq.l #11,d4
    moveq.l #6,d5
    jsr _LVOBltTemplate(a6)

nocycletopdraw:
    move.l pdb_cyclebot(a5),d0
    tst.l d0
    beq.s nocyclebotdraw
    move.l d0,a0
    moveq.l #0,d0
    moveq.l #2,d1
    move.l a2,a1
    move.l a3,d3
    moveq.l #2,d5
    jsr _LVOBltTemplate(a6)
nocyclebotdraw:
    movem.l (sp)+,d0-d5

    move.l a2,a1
    move.l d6,d0
    addq.l #4,d0
    move.l d0,a3
    move.l d7,d1
    addq.l #7,d1
    move.l d1,a4
    jsr _LVOMove(a6)

    move.l a2,a1
    move.l a3,d0
    move.l a4,d1
    move.l d3,a5
    sub.l #12,a5
    add.l a5,d1
    jsr _LVODraw(a6)

    move.l a2,a1
    move.l d6,d0
    addq.l #5,d0
    move.l d0,a3
    move.l a4,d1
    jsr _LVOMove(a6)

    move.l a2,a1
    move.l a3,d0
    move.l a4,d1
    add.l a5,d1
    jsr _LVODraw(a6)

    move.l a2,a1
    move.l d6,d0
    add.l #18,d0
    move.l d0,a3
    move.l d7,d1
    addq.l #1,d1
    move.l d1,a4
    jsr _LVOMove(a6)

    move.l a2,a1
    move.l a3,d0
    move.l d7,d1
    move.l d3,a5
    sub.l #2,a5
    add.l a5,d1
    jsr _LVODraw(a6)

    move.l a2,a1
    move.l d4,d0
    jsr _LVOSetAPen(a6)

    move.l a2,a1
    move.l d6,d0
    add.l #19,d0
    move.l d0,a3
    move.l a4,d1
    jsr _LVOMove(a6)

    move.l a2,a1
    move.l a3,d0
    move.l d7,d1
    add.l a5,d1
    jsr _LVODraw(a6)

    move.l a2,a1
    move.l (sp)+,d0
    jsr _LVOSetDrMd(a6)

    movem.l (sp)+,a2-a6/d2-d7
    rts

_DrawCheckMark:
    movem.l a2/a5/a6/d2-d7,-(sp)
    move.l d2,d6
    move.l a6,a5

    move.l a0,a2
    move.l a2,a1
    move.l d0,d2
    move.l d1,d3
    moveq.l #13,d4
    moveq.l #7,d5
    move.l db_GfxBase(a6),a6
    move.b 28(a1),-(sp)
    moveq.l #0,d0
    jsr _LVOSetDrMd(a6)

    tst.l d6
    bne.s notclear
    move.l a2,a1
    moveq.l #0,d7
    move.b 25(a1),d7
    ext.l d7
    moveq.l #0,d0
    move.b 26(a1),d0
    jsr _LVOSetAPen(a6)

notclear:
    move.l pdb_check(a5),a0
    move.l a0,d0
    tst.l d0
    beq nodrawcheck
    moveq.l #0,d0
    moveq.l #2,d1
    move.l a2,a1
    jsr _LVOBltTemplate(a6)
nodrawcheck:
    tst.l d6
    bne.s notclear2
    move.l d7,d0
    move.l a2,a1
    jsr _LVOSetAPen(a6)
notclear2:
    moveq.l #0,d0
    move.b (sp)+,d0
    ext.l d0
    move.l a2,a1
    jsr _LVOSetDrMd(a6)
    movem.l (sp)+,a2/a5/a6/d2-d7
    rts

_HiliteGad:
    movem.l a2/a3/a6/d2-d4,-(sp)
    moveq.l #0,d4
    move.b 28(a1),d4
    move.l a0,a2
    move.l a1,a3
    moveq.l #2,d0
    move.l db_GfxBase(a6),a6
    jsr _LVOSetDrMd(a6)
    move.l a3,a1
    moveq.l #0,d0
    moveq.l #0,d1
    move.w 4(a2),d0
    move.w 6(a2),d1
    move.l d0,d2
    add.w 8(a2),d2
    subq.w #1,d2
    move.l d1,d3
    add.w 10(a2),d3
    subq.w #1,d3
    jsr _LVORectFill(a6)
    move.l a3,a1
    move.l d4,d0
    jsr _LVOSetDrMd(a6)
    movem.l (sp)+,a2/a3/a6/d2-d4
    rts

    end
