// GBT Player v3.0.9
//
// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2020, Antonio Niño Díaz <antonio_nd@outlook.com>

#ifndef GBT_PLAYER_H__
#define GBT_PLAYER_H__

// Starts playing a song. If the speed is 0, it acts as if it was 256.
void gbt_play(const void *song, int speed);

//Pauses or unpauses the song. 0 to pause, anything else to unpause.
void gbt_pause(int play);

// Enables/disables looping at the end of the song. 0 to stop at the end,
// anything else to loop.
void gbt_loop(int loop);

// Stops the song.
void gbt_stop(void);

// Enables given channels.
// a = channel flags ORed:
//         channel 1 = 1
//         channel 2 = 2
//         channel 3 = 4
//         channel 4 = 8
void gbt_enable_channels(int flags);

//Updates the player, must be called each VBL.
void gbt_update(void);

#endif // GBT_PLAYER_H__
