// GBT Player v4.1.0
//
// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

#include <gba.h>

#include "gbt_player.h"

extern const uint8_t *template[];

int main(int argc, char *argv[])
{
    irqInit();
    irqEnable(IRQ_VBLANK);

    gbt_play(template, -1);

    while (1)
    {
        VBlankIntrWait();
        gbt_update();
    }
}
