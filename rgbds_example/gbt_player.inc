
;        --------------------------------------------------------------
;        ---                                                        ---
;        ---                                                        ---
;        ---                       GBT PLAYER  3.0.2                ---
;        ---                                                        ---
;        ---                                                        ---
;        ---              Copyright (C) 2009-2015 Antonio Niño Díaz ---
;        ---                      All rights reserved.              ---
;        --------------------------------------------------------------
;
;                                          antonio_nd@outlook.com

    IF !DEF(GBT_PLAYER_INC)
GBT_PLAYER_INC SET 1

; ------------------------------------------------------------------------
    GLOBAL  gbt_play ; Starts playing a song.
                     ; ---------------------------------------------------
                     ; de = pointer to song data
                     ; a = default speed
                     ; bc = data bank (b ignored if ROM with < 256 banks)
                     ; THIS WILL CHANGE ROM BANK!!!
; ------------------------------------------------------------------------
    GLOBAL  gbt_pause ; Pauses or unpauses the song.
                      ; --------------------------------------------------
                      ; a = 0 to unpause, anything else to pause.
; ------------------------------------------------------------------------
    GLOBAL  gbt_loop ; Enables/disables looping at the end of the song.
                     ; ---------------------------------------------------
                     ; a = 0 to stop at the end, anything else to loop
; ------------------------------------------------------------------------
    GLOBAL  gbt_stop   ; Stops the song.
; ------------------------------------------------------------------------
    GLOBAL  gbt_enable_channels ; Enables given channels.
                                ; ----------------------------------------
                                ; a = channel flags ORed:
                                ;         channel 1 = 1
                                ;         channel 2 = 2
                                ;         channel 3 = 4
                                ;         channel 4 = 8
; ------------------------------------------------------------------------
    GLOBAL  gbt_update  ; Updates the player, must be called each VBL.
                        ; THIS WILL CHANGE ROM BANK!!!
; ------------------------------------------------------------------------
; - If the following value is uncomented, the total of banks allowed is 512
; (or more), but it's a bit slower. MBC5 ONLY, DOESN'T WORK WITH OTHERS!!!
; YOU MUST USE THE -512-banks OPTION WHEN CONVERTING A SONG WITH mod2gbt!!!
; - If it's commented, only 256 banks are allowed, it's a little bit faster
; and saves a few bytes. MBC1, MBC3 and MBC5 (and others).
;GBT_USE_MBC5_512BANKS SET 1
; -----------------------------------------------------------------------

    ENDC ;GBT_PLAYER_INC

; -----------------------------------------------------------------------

