 --------------------------------------------------------------
 ---                                                        ---
 ---                                                        ---
 ---                       GBT PLAYER  v1.2                 ---
 ---                                                        ---
 ---                                                        ---
 ---              Copyright (C) 2009-2014 Antonio Niño Díaz ---
 ---                      All rights reserved.              ---
 --------------------------------------------------------------
 
Email: antonio_nd@outlook.com

A music player library for Game Boy that can be used with RGBDS or GBDK:

[https://github.com/bentley/rgbds](https://github.com/bentley/rgbds)
[http://gbdk.sourceforge.net/](http://gbdk.sourceforge.net/)

INTRODUCTION:
-------------

GBT Player is a music creation environment for GB and GBC. It is composed by mod2gbt, which converts a mod file into a gbt (GameBoy Tracker) file, and gbt player, which will be used to play that song in the GB.

mod2gbt is writen in C, and should compile in every machine where you can code in C. Only a windows binary is included.

GBT Player is writen in 100% assembler. That means that it is FAST, it won't need a lot of CPU time (around 7%?), and you will have a lot of time for your game logic. There is source code for GBDK and RGBDS, the main free alternatives for GameBoy developement (in my opinion).

GBT Player is open source, and it is licensed under the BSD license. That means that you can use and modify it but you have to give credit for the original work. It would be nice to you tell me if you use it, anyway. :)


HOW TO COMPILE EXAMPLES:
------------------------

-RGBDS: Put rgbasm, rgbfix and xlink in the source folder and double clic the bat file. Compiled binary included.
-GBDK: Open the bat file, change it to set the correct path to your lcc binary and double clic the bat.

NOTES:
------

A nice tracker to modify the mod file is OpenMPT. You can download it here:

[http://openmpt.org/](http://openmpt.org/)

I don't use bat files in my projects, I use makefiles, but I thought that the examples would be too simple for a makefile to be useful.

instrument_test.gb is a sample of the default sounds.

The mod file isn't 100% accurate. Sounds are a bit different from the real ones, so you should make roms and test them in emulators or real GB often.

CHANGELOG:
----------

v1.2 - Fixed lots of things regarding the mod file template and mod2gbt, the converter. Old songs won't be converted right with this new version. You should copy pattern data into the new mod template and transpose it 17 semitones to make it work again.

v1.1 - Fixed definitions for enabling and disabling channels.
     - Changed email address.

v1.0 - Initial release
