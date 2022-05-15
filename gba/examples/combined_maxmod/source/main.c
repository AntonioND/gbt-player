// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

// Example that shows how to play a song with Maxmod and GBT Player at the same
// time and keep them in sync.

#include <gba.h>
#include <maxmod.h>

#include "gbt_player/gbt_player.h"

#include "soundbank.h"
#include "soundbank_bin.h"

extern const uint8_t *template_combined_psg[];

void gbt_sync_to_maxmod(void)
{
    int tries = 5;

    while (tries > 0)
    {
        tries--;

        gbt_update();

        int tick;
        gbt_get_position(NULL, NULL, &tick);

        if (tick == mmGetPositionTick())
            break;
    }
}

void vbl_handler(void)
{
    mmVBlank(); // This has to be called exactly at the beginning of VBL

    // This can be called in the VBL handler or outside
    mmFrame();
    gbt_sync_to_maxmod();
}

int main(int argc, char *argv[])
{
    irqInit();

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
