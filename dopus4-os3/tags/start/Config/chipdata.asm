    XDEF _pageflip_data1
    XDEF _pageflip_data2
    XDEF _dropbox_data
    XDEF _glass_data
    XDEF _uparrow_data
    XDEF _downarrow_data


    section .data_chip,DATA

_pageflip_data1:
    dc.w $4000,$a000,$9000,$8800,$fc00

_pageflip_data2:
    dc.w $4000,$6000,$7000

_dropbox_data:
    dc.w $0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
    dc.w $0000,$0000,$0000,$0000,$3f3f,$1f3f,$0000,$0000
    dc.w $3f80,$0000,$31b1,$b1b1,$8000,$0000,$3f80,$0000
    dc.w $31b1,$b1b1,$8000,$0003,$f800,$0000,$31bf,$31bf
    dc.w $0000,$0007,$f800,$0000,$31b6,$31b0,$0000,$0e0f
    dc.w $8000,$0000,$31b3,$31b0,$0000,$0e0f,$0000,$0000
    dc.w $3f31,$9f30,$0000,$1e0e,$0000,$0000,$0000,$0000
    dc.w $0000,$1e0e,$0000,$0000,$0000,$0000,$00f0,$1c06
    dc.w $0f00,$0000,$0000,$0000,$00f0,$1c06,$0700,$0000
    dc.w $0000,$0000,$01e0,$1c03,$0000,$0000,$0000,$000f
    dc.w $e1e0,$0c00,$0010,$0000,$0000,$01ff,$f1c0,$0c00
    dc.w $0000,$0000,$0000,$01fe,$01c0,$0e00,$0000,$0000
    dc.w $0000,$0fc0,$00c0,$0e00,$00e0,$0000,$0000,$0f80
    dc.w $00c0,$0207,$83e0,$0000,$007e,$0e00,$3060,$0207
    dc.w $ffe0,$0000,$007e,$0e00,$3860,$0383,$0f80,$0000
    dc.w $07f0,$0e00,$7860,$0383,$0000,$0000,$1ff0,$0e03
    dc.w $f060,$0381,$0000,$0000,$1f00,$060f,$f060,$3f80
    dc.w $0000,$0000,$3c00,$0606,$0070,$7f80,$0000,$0000
    dc.w $3c03,$0300,$0070,$3800,$0000,$0000,$3803,$8300
    dc.w $0030,$0000,$0000,$0000,$3801,$8300,$0000,$0000
    dc.w $0000,$0000,$3801,$8100,$0000,$0000,$0000,$0000
    dc.w $3801,$c100,$0000,$0000,$0000,$0000,$3801,$c1c0
    dc.w $0000,$0000,$0000,$0000,$3801,$c1c0,$0000,$0000
    dc.w $0000,$0000,$3801,$c1c0,$0000,$0000,$0000,$0000
    dc.w $3801,$c1c0,$0000,$0000,$0000,$0000,$1807,$c0c0
    dc.w $0000,$007e,$3e61,$8000,$181f,$c000,$0000,$0063
    dc.w $6333,$0000,$0c1f,$0000,$0000,$0063,$631e,$0000
    dc.w $0c00,$0000,$0000,$007e,$630c,$0000,$0000,$0000
    dc.w $0000,$0063,$631e,$0000,$0000,$0000,$0000,$0063
    dc.w $6333,$0000,$0000,$0000,$0000,$007e,$3e61,$8000
    dc.w $0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
    dc.w $0000,$0000,$0000,$0000
    dc.w $0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
    dc.w $0000,$0000,$0000,$0000,$3f3f,$1f3f,$0000,$0000
    dc.w $0000,$0000,$31b1,$b1b1,$8000,$0000,$0000,$0000
    dc.w $31b1,$b1b1,$8000,$0000,$07f0,$0000,$31bf,$31bf
    dc.w $0000,$0000,$07f0,$0000,$31b6,$31b0,$0000,$0000
    dc.w $7ffc,$0000,$31b3,$31b0,$0000,$0000,$fffe,$0000
    dc.w $3f31,$9f30,$0000,$01c1,$f03c,$0000,$0000,$0000
    dc.w $0000,$01c1,$f03c,$0000,$0000,$0000,$0000,$03e1
    dc.w $f000,$0000,$0000,$0000,$0000,$03e1,$f800,$0000
    dc.w $0000,$0000,$001e,$03e0,$ffe0,$0000,$0000,$0000
    dc.w $001e,$03e0,$ffe0,$0000,$0000,$0000,$003e,$03f0
    dc.w $7ffc,$0000,$0000,$0001,$fc3f,$01f0,$0ffe,$0000
    dc.w $0000,$003f,$fe3f,$01f0,$001f,$0000,$0000,$003f
    dc.w $fe3f,$01f0,$001f,$0000,$0000,$01ff,$8f9f,$01f0
    dc.w $001f,$0000,$0000,$01f0,$079f,$007c,$f07f,$0000
    dc.w $000f,$c1f0,$079f,$807c,$fffe,$0000,$000f,$c1f0
    dc.w $079f,$807c,$fffe,$0000,$00ff,$f9f0,$0f9f,$807c
    dc.w $fffc,$0000,$03ff,$f9f8,$7e0f,$807c,$3f80,$0000
    dc.w $03f0,$fcff,$fe0f,$c7fc,$0000,$0000,$07c0,$7cff
    dc.w $c00f,$fff0,$0000,$0000,$07c0,$7cff,$c00f,$fff0
    dc.w $0000,$0000,$07c0,$7cf8,$0007,$ffe0,$0000,$0000
    dc.w $07c0,$3cfe,$0000,$fc00,$0000,$0000,$07c0,$3e3e
    dc.w $0000,$0000,$0000,$0000,$07c0,$3e3e,$0000,$0000
    dc.w $0000,$0000,$07c0,$3e3e,$0000,$0000,$0000,$0000
    dc.w $07c0,$3e3f,$0000,$0000,$0000,$0000,$07e0,$3e3f
    dc.w $0000,$007e,$3e61,$8000,$07e0,$3e3f,$0000,$0063
    dc.w $6333,$0000,$03e0,$fe1e,$0000,$0063,$631e,$0000
    dc.w $03ff,$fc00,$0000,$007e,$630c,$0000,$01ff,$f800
    dc.w $0000,$0063,$631e,$0000,$01ff,$f800,$0000,$0063
    dc.w $6333,$0000,$003f,$0000,$0000,$007e,$3e61,$8000
    dc.w $0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
    dc.w $0000,$0000,$0000,$0000

_glass_data:
    dc.w $0fe0,$0000,$701c,$0000,$c286,$0000,$c806,$0000
    dc.w $c006,$0000,$701c,$0000,$0fef,$0000,$0003,$c000
    dc.w $0000,$f000
    dc.w $0000,$0000,$0fe0,$0000,$3d78,$0000,$37f8,$0000
    dc.w $3ff8,$0000,$0fe0,$0000,$0000,$0000,$0000,$0000
    dc.w $0000,$0000

_uparrow_data:
    dc.w $0600,$0f00,$1f80,$3fc0,$7fe0,$fff0

_downarrow_data:
    dc.w $fff0,$7fe0,$3fc0,$1f80,$0f00,$0600
