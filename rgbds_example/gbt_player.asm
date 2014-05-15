
;        --------------------------------------------------------------
;        ---                                                        ---
;        ---                                                        ---
;        ---                       GBT PLAYER  v1.2.1               ---
;        ---                                                        ---
;        ---                                                        ---
;        ---              Copyright (C) 2009-2014 Antonio Niño Díaz ---
;        ---                      All rights reserved.              ---
;        --------------------------------------------------------------
;
;                                          antonio_nd@outlook.com

	INCLUDE	"hardware.inc"

; -----------------------------------------------------------------------

	SECTION "GBT_VAR_1", BSS
	
gbt_playing::	DS	1

; pointer to the pointer array
gbt_song::	DS	2
; bank with the data
gbt_bank:: 	DS	1
gbt_speed::	DS	1

gbt_play_data::	DS	8
	
gbt_loop_enabled::	DS	1
gbt_ticks_elapsed::	DS	1
gbt_current_step::	DS	1
gbt_current_pattern_order::	DS	1

gbt_channels_enabled::	DS	1
	
gbt_pan::	DS	4
gbt_vol::	DS	4
gbt_instr::	DS	4
gbt_freq::	DS	6

gbt_ignore_step_update::	DS	1

; -----------------------------------------------------------------------

	SECTION "GBT_BANK0", HOME
	
gbt_play:: ; de = data, b = speed, c = bank

	ld	hl,gbt_song
	ld	[hl],d
	inc	hl
	ld	[hl],e
	
	ld	a,c
	ld	[gbt_bank],a
	ld	a,b
	ld	[gbt_speed],a
	
	ld	a,$01
	ld	[gbt_playing],a
	
	xor	a,a
	ld	[gbt_current_step],a
	ld	[gbt_current_pattern_order],a
	ld	[gbt_ticks_elapsed],a
	ld	[gbt_loop_enabled],a
	
	ld	a,$0F
	ld	[gbt_channels_enabled],a
	
	ld	a,$80
	ldh	[rNR52],a
	ld	a,$00
	ldh	[rNR51],a
	ld	a,$77
	ldh	[rNR50],a
	
	ld	hl,gbt_pan
	ld	a,$11
	ld	[hl+],a
	sla	a
	ld	[hl+],a
	sla	a
	ld	[hl+],a
	sla	a
	ld	[hl],a
	
	ret

; -----------------------------------------------------------------------

gbt_pause:: ; a = pause/unpause
	ld	[gbt_playing],a
	or	a,a
	ret	z
	xor	a,a
	ldh	[rNR50],a
	ret

; -----------------------------------------------------------------------
	
gbt_loop:: ; a = loop/don't loop
	ld	[gbt_loop_enabled],a
	ret

; -----------------------------------------------------------------------

gbt_stop::
	xor	a
	ld	[gbt_playing],a
	ldh	[rNR50],a
	ldh	[rNR51],a
	ldh	[rNR52],a
	ret
	
; -----------------------------------------------------------------------

gbt_enable_channels:: ; a = channel flags (channel flag = (1<<(channel_num-1)))
	ld	[gbt_channels_enabled],a
	ret

; -----------------------------------------------------------------------
	
	GLOBAL	gbt_update_bank1
	
gbt_update::

	ld	a,[gbt_playing]
	or	a,a
	ret	z
	
	ld	hl,gbt_ticks_elapsed
	ld	a,[gbt_speed] ; a = total ticks
	ld	b,[hl] ; b = ticks elapsed
	inc	b
	ld	[hl],b
	cp	a,b
	ret	nz
	ld	[hl],$00 ; reset tick counter
	
	ld	a,[gbt_bank]
	ld	[$2000],a

.get_data_ptr: ; this label is used by looping code
	
	ld	a,[gbt_current_pattern_order]
	
	ld	e,a
	ld	d,$00
	
	ld	a,[gbt_song]
	ld	h,a
	ld	a,[gbt_song+1]
	ld	l,a
	add	hl,de
	add	hl,de ; hl now points to the pointer to the pattern data :P
	
	ld	e,[hl]
	inc	hl
	ld	d,[hl] ; de now points to the pattern data
	
	ld	h,d
	ld	l,e ; hl now points to the pattern data
	
	ld	a,h
	or	a,l
	jr	nz,.not_last_pattern
	; last pattern, check if loop enabled to reset it...	
	ld	a,[gbt_loop_enabled]
	or	a,a
	jr	z,.loop_not_enabled
	; enabled
	xor	a,a
	ld	[gbt_current_pattern_order],a
	jr	.get_data_ptr
.loop_not_enabled:
	; not enabled
	call	gbt_stop ; stop song...
	ret
	
.not_last_pattern:
	ld	a,[gbt_current_step]
	sla	a
	sla	a ; a = step * 4
	ld	e,a
	ld	d,$00
	
	add	hl,de
	add	hl,de ; hl = pattern + [2 * [step * 4] ]
	
	ld	d,h
	ld	e,l
	
	ld	hl,gbt_play_data
	
	ld	c,$08
	
	; copy data to ram...
.repeat:
	ld	a,[de]
	inc	de
	ld	[hl+],a
	dec	c
	jr	nz,.repeat
	
	ld	a,$01
	ld	[$2000],a
	call	gbt_update_bank1 ; in gbt_player_bank1.s

	ret
	
