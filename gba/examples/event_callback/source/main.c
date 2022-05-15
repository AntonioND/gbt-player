// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

// Example that shows how to handle event callbacks in GBT Player.

#include <stdio.h>

#include <gba.h>

#include "gbt_player/gbt_player.h"

extern const uint8_t *template[];

void callback(int event, int order, int row)
{
    if (event == GBT_EVENT_END)
        iprintf("Song end\n");
    else
        iprintf("Event %X | %d:%d\n", event, order, row);
}

int main(int argc, char *argv[])
{
    irqInit();
    irqSet(IRQ_VBLANK, gbt_update);
    irqEnable(IRQ_VBLANK);

    gbt_play(template, -1);
    gbt_set_event_callback_handler(callback);

    consoleDemoInit();

    while (1)
        VBlankIntrWait();
}
