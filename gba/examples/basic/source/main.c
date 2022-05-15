// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

// Example that shows how to play a song.

#include <gba.h>

#include "gbt_player/gbt_player.h"

extern const uint8_t *template[];

int main(int argc, char *argv[])
{
    irqInit();
    irqSet(IRQ_VBLANK, gbt_update);
    irqEnable(IRQ_VBLANK);

    gbt_play(template, -1);

    while (1)
        VBlankIntrWait();
}
