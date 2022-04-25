#!/bin/sh

mkdir -p music

# MOD files:
#../mod2gbt/mod2gbt template.mod template
#mv template.c music

# S3M files:
../s3m2gbt/s3m2gbt.py --input template.s3m --name template --output music/template.c

make
