
;        --------------------------------------------------------------
;        ---                                                        ---
;        ---                                                        ---
;        ---                       GBT PLAYER  v1.2                 ---
;        ---                                                        ---
;        ---                                                        ---
;        ---              Copyright (C) 2009-2014 Antonio Niño Díaz ---
;        ---                      All rights reserved.              ---
;        --------------------------------------------------------------
;
;                                          antonio_nd@outlook.com

	INCLUDE	"hardware.inc"
	
	GLOBAL	gbt_speed
	
	GLOBAL	gbt_play_data
	
	GLOBAL	gbt_ticks_elapsed
	GLOBAL	gbt_current_step
	GLOBAL	gbt_current_pattern_order
	
	GLOBAL	gbt_channels_enabled
	
	GLOBAL	gbt_pan
	GLOBAL	gbt_vol
	GLOBAL	gbt_instr
	GLOBAL	gbt_freq
	
	GLOBAL	gbt_ignore_step_update
	
; -----------------------------------------------------------------------

	SECTION "GBT_DATA_BANK1", DATA, BANK[1]

gbt_wave: ; 8 sounds
	DB	$A5,$D7,$C9,$E1,$BC,$9A,$76,$31,$0C,$BA,$DE,$60,$1B,$CA,$03,$93 ; random :P
	DB	$F0,$E1,$D2,$C3,$B4,$A5,$96,$87,$78,$69,$5A,$4B,$3C,$2D,$1E,$0F
	DB	$FD,$EC,$DB,$CA,$B9,$A8,$97,$86,$79,$68,$57,$46,$35,$24,$13,$02 ; little up-downs
	DB	$DE,$FE,$DC,$BA,$9A,$A9,$87,$77,$88,$87,$65,$56,$54,$32,$10,$12 
	DB	$AB,$CD,$EF,$ED,$CB,$A0,$12,$3E,$DC,$BA,$BC,$DE,$FE,$DC,$32,$10 ; triangular broken
	DB	$FF,$EE,$DD,$CC,$BB,$AA,$99,$88,$77,$66,$55,$44,$33,$22,$11,$00 ; triangular
	DB	$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$00,$00,$00,$00,$00,$00,$00,$00 ; square 50%
	DB	$79,$BC,$DE,$EF,$FF,$EE,$DC,$B9,$75,$43,$21,$10,$00,$11,$23,$45 ; sine
	
gbt_noise: ; 16 sounds
	; 7 bit
	DB	$5F,$5B,$4B,$2F,$3B,$58,$1F,$0F
	; 15 bit
	DB	$90,$80,$70,$50,$00
	DB	$67,$63,$53

gbt_frequencies:
	DW	  44,  156,  262,  363,  457,  547,  631,  710,  786,  854,  923,  986
	DW	1046, 1102, 1155, 1205, 1253, 1297, 1339, 1379, 1417, 1452, 1486, 1517
	DW	1546, 1575, 1602, 1627, 1650, 1673, 1694, 1714, 1732, 1750, 1767, 1783
	DW	1798, 1812, 1825, 1837, 1849, 1860, 1871, 1881, 1890, 1899, 1907, 1915
	DW	1923, 1930, 1936, 1943, 1949, 1954, 1959, 1964, 1969, 1974, 1978, 1982
	DW	1985, 1988, 1992, 1995, 1998, 2001, 2004, 2006, 2009, 2011, 2013, 2015

; -----------------------------------------------------------------------
	
	SECTION "GBT_BANK1", CODE, BANK[1]
	
_gbt_get_freq_from_index: ; a = index, de = returned freq
	ld	hl,gbt_frequencies
	ld	e,a
	ld	d,$00
	add	hl,de
	add	hl,de
	ld	e,[hl]
	inc	hl
	ld	d,[hl]
	ret

; ------------------------------ channel 1 ------------------------------

gbt_channel_1_handle:: ; bc = info

	ld	a,[gbt_channels_enabled]
	and	a,$01
	ret	z
	
	bit	7,b 
	jr	z,.channel1_ctrl
	; ----- bit 7 = 1 -- frequency change
	ld	a,c
	and	a,$7F
	call	_gbt_get_freq_from_index ; get frequency
	ld	hl,gbt_freq+0 ; save frequency
	ld	[hl],e
	inc	hl
	ld	a,d
	and	a,$07
	or	a,$80
	ld	[hl],a
	ld	a,b
	sla	a
	sla a
	and	a,$C0
	ld	[gbt_instr+0],a ; save instrument
	bit 6,b
	jr	z,.channel1_no_volumechange
	bit 7,c
	jr	z,.channel1_volumechange
	; panning change
	xor	a,a
	bit 0,b
	jr	z,.channel1_no_right
	or	a,$01
.channel1_no_right:
	bit 1,b
	jr	z,.channel1_no_left
	or	a,$10
.channel1_no_left:
	ld	[gbt_pan + 0],a
	jr .channel1_no_volumechange
.channel1_volumechange:
	; volume change
	ld	a,b
	and	a,$0F
	swap	a
	ld	[gbt_vol + 0],a
.channel1_no_volumechange:
	ld	a,$00
	ldh	[rNR14],a
	ldh	[rNR10],a ; sweep
	ld	a,[gbt_instr]
	ldh	[rNR11],a ; instr
	ld	a,[gbt_vol]
	ldh	[rNR12],a ; vol
	ld	a,[gbt_freq]
	ldh	[rNR13],a ; freq low
	ld	a,[gbt_freq+1]
	ldh	[rNR14],a ; freq hi
	jr	.channel1_end
.channel1_ctrl:
	; ----- bit 7 = 0 
	ld	a,b
	and	a,$07
	jr	z,.channel1_end ; nothing
	dec	a
	jr	nz,.channel1_not_jump
	; jump
	ld	a,c
	ld	[gbt_current_pattern_order],a
	ld	a,$01
	ld	[gbt_ignore_step_update],a
	jr	.channel1_end
.channel1_not_jump:
	dec	a
	jr	nz,.channel1_not_break
	; break
	ld	a,c
	ld	[gbt_current_step],a
	ld	a,[gbt_current_pattern_order]
	inc	a
	ld	[gbt_current_pattern_order],a
	ld	a,$01
	ld	[gbt_ignore_step_update],a
	jr	.channel1_end
.channel1_not_break:
	dec	a
	jr	nz,.channel1_not_speed
	; speed
	ld	a,c
	ld  [gbt_speed],a
	xor	a,a
	ld	[gbt_ticks_elapsed],a
	jr	.channel1_end
.channel1_not_speed:
	dec	a
	jr	nz,.channel1_not_vol
	; volume
	ld	a,c
	and	a,$0F
	swap	a
	ld	[gbt_vol + 0],a
	ld	a,$00
	ldh	[rNR14],a
	ldh	[rNR10],a ; sweep
	ld	a,[gbt_instr]
	ldh	[rNR11],a ; instr
	ld	a,[gbt_vol]
	ldh	[rNR12],a ; vol
	ld	a,[gbt_freq]
	ldh	[rNR13],a ; freq low
	ld	a,[gbt_freq+1]
	ldh	[rNR14],a ; freq hi
	jr	.channel1_end
.channel1_not_vol:
	dec	a
	jr	nz,.channel1_not_pan
	; pan
	xor	a,a
	bit 0,c
	jr	z,.__channel1_no_right
	or	a,$01
.__channel1_no_right:
	bit 1,c
	jr	z,.__channel1_no_left
	or	a,$10
.__channel1_no_left:
	ld	[gbt_pan + 0],a
.channel1_not_pan: ; nothing else... next channel
.channel1_end:
	ret

; ------------------------------ channel 2 ------------------------------

gbt_channel_2_handle::
	
	ld	a,[gbt_channels_enabled]
	and	a,$02
	ret	z
	
	bit	7,b
	jr	z,.channel2_ctrl
	; ----- bit 7 = 1 -- frequency change
	ld	a,c
	and	a,$7F
	call	_gbt_get_freq_from_index
	ld	hl,gbt_freq+2 ; save frequency
	ld	[hl],e
	inc	hl
	ld	a,d
	and	a,$07
	or	a,$80
	ld	[hl],a
	ld	a,b
	sla	a
	sla a
	and	a,$C0
	ld	[gbt_instr+1],a ; save instrument
	bit 6,b
	jr	z,.channel2_no_volumechange
	bit 7,c
	jr	z,.channel2_volumechange
	; panning change
	xor	a,a
	bit 0,b
	jr	z,.channel2_no_right
	or	a,$02
.channel2_no_right:
	bit 1,b
	jr	z,.channel2_no_left
	or	a,$20
.channel2_no_left:
	ld	[gbt_pan + 1],a
	jr .channel2_no_volumechange
.channel2_volumechange:
	; volume change
	ld	a,b
	and	a,$0F
	swap	a
	ld	[gbt_vol + 1],a
.channel2_no_volumechange:
	ld	a,$00
	ldh	[rNR24],a
	ld	a,[gbt_instr+1]
	ldh	[rNR21],a ; instr
	ld	a,[gbt_vol+1]
	ldh	[rNR22],a ; vol
	ld	a,[gbt_freq+2]
	ldh	[rNR23],a ; freq low
	ld	a,[gbt_freq+3]
	ldh	[rNR24],a ; freq hi
	jr	.channel2_end
.channel2_ctrl:
	; ----- bit 7 = 0 
	ld	a,b
	and	a,$07
	jr	z,.channel2_end ; nothing
	dec	a
	jr	nz,.channel2_not_jump
	; jump
	ld	a,c
	ld	[gbt_current_pattern_order],a
	ld	a,$01
	ld	[gbt_ignore_step_update],a
	jr	.channel2_end
.channel2_not_jump:
	dec	a
	jr	nz,.channel2_not_break
	; break
	ld	a,c
	ld	[gbt_current_step],a
	ld	a,[gbt_current_pattern_order]
	inc	a
	ld	[gbt_current_pattern_order],a
	ld	a,$01
	ld	[gbt_ignore_step_update],a
	jr	.channel2_end
.channel2_not_break:
	dec	a
	jr	nz,.channel2_not_speed
	; speed
	ld	a,c
	ld  [gbt_speed],a
	xor	a,a
	ld	[gbt_ticks_elapsed],a
	jr	.channel2_end
.channel2_not_speed:
	dec	a
	jr	nz,.channel2_not_vol
	; volume
	ld	a,c
	and	a,$0F
	swap	a
	ld	[gbt_vol + 1],a
	ld	a,$00
	ldh	[rNR24],a
	ld	a,[gbt_instr+1]
	ldh	[rNR21],a ; instr
	ld	a,[gbt_vol+1]
	ldh	[rNR22],a ; vol
	ld	a,[gbt_freq+2]
	ldh	[rNR23],a ; freq low
	ld	a,[gbt_freq+3]
	ldh	[rNR24],a ; freq hi
	jr	.channel2_end
.channel2_not_vol:
	dec	a
	jr	nz,.channel2_not_pan	
	; pan
	xor	a,a
	bit 0,c
	jr	z,.__channel2_no_right
	or	a,$02
.__channel2_no_right:
	bit 1,c
	jr	z,.__channel2_no_left
	or	a,$20
.__channel2_no_left:
	ld	[gbt_pan + 1],a
.channel2_not_pan: ; nothing else... next channel
.channel2_end:
	ret

; ------------------------------ channel 3 ------------------------------

gbt_channel_3_handle::
	
	ld	a,[gbt_channels_enabled]
	and	a,$04
	ret	z
	
	bit	7,b
	jr	z,.channel3_ctrl
	; ----- bit 7 = 1 -- frequency change
	ld	a,c
	and	a,$7F
	call	_gbt_get_freq_from_index
	ld	hl,gbt_freq+4 ; save frequency
	ld	[hl],e
	inc	hl
	ld	a,d
	and	a,$07
	or	a,$80
	ld	[hl],a
	ld	a,b
	and	a,$07
	ld	[gbt_instr+2],a ; save instrument
	bit 6,b
	jr	z,.channel3_no_volumechange
	bit 5,b
	jr	z,.channel3_volumechange
	; panning change
	xor	a,a
	bit 3,b
	jr	z,.channel3_no_right
	or	a,$04
.channel3_no_right:
	bit 4,b
	jr	z,.channel3_no_left
	or	a,$40
.channel3_no_left:
	ld	[gbt_pan + 2],a
	jr .channel3_no_volumechange
.channel3_volumechange:
	; volume change
	ld	a,b
	sla	a
	sla	a
	and	a,$60
	ld	[gbt_vol + 2],a
.channel3_no_volumechange:
	ld	a,$00
	ldh	[rNR30],a
	ldh	[rNR34],a
	ld	a,[gbt_instr+2]
	push	bc
	swap	a
	and	a,$70
	ld	b,$00
	ld	c,a
	ld	hl,gbt_wave
	add	hl,bc
	ld	b,$10
	ld	c,$30
.continue_copy:
	ld	a,[hl+]
	ld	[$FF00+c],a
	inc	c
	dec	b
	jr	nz,.continue_copy	
	pop	bc
	ld	a,[gbt_vol+2]
	ldh	[rNR32],a ; vol
	ld	a,$80
	ldh	[rNR30],a
	ld	a,$FF
	ldh	[rNR31],a ; len??
	ld	a,[gbt_freq+4]
	ldh	[rNR33],a ; freq low
	ld	a,[gbt_freq+5]
	ldh	[rNR34],a ; freq hi
	jr	.channel3_end
.channel3_ctrl:
	; ----- bit 7 = 0 
	ld	a,b
	and	a,$07
	jr	z,.channel3_end ; nothing
	dec	a
	jr	nz,.channel3_not_jump
	; jump
	ld	a,c
	ld	[gbt_current_pattern_order],a
	ld	a,$01
	ld	[gbt_ignore_step_update],a
	jr	.channel3_end
.channel3_not_jump:
	dec	a
	jr	nz,.channel3_not_break
	; break
	ld	a,c
	ld	[gbt_current_step],a
	ld	a,[gbt_current_pattern_order]
	inc	a
	ld	[gbt_current_pattern_order],a
	ld	a,$01
	ld	[gbt_ignore_step_update],a
	jr	.channel3_end
.channel3_not_break:
	dec	a
	jr	nz,.channel3_not_speed
	; speed
	ld	a,c
	ld  [gbt_speed],a
	xor	a,a
	ld	[gbt_ticks_elapsed],a
	jr	.channel3_end
.channel3_not_speed:
	dec	a
	jr	nz,.channel3_not_vol
	; volume
	ld	a,c
	swap	a
	sla	a
	and	a,$60
	ld	[gbt_vol + 2],a
	ld	a,$00
	ldh	[rNR30],a
	ldh	[rNR34],a
	ld	a,[gbt_vol+2]
	ldh	[rNR32],a ; vol
	ld	a,$80
	ldh	[rNR30],a
	ld	a,$FF
	ldh	[rNR31],a ; len??
	ld	a,[gbt_freq+4]
	ldh	[rNR33],a ; freq low
	ld	a,[gbt_freq+5]
	ldh	[rNR34],a ; freq hi
	jr	.channel3_end
.channel3_not_vol:
	dec	a
	jr	nz,.channel3_not_pan	
	; pan
	xor	a,a
	bit 0,c
	jr	z,.__channel3_no_right
	or	a,$04
.__channel3_no_right:
	bit 1,c
	jr	z,.__channel3_no_left
	or	a,$40
.__channel3_no_left:
	ld	[gbt_pan + 2],a
.channel3_not_pan: ; nothing else... next channel
.channel3_end:
	ret

; ------------------------------ channel 4 ------------------------------

gbt_channel_4_handle::
	
	ld	a,[gbt_channels_enabled]
	and	a,$08
	ret	z
	
	bit	7,b
	jr	z,.channel4_ctrl
	; ----- bit 7 = 1 -- instrument change	
	ld	a,c
	and	a,$0F
	ld	hl,gbt_noise
	ld	d,$00
	ld	e,a
	add	hl,de
	ld	a,[hl]
	ld	[gbt_instr+3],a ; save instrument
	bit 6,b
	jr	z,.channel4_no_volumechange
	bit 7,c
	jr	z,.channel4_volumechange
	; panning change
	xor	a,a
	bit 0,b
	jr	z,.channel4_no_right
	or	a,$08
.channel4_no_right:
	bit 1,b
	jr	z,.channel4_no_left
	or	a,$80
.channel4_no_left:
	ld	[gbt_pan + 3],a
	jr .channel4_no_volumechange
.channel4_volumechange:
	; volume change
	ld	a,b
	and	a,$0F
	swap	a
	ld	[gbt_vol + 3],a
.channel4_no_volumechange:
	ld	a,$00
	ldh	[rNR44],a
	ld	a,[gbt_vol+3]
	ldh	[rNR42],a ; vol
	ld	a,[gbt_instr+3]
	ldh	[rNR43],a
	ld	a,$80
	ldh	[rNR44],a
	jr	.channel4_end
.channel4_ctrl:
	; ----- bit 7 = 0 
	ld	a,b
	and	a,$07
	jr	z,.channel4_end ; nothing
	dec	a
	jr	nz,.channel4_not_jump
	; jump
	ld	a,c
	ld	[gbt_current_pattern_order],a
	ld	a,$01
	ld	[gbt_ignore_step_update],a
	jr	.channel4_end
.channel4_not_jump:
	dec	a
	jr	nz,.channel4_not_break
	; break
	ld	a,c
	ld	[gbt_current_step],a
	ld	a,[gbt_current_pattern_order]
	inc	a
	ld	[gbt_current_pattern_order],a
	ld	a,$01
	ld	[gbt_ignore_step_update],a
	jr	.channel4_end
.channel4_not_break:
	dec	a
	jr	nz,.channel4_not_speed
	; speed
	ld	a,c
	ld  [gbt_speed],a
	xor	a,a
	ld	[gbt_ticks_elapsed],a
	jr	.channel4_end
.channel4_not_speed:
	dec	a
	jr	nz,.channel4_not_vol
	; volume
	ld	a,c
	and	a,$0F
	swap	a
	ld	[gbt_vol + 3],a
	ld	a,$00
	ldh	[rNR44],a
	ld	a,[gbt_vol+3]
	ldh	[rNR42],a ; vol
	ld	a,[gbt_instr+3]
	ldh	[rNR43],a
	ld	a,$80
	ldh	[rNR44],a
	jr	.channel4_end
.channel4_not_vol:
	dec	a
	jr	nz,.channel4_not_pan	
	; pan
	xor	a,a
	bit 0,c
	jr	z,.__channel4_no_right
	or	a,$08
.__channel4_no_right:
	bit 1,c
	jr	z,.__channel4_no_left
	or	a,$80
.__channel4_no_left:
	ld	[gbt_pan + 3],a
.channel4_not_pan: ; nothing else...
.channel4_end:
	ret
	
; -----------------------------------------------------------------------

gbt_update_bank1::
	
	xor	a,a
	ld	[gbt_ignore_step_update],a
	
	ld	hl,gbt_play_data
	; ----- channel 1
	ld	b,[hl]
	inc	hl
	ld	c,[hl]
	inc	hl
	push	hl
	call	gbt_channel_1_handle
	pop	hl
	; ----- channel 2
	ld	b,[hl]
	inc	hl
	ld	c,[hl]
	inc	hl
	push	hl
	call	gbt_channel_2_handle
	pop	hl
	; ----- channel 3
	ld	b,[hl]
	inc	hl
	ld	c,[hl]
	inc	hl
	push	hl
	call	gbt_channel_3_handle
	pop	hl
	; ----- channel 4
	ld	b,[hl]
	inc	hl
	ld	c,[hl]
	push	hl
	call	gbt_channel_4_handle
	pop	hl
	
	; end of channel handling
	
	ld	hl,gbt_pan
	ld	a,[hl+]
	or	a,[hl]
	inc	hl
	or	a,[hl]
	inc hl
	or	a,[hl]
	ldh	[rNR51],a ; handle panning...
	
	; update step / pattern
	
	ld	a,[gbt_ignore_step_update]
	or	a,a
	ret	nz
	
	ld	a,[gbt_current_step]
	cp	a,63
	jr	z,.update_pattern
	inc	a
	ld	[gbt_current_step],a
	ret
	
.update_pattern:
	xor	a,a
	ld	[gbt_current_step],a
	ld	a,[gbt_current_pattern_order]
	inc	a
	ld	[gbt_current_pattern_order],a ; we check in bank 0 if finished, not here

	ret
	



