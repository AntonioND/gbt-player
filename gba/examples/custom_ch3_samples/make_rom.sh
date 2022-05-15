#!/bin/sh

mkdir -p music

../../s3m2gbt/s3m2gbt.py \
    --input template.s3m \
    --name template \
    --output music/template.c \
    --instruments

make
