
	INCLUDE	"hardware.inc"
	INCLUDE	"gbt_player.inc"
	
	GLOBAL	song_Data
	
;--------------------------------------------------------------------------
;-                            INTERRUPT VECTORS                           -
;--------------------------------------------------------------------------
	
	SECTION	"VBL Interrupt Vector",HOME[$0040]
	push	hl
	push	de
	push	bc
	push	af
	
	call	gbt_update
	
	pop	af
	pop	bc
	pop	de
	pop	hl
	reti	

;--------------------------------------------------------------------------
;-                             CARTRIDGE HEADER                           -
;--------------------------------------------------------------------------
	
	SECTION	"Cartridge Header",HOME[$0100]
	
	nop
	jp	StartPoint

	DB	$CE,$ED,$66,$66,$CC,$0D,$00,$0B,$03,$73,$00,$83,$00,$0C,$00,$0D
	DB	$00,$08,$11,$1F,$88,$89,$00,$0E,$DC,$CC,$6E,$E6,$DD,$DD,$D9,$99
	DB	$BB,$BB,$67,$63,$6E,$0E,$EC,$CC,$DD,$DC,$99,$9F,$BB,$B9,$33,$3E

		;0123456789ABC
	DB	"GBT Tester"
	DW	$0000
	DB  $00 ;GBC flag
	DB	0,0,0	;SuperGameboy
	DB	$01 ;CARTTYPE (MBC1)
	DB	0	;ROMSIZE
	DB	0	;RAMSIZE

	DB	$01 ;Destination (0 = Japan, 1 = Non Japan)
	DB	$00	;Manufacturer

	DB	0	;Version
	DB	0	;Complement check
	DW	0	;Checksum

;--------------------------------------------------------------------------
;-                          INITIALIZE THE GAMEBOY                        -
;--------------------------------------------------------------------------

	SECTION	"Program Start",HOME[$0150]
	
StartPoint:

	di
	
	ld	sp,$D000 ; Set stack
	
	ld	a,$01
	ld	[rIE],a
	
	ei
	
	ld	de,song_Data
	ld	b,$05
	ld	c,$02
	call	gbt_play
	
.loopme:
	jr	.loopme
	
	
	
