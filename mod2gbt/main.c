/*
*        --------------------------------------------------------------
*        ---                                                        ---
*        ---                                                        ---
*        ---            mod2gbt v1.2 (part of GBT Player)           ---
*        ---                                                        ---
*        ---                                                        ---
*        ---              Copyright (C) 2009-2014 Antonio Niño Díaz ---
*        ---                      All rights reserved.              ---
*        --------------------------------------------------------------
*
*                                          antonio_nd@outlook.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_ROM_BANK (2)

typedef unsigned char u8;
typedef signed   char s8;
typedef unsigned short int u16;
typedef signed   short int s16;

#define abs(x) ( ((x) > 0) ? (x) : -(x))
//#define swap16(x) ( (((x)&0xFF)<<8) || (((x)>>8)&0xFF) )
#define BIT(n) (1<<(n))

//--------------------------------------------------------------------------------
//--                                                                            --
//--                            READ MOD FILE                                   --
//--                                                                            --
//--------------------------------------------------------------------------------

typedef struct __attribute__((packed)) {
    char name[22];
    u16  lenght;
    u8   finetune; //4 lower bits
    u8   volume; //0-64
    u16  repeat_point;
    u16  repeat_lenght; //loop if > 1
} _sample_t;

typedef struct __attribute__((packed)) {
    u8 info[64][4][4]; //[step][channel][byte]
} _pattern_t;

typedef struct __attribute__((packed)) {
    char name[20];
    _sample_t sample[31];
    u8 song_lenght; //in patterns
    u8 unused; //set to 127, used by Noisetracker
    u8 pattern_table[128]; //0..63
    char identifier[4];
    _pattern_t pattern[256]; //only 64 allowed (see pattern_table) but set to 256 anyway...
    //sample data... unused here
} mod_file_t;

void * load_file(const char * filename)
{
	FILE * datafile = fopen(filename, "rb");
	unsigned int size;
	void * buffer = NULL;

	if(datafile == NULL)
	{
        printf("\n\nERROR: %s couldn't be opened!\n\n",filename);
        return NULL;
    }

    fseek (datafile , 0 , SEEK_END);
	size = ftell (datafile);
	if(size == 0)
	{
        printf("\n\nERROR: Size of %s is 0!\n\n",filename);
        fclose(datafile);
        return NULL;
    }
	rewind (datafile);
	buffer = malloc(size);
	if(buffer == NULL)
	{
        printf("\n\nERROR: Not enought memory to load %s!\n\n",filename);
        fclose(datafile);
        return NULL;
    }
	if(fread(buffer,size,1,datafile) != 1)
	{
        printf("\n\nERROR: Error while reading.\n\n");
        fclose(datafile);
		free(buffer);
        return NULL;
    }

	fclose(datafile);

	return buffer;
}

void unpack_info(u8 * info, u8 * sample_num, u16 * sample_period, u8 * effect_num, u8 * effect_param)
{
    *sample_num = (info[0]&0xF0)|((info[2]&0xF0)>>4);
    *sample_period = info[1]|((info[0]&0xF)<<8);
    *effect_num = info[2]&0xF;
    *effect_param = info[3];
}

const u16 mod_period[6*12] = {
    1712,1616,1524,1440,1356,1280,1208,1140,1076,1016, 960, 907,
     856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453,
     428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226,
     214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113,
     107, 101,  95,  90,  85,  80,  75,  71,  67,  63,  60,  56,
      53,  50,  47,  45,  42,  40,  37,  35,  33,  31,  30,  28
};

u8 mod_get_index_from_period(u16 period, int pattern, int step, int channel)
{
    if(period > 0)
    {
        if(period < mod_period[(6*12)-1])
            if(channel != 4) // noise doesn't matter
                    printf("\nPattern %d, Step %d, Channel %d. Note too high!\n",pattern,step,channel);

        if(period > mod_period[0])
            if(channel != 4) // noise doesn't matter
                printf("\nPattern %d, Step %d, Channel %d. Note too low!\n",pattern,step,channel);
    }
    else
    {
        return -1;
    }

    int i;
    for( i = 0; i < 6*12; i++)
        if(period == mod_period[i])
            return i;

    //problems here... get nearest value

    u16 nearest_value = 0xFFFF;
    u8 nearest_index = 0;
    for( i = 0; i < 6*12; i++)
    {
        int test_distance = abs( ((int)period) - ((int)mod_period[i]) );
        int nearest_distance = abs( ((int)period) - nearest_value );

        if(test_distance < nearest_distance)
        {
            nearest_value = mod_period[i];
            nearest_index = i;
        }
    }
    return nearest_index;
}

//--------------------------------------------------------------------------------
//--                                                                            --
//--                            SAVE OUTPUT                                     --
//--                                                                            --
//--------------------------------------------------------------------------------

FILE * output_file;
int output_asm;
char label_name[64];

void out_open(void)
{
    output_file = fopen(output_asm ? "output.asm" : "output.c","w");
}

void out_write_str(const char * c_str, const char * asm_str)
{
    fprintf(output_file,output_asm ? asm_str : c_str);
}

void out_write_dec(u8 number)
{
    fprintf(output_file,"%d",number);
}

void out_write_hex(u8 number)
{
    fprintf(output_file,"%02x",number);
}

void out_close(void)
{
    fclose(output_file);
}

//--------------------------------------------------------------------------------
//--                                                                            --
//--                           SAVE TO GAMEBOY                                  --
//--                                                                            --
//--------------------------------------------------------------------------------

/*
SAMPLE PERIOD LUT - MOD values
          C    C#   D    D#   E    F    F#   G    G#   A    A#   B
Octave 0:1712,1616,1525,1440,1357,1281,1209,1141,1077,1017, 961, 907  // C3 to B3
Octave 1: 856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453  // C4 to B4
Octave 2: 428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226  // C5 to B5
Octave 3: 214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113  // C6 to B6
Octave 4: 107, 101,  95,  90,  85,  80,  76,  71,  67,  64,  60,  57  // C7 to B7
Octave 5:  53,  50,  47,  45,  42,  40,  37,  35,  33,  31,  30,  28  // C8 to B8

//From C3 to B8  |  A5 = 1750 = 440.00Hz  |  C5 = 1546
const UWORD GB_frequencies[] = {
	  44,  156,  262,  363,  457,  547,  631,  710,  786,  854,  923,  986,  // C3 to B3
	1046, 1102, 1155, 1205, 1253, 1297, 1339, 1379, 1417, 1452, 1486, 1517,  // C4 to B4
	1546, 1575, 1602, 1627, 1650, 1673, 1694, 1714, 1732, 1750, 1767, 1783,  // C5 to B5
	1798, 1812, 1825, 1837, 1849, 1860, 1871, 1881, 1890, 1899, 1907, 1915,  // C6 to B6
	1923, 1930, 1936, 1943, 1949, 1954, 1959, 1964, 1969, 1974, 1978, 1982,  // C7 to B7
	1985, 1988, 1992, 1995, 1998, 2001, 2004, 2006, 2009, 2011, 2013, 2015   // C8 to B8
};

That means... MOD C0 (period 1712) = GB C3 (freq 44, index 0)
Anyway, they don't sound the same...
*/

inline int VOLUME_MOD_TO_GB(int v)
{
    return ((v) == 64 ? 0xF : ( (v) >> 2 ));
}

inline int SPEED_MOD_TO_GB(int s)
{
    return (s*6)/5;
}

void get_result_no_frequency(u8 * data, u8 effectnum, u8 effectparams, u8 channel)
{
    switch(effectnum)
    {
        case 0xB: //jump
            data[0] = 1;
            data[1] = effectparams;
            break;
        case 0xC: //vol
            if(channel == 3)
            {
                switch(VOLUME_MOD_TO_GB(effectparams))
                {
                    case 0: case 1: case 2: case 3: effectparams = 0; break;
                    case 4: case 5: case 6: case 7: effectparams = 3; break;
                    case 8: case 9: case 10: case 11: effectparams = 2; break;
                    default:
                    case 12: case 13: case 14: case 15: effectparams = 1; break;
                }
                data[0] = 4;
                data[1] = (effectparams & 0x3);
            }
            else
            {
                effectparams = VOLUME_MOD_TO_GB(effectparams);
                data[0] = 4;
                data[1] = (effectparams & 0xF);
            }
            break;
        case 0xD: //break
            data[0] = 2;
            data[1] = (((effectparams&0xF0) >> 4) * 10) + (effectparams&0xF);
            break;
        case 0xE:
            if((effectparams&0xF0) == 0x80) //pan
            {
                u8 pan;

                switch(effectparams & 0xF)
                {
                    case 0: case 1: case 2: case 3: pan = BIT(1); break;
                    default:
                    case 4: case 5: case 6: case 7:
                    case 8: case 9: case 10: case 11: pan = BIT(1) | BIT(0); break;
                    case 12: case 13: case 14: case 15: pan = BIT(0); break;
                }

                data[0] = 5;
                data[1] = pan;
             }
             else //nothing
             {
                data[0] = data[1] = 0;
             }
             break;
        case 0xf:
            if(effectparams > 0x1F) //nothing
            {
                data[0] = data[1] = 0;
            }
            else //speed
            {
                data[0] = 3;
                data[1] = SPEED_MOD_TO_GB(effectparams);
            }
            break;
        default: //nothing
            data[0] = data[1] = 0;
            break;
    }
}

void convert_pattern(_pattern_t * pattern, u8 number)
{
    out_write_str("const unsigned char ","");
    out_write_str(label_name,label_name);
    out_write_dec(number);
    out_write_str("[] = {\n",":\n");

    int step;
    for(step = 0; step < 64; step ++)
    {
        u8 data[4]; //packed data
        u8 samplenum; u16 sampleperiod; u8 effectnum, effectparams; //unpacked data

        u8 note_index, instrument;
        u8 result[2];

        //Channel 1
        memcpy(data,pattern->info[step][0],4);
        unpack_info(data,&samplenum,&sampleperiod,&effectnum,&effectparams);
        note_index = mod_get_index_from_period(sampleperiod,number,step,1);
        if(note_index > (6*12-1)) //not valid,
        {
            get_result_no_frequency(result,effectnum,effectparams,1);
        }
        else
        {
            instrument = samplenum & 3;
            switch(effectnum)
            {
                case 0xC: //freq + instr + vol
                    effectparams = VOLUME_MOD_TO_GB(effectparams);
                    result[0] = BIT(7) | BIT(6) | ((instrument&3)<<4) | (effectparams & 0xF);
                    result[1] = note_index & 0x7F;
                    break;
                case 0xE:
                    if((effectparams&0xF0) == 0x80) //freq + instr + pan
                    {
                        u8 pan;

                        switch(effectparams & 0xF)
                        {
                            case 0: case 1: case 2: case 3: pan = BIT(1); break;
                            default:
                            case 4: case 5: case 6: case 7:
                            case 8: case 9: case 10: case 11: pan = BIT(1) | BIT(0); break;
                            case 12: case 13: case 14: case 15: pan = BIT(0); break;
                        }

                        result[0] = BIT(7) | BIT(6) | ((instrument&3)<<4) | pan;
                        result[1] = BIT(7) | (note_index & 0x7F);
                    }
                    else //freq + instr
                    {
                        result[0] = BIT(7) | ((instrument&3)<<4);
                        result[1] = note_index & 0x7F;
                    }
                    break;
                default: //freq + instr
                    result[0] = BIT(7) | ((instrument&3)<<4);
                    result[1] = note_index & 0x7F;
                    break;
            }
        }

        out_write_str("  0x","  DB  $");
        out_write_hex(result[0]);
        out_write_str(",0x",",$");
        out_write_hex(result[1]);
        out_write_str(", ",", ");

        //Channel 2
        memcpy(data,pattern->info[step][1],4);
        unpack_info(data,&samplenum,&sampleperiod,&effectnum,&effectparams);
        note_index = mod_get_index_from_period(sampleperiod,number,step,2);
        if(note_index > (6*12-1)) //not valid,
        {
            get_result_no_frequency(result,effectnum,effectparams,2);
        }
        else
        {
            instrument = samplenum & 3;
            switch(effectnum)
            {
                case 0xC: //freq + instr + vol
                    effectparams = VOLUME_MOD_TO_GB(effectparams);
                    result[0] = BIT(7) | BIT(6) | ((instrument&3)<<4) | (effectparams & 0xF);
                    result[1] = note_index & 0x7F;
                    break;
                case 0xE:
                    if((effectparams&0xF0) == 0x80) //freq + instr + pan
                    {
                        u8 pan;

                        switch(effectparams & 0xF)
                        {
                            case 0: case 1: case 2: case 3: pan = BIT(1); break;
                            default:
                            case 4: case 5: case 6: case 7:
                            case 8: case 9: case 10: case 11: pan = BIT(1) | BIT(0); break;
                            case 12: case 13: case 14: case 15: pan = BIT(0); break;
                        }

                        result[0] = BIT(7) | BIT(6) | ((instrument&3)<<4) | pan;
                        result[1] = BIT(7) | (note_index & 0x7F);
                    }
                    else //freq + instr
                    {
                        result[0] = BIT(7) | ((instrument&3)<<4);
                        result[1] = note_index & 0x7F;
                    }
                    break;
                default: //freq + instr
                    result[0] = BIT(7) | ((instrument&3)<<4);
                    result[1] = note_index & 0x7F;
                    break;
            }
        }
        out_write_str("0x","$");
        out_write_hex(result[0]);
        out_write_str(",0x",",$");
        out_write_hex(result[1]);
        out_write_str(", ",", ");

        //Channel 3
        memcpy(data,pattern->info[step][2],4);
        unpack_info(data,&samplenum,&sampleperiod,&effectnum,&effectparams);
        note_index = mod_get_index_from_period(sampleperiod,number,step,3);
        if(note_index > (6*12-1)) //not valid,
        {
            get_result_no_frequency(result,effectnum,effectparams,3);
        }
        else
        {
            instrument = samplenum & 7;
            switch(effectnum)
            {
                case 0xC: //freq + instr + vol
                    switch(VOLUME_MOD_TO_GB(effectparams))
                    {
                        case 0: case 1: case 2: case 3: effectparams = 0; break;
                        case 4: case 5: case 6: case 7: effectparams = 3; break;
                        case 8: case 9: case 10: case 11: effectparams = 2; break;
                        default:
                        case 12: case 13: case 14: case 15: effectparams = 1; break;
                    }
                    result[0] = BIT(7) | BIT(6) | (effectparams << 3) | (instrument&7);
                    result[1] = note_index & 0x7F;
                    break;
                case 0xE:
                    if((effectparams&0xF0) == 0x80) //freq + instr + pan
                    {
                        u8 pan;

                        switch(effectparams & 0xF)
                        {
                            case 0: case 1: case 2: case 3: pan = BIT(1); break;
                            default:
                            case 4: case 5: case 6: case 7:
                            case 8: case 9: case 10: case 11: pan = BIT(1) | BIT(0); break;
                            case 12: case 13: case 14: case 15: pan = BIT(0); break;
                        }

                        result[0] = BIT(7) | BIT(6) | BIT(5) | (pan<<3) | (instrument&7);;
                        result[1] = (note_index & 0x7F);
                    }
                    else //freq + instr
                    {
                        result[0] = BIT(7) | (instrument&7);
                        result[1] = note_index & 0x7F;
                    }
                    break;
                default: //freq + instr
                    result[0] = BIT(7) | (instrument&7);
                    result[1] = note_index & 0x7F;
                    break;
            }
        }
        out_write_str("0x","$");
        out_write_hex(result[0]);
        out_write_str(",0x",",$");
        out_write_hex(result[1]);
        out_write_str(", ",", ");

        //Channel 4
        memcpy(data,pattern->info[step][3],4);
        unpack_info(data,&samplenum,&sampleperiod,&effectnum,&effectparams);
        note_index = mod_get_index_from_period(sampleperiod,number,step,4);
        if(note_index > (6*12-1)) //not valid,
        {
            get_result_no_frequency(result,effectnum,effectparams,4);
        }
        else
        {
            instrument = samplenum & 0xF;
            switch(effectnum)
            {
                case 0xC: //instr + vol
                    effectparams = VOLUME_MOD_TO_GB(effectparams);
                    result[0] = BIT(7) | BIT(6) | (effectparams & 0xF);
                    result[1] = instrument & 0x7F;
                    break;
                case 0xE:
                    if((effectparams&0xF0) == 0x80) //instr + pan
                    {
                        u8 pan;

                        switch(effectparams & 0xF)
                        {
                            case 0: case 1: case 2: case 3: pan = BIT(1); break;
                            default:
                            case 4: case 5: case 6: case 7:
                            case 8: case 9: case 10: case 11: pan = BIT(1) | BIT(0); break;
                            case 12: case 13: case 14: case 15: pan = BIT(0); break;
                        }

                        result[0] = BIT(7) | BIT(6) | pan;
                        result[1] = BIT(7) | (instrument & 0x7F);
                    }
                    else // instr
                    {
                        result[0] = BIT(7);
                        result[1] = instrument & 0x7F;
                    }
                    break;
                default: // instr
                    result[0] = BIT(7);
                    result[1] = instrument & 0x7F;
                    break;
            }
        }

        out_write_str("0x","$");
        out_write_hex(result[0]);
        out_write_str(",0x",",$");
        out_write_hex(result[1]);

        if(step == 63) out_write_str("\n","\n");
        else out_write_str(",\n","\n");
    }

    out_write_str("};\n\n","\n\n");
}

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

void print_usage(void)
{
    printf("Usage: mod2gtb modfile.mod label_name [-c/-a] N\n\n");
    printf("       -c: Write GBDK output.c file.\n");
    printf("       -a: Write RGBDS output.asm file.\n");
    printf("       N: Set output to ROM bank N. If not defined, it will be %d.",DEFAULT_ROM_BANK);
    printf("\n\n");
}

int main(int argc, char * argv[])
{
    printf("     +-------------------------------------------+\n");
    printf("     |                                           |\n");
    printf("     |     mod2gbt v1.2 (part of GBT Player)     |\n");
    printf("     |                                           |\n");
    printf("     |                                           |\n");
    printf("     | Copyright (C) 2009-2014 Antonio Niño Díaz |\n");
    printf("     |                      All rights reserved. |\n");
    printf("     |                                           |\n");
	printf("     |                   antonio_nd@outlook.com  |\n");
    printf("     |                                           |\n");
    printf("     +-------------------------------------------+\n");

    printf("\n");

    if( (argc != 4) && (argc != 5))
    {
        print_usage();
        return -1;
    }

    strcpy(label_name,argv[2]);

    if(strcmp(argv[3],"-a") == 0)
    {
        output_asm = 1;
    }
    else if(strcmp(argv[3],"-c") == 0)
    {
        output_asm = 0;
    }
    else
    {
        print_usage();
        return -1;
    }

    int output_bank = DEFAULT_ROM_BANK;

    if(argc == 5)
    {
        if(sscanf(argv[4],"%d",&output_bank) != 1)
        {
            printf("Invalid bank: %s\n\n",argv[4]);
            print_usage();
            return -2;
        }
        else
        {
            printf("Output to bank: %d\n",output_bank);
        }
    }

    int i;
    mod_file_t * modfile = load_file(argv[1]);
    if(modfile == NULL) return -2;

    printf("\n%s loaded!\n",argv[1]);
    if(strncmp(modfile->identifier,"M.K.",4) == 0)
    {
        printf("\nValid mod file!\n");
    }
    else
    {
        printf("\nERROR: Not a valid mod file.\nOnly 4 channel mod files with 31 samples allowed.\n\n");
        return -3;
    }

    printf("\nSong name: ");
    for(i = 0; i < 20; i++) if(modfile->name[i]) printf("%c",modfile->name[i]);

    printf("\n\nSample names:\n");

    for(i = 0; i < 31; i++) if(modfile->sample[i].name[0])
        printf("%d: %s\n",i,modfile->sample[i].name);

    printf("\n");

    u8 num_patterns = 0;
    for(i = 0; i < 128; i++) if(modfile->pattern_table[i] > num_patterns)
            num_patterns = modfile->pattern_table[i];
    num_patterns ++;

    printf("Number of patterns: %d\n",num_patterns);

    out_open();

    out_write_str("\n// File created by mod2gbt\n\n","\n; File created by mod2gbt\n\n");

    out_write_str("#pragma bank=","\tSECTION \"");
    out_write_str("",label_name);
    out_write_str("","\", DATA, BANK[");
    out_write_dec(output_bank);
    out_write_str("\n\n","]\n\n");

    printf("\nConverting patterns");
    for(i = 0; i < num_patterns; i++)
    {
        printf(".");
        convert_pattern(&(modfile->pattern[i]),i);
    }
    printf("\n");

    printf("\nPattern order...\n");

    out_write_str("const unsigned char * ","");
    out_write_str(label_name,label_name);
    out_write_str("_Data[] = {\n","_Data::\n");
    for(i = 0; i < modfile->song_lenght; i ++)
    {
        out_write_str("    ","    DW  ");
        out_write_str(label_name,label_name);
        out_write_dec(modfile->pattern_table[i]);
        out_write_str(",\n","\n");
    }
    out_write_str("    0x0000\n","    DW  $0000\n");
    out_write_str("};\n\n","\n\n");

    out_close();

    printf("\nDone!\n\n");

    return 0;
}
