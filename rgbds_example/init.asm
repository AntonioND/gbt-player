
	INCLUDE	"hardware.inc"
	INCLUDE	"gbt_player.inc"
	
	GLOBAL	song_data

;--------------------------------------------------------------------------
;-                             CARTRIDGE HEADER                           -
;--------------------------------------------------------------------------
	
	SECTION	"Cartridge Header",ROM0[$0100]
	
	nop
	jp	StartPoint

	NINTENDO_LOGO
	
		;0123456789ABC
	DB	"GBT Tester"
	DW	$0000
	DB	$00     ;GBC flag
	DB	0,0,0	;SuperGameboy
	DB	CART_ROM_MBC5 ;CARTTYPE (MBC5)
	DB	0	;ROMSIZE
	DB	0	;RAMSIZE

	DB	$01     ;Destination (0 = Japan, 1 = Non Japan)
	DB	$00	;Manufacturer

	DB	0	;Version
	DB	0	;Complement check
	DW	0	;Checksum

;--------------------------------------------------------------------------
;-                          INITIALIZE THE GAMEBOY                        -
;--------------------------------------------------------------------------

	SECTION	"Program Start",ROM0[$0150]
	
StartPoint:

	di
	
	ld	sp,$D000 ; Set stack
	
	ld	a,$01
	ld	[rIE],a ; Enable VBL interrupt
	
	ld	de,song_data
	ld	bc,BANK(song_data)
	ld	a,$05
	call	gbt_play ; Play song
	
.loop:
	halt
	xor	a,a
	ld	[rIF],a
	
	call	gbt_update ; Update player
	jr	.loop

	
