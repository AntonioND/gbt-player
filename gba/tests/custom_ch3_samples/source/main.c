// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

// This example uses a S3M file with modified channel 3 instruments that will
// get exported with the rest of the song. GBT Player will load the modified
// instruments on the GBA. Only instruments with 32 or 64 samples can be
// exported.
//
// Remember to use the ``--instruments`` flag when using ``s3m2gbt.py``.

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
