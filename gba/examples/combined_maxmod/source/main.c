// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

// Example that shows how to play a song with Maxmod and GBT Player at the same
// time and keep them in sync.

#include <stdio.h>

#include <gba.h>
#include <maxmod.h>

#include "gbt_player/gbt_player.h"

#include "soundbank.h"
#include "soundbank_bin.h"

extern const uint8_t *template_combined_psg[];

void gbt_sync_to_maxmod(void)
{
    if (!gbt_is_playing())
        return;

    if (!mmActive())
        return;

    int tries = 5;

    while (tries > 0)
    {
        tries--;

        gbt_update();

        int order, row, tick;
        gbt_get_position(&order, &row, &tick);

        if (order != mmGetPosition())
            continue;

        if (row != mmGetPositionRow())
            continue;

        if (tick != mmGetPositionTick())
            continue;

        break;
    }

    // Something went wrong somehow!
}

void vbl_handler(void)
{
    mmVBlank(); // This has to be called exactly at the beginning of VBL

    // This must be called in the VBL handler, but with lower priority
    mmFrame();
    gbt_sync_to_maxmod();

    // Print some debug information
    int order, row, tick;
    gbt_get_position(&order, &row, &tick);
    iprintf("(%s) %d %2d %d | (%s) %d %2d %d\n",
            gbt_is_playing() ? "ON" : "OFF",
            order, row, tick,
            mmActive() ? "ON" : "OFF",
            mmGetPosition(), mmGetPositionRow(), mmGetPositionTick());
}

int main(int argc, char *argv[])
{
    irqInit();

    consoleDemoInit();

    irqSet(IRQ_VBLANK, vbl_handler);
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
        VBlankIntrWait();
}
