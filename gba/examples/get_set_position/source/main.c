// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

// This example shows how to set and get the position of the current song.

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
    iprintf("A: Jump to order 2, row 32\n");
    iprintf("B: Jump to order 1, row 0\n");
    iprintf("START: Jump to start\n");

    gbt_play(template, -1);

    while (1)
    {
        VBlankIntrWait();

        scanKeys();
        int keys = keysDown();

        if (keys & KEY_A)
        {
            gbt_set_position(2, 32);
        }
        if (keys & KEY_B)
        {
            gbt_set_position(1, 0);
        }
        if (keys & KEY_START)
        {
            gbt_set_position(0, 0);
        }

        int order, row, tick;
        gbt_get_position(&order, &row, &tick);
        iprintf("\x1b[0;0HOrder %d | Row %2d | Tick %d", order, row, tick);
    }
}

