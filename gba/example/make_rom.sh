#!/bin/sh

../mod2gbt/mod2gbt template.mod template

mkdir -p music
mv template.c music

make