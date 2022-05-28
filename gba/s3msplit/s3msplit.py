#!/usr/bin/env python3

# s3msplit v4.4.1 (Part of GBT Player)
#
# SPDX-License-Identifier: MIT
#
# Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

def read_bytes(path):
    with open(path, "rb") as file:
        file_bytes = file.read()
    return bytearray(file_bytes)

def write_bytes(path, file_bytes):
    with open(path, "wb") as file:
        file.write(file_bytes)

def read_u16(array, base_offset):
    return int((array[base_offset + 1] << 8) | array[base_offset])

def s3m_split(s3m_in_path, s3m_psg_path, s3m_dma_path):
    s3m_in = read_bytes(s3m_in_path)

    # Create actual copies, don't just copy the reference
    s3m_psg = s3m_in[:]
    s3m_dma = s3m_in[:]

    name = s3m_in[0:28].decode("utf-8")

    num_orders = read_u16(s3m_in, 32)
    num_instruments = read_u16(s3m_in, 34)
    num_patterns = read_u16(s3m_in, 36)

    print(f"Song name: '{name}'")
    print(f"Number of orders: {num_orders}")
    print(f"Number of instruments: {num_instruments}")
    print(f"Number of patterns: {num_patterns}")

    offset_para_instruments = 0x60 + num_orders
    if num_orders % 2 == 1:
        offset_para_instruments += 1

    offset_para_patterns = offset_para_instruments + num_instruments * 2

    print(f"Offset of instrument parapointers: {offset_para_instruments}")
    print(f"Offset of pattern parapointers: {offset_para_patterns}")

    # Handle instruments
    # ------------------

    for i in range(num_instruments):
        inst_number = i + 1

        offset = offset_para_instruments + i * 2
        offset_instrument = read_u16(s3m_in, offset) * 16

        if inst_number < 32: # PSG instruments, remove from DMA output

            # Set type of instrument to an invalid type
            s3m_dma[offset_instrument + 0] = 0

            # Clear sample name
            s3m_dma[offset_instrument + 0x30] = 0

            # Set length to 0
            s3m_dma[offset_instrument + 0x10 + 0] = 0
            s3m_dma[offset_instrument + 0x10 + 1] = 0
            s3m_dma[offset_instrument + 0x10 + 2] = 0
            s3m_dma[offset_instrument + 0x10 + 3] = 0

            # Set magic to 0
            s3m_dma[offset_instrument + 0x4C + 0] = 0
            s3m_dma[offset_instrument + 0x4C + 1] = 0
            s3m_dma[offset_instrument + 0x4C + 2] = 0
            s3m_dma[offset_instrument + 0x4C + 3] = 0

        else: # DMA instruments, remove from PSG output

            # Set type of instrument to an invalid type
            s3m_psg[offset_instrument + 0] = 0

            # Clear sample name
            s3m_psg[offset_instrument + 0x30] = 0

            # Set length to 0
            s3m_psg[offset_instrument + 0x10 + 0] = 0
            s3m_psg[offset_instrument + 0x10 + 1] = 0
            s3m_psg[offset_instrument + 0x10 + 2] = 0
            s3m_psg[offset_instrument + 0x10 + 3] = 0

            # Set magic to 0
            s3m_psg[offset_instrument + 0x4C + 0] = 0
            s3m_psg[offset_instrument + 0x4C + 1] = 0
            s3m_psg[offset_instrument + 0x4C + 2] = 0
            s3m_psg[offset_instrument + 0x4C + 3] = 0

    # Handle patterns
    # ---------------

    for i in range(num_patterns):
        offset = offset_para_patterns + i * 2
        offset_pattern = read_u16(s3m_in, offset) * 16

        if offset_pattern == 0:
             # Empty pattern found! Nothing to do here...
             continue

        combined_len = read_u16(s3m_in, offset_pattern)

        psg_pattern = []
        dma_pattern = []

        # Each pattern is composed of "steps". We need to figure out which ones
        # belong to channels 1-4 and which ones to the rest (and which ones are
        # common).

        read_pointer = 2
        while read_pointer < combined_len:
            header = s3m_in[offset_pattern + read_pointer]
            read_pointer += 1

            # If this step is a marker of "new row", add it to both outputs
            if header == 0:
                psg_pattern.append(0)
                dma_pattern.append(0)
                continue;

            channel = header & 31

            step = [header]

            if header & (1 << 5): # Note and instrument
                data1 = s3m_in[offset_pattern + read_pointer + 0]
                data2 = s3m_in[offset_pattern + read_pointer + 1]
                read_pointer += 2
                step.append(data1)
                step.append(data2)

            if header & (1 << 6): # Volume
                data = s3m_in[offset_pattern + read_pointer]
                read_pointer += 1
                step.append(data)

            if header & (1 << 7): # Effect and arguments
                data1 = s3m_in[offset_pattern + read_pointer + 0]
                data2 = s3m_in[offset_pattern + read_pointer + 1]
                read_pointer += 2
                step.append(data1)
                step.append(data2)

            if channel < 4:
                psg_pattern.extend(step)
            else:
                # Shift all channels 4 channels to the left
                step[0] = (header & 0xE0) | (channel - 4)
                dma_pattern.extend(step)

        # The size of the length field is included in the length
        len_psg_pattern = len(psg_pattern) + 2
        len_dma_pattern = len(dma_pattern) + 2

        # Overwrite lenght

        s3m_psg[offset_pattern + 0] = len_psg_pattern & 0xFF
        s3m_psg[offset_pattern + 1] = (len_psg_pattern >> 8) & 0xFF

        s3m_dma[offset_pattern + 0] = len_dma_pattern & 0xFF
        s3m_dma[offset_pattern + 1] = (len_dma_pattern >> 8) & 0xFF

        # Overwrite steps

        for p in range(len(psg_pattern)):
            s3m_psg[offset_pattern + 2 + p] = psg_pattern[p]

        for p in range(len(dma_pattern)):
            s3m_dma[offset_pattern + 2 + p] = dma_pattern[p]

    # Save patched files

    write_bytes(s3m_psg_path, s3m_psg)
    write_bytes(s3m_dma_path, s3m_dma)

if __name__ == "__main__":

    import argparse
    import sys

    print("s3msplit v4.4.1 (part of GBT Player)")
    print("Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>")
    print("All rights reserved")
    print("")

    parser = argparse.ArgumentParser(
                        description='Splits a GBT Player S3M file into two.')
    parser.add_argument("--input", default=None, required=True,
                        help="input file")
    parser.add_argument("--psg", default=None, required=True,
                        help="output file with only PSG channels and instruments")
    parser.add_argument("--dma", default=None, required=True,
                        help="output file without DMG channels or instruments")

    args = parser.parse_args()

    s3m_split(args.input, args.psg, args.dma)

    print("Done!")

    sys.exit(0)
