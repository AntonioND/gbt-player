/*
 * GBT Player v2.1.2
 *
 * Copyright (c) 2009-2018, Antonio Niño Díaz <antonio_nd@outlook.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *  list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES * LOSS OF USE, DATA, OR PROFITS * OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _GBT_PLAYER_
#define _GBT_PLAYER_

#include <gb/gb.h>

// Plays the song pointed by data (pointer array to patterns) in given bank at
// given initial speed.
void gbt_play(void *data, UINT8 bank, UINT8 speed);

// Pauses or unpauses music.
void gbt_pause(UINT8 pause);

// Stops music and turns off sound system. Called automatically when the last
// pattern ends and autoloop isn't activated.
void gbt_stop(void);

// Enables or disables autoloop
void gbt_loop(UINT8 loop);

// Updates player, should be called every frame.
// NOTE: This will change the active ROM bank to 1.
void gbt_update(void);

// Set enabled channels to prevent the player from using that channel.
// NOTE: If a channel is re-enabled, it can take some time to sound OK (until
// pan and volume are modified in the song). You should only disable unused
// channels or channels that don't change pan or volume.
void gbt_enable_channels(UINT8 channel_flags);

#define GBT_CHAN_1 (1<<0)
#define GBT_CHAN_2 (1<<1)
#define GBT_CHAN_3 (1<<2)
#define GBT_CHAN_4 (1<<3)

#endif //_GBT_PLAYER_
