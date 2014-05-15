
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

	.NR50 = 0x24 
	.NR51 = 0x25 
	.NR52 = 0x26 
	
	.area	_DATA
	
gbt_playing::
	.ds	1

gbt_song:: ; pointer to the pointer array
	.ds	2
gbt_bank:: ; bank with the data
	.ds 1
gbt_speed::
	.ds 1

gbt_play_data::
	.ds 8
	
gbt_loop_enabled::
	.ds 1
gbt_ticks_elapsed::
	.ds	1	
gbt_current_step::
	.ds	1
gbt_current_pattern_order::
	.ds	1

gbt_channels_enabled::
	.ds	1
	
gbt_pan::
	.ds	4
gbt_vol::
	.ds	4
gbt_instr::
	.ds	4
gbt_freq::
	.ds	6

; -----------------------------------------------------------------------

	.area	_CODE
	
_gbt_play::
	push	bc
	
	di
	lda	sp,4(sp)
	pop	de ; de = data
	pop	bc ; b = speed , c = bank
	lda	sp,-8(sp)
	ei
	
	ld	hl,#gbt_song
	ld	(hl),d
	inc	hl
	ld	(hl),e
	
	ld	a,c
	ld	(gbt_bank),a
	ld	a,b
	ld	(gbt_speed),a
	
	ld	a,#1
	ld	(gbt_playing),a
	
	xor	a
	ld	(gbt_current_step),a
	ld	(gbt_current_pattern_order),a
	ld	(gbt_ticks_elapsed),a
	ld	(gbt_loop_enabled),a
	
	ld	a,#0x0F
	ld	(gbt_channels_enabled),a
	
	ld	a,#0x80
	ldh	(.NR52),a
	ld	a,#0x00
	ldh	(.NR51),a
	ld	a,#0x77
	ldh	(.NR50),a
	
	ld	hl,#gbt_pan
	ld	a,#0x11
	ld	(hl+),a
	sla	a
	ld	(hl+),a
	sla	a
	ld	(hl+),a
	sla	a
	ld	(hl),a
	
	pop	bc
	ret

; -----------------------------------------------------------------------

_gbt_pause::
	lda	hl,2(sp)
	ld	a,(hl)
	ld	(gbt_playing),a
	or	a
	ret	z
	xor	a
	ldh	(#.NR50),a
	ret

; -----------------------------------------------------------------------
	
_gbt_loop::
	lda	hl,2(sp)
	ld	a,(hl)
	ld	(gbt_loop_enabled),a
	ret

; -----------------------------------------------------------------------

_gbt_stop::
	xor	a
	ld	(gbt_playing),a
	ldh	(#.NR50),a
	ldh	(#.NR51),a
	ldh	(#.NR52),a
	ret
	
; -----------------------------------------------------------------------

_gbt_enable_channels::
	lda	hl,2(sp)
	ld	a,(hl)
	ld	(gbt_channels_enabled),a
	ret

; -----------------------------------------------------------------------

_gbt_update::

	push	bc
	
	ld	a,(gbt_playing)
	or	a
	jr	z,exit$
	
	ld	hl,#gbt_ticks_elapsed
	ld	a,(gbt_speed) ; a = total ticks
	ld	b,(hl) ; b = ticks elapsed
	inc	b
	ld	(hl),b
	cp	b
	jr	nz,exit$
	ld	(hl),#0 ; reset tick counter
	
	ld	a,(gbt_bank)
	ld	(#0x2000),a

get_data_ptr$: ; this label is used by looping code
	
	ld	a,(gbt_current_pattern_order)
	
	ld	e,a
	ld	d,#0
	
	ld	a,(gbt_song)
	ld	h,a
	ld	a,(gbt_song+1)
	ld	l,a
	add	hl,de
	add	hl,de ; hl now points to the pointer to the pattern data :P
	
	ld	e,(hl)
	inc	hl
	ld	d,(hl) ; de now points to the pattern data
	
	ld	h,d
	ld	l,e ; hl now points to the pattern data
	
	ld	a,h
	or	l
	jr	nz,not_last_pattern$
	; last pattern, check if loop enabled to reset it...	
	ld	a,(gbt_loop_enabled)
	or	a
	jr	z,loop_not_enabled$
	; enabled
	xor	a
	ld	(gbt_current_pattern_order),a
	jr	get_data_ptr$
loop_not_enabled$:
	; not enabled
	call	_gbt_stop ; stop song...
	jr	exit$
	
not_last_pattern$:
	ld	a,(gbt_current_step)
	sla	a
	sla	a ; a = step * 4
	ld	e,a
	ld	d,#0
	
	add	hl,de
	add	hl,de ; hl = pattern + (2 * (step * 4) )
	
	ld	d,h
	ld	e,l
	
	ld	hl,#gbt_play_data
	
	ld	c,#8
	
	; copy data to ram...
repeat$:
	ld	a,(de)
	inc	de
	ld	(hl+),a
	dec	c
	jr	nz,repeat$
	
	ld	a,#1
	ld	(#0x2000),a
	call	gbt_update_bank1 ; in gbt_player_bank1.s

exit$:
	pop	bc
	ret
	
