#!/bin/sh

mkdir -p music
mkdir -p maxmod

../../s3msplit/s3msplit.py \
    --input template_combined.s3m \
    --psg music/template_combined_psg.s3m \
    --dma maxmod/template_combined_dma.s3m

../../s3m2gbt/s3m2gbt.py \
    --input music/template_combined_psg.s3m \
    --name template_combined_psg \
    --output music/template_combined_psg.c \
    --instruments

make
