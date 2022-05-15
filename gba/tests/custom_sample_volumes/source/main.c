// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

// This example uses a S3M file with an instrument that doesn't have a default
// volume of 64. The default volume of an instrument is used when you write a
// note, an instrument, but leave the volume field empty. If you try to delete
// it in a tracker such as OpenMPT, you won't be able to do it, and the default
// volume of the sample will show up.

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
