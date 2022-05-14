// GBT Player v4.2.0
//
// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2022 Antonio Niño Díaz <antonio_nd@outlook.com>

#ifndef GBT_PLAYER_H__
#define GBT_PLAYER_H__

// Starts playing a song. The speed is only used for MOD songs. S3M songs
// include their own startup speed, so this value is ignored. If the speed is 0,
// it acts as if it was 256.
void gbt_play(const void *song, int speed);

// Pauses or unpauses the song. 0 pauses the song, anything else unpauses it.
void gbt_pause(int play);

// Enables/disables looping at the end of the song. 0 stops playback when the
// end of the song is reached. Anything else will loop to the beginning.
void gbt_loop(int loop);

// Stops the song.
void gbt_stop(void);

#define GBT_VOLUME_MAX      8

// Sets volume for left and right speakers. Valid values are 0 to 8.
void gbt_volume(unsigned int left, unsigned int right);

// Defines to specify which channels to enable
#define GBT_ENABLE_CH1      (1 << 0)
#define GBT_ENABLE_CH2      (1 << 1)
#define GBT_ENABLE_CH3      (1 << 2)
#define GBT_ENABLE_CH4      (1 << 3)

#define GBT_ENABLE_CH_ALL   (GBT_ENABLE_CH1 | GBT_ENABLE_CH2 | \
                             GBT_ENABLE_CH3 | GBT_ENABLE_CH4)

// Enables given channels. For example: GBT_ENABLE_CH1 | GBT_ENABLE_CH2
void gbt_enable_channels(int flags);

// Updates the player, must be called each VBL.
void gbt_update(void);

// Returns the current pattern order and current row inside the pattern, as well
// as the current tick. It is possible to pass NULL as argument to tick if you
// don't need that value.
void gbt_get_position(int *order, int *row, int *tick);

// Sets the current position in the song. This isn't normally meant to happen,
// so some effects (like arpeggio) may be broken right after the jump until they
// are restarted.
void gbt_set_position(int order, int row);

#endif // GBT_PLAYER_H__
