// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

// Example that shows how to play sound effects while GBT Player is playing a
// song. It does that by disabling the channel in GBT Player.

#include <stdio.h>

#include <gba.h>

#include "gbt_player/gbt_player.h"
#include "sfx.h"

extern const uint8_t *template[];

int main(int argc, char *argv[])
{
    irqInit();
    irqSet(IRQ_VBLANK, gbt_update);
    irqEnable(IRQ_VBLANK);

    consoleDemoInit();

    iprintf("Channel 2 enabled \n");
    iprintf("\n");
    iprintf("A: Enable channel 2 in GBT\n");
    iprintf("B: Disable channel 2 in GBT\n");
    iprintf("L: Play sound on left side\n");
    iprintf("R: Play sound on right side\n");
    iprintf("START: Start song\n");
    iprintf("SELECT: Stop song\n");
    iprintf("\n");
    iprintf("If the sound is played before\n");
    iprintf("disabling channel 2, it will\n");
    iprintf("be interrupted by the song\n");
    iprintf("playback.");

    while (1)
    {
        scanKeys();
        int keys = keysDown();

        if (keys & KEY_A)
        {
            gbt_enable_channels(GBT_ENABLE_CH_ALL);

            iprintf("\x1b[0;0HChannel 2 enabled ");
        }

        if (keys & KEY_B)
        {
            gbt_enable_channels(GBT_ENABLE_CH_ALL & ~GBT_ENABLE_CH2);
            // Disabling a channel will leave it playing whatever note was being
            // played before disabling it. It is needed to silence it manually.
            sfx_channel2_silence();

            iprintf("\x1b[0;0HChannel 2 disabled");
        }

        if (keys & KEY_L)
            sfx_channel2_left();

        if (keys & KEY_R)
            sfx_channel2_right();

        if (keys & KEY_START)
            gbt_play(template, -1);

        if (keys & KEY_SELECT)
            gbt_stop();

        VBlankIntrWait();
    }
}
