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

_LVOSignal equ -$144

	XDEF @InputHandler
	XDEF _InputHandler

	section code

_InputHandler:
@InputHandler:
	cmp.b #2,4(a0)
	bne.s skiphandler
	cmp.w #$68,6(a0)
	bne.s notdown
	moveq.l #1,d0
	bra.s sendsignal
notdown:
	cmp.w #$e8,6(a0)
	bne.s skiphandler
	moveq.l #2,d0
sendsignal:
	move.l 4,a6
	move.l a0,-(sp)
	jsr _LVOSignal(a6)
	move.l (sp)+,a0
skiphandler:
	move.l a0,d0
	rts

	end
