// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

// Some definitions in gbt_hardware.h conflict with gba.h. The routines below
// have been taken out of main.c because gba.h is needed in that file.

#include "gbt_player/gbt_hardware.h"

void sfx_channel2_left(void)
{
    uint16_t mask = SOUNDCNT_L_PSG_2_ENABLE_RIGHT
                    | SOUNDCNT_L_PSG_2_ENABLE_LEFT;

    REG_SOUNDCNT_L &= ~mask;
    REG_SOUNDCNT_L |= SOUNDCNT_L_PSG_2_ENABLE_LEFT;

    REG_SOUND2CNT_L = SOUND2CNT_L_WAVE_DUTY_12
                    | SOUND2CNT_L_ENV_STEP_TIME_SET(4)
                    | SOUND2CNT_L_ENV_DIR_DEC
                    | SOUND2CNT_L_ENV_VOLUME_SET(15);

    REG_SOUND2CNT_H = SOUND2CNT_H_FREQUENCY_SET(1500)
                    | SOUND2CNT_H_RESTART;
}

void sfx_channel2_right(void)
{
    uint16_t mask = SOUNDCNT_L_PSG_2_ENABLE_RIGHT
                    | SOUNDCNT_L_PSG_2_ENABLE_LEFT;

    REG_SOUNDCNT_L &= ~mask;
    REG_SOUNDCNT_L |= SOUNDCNT_L_PSG_2_ENABLE_RIGHT;

    REG_SOUND2CNT_L = SOUND2CNT_L_WAVE_DUTY_12
                    | SOUND2CNT_L_ENV_STEP_TIME_SET(4)
                    | SOUND2CNT_L_ENV_DIR_DEC
                    | SOUND2CNT_L_ENV_VOLUME_SET(15);

    REG_SOUND2CNT_H = SOUND2CNT_H_FREQUENCY_SET(1500)
                    | SOUND2CNT_H_RESTART;
}

void sfx_channel2_silence(void)
{
    REG_SOUND2CNT_L = SOUND2CNT_L_ENV_VOLUME_SET(0);
    REG_SOUND2CNT_H = SOUND2CNT_H_RESTART;
}
