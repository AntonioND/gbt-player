// GBT Player v4.2.0
//
// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

#include <gba.h>
#include <maxmod.h>

#include "gbt_player.h"

#include "soundbank.h"
#include "soundbank_bin.h"

extern const uint8_t *template_combined_psg[];

void gbt_sync_to_maxmod(void)
{
    while (1)
    {
        gbt_update();

        int order, row, tick;
        gbt_get_position(&order, &row, &tick);

        if (tick == mmGetPositionTick())
            break;
    }
}

int main(int argc, char *argv[])
{
    irqInit();

    irqSet(IRQ_VBLANK, mmVBlank);
    irqEnable(IRQ_VBLANK);

    // Initialize maxmod with soundbank and 4 channels
    mmInitDefault((mm_addr)soundbank_bin, 4);
    // PSG channels have 1/4th of the range of DMA channels
    mmSetModuleVolume(1024 / 4);

    // Start both songs
    gbt_play(template_combined_psg, -1);
    gbt_loop(1);
    mmStart(MOD_TEMPLATE_COMBINED_DMA, MM_PLAY_LOOP);

    while (1)
    {
        VBlankIntrWait();
        mmFrame();
        gbt_sync_to_maxmod();
    }
}
