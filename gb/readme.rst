GBT Player v3.1.0
=================

Music player library and converter kit for Game Boy and Game Boy Color that can
be used with `RGBDS <https://github.com/gbdev/rgbds>`__ (and, with
limitations, with GBDK).

IMPORTANT NOTE: Version 1.x.x converted songs won't work with player version
2.0.0 or higher. The same happens with 2.x.x and version 3.0.0.  They have to be
converted again.

How to compile the example
--------------------------

Compile ``mod2gbt``. In Windows you can use a command line like:

``gcc -o mod2gbt.exe mod2gbt``

A Makefile for Linux has been included in case you are feeling lazy. Just do:

``make``

Put ``rgbasm``, ``rgbfix`` and ``rgblink`` in the ``gb/rgbds`` folder and
run the bat/sh file in the ``rbgds_example`` folder. A compiled GB binary is
included.

Notes
-----

A nice tracker to modify the mod file is **OpenMPT**. You can download it here:

http://openmpt.org/

I don't use bat or sh files in my projects, I use makefiles, but I thought that
the examples would be too simple for a makefile to be useful.

``instr_test.gb`` is a sample of the default sounds.

``range_test.gb`` is just a test of what notes the GB can reach (C3 - B8).

``effects_test.gb`` tests arpeggio and "cut note" effects.

``volume_test.gb`` tests the range of volumes supported by each channel.

The mod file isn't 100% accurate. Sounds are a bit different from the real ones,
so you should make roms and test them in emulators or real GB often.

If you don't like the speed conversion done by ``mod2gbt`` (from 50 Hz to 60 Hz)
you can use the ``-speed`` argument for ``mod2gbt``. The speed will be higher
and it will probably have to be adjusted manually.

The initial speed of the song is set by the start function, and it will run at
that speed until it finds a change speed command in the song. If the first step
of your song takes forever, this is the reason.

**For GB/GBC**: When creating **really** big ROMs (more than 4 MiB), the define
``GBT_USE_MBC5_512BANKS`` in ``gbt_player.inc`` should be uncommented to allow
allocation of the music data in banks higher than 255. Also, songs must be
converted adding ``-512-banks`` to the ``mod2gbt`` command line.

GBDK notes
----------

GBDK: http://gbdk.sourceforge.net/

``GBDK`` default assembler (``as-gbz80``) is no longer supported. ``RGBDS`` must
be used, and I don't really know if ``GBDK`` will work with latest ``RGBDS``
version. Adding ``-W--asm=rgbds`` to the command line when compiling ``GBDK``
code should make it work, but it hasn't been tested.

Since new functionality needs advanced macros like ``BANK()``, which are only
supported by ``RGBDS``, the ``GBDK`` version is discontinued. The latest version
that can be used with ``GBDK`` default assembler (2.1.2) is in the folder
``legacy_gbdk``, as well as the corresponding converter. I don't really know if
the same effect can be obtained with ``as-gbz80``. If it can be done, open an
issue and I may update ``GBDK`` version again. For now, this old legacy version
is mostly unmaintained.

This is an article I wrote related to bank switching:

http://www.skylyrac.net/2015-04-08-gbt-player-usage.html

To compile the ``GBDK`` example: Open the bat file, change it to set the correct
path to your ``lcc`` binary and double clic the bat.

To Do
-----

- Store channel 3 samples in RAM to be able to change them in execution time by
  the user?
- End song callback? Special effect for callback? To synchronize game events or
  things like that.
- ``WLA-DX`` version.

Known bugs
----------

- Effect ``Dxx``, when used the last step of a pattern, will jump 2 patterns
  instead of 1.

About
-----

Licensed under the MIT license.

Copyright (C) 2009-2022 Antonio Niño Díaz

All rights reserved.

Email: antonio_nd@outlook.com

Web: www.skylyrac.net/

Latest version: https://github.com/AntonioND/gbt-player

