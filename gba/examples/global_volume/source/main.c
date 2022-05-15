// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

// Example that shows how to set the global volume of PSG channels.

#include <stdio.h>

#include <gba.h>

#include "gbt_player/gbt_player.h"

extern const uint8_t *template[];

int main(int argc, char *argv[])
{
    irqInit();
    irqSet(IRQ_VBLANK, gbt_update);
    irqEnable(IRQ_VBLANK);

    consoleDemoInit();

    iprintf("\n");
    iprintf("\n");
    iprintf("Up:   Volume right +\n");
    iprintf("Down: Volume right -\n");
    iprintf("L:    Volume left -\n");
    iprintf("R:    Volume left +\n");

    int left = GBT_VOLUME_MAX;
    int right = GBT_VOLUME_MAX;

    gbt_play(template, -1);

    while (1)
    {
        VBlankIntrWait();

        scanKeys();
        int keys = keysDown();

        iprintf("\x1b[0;0HVolume | Left: %d | Right: %d", left, right);

        if (keys & KEY_UP)
        {
            if (right < GBT_VOLUME_MAX)
                right++;
        }
        if (keys & KEY_DOWN)
        {
            if (right > 0)
                right--;
        }

        if (keys & KEY_R)
        {
            if (left < GBT_VOLUME_MAX)
                left++;
        }
        if (keys & KEY_L)
        {
            if (left > 0)
                left--;
        }

        gbt_volume(left, right);
    }
}
