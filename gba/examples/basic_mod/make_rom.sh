#!/bin/sh

mkdir -p music

../../mod2gbt/mod2gbt.py template.mod template
mv template.c music

make
