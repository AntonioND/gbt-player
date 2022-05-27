Example of combined GBT Player and Maxmod usage
===============================================

GBT Player only uses the PSG sound channels of the GBA. If you want to take
advantage of the two DMA channels, you need to use a different library.

The main reason to use another audio library at the same time as GBT Player is
that software mixing (which is needed to use the DMA audio channels) takes a lot
of CPU time. This may be a problem in games that need to use the CPU for other
tasks, like 3D software rendering.

Using GBT Player for some audio channels, while using another library for extra
audio channels, reduces CPU usage (if the limitations of the PSG channels are
acceptable).

GBT Player supports this scenario the following way:

1. The composer can create a S3M file (starting with one of the S3M templates
   included in this repository).

   - The first 4 channels are mapped to the 4 PSG channels of the GBA, and you
     need to check the instructions provided with ``s3m2gbt`` to know how to use
     them. Samples 1-31 are used by the 4 PSG channels as well.

   - Any sample over number 31, and any channel over 5 can be used for software
     mixing audio players.

   This way the music composer can comfortably create the song, as it is
   possible to hear all channels at the same time.

2. Once your song is ready, you need to use ``s3msplit`` to divide it into two
   S3M files:

   - A PSG S3M file with the 4 PSG channels and samples 1-31,
   - A DMA S3M file with samples 32 onwards, and channels 5 onwards.

3. The DMA S3M file needs to be passed to your software mixing audio player. The
   PSG S3M file has to be converted using ``s3m2gbt`` to be used by GBT Player as
   normal.

This system uses S3M files for everything, which restricts the number of
libraries you can use. Two options are:

- `Krawall <https://github.com/sebknzl/krawall>`_
- `Maxmod <https://maxmod.devkitpro.org/>`_: Included with devkitPro.

In this example I've used the version of Maxmod that comes with devkitPro. It
should be at least version ``1.0.15``. If you don't have that version, update
your packages.

IMPORTANT: The reason why you can't just start Maxmod and GBT Player at the same
time is that timing is slightly different, and that is very noticeable as time
passes after the beginning of the song.
