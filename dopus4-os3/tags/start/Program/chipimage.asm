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

    XDEF _uniconifydata
    XDEF _appicon_data
    XDEF _pageflip_data1
    XDEF _pageflip_data2
    XDEF _null_pointer
    XDEF _beepwave

    section .data_chip, DATA

_uniconifydata:
    dc.w $3f80,$a0a0,$2080,$a0a0,$3f80,$8020,$0000,$8020,$0000,$aaa0
    dc.w $0000,$1f00,$1f00,$1f00,$0000,$0000,$0000,$0000,$0000,$0000
    dc.w $ffe0,$8020,$ffe0,$8020,$8020,$8020,$8020,$8020,$8020,$ffe0
    dc.w $0000,$7fc0,$0000,$7fc0,$7fc0,$7fc0,$7fc0,$7fc0,$7fc0,$0000

_appicon_data:
    dc.w $0000,$0000,$0000,$1fff,$ffff,$0000,$0000,$0000
    dc.w $2000,$0001,$0000,$0000,$0000,$4fff,$fffd,$0000
    dc.w $0c00,$0000,$9fff,$fffd,$0000,$1a00,$0001,$3fff
    dc.w $fffd,$0000,$2500,$0002,$7fff,$fead,$a968,$7280
    dc.w $0004,$ffff,$ff55,$88a4,$e940,$0009,$ffff,$ffad
    dc.w $fffd,$ccbf,$fff3,$ffff,$ffd5,$9003,$ae51,$4027
    dc.w $ffff,$ffed,$95b7,$5729,$574f,$ffff,$ffd5,$900e
    dc.w $b795,$405f,$ffff,$ffed,$95dd,$7fca,$00df,$ff27
    dc.w $fff5,$903a,$ffe4,$7e07,$fe53,$ffed,$9475,$fff1
    dc.w $817a,$0c21,$fff5,$90af,$ffe2,$0085,$f505,$ffed
    dc.w $95d7,$ffc4,$7906,$0a8b,$fff5,$932f,$ffb8,$1000
    dc.w $0505,$ffed,$939f,$ff4c,$21e0,$0221,$fff5,$95df
    dc.w $fe80,$7840,$0453,$ffed,$92ff,$fc80,$0087,$8227
    dc.w $fff5,$917f,$f880,$79e1,$017f,$ffed,$96bf,$f040
    dc.w $1002,$02ff,$ffd5,$905f,$e030,$2007,$85ff,$ffed
    dc.w $972f,$c01f,$7800,$1bff,$ffd5,$9017,$803e,$0020
    dc.w $17ff,$ffad,$96ab,$2047,$00e0,$6fff,$ff55,$9005
    dc.w $50c3,$ff9f,$9fff,$fead,$96da,$a962,$0040,$1fff
    dc.w $fdd5,$9001,$523d,$405f,$8fff,$fc6d,$9758,$a9c1
    dc.w $55df,$c000,$0bb5,$9000,$5221,$405f,$be00,$145d
    dc.w $96e0,$2dc1,$405f,$dfff,$9455,$9000,$3801,$405f
    dc.w $abff,$f45d,$9fff,$ffff,$7fdf,$d555,$5bbd,$9207
    dc.w $c02a,$2a5f,$aaaa,$ac7d,$9fff,$ffff,$ffdf,$d555
    dc.w $57fd,$9020,$2040,$405f,$ffff,$fffd,$95ad,$2b5f
    dc.w $4d40,$0000,$0001,$ffff,$ffff,$ffff,$ffff,$ffff
    dc.w $0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
    dc.w $1fff,$fffe,$0000,$0000,$0000,$3000,$0002,$0000
    dc.w $0000,$0000,$6000,$0002,$0000,$0000,$0000,$c1fc
    dc.w $0002,$ffff,$d0ff,$fffd,$8fff,$8152,$0000,$8800
    dc.w $0003,$1fff,$c0aa,$2001,$1400,$0006,$3fff,$e052
    dc.w $0002,$3200,$000c,$7fff,$f02a,$0004,$5100,$0018
    dc.w $ffff,$f812,$0008,$a880,$0031,$ffff,$fc2a,$0011
    dc.w $4840,$0021,$ff07,$fc12,$0022,$8320,$0021,$fedb
    dc.w $fc0a,$0045,$0690,$0003,$fdad,$fe12,$008a,$0548
    dc.w $7e02,$0bde,$fe0a,$0150,$1291,$ff78,$02fa,$fe12
    dc.w $0228,$2a03,$86f9,$f174,$fe0a,$04d0,$5407,$efff
    dc.w $f8fa,$fe12,$0860,$9033,$de1f,$fcde,$fe0a,$0823
    dc.w $607f,$87bf,$f9ad,$fe12,$0404,$a27f,$ff78,$7cdb
    dc.w $fc0a,$0204,$547f,$861e,$fe07,$fc12,$0102,$8b3f
    dc.w $effd,$fcff,$fc2a,$0081,$168f,$dff8,$79ff,$f812
    dc.w $0040,$2c40,$87ff,$e3ff,$f02a,$0020,$5c01,$ffdf
    dc.w $e7ff,$e052,$0010,$8e38,$ff1f,$83ff,$c0aa,$0008
    dc.w $063c,$0000,$07ff,$8152,$0004,$009c,$0020,$61fc
    dc.w $022a,$0002,$01c0,$0020,$7000,$0392,$0001,$0000
    dc.w $0020,$3fff,$f44a,$0000,$81c0,$0020,$41ff,$eba2
    dc.w $0000,$4000,$0020,$2000,$6baa,$0000,$0000,$0020
    dc.w $5400,$0ba2,$0000,$0000,$0020,$2aaa,$a442,$0000
    dc.w $0000,$0020,$5555,$5382,$0000,$0000,$0020,$2aaa
    dc.w $a802,$0000,$0000,$0020,$0000,$0002,$0000,$0000
    dc.w $003f,$ffff,$fffe,$0000,$0000,$0000,$0000,$0000

_pageflip_data1:
    dc.w $4000,$a000,$9000,$8800,$fc00

_pageflip_data2:
    dc.w $4000,$6000,$7000

_null_pointer:
    dc.w 0,0,0,0,0,0

_beepwave:
    dc.b 0,30,60,95,127,95,60,30,0,-30,-60,-95,-127,-95,-60,-30
