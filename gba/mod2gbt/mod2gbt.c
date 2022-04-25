/*
 * mod2gbt v4.0.0 (Part of GBT Player)
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2009-2022 Antonio Niño Díaz <antonio_nd@outlook.com>
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int perform_speed_conversion = 1;

#define abs(x) (((x) > 0) ? (x) : -(x))
#define BIT(n) (1 << (n))

//------------------------------------------------------------------------------
//--                                                                          --
//--                           Read MOD file                                  --
//--                                                                          --
//------------------------------------------------------------------------------

typedef struct __attribute__((packed)) {
    char        name[22];
    uint16_t    length;
    uint8_t     finetune; // 4 lower bits
    uint8_t     volume; // 0-64
    uint16_t    repeat_point;
    uint16_t    repeat_length; // Loop if length > 1
} _sample_t;

typedef struct __attribute__((packed)) {
    uint8_t     info[64][4][4]; // [step][channel][byte]
} _pattern_t;

typedef struct __attribute__((packed)) {
    char        name[20];
    _sample_t   sample[31];
    uint8_t     song_length; // Length in patterns
    uint8_t     unused; // Set to 127, used by Noisetracker
    uint8_t     pattern_table[128]; //0..63
    char        identifier[4];
    // Only 64 patterns allowed (see pattern_table) but set to 256 anyway...
    _pattern_t  pattern[256];
    // Followed by sample data, unused by the converter
} mod_file_t;

//------------------------------------------------------------------------------

void *load_file(const char *filename)
{
    unsigned int size;
    void *buffer = NULL;
    FILE *datafile = fopen(filename, "rb");

    if (datafile == NULL)
    {
        printf("ERROR: %s couldn't be opened!\n", filename);
        return NULL;
    }

    fseek(datafile, 0, SEEK_END);
    size = ftell(datafile);
    if (size == 0)
    {
        printf("ERROR: Size of %s is 0!\n", filename);
        fclose(datafile);
        return NULL;
    }

    rewind(datafile);
    buffer = malloc(size);
    if (buffer == NULL)
    {
        printf("ERROR: Not enought memory to load %s!\n", filename);
        fclose(datafile);
        return NULL;
    }

    if (fread(buffer, size, 1, datafile) != 1)
    {
        printf("ERROR: Error while reading.\n");
        fclose(datafile);
        free(buffer);
        return NULL;
    }

    fclose(datafile);

    return buffer;
}

//------------------------------------------------------------------------------

void unpack_info(uint8_t *info, uint8_t *sample_num, uint16_t *sample_period,
                 uint8_t *effect_num, uint8_t *effect_param)
{
    *sample_num    = (info[0] & 0xF0) | ((info[2] & 0xF0) >> 4);
    *sample_period =  info[1]         | ((info[0] & 0x0F) << 8);
    *effect_num    =  info[2] & 0x0F;
    *effect_param  =  info[3];
}

const uint16_t mod_period[6 * 12] = {
    1712,1616,1524,1440,1356,1280,1208,1140,1076,1016, 960, 907,
     856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453,
     428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226,
     214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113,
     107, 101,  95,  90,  85,  80,  75,  71,  67,  63,  60,  56,
      53,  50,  47,  45,  42,  40,  37,  35,  33,  31,  30,  28
};

uint8_t mod_get_index_from_period(uint16_t period, int pattern, int step, int channel)
{
    if (period > 0)
    {
        if (period < mod_period[(6 * 12) - 1])
        {
            if (channel != 4) // Pitch ignored for noise channel
            {
                printf("\nWARNING: Pattern %d, Step %d, Channel %d. Note too high!\n",
                       pattern, step, channel);
            }
        }
        else if (period > mod_period[0])
        {
            if (channel != 4) // Pitch ignored for noise channel
            {
                printf("\nWARNING: Pattern %d, Step %d, Channel %d. Note too low!\n",
                       pattern, step, channel);
            }
        }
    }
    else
    {
        return -1;
    }

    for (int i = 0; i < 6 * 12; i++)
        if (period == mod_period[i])
            return i;

    // Couldn't find exact match... get nearest value

    uint16_t nearest_value = 0xFFFF;
    uint8_t nearest_index = 0;
    for (int i = 0; i < 6 * 12; i++)
    {
        int test_distance = abs(((int)period) - ((int)mod_period[i]));
        int nearest_distance = abs(((int)period) - nearest_value);

        if (test_distance < nearest_distance)
        {
            nearest_value = mod_period[i];
            nearest_index = i;
        }
    }

    return nearest_index;
}

//------------------------------------------------------------------------------
//--                                                                          --
//--                           Save output                                    --
//--                                                                          --
//------------------------------------------------------------------------------

FILE *output_file;
char label_name[64];

void out_open(const char *filename)
{
    output_file = fopen(filename, "w");
}

void out_write_str(const char *asm_str)
{
    fprintf(output_file, "%s", asm_str);
}

void out_write_dec(uint8_t number)
{
    fprintf(output_file, "%d", number);
}

void out_write_hex(uint8_t number)
{
    fprintf(output_file, "0x%02X", number);
}

void out_close(void)
{
    fclose(output_file);
}

//------------------------------------------------------------------------------
//--                                                                          --
//--                          Save to Game Boy                                --
//--                                                                          --
//------------------------------------------------------------------------------

/*
SAMPLE PERIOD LUT - MOD values
           C    C#   D    D#   E    F    F#   G    G#   A    A#   B
Octave 0:1712,1616,1525,1440,1357,1281,1209,1141,1077,1017, 961, 907 // C3 to B3
Octave 1: 856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453 // C4 to B4
Octave 2: 428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226 // C5 to B5
Octave 3: 214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113 // C6 to B6
Octave 4: 107, 101,  95,  90,  85,  80,  76,  71,  67,  64,  60,  57 // C7 to B7
Octave 5:  53,  50,  47,  45,  42,  40,  37,  35,  33,  31,  30,  28 // C8 to B8

//From C3 to B8  |  A5 = 1750 = 440.00Hz  |  C5 = 1546
const UWORD GB_frequencies[] = {
      44, 156, 262, 363, 457, 547, 631, 710, 786, 854, 923, 986, // C3 to B3
    1046,1102,1155,1205,1253,1297,1339,1379,1417,1452,1486,1517, // C4 to B4
    1546,1575,1602,1627,1650,1673,1694,1714,1732,1750,1767,1783, // C5 to B5
    1798,1812,1825,1837,1849,1860,1871,1881,1890,1899,1907,1915, // C6 to B6
    1923,1930,1936,1943,1949,1954,1959,1964,1969,1974,1978,1982, // C7 to B7
    1985,1988,1992,1995,1998,2001,2004,2006,2009,2011,2013,2015  // C8 to B8
};

That means... MOD C0 (period 1712) = GB C3 (freq 44, index 0)
Anyway, they don't sound the same...
*/

int volume_mod_to_gb(int v) // Channels 1,2,4
{
    return (v >= 64) ? 0xF : (v >> 2);
}

int volume_mod_to_gb_ch3(int v) // Channel 3
{
    int vol = volume_mod_to_gb(v);

    switch (vol)
    {
        case 0: case 1: case 2: case 3:
            return 0; // 0%

        case 4: case 5: case 6:
            return 3; // 25%

        case 7: case 8: case 9:
            return 2; // 50%

        case 10: case 11: case 12:
            return 4; // 75%

        default:
        case 13: case 14: case 15:
            return 1; // 100%
    }

    return 0;
}

int speed_mod_to_gb(int s)
{
    if (perform_speed_conversion) // Amiga's 50 Hz to GB's 60 Hz
        return (s * 60) / 50;
    else
        return s;
}

// Returns 1 if ok
int effect_mod_to_gb(uint8_t pattern_number, uint8_t step_number,
                     uint8_t channel, uint8_t effectnum, uint8_t effectparams,
                     uint8_t *converted_num, uint8_t *converted_params)
{
    switch (effectnum)
    {
        case 0x0: // Arpeggio
        {
            *converted_num = 1;
            *converted_params = effectparams;
            return 1;
        }
        case 0xB: // Jump to pattern
        {
            *converted_num = 8;
            *converted_params = effectparams;
            // TODO: Fail if this jumps out of bounds
            return 1;
        }
        case 0xC: // Volume -> Not handled here
        {
            printf("ERROR: Strange error at pattern %d, step %d, channel %d: "
                   "%01X%02X\n", pattern_number, step_number, channel,
                   effectnum, effectparams);
            return 0;
        }
        case 0xD: // Break + Set step
        {
            *converted_num = 9; // Effect value is BCD, convert to integer
            *converted_params = (((effectparams & 0xF0) >> 4) * 10)
                              +   (effectparams & 0x0F);
            return 1;
        }
        case 0xE:
        {
            if ((effectparams & 0xF0) == 0x80) // Pan
            {
                uint8_t left = 0;
                uint8_t right = 0;

                switch (effectparams & 0xF)
                {
                    case 0: case 1: case 2: case 3:
                        left = 1;
                        break;

                    default:
                    case 4: case 5: case 6: case 7:
                    case 8: case 9: case 10: case 11:
                        left = 1;
                        right = 1;
                        break;

                    case 12: case 13: case 14: case 15:
                        right = 1;
                        break;
                }
                *converted_num = 0;
                *converted_params = (left << (3 + channel))
                                  | (right << (channel - 1)); // Channel 1-4
                return 1;
            }
            if ((effectparams & 0xF0) == 0xC0) // Cut note
            {
                *converted_num = 2;
                *converted_params = (effectparams & 0xF);
                return 1;
            }
            else // Error
            {
                printf("WARNING: Unsupported effect at pattern %d, step %d, channel %d: "
                       "%01X%02X\n", pattern_number, step_number, channel,
                       effectnum, effectparams);
                return 0;
            }
            break;
        }
        case 0xF: // Speed
        {
            if (effectparams > 0x1F) // Nothing
            {
                printf("WARNING: Unsupported BPM speed effect at pattern %d, step %d, "
                       "channel %d: %01X%02X\n", pattern_number, step_number,
                       channel, effectnum, effectparams);
                return 0;
            }
            else // Speed
            {
                *converted_num = 10;
                *converted_params = speed_mod_to_gb(effectparams);
                return 1;
            }
            break;
        }
        default: // Nothing
        {
            printf("WARNING: Unsupported effect at pattern %d, step %d, channel %d: "
                   "%01X%02X\n", pattern_number, step_number, channel,
                   effectnum, effectparams);
            return 0;
        }
    }
    return 0;
}

#define HAS_VOLUME      BIT(4)
#define HAS_INSTRUMENT  BIT(5)
#define HAS_EFFECT      BIT(6)
#define HAS_NOTE        BIT(7)
#define HAS_KIT         BIT(7)

void convert_channel1(uint8_t pattern_number, uint8_t step_number,
                      uint8_t note_index, uint8_t samplenum, uint8_t effectnum,
                      uint8_t effectparams)
{
    uint8_t command[4] = { 0, 0, 0, 0 }; // NOP
    int command_ptr = 1;

    int volume = -1;

    // If the effect is "Set Volume", handle it before and clear it so that it
    // isn't handled later.
    if (effectnum == 0xC)
    {
        volume = effectparams;
        effectnum = 0;
        effectparams = 0;
    }

    // Check if it's needed to add a note
    if (note_index <= (6 * 12 - 1))
    {
        command[0] |= HAS_NOTE;
        command[command_ptr] = note_index & 0x7F;
        command_ptr++;

        // If a note is set with no volume, set volume to the max
        // TODO: This should take the volume from the sample volume
        if (volume == -1)
            volume = 64;
    }

    // Check if there is a sample defined
    if (samplenum > 0)
    {
        uint32_t instrument = samplenum & 3;

        command[0] |= HAS_INSTRUMENT;
        command[command_ptr] = (instrument << 4) & 0x30;
    }

    if ((effectnum != 0) || (effectparams != 0))
    {
        uint8_t converted_num, converted_params;
        if (effect_mod_to_gb(pattern_number, step_number, 1, effectnum,
                             effectparams, &converted_num,
                             &converted_params))
        {
            command[0] |= HAS_EFFECT;
            command[command_ptr] |= converted_num & 0x0F;
            command_ptr++;
            command[command_ptr] = converted_params & 0xFF;
        }
        else
        {
            printf("WARNING: Invalid command: Pattern %d, Step %d, Channel 1: "
                   "%01X%02X\n", pattern_number, step_number,
                   effectnum, effectparams);
        }
    }

    // Check if it's needed to add a volume
    if (volume > -1)
    {
        command[0] |= HAS_VOLUME;
        command[0] |= volume_mod_to_gb(volume) & 0x0F;
    }

    // Note: The volume bit doesn't affect the final size.
    const uint8_t sizes[8] = { 1, 2, 3, 3, 2, 3, 4, 4 };
    uint8_t bits = command[0] >> 5;

    for (int i = 0; i < sizes[bits]; i++)
    {
        out_write_hex(command[i]);
        out_write_str(",");
    }
}

void convert_channel2(uint8_t pattern_number, uint8_t step_number,
                      uint8_t note_index, uint8_t samplenum, uint8_t effectnum,
                      uint8_t effectparams)
{
    uint8_t command[4] = { 0, 0, 0, 0 }; // NOP
    int command_ptr = 1;

    int volume = -1;

    // If the effect is "Set Volume", handle it before and clear it so that it
    // isn't handled later.
    if (effectnum == 0xC)
    {
        volume = effectparams;
        effectnum = 0;
        effectparams = 0;
    }

    // Check if it's needed to add a note
    if (note_index <= (6 * 12 - 1))
    {
        command[0] |= HAS_NOTE;
        command[command_ptr] = note_index & 0x7F;
        command_ptr++;

        // If a note is set with no volume, set volume to the max
        // TODO: This should take the volume from the sample volume
        if (volume == -1)
            volume = 64;
    }

    // Check if there is a sample defined
    if (samplenum > 0)
    {
        uint32_t instrument = samplenum & 3;

        command[0] |= HAS_INSTRUMENT;
        command[command_ptr] = (instrument << 4) & 0x30;
    }

    if ((effectnum != 0) || (effectparams != 0))
    {
        uint8_t converted_num, converted_params;
        if (effect_mod_to_gb(pattern_number, step_number, 1, effectnum,
                             effectparams, &converted_num,
                             &converted_params))
        {
            command[0] |= HAS_EFFECT;
            command[command_ptr] |= converted_num & 0x0F;
            command_ptr++;
            command[command_ptr] = converted_params & 0xFF;
        }
        else
        {
            printf("WARNING: Invalid command: Pattern %d, Step %d, Channel 1: "
                   "%01X%02X\n", pattern_number, step_number,
                   effectnum, effectparams);
        }
    }

    // Check if it's needed to add a volume
    if (volume > -1)
    {
        command[0] |= HAS_VOLUME;
        command[0] |= volume_mod_to_gb(volume) & 0x0F;
    }

    // Note: The volume bit doesn't affect the final size.
    const uint8_t sizes[8] = { 1, 2, 3, 3, 2, 3, 4, 4 };
    uint8_t bits = command[0] >> 5;

    for (int i = 0; i < sizes[bits]; i++)
    {
        out_write_hex(command[i]);
        out_write_str(",");
    }
}

void convert_channel3(uint8_t pattern_number, uint8_t step_number,
                      uint8_t note_index, uint8_t samplenum, uint8_t effectnum,
                      uint8_t effectparams)
{
    uint8_t command[4] = { 0, 0, 0, 0 }; // NOP
    int command_ptr = 1;

    int volume = -1;

    // If the effect is "Set Volume", handle it before and clear it so that it
    // isn't handled later.
    if (effectnum == 0xC)
    {
        volume = effectparams;
        effectnum = 0;
        effectparams = 0;
    }

    // Check if it's needed to add a note
    if (note_index <= (6 * 12 - 1))
    {
        command[0] |= HAS_NOTE;
        command[command_ptr] = note_index & 0x7F;
        command_ptr++;

        // If a note is set with no volume, set volume to the max
        // TODO: This should take the volume from the sample volume
        if (volume == -1)
            volume = 64;
    }

    // Check if there is a sample defined
    if (samplenum > 0)
    {
        uint32_t instrument = samplenum & 0x7;

        command[0] |= HAS_INSTRUMENT;
        command[command_ptr] = (instrument << 4) & 0xF0;
    }

    if ((effectnum != 0) || (effectparams != 0))
    {
        uint8_t converted_num, converted_params;
        if (effect_mod_to_gb(pattern_number, step_number, 1, effectnum,
                             effectparams, &converted_num,
                             &converted_params))
        {
            command[0] |= HAS_EFFECT;
            command[command_ptr] |= converted_num & 0x0F;
            command_ptr++;
            command[command_ptr] = converted_params & 0xFF;
        }
        else
        {
            printf("WARNING: Invalid command: Pattern %d, Step %d, Channel 1: "
                   "%01X%02X\n", pattern_number, step_number,
                   effectnum, effectparams);
        }
    }

    // Check if it's needed to add a volume
    if (volume > -1)
    {
        command[0] |= HAS_VOLUME;
        command[0] |= volume_mod_to_gb_ch3(volume) & 0x07;
    }

    // Note: The volume bit doesn't affect the final size.
    const uint8_t sizes[8] = { 1, 2, 3, 3, 2, 3, 4, 4 };
    uint8_t bits = command[0] >> 5;

    for (int i = 0; i < sizes[bits]; i++)
    {
        out_write_hex(command[i]);
        out_write_str(",");
    }
}

void convert_channel4(uint8_t pattern_number, uint8_t step_number,
                      uint8_t note_index, uint8_t samplenum, uint8_t effectnum,
                      uint8_t effectparams)
{
    uint8_t command[4] = { 0, 0, 0, 0 }; // NOP
    int command_ptr = 1;

    int volume = -1;

    // If the effect is "Set Volume", handle it before and clear it so that it
    // isn't handled later.
    if (effectnum == 0xC)
    {
        volume = effectparams;
        effectnum = 0;
        effectparams = 0;
    }

    // Check if there is a sample defined
    if (samplenum > 0)
    {
        uint32_t kit = samplenum & 0xF;

        command[0] |= HAS_KIT;
        command[command_ptr] = kit & 0x0F;
        command_ptr++;

        // If a note is set with no volume, set volume to the max
        // TODO: This should take the volume from the sample volume
        if (volume == -1)
            volume = 64;
    }

    if ((effectnum != 0) || (effectparams != 0))
    {
        uint8_t converted_num, converted_params;
        if (effect_mod_to_gb(pattern_number, step_number, 1, effectnum,
                             effectparams, &converted_num,
                             &converted_params))
        {
            command[0] |= HAS_EFFECT;
            command[command_ptr] |= converted_num & 0x0F;
            command_ptr++;
            command[command_ptr] = converted_params & 0xFF;
        }
        else
        {
            printf("WARNING: Invalid command: Pattern %d, Step %d, Channel 4: "
                   "%01X%02X\n", pattern_number, step_number,
                   effectnum, effectparams);
        }
    }

    // Check if it's needed to add a volume
    if (volume > -1)
    {
        command[0] |= HAS_VOLUME;
        command[0] |= volume_mod_to_gb(volume) & 0x0F;
    }

    // Note: The volume bit doesn't affect the final size.
    const uint8_t sizes[4] = { 1, 3, 2, 4 };
    uint8_t bits = command[0] >> 6;

    for (int i = 0; i < sizes[bits]; i++)
    {
        out_write_hex(command[i]);
        out_write_str(",");
    }
}

void convert_pattern(_pattern_t *pattern, uint8_t number)
{
    out_write_str("static const uint8_t ");
    out_write_str(label_name);
    out_write_str("_");
    out_write_dec(number);
    out_write_str("[] = {\n");

    int step;
    for (step = 0; step < 64; step++)
    {
        out_write_str("    ");

        uint8_t data[4]; // Packed data

        uint8_t samplenum; // Unpacked data
        uint16_t sampleperiod;
        uint8_t effectnum, effectparams;

        uint8_t note_index;

        // Channel 1
        memcpy(data, pattern->info[step][0], 4);
        unpack_info(data, &samplenum, &sampleperiod, &effectnum, &effectparams);
        note_index = mod_get_index_from_period(sampleperiod, number, step, 1);
        convert_channel1(number, step, note_index, samplenum, effectnum,
                         effectparams);

        out_write_str(" ");

        // Channel 2
        memcpy(data, pattern->info[step][1], 4);
        unpack_info(data, &samplenum, &sampleperiod, &effectnum, &effectparams);
        note_index = mod_get_index_from_period(sampleperiod, number, step, 2);
        convert_channel2(number, step, note_index, samplenum, effectnum,
                         effectparams);

        out_write_str(" ");

        // Channel 3
        memcpy(data, pattern->info[step][2], 4);
        unpack_info(data, &samplenum, &sampleperiod, &effectnum, &effectparams);
        note_index = mod_get_index_from_period(sampleperiod, number, step, 3);
        convert_channel3(number, step, note_index, samplenum, effectnum,
                         effectparams);

        out_write_str(" ");

        // Channel 4
        memcpy(data, pattern->info[step][3], 4);
        unpack_info(data, &samplenum, &sampleperiod, &effectnum, &effectparams);
        note_index = mod_get_index_from_period(sampleperiod, number, step, 4);
        convert_channel4(number, step, note_index, samplenum, effectnum,
                         effectparams);

        out_write_str("\n");
    }

    out_write_str("};\n");
    out_write_str("\n");
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void print_usage(void)
{
    printf("Usage: mod2gbt modfile.mod song_name [-speed]\n\n");
    printf("       -speed      Don't convert speed from 50 Hz to 60 Hz.\n");
    printf("\n\n");
}

int main(int argc, char *argv[])
{
    printf("mod2gbt v4.0.0 (part of GBT Player)\n");
    printf("Copyright (c) 2009-2022 Antonio Niño Díaz "
           "<antonio_nd@outlook.com>\n");
    printf("All rights reserved\n");
    printf("\n");

    if (argc < 3)
    {
        print_usage();
        return -1;
    }

    strncpy(label_name, argv[2], sizeof(label_name));

    for (int i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "-speed") == 0)
        {
            perform_speed_conversion = 0;
            printf("Speed conversion disabled.\n\n");
        }
        else
        {
            print_usage();
            return -1;
        }
    }

    mod_file_t *modfile = load_file(argv[1]);

    if (modfile == NULL)
        return -2;

    printf("%s loaded!\n", argv[1]);

    if (strncmp(modfile->identifier, "M.K.", 4) == 0)
    {
        printf("Valid mod file!\n");
    }
    else
    {
        printf("ERROR: Not a valid mod file.\n"
               "Only 4 channel mod files with 31 samples allowed.\n");
        return -3;
    }

    printf("\nSong name: ");
    for (int i = 0; i < 20; i++)
        if (modfile->name[i])
            printf("%c", modfile->name[i]);
    printf("\n");

    uint8_t num_patterns = 0;

    for (int i = 0; i < 128; i++)
        if (modfile->pattern_table[i] > num_patterns)
            num_patterns = modfile->pattern_table[i];

    num_patterns++;

    printf("Number of patterns: %d\n", num_patterns);

    const char *extension = ".c";

    char *filename = malloc(strlen(label_name) + strlen(extension));
    if (filename == NULL)
    {
        printf("ERROR: Can't allocate memory for file name\n");
        return -4;
    }

    sprintf(filename, "%s%s", label_name, extension);
    out_open(filename);
    free(filename);

    out_write_str("\n// File created by mod2gbt\n\n"
                  "#include <stddef.h>\n#include <stdint.h>\n\n");

    printf("\nConverting patterns...\n");
    for (int i = 0; i < num_patterns; i++)
    {
        printf(".");
        convert_pattern(&(modfile->pattern[i]), i);
    }

    printf("\n\nPattern order...\n");

    out_write_str("const uint8_t *");
    out_write_str(label_name);
    out_write_str("[] = {\n");

    out_write_str("    NULL,\n"); // MOD files have no initial state

    for (int i = 0; i < modfile->song_length; i++)
    {
        out_write_str("    ");
        out_write_str(label_name);
        out_write_str("_");
        out_write_dec(modfile->pattern_table[i]);
        out_write_str(",\n");
    }
    out_write_str("    NULL\n");
    out_write_str("};");

    out_close();

    printf("\nDone!\n");

    return 0;
}
