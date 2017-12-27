        output  loader.bin
        ld      b, $7d
        ld      de, $fd8e
        db      $c0, $37, $0e, $8f, $39, $96 ;OVER USR 7 ($5ccb)
        ld      hl, $da6d
        lddr
        inc     de
        ex      de, hl
        ld      sp, hl
        jp      (hl)
