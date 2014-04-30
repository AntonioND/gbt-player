
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

	.NR10 = 0x10
	.NR11 = 0x11 
	.NR12 = 0x12 
	.NR13 = 0x13 
	.NR14 = 0x14 
	.NR21 = 0x16 
	.NR22 = 0x17 
	.NR23 = 0x18 
	.NR24 = 0x19 
	.NR30 = 0x1A 
	.NR31 = 0x1B 
	.NR32 = 0x1C 
	.NR33 = 0x1D 
	.NR34 = 0x1E 
	.NR41 = 0x20 
	.NR42 = 0x21 
	.NR43 = 0x22 
	.NR44 = 0x23 
	.NR50 = 0x24 
	.NR51 = 0x25 
	.NR52 = 0x26 
	
	.area	_DATA
	
_gbt_ignore_step_update:
	.ds	1
	
; -----------------------------------------------------------------------

	.area	_CODE_1

gbt_wave: ; 8 sounds
	.DB	0xA5,0xD7,0xC9,0xE1,0xBC,0x9A,0x76,0x31,0x0C,0xBA,0xDE,0x60,0x1B,0xCA,0x03,0x93 ; random :P
	.DB	0xF0,0xE1,0xD2,0xC3,0xB4,0xA5,0x96,0x87,0x78,0x69,0x5A,0x4B,0x3C,0x2D,0x1E,0x0F
	.DB	0xFD,0xEC,0xDB,0xCA,0xB9,0xA8,0x97,0x86,0x79,0x68,0x57,0x46,0x35,0x24,0x13,0x02 ; little up-downs
	.DB	0xDE,0xFE,0xDC,0xBA,0x9A,0xA9,0x87,0x77,0x88,0x87,0x65,0x56,0x54,0x32,0x10,0x12 
	.DB	0xAB,0xCD,0xEF,0xED,0xCB,0xA0,0x12,0x3E,0xDC,0xBA,0xBC,0xDE,0xFE,0xDC,0x32,0x10 ; triangular broken
	.DB	0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00 ; triangular
	.DB	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 ; square 50%
	.DB	0x79,0xBC,0xDE,0xEF,0xFF,0xEE,0xDC,0xB9,0x75,0x43,0x21,0x10,0x00,0x11,0x23,0x45 ; sine
	
gbt_noise: ; 16 sounds
	; 7 bit
	.DB	0x5F,0x5B,0x4B,0x2F,0x3B,0x58,0x1F,0x0F
	; 15 bit
	.DB	0x90,0x80,0x70,0x50,0x00
	.DB	0x67,0x63,0x53

gbt_frequencies:
	.DW	  44,  156,  262,  363,  457,  547,  631,  710,  786,  854,  923,  986
	.DW	1046, 1102, 1155, 1205, 1253, 1297, 1339, 1379, 1417, 1452, 1486, 1517
	.DW	1546, 1575, 1602, 1627, 1650, 1673, 1694, 1714, 1732, 1750, 1767, 1783
	.DW	1798, 1812, 1825, 1837, 1849, 1860, 1871, 1881, 1890, 1899, 1907, 1915
	.DW	1923, 1930, 1936, 1943, 1949, 1954, 1959, 1964, 1969, 1974, 1978, 1982
	.DW	1985, 1988, 1992, 1995, 1998, 2001, 2004, 2006, 2009, 2011, 2013, 2015

; -----------------------------------------------------------------------

_gbt_get_freq_from_index: ; a = index, de = returned freq
	ld	hl,#gbt_frequencies
	ld	e,a
	ld	d,#0
	add	hl,de
	add	hl,de
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	ret

; ------------------------------ channel 1 ------------------------------

gbt_channel_1_handle:: ; bc = info

	ld	a,(gbt_channels_enabled)
	and	a,#0x01
	ret	z
	
	bit	7,b 
	jr	z,channel1_ctrl$
	; ----- bit 7 = 1 -- frequency change
	ld	a,c
	and	#0x7F
	call	_gbt_get_freq_from_index ; get frequency
	ld	hl,#gbt_freq+0 ; save frequency
	ld	(hl),e
	inc	hl
	ld	a,d
	and	#0x07
	or	#0x80
	ld	(hl),a
	ld	a,b
	sla	a
	sla a
	and	#0xC0
	ld	(gbt_instr+0),a ; save instrument
	bit 6,b
	jr	z,channel1_no_volumechange$
	bit 7,c
	jr	z,channel1_volumechange$
	; panning change
	xor	a
	bit 0,b
	jr	z,channel1_no_right$
	or	#0x01
channel1_no_right$:
	bit 1,b
	jr	z,channel1_no_left$
	or	#0x10
channel1_no_left$:
	ld	(gbt_pan + 0),a
	jr channel1_no_volumechange$
channel1_volumechange$:
	; volume change
	ld	a,b
	and	#0x0F
	swap	a
	ld	(gbt_vol + 0),a
channel1_no_volumechange$:
	ld	a,#0
	ldh	(#.NR14),a
	ldh	(#.NR10),a ; sweep
	ld	a,(gbt_instr)
	ldh	(#.NR11),a ; instr
	ld	a,(gbt_vol)
	ldh	(#.NR12),a ; vol
	ld	a,(gbt_freq)
	ldh	(#.NR13),a ; freq low
	ld	a,(gbt_freq+1)
	ldh	(#.NR14),a ; freq hi
	jr	channel1_end$
channel1_ctrl$:
	; ----- bit 7 = 0 
	ld	a,b
	and	#0x07
	jr	z,channel1_end$ ; nothing
	dec	a
	jr	nz,channel1_not_jump$
	; jump
	ld	a,c
	ld	(gbt_current_pattern_order),a
	ld	a,#1
	ld	(_gbt_ignore_step_update),a
	jr	channel1_end$
channel1_not_jump$:
	dec	a
	jr	nz,channel1_not_break$
	; break
	ld	a,c
	ld	(gbt_current_step),a
	ld	a,(gbt_current_pattern_order)
	inc	a
	ld	(gbt_current_pattern_order),a
	ld	a,#1
	ld	(_gbt_ignore_step_update),a
	jr	channel1_end$
channel1_not_break$:
	dec	a
	jr	nz,channel1_not_speed$
	; speed
	ld	a,c
	ld  (gbt_speed),a
	xor	a
	ld	(gbt_ticks_elapsed),a
	jr	channel1_end$
channel1_not_speed$:
	dec	a
	jr	nz,channel1_not_vol$
	; volume
	ld	a,c
	and	#0x0F
	swap	a
	ld	(gbt_vol + 0),a
	ld	a,#0
	ldh	(#.NR14),a
	ldh	(#.NR10),a ; sweep
	ld	a,(gbt_instr)
	ldh	(#.NR11),a ; instr
	ld	a,(gbt_vol)
	ldh	(#.NR12),a ; vol
	ld	a,(gbt_freq)
	ldh	(#.NR13),a ; freq low
	ld	a,(gbt_freq+1)
	ldh	(#.NR14),a ; freq hi
	jr	channel1_end$
channel1_not_vol$:
	dec	a
	jr	nz,channel1_not_pan$	
	; pan
	xor	a
	bit 0,c
	jr	z,__channel1_no_right$
	or	#0x01
__channel1_no_right$:
	bit 1,c
	jr	z,__channel1_no_left$
	or	#0x10
__channel1_no_left$:
	ld	(gbt_pan + 0),a
channel1_not_pan$: ; nothing else... next channel
channel1_end$:
	ret

; ------------------------------ channel 2 ------------------------------

gbt_channel_2_handle::
	
	ld	a,(gbt_channels_enabled)
	and	a,#0x02
	ret	z
	
	bit	7,b
	jr	z,channel2_ctrl$
	; ----- bit 7 = 1 -- frequency change
	ld	a,c
	and	#0x7F
	call	_gbt_get_freq_from_index
	ld	hl,#gbt_freq+2 ; save frequency
	ld	(hl),e
	inc	hl
	ld	a,d
	and	#0x07
	or	#0x80
	ld	(hl),a
	ld	a,b
	sla	a
	sla a
	and	#0xC0
	ld	(gbt_instr+1),a ; save instrument
	bit 6,b
	jr	z,channel2_no_volumechange$
	bit 7,c
	jr	z,channel2_volumechange$
	; panning change
	xor	a
	bit 0,b
	jr	z,channel2_no_right$
	or	#0x02
channel2_no_right$:
	bit 1,b
	jr	z,channel2_no_left$
	or	#0x20
channel2_no_left$:
	ld	(gbt_pan + 1),a
	jr channel2_no_volumechange$
channel2_volumechange$:
	; volume change
	ld	a,b
	and	#0x0F
	swap	a
	ld	(gbt_vol + 1),a
channel2_no_volumechange$:
	ld	a,#0
	ldh	(#.NR24),a
	ld	a,(gbt_instr+1)
	ldh	(#.NR21),a ; instr
	ld	a,(gbt_vol+1)
	ldh	(#.NR22),a ; vol
	ld	a,(gbt_freq+2)
	ldh	(#.NR23),a ; freq low
	ld	a,(gbt_freq+3)
	ldh	(#.NR24),a ; freq hi
	jr	channel2_end$
channel2_ctrl$:
	; ----- bit 7 = 0 
	ld	a,b
	and	#0x07
	jr	z,channel2_end$ ; nothing
	dec	a
	jr	nz,channel2_not_jump$
	; jump
	ld	a,c
	ld	(gbt_current_pattern_order),a
	ld	a,#1
	ld	(_gbt_ignore_step_update),a
	jr	channel2_end$
channel2_not_jump$:
	dec	a
	jr	nz,channel2_not_break$
	; break
	ld	a,c
	ld	(gbt_current_step),a
	ld	a,(gbt_current_pattern_order)
	inc	a
	ld	(gbt_current_pattern_order),a
	ld	a,#1
	ld	(_gbt_ignore_step_update),a
	jr	channel2_end$
channel2_not_break$:
	dec	a
	jr	nz,channel2_not_speed$
	; speed
	ld	a,c
	ld  (gbt_speed),a
	xor	a
	ld	(gbt_ticks_elapsed),a
	jr	channel2_end$
channel2_not_speed$:
	dec	a
	jr	nz,channel2_not_vol$
	; volume
	ld	a,c
	and	#0x0F
	swap	a
	ld	(gbt_vol + 1),a
	ld	a,#0
	ldh	(#.NR24),a
	ld	a,(gbt_instr+1)
	ldh	(#.NR21),a ; instr
	ld	a,(gbt_vol+1)
	ldh	(#.NR22),a ; vol
	ld	a,(gbt_freq+2)
	ldh	(#.NR23),a ; freq low
	ld	a,(gbt_freq+3)
	ldh	(#.NR24),a ; freq hi
	jr	channel2_end$
channel2_not_vol$:
	dec	a
	jr	nz,channel2_not_pan$	
	; pan
	xor	a
	bit 0,c
	jr	z,__channel2_no_right$
	or	#0x02
__channel2_no_right$:
	bit 1,c
	jr	z,__channel2_no_left$
	or	#0x20
__channel2_no_left$:
	ld	(gbt_pan + 1),a
channel2_not_pan$: ; nothing else... next channel
channel2_end$:
	ret

; ------------------------------ channel 3 ------------------------------

gbt_channel_3_handle::
	
	ld	a,(gbt_channels_enabled)
	and	a,#0x04
	ret	z
	
	bit	7,b
	jr	z,channel3_ctrl$
	; ----- bit 7 = 1 -- frequency change
	ld	a,c
	and	#0x7F
	call	_gbt_get_freq_from_index
	ld	hl,#gbt_freq+4 ; save frequency
	ld	(hl),e
	inc	hl
	ld	a,d
	and	#0x07
	or	#0x80
	ld	(hl),a
	ld	a,b
	and	#0x07
	ld	(gbt_instr+2),a ; save instrument
	bit 6,b
	jr	z,channel3_no_volumechange$
	bit 5,b
	jr	z,channel3_volumechange$
	; panning change
	xor	a
	bit 3,b
	jr	z,channel3_no_right$
	or	#0x04
channel3_no_right$:
	bit 4,b
	jr	z,channel3_no_left$
	or	#0x40
channel3_no_left$:
	ld	(gbt_pan + 2),a
	jr channel3_no_volumechange$
channel3_volumechange$:
	; volume change
	ld	a,b
	sla	a
	sla	a
	and	#0x60
	ld	(gbt_vol + 2),a
channel3_no_volumechange$:
	ld	a,#0
	ldh	(#.NR30),a
	ldh	(#.NR34),a
	ld	a,(gbt_instr+2)
	push	bc
	swap	a
	and	#0x70
	ld	b,#0
	ld	c,a
	ld	hl,#gbt_wave
	add	hl,bc
	ld	b,#16
	ld	c,#0x30
continue_copy$:
	ld	a,(hl+)
	ldh	(c),a
	inc	c
	dec	b
	jr	nz,continue_copy$	
	pop	bc
	ld	a,(gbt_vol+2)
	ldh	(#.NR32),a ; vol
	ld	a,#0x80
	ldh	(#.NR30),a
	ld	a,#0xFF
	ldh	(#.NR31),a ; len??
	ld	a,(gbt_freq+4)
	ldh	(#.NR33),a ; freq low
	ld	a,(gbt_freq+5)
	ldh	(#.NR34),a ; freq hi
	jr	channel3_end$
channel3_ctrl$:
	; ----- bit 7 = 0 
	ld	a,b
	and	#0x07
	jr	z,channel3_end$ ; nothing
	dec	a
	jr	nz,channel3_not_jump$
	; jump
	ld	a,c
	ld	(gbt_current_pattern_order),a
	ld	a,#1
	ld	(_gbt_ignore_step_update),a
	jr	channel3_end$
channel3_not_jump$:
	dec	a
	jr	nz,channel3_not_break$
	; break
	ld	a,c
	ld	(gbt_current_step),a
	ld	a,(gbt_current_pattern_order)
	inc	a
	ld	(gbt_current_pattern_order),a
	ld	a,#1
	ld	(_gbt_ignore_step_update),a
	jr	channel3_end$
channel3_not_break$:
	dec	a
	jr	nz,channel3_not_speed$
	; speed
	ld	a,c
	ld  (gbt_speed),a
	xor	a
	ld	(gbt_ticks_elapsed),a
	jr	channel3_end$
channel3_not_speed$:
	dec	a
	jr	nz,channel3_not_vol$
	; volume
	ld	a,c
	swap	a
	sla	a
	and	#0x60
	ld	(gbt_vol + 2),a
	ld	a,#0
	ldh	(#.NR30),a
	ldh	(#.NR34),a
	ld	a,(gbt_vol+2)
	ldh	(#.NR32),a ; vol
	ld	a,#0x80
	ldh	(#.NR30),a
	ld	a,#0xFF
	ldh	(#.NR31),a ; len??
	ld	a,(gbt_freq+4)
	ldh	(#.NR33),a ; freq low
	ld	a,(gbt_freq+5)
	ldh	(#.NR34),a ; freq hi
	jr	channel3_end$
channel3_not_vol$:
	dec	a
	jr	nz,channel3_not_pan$	
	; pan
	xor	a
	bit 0,c
	jr	z,__channel3_no_right$
	or	#0x04
__channel3_no_right$:
	bit 1,c
	jr	z,__channel3_no_left$
	or	#0x40
__channel3_no_left$:
	ld	(gbt_pan + 2),a
channel3_not_pan$: ; nothing else... next channel
channel3_end$:
	ret

; ------------------------------ channel 4 ------------------------------

gbt_channel_4_handle::
	
	ld	a,(gbt_channels_enabled)
	and	a,#0x08
	ret	z
	
	bit	7,b
	jr	z,channel4_ctrl$
	; ----- bit 7 = 1 -- instrument change	
	ld	a,c
	and	#0x0F
	ld	hl,#gbt_noise
	ld	d,#0
	ld	e,a
	add	hl,de
	ld	a,(hl)
	ld	(gbt_instr+3),a ; save instrument
	bit 6,b
	jr	z,channel4_no_volumechange$
	bit 7,c
	jr	z,channel4_volumechange$
	; panning change
	xor	a
	bit 0,b
	jr	z,channel4_no_right$
	or	#0x08
channel4_no_right$:
	bit 1,b
	jr	z,channel4_no_left$
	or	#0x80
channel4_no_left$:
	ld	(gbt_pan + 3),a
	jr channel4_no_volumechange$
channel4_volumechange$:
	; volume change
	ld	a,b
	and	#0x0F
	swap	a
	ld	(gbt_vol + 3),a
channel4_no_volumechange$:
	ld	a,#0
	ldh	(#.NR44),a
	ld	a,(gbt_vol+3)
	ldh	(#.NR42),a ; vol
	ld	a,(gbt_instr+3)
	ldh	(#.NR43),a
	ld	a,#0x80
	ldh	(#.NR44),a
	jr	channel4_end$
channel4_ctrl$:
	; ----- bit 7 = 0 
	ld	a,b
	and	#0x07
	jr	z,channel4_end$ ; nothing
	dec	a
	jr	nz,channel4_not_jump$
	; jump
	ld	a,c
	ld	(gbt_current_pattern_order),a
	ld	a,#1
	ld	(_gbt_ignore_step_update),a
	jr	channel4_end$
channel4_not_jump$:
	dec	a
	jr	nz,channel4_not_break$
	; break
	ld	a,c
	ld	(gbt_current_step),a
	ld	a,(gbt_current_pattern_order)
	inc	a
	ld	(gbt_current_pattern_order),a
	ld	a,#1
	ld	(_gbt_ignore_step_update),a
	jr	channel4_end$
channel4_not_break$:
	dec	a
	jr	nz,channel4_not_speed$
	; speed
	ld	a,c
	ld  (gbt_speed),a
	xor	a
	ld	(gbt_ticks_elapsed),a
	jr	channel4_end$
channel4_not_speed$:
	dec	a
	jr	nz,channel4_not_vol$
	; volume
	ld	a,c
	and	#0x0F
	swap	a
	ld	(gbt_vol + 3),a
	ld	a,#0
	ldh	(#.NR44),a
	ld	a,(gbt_vol+3)
	ldh	(#.NR42),a ; vol
	ld	a,(gbt_instr+3)
	ldh	(#.NR43),a
	ld	a,#0x80
	ldh	(#.NR44),a
	jr	channel4_end$
channel4_not_vol$:
	dec	a
	jr	nz,channel4_not_pan$	
	; pan
	xor	a
	bit 0,c
	jr	z,__channel4_no_right$
	or	#0x08
__channel4_no_right$:
	bit 1,c
	jr	z,__channel4_no_left$
	or	#0x80
__channel4_no_left$:
	ld	(gbt_pan + 3),a
channel4_not_pan$: ; nothing else...
channel4_end$:
	ret
	
; -----------------------------------------------------------------------

gbt_update_bank1::
	
	xor	a
	ld	(_gbt_ignore_step_update),a
	
	ld	hl,#gbt_play_data
	; ----- channel 1
	ld	b,(hl)
	inc	hl
	ld	c,(hl)
	inc	hl
	push	hl
	call	gbt_channel_1_handle
	pop	hl
	; ----- channel 2
	ld	b,(hl)
	inc	hl
	ld	c,(hl)
	inc	hl
	push	hl
	call	gbt_channel_2_handle
	pop	hl
	; ----- channel 3
	ld	b,(hl)
	inc	hl
	ld	c,(hl)
	inc	hl
	push	hl
	call	gbt_channel_3_handle
	pop	hl
	; ----- channel 4
	ld	b,(hl)
	inc	hl
	ld	c,(hl)
	push	hl
	call	gbt_channel_4_handle
	pop	hl
	
	; end of channel handling
	
	ld	hl,#gbt_pan
	ld	a,(hl+)
	or	(hl)
	inc	hl
	or	(hl)
	inc hl
	or (hl)
	ldh	(#.NR51),a ; handle panning...
	
	; update step / pattern
	
	ld	a,(_gbt_ignore_step_update)
	or	a
	;jr	nz,exit$
	ret	nz
	
	ld	a,(gbt_current_step)
	cp	#63
	jr	z,update_pattern$
	inc	a
	ld	(gbt_current_step),a
	jr	exit$
	
update_pattern$:
	xor	a
	ld	(gbt_current_step),a
	ld	a,(gbt_current_pattern_order)
	inc	a
	ld	(gbt_current_pattern_order),a ; we check in bank 0 if finished, not here
exit$:

	ret
	



