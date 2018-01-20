;-------------------------------------------------------------------------------

    INCLUDE "hardware.inc"
    INCLUDE "gbt_player.inc"

;-------------------------------------------------------------------------------

    GLOBAL  song_data

;-------------------------------------------------------------------------------
;-                                Cartridge header                             -
;-------------------------------------------------------------------------------

    SECTION "VBL interrupt vector", ROM0[$0040]

    reti

;-------------------------------------------------------------------------------
;-                                Cartridge header                             -
;-------------------------------------------------------------------------------

    SECTION "Cartridge header", ROM0[$0100]

    nop
    jp      StartPoint

    NINTENDO_LOGO

    ;    0123456789ABCDE
    DB  "GBT TESTER     "
    DB  $00 ;GBC flag
    DB  0,0,0 ;SGB
    DB  CART_ROM_MBC5 ;CARTTYPE (MBC5)
    DB  0   ;ROMSIZE
    DB  0   ;RAMSIZE

    DB  $01 ;Destination (0 = Japan, 1 = Non Japan)
    DB  $00 ;Manufacturer

    DB  0   ;Version
    DB  0   ;Complement check
    DW  0   ;Checksum

;-------------------------------------------------------------------------------
;-                             Initialize the Game Boy                         -
;-------------------------------------------------------------------------------

    SECTION "Program Start", ROM0[$0150]

StartPoint:

    di

    ld      sp,$D000 ; Set stack

    ld      a,$01
    ld      [rIE],a ; Enable VBL interrupt

    ei

    ld      de,song_data
    ld      bc,BANK(song_data)
    ld      a,$05
    call    gbt_play ; Play song

.loop:

    halt

    call    gbt_update ; Update player

    jr      .loop

;-------------------------------------------------------------------------------
