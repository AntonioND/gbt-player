Changelog
=========

- Version 4.4.0 (2022/5/20)

  - GBA-only release.
  - Rewrite ``mod2gbt`` in Python.
  - Remove external dependencies of ``s3m2gbt``.
  - Add ``gbt_is_playing()``.
  - Update documentation.

- Version 4.3.0 (2022/5/16)

  - GBA-only release.
  - Refactor ``s3m2gbt``.
  - Refactor the C library.
  - Implement event callback system.
  - Add ``s3msplit`` to split S3M files into a GBT Player template file and a
    regular S3M file to be played by other regular S3M players.
  - Add ``gbt_set_position()`` and ``gbt_get_position()``.
  - Add examples, including one of using GBT Player at the same time as Maxmod.
  - Fix code that disables channels in GBT Player.

- Version 4.2.0 (2022/5/11)

  - GBA-only release.
  - Refactor ``s3m2gbt``, specially error handling.

    - Use Python exceptions.
    - Any unknown effect causes an exception.
    - Don't export unused patterns.

  - Refactor player. Group variables on a per-channel basis.
  - Fix notes with no defined volume (this time it's for real, unlike in 4.1.0).

- Version 4.1.1 (2022/5/6)

  - GBA-only release.
  - Add sample S3M modules for vibrato and channel 3 instrument change.
  - Make samples of channels 1 and 2 more accurate.

- Version 4.1.0 (2022/5/6)

  - GBA-only release.
  - Improve S3M support:

    - Note cut with ``^^`` supported.
    - Vibrato is now supported.
    - Fix notes with no defined volume (that should preserve the volume of the
      channel).
    - Implement volume slides.
    - Add ``gbt_volume()`` function.
    - Let user replace channel 3 waveforms by exporting them from the S3M file.

- Version 4.0.1 (2022/4/25)

  - Fix compatibility with old MOD songs in ``mod2gbt`` in the GBA port.
  - Fix crash in ``mod2gbt``.

- Version 4.0.0 (2022/4/25)

  - This is a GBA only release.
  - Break between GB and GBA versions. In the future they may be reunified.
  - Added support for S3M files to the GBA version.
  - Added ``s3m2gbt`` tool.

- Version 3.1.0 (2022/4/19)

  - Bugfix release for GBA.

- Version 3.0.9 (2022/4/18)

  - First version for GBA.
  - Directory structure reorganized.

- Version 3.0.8 and 2.1.3 (2020/4/21)

  - Fix effect ``Dnn`` in the last pattern of a song (Nolan Baker). Now it jumps
    to pattern 0 instead of crashing.
  - Relicense under the MIT license (rather than BSD-2-Clause).

- Version 3.0.7 (2019/6/3)

  - Fix panning in channel 4.

- Version 3.0.6 (2018/1/20)

  - Code cleanup.
  - Fix example code.
  - Increase version of ``mod2gbt`` to v3.1.
  - Some C fixes (Zalo)

- Version 2.1.2 (2018/1/20)

  - Cleanup of legacy version.
  - ``mod2gbt`` can no longer output assembly files. Set version number to v2.2.
  - Added license headers to all files so that it is easier to bundle code with
    other projects.

- Version 3.0.5 (2016/5/2)

  - Bit shift optimizations.

- Version 3.0.4 (2016/4/5)

  - Code reorganized and added license notices to source files.

- Version 3.0.3 (2016/2/6)

  - Code reorganized to fit in 80 columns.

- Version 3.0.2 (2015/5/3)

  - Corrected tabulations in RGBDS code.

- Version 3.0.1 (2015/4/27)

  - Replaced tabs by spaces in asm code.

- Version 3.0.0 (2015/4/22)

  - Added support for multiple bank songs.
  - ``GBDK`` default assembler (``as-gbz80``) version discontinued. Version
    2.1.1 will be kept in case someone wants to use it.
  - Previously converted songs must be converted again.
  - ``gbt_play()``: registers used for arguments have changed a bit.

- Version 2.1.1 (2015/4/7)

  - Simplified ``GBDK`` example because it was confusing a lot of people...

- Version 2.1.0 (2014/5/24)

  - Fixed arpeggio effect. Now it keeps looping until tick = 0 (previously it
    only looped once). It loops 3 steps, not 4.
  - Added "Cut Note" effect.
  - Effects optimizations.

- Version 2.0.1 (2014/5/23)

  - Fixed effects in channels 1, 2 and 3 in GBDK version.

- Version 2.0.0 (2014/5/22)

  - Rewritten library and converter.
  - Arpeggio effect added.
  - Song size should be reduced to about 60-75% (but it can go as high as 150%
    if it uses effects all time in all channels).
  - Old converted song data won't work, songs have to be converted again.

- Version 1.2.1 (2014/5/15)

  - Fixed ``Bnn`` command.

- Version 1.2 (2014/5/1)

  - Fixed lots of things regarding the mod file template and ``mod2gbt``, the
    converter. Old songs won't be converted right with this new version. You
    should copy pattern data into the new mod template and transpose it 17
    semitones to make it work again.
  - Fixed a typo in a variable name.

- Version 1.1 (2013)

  - Fixed definitions for enabling and disabling channels.
  - Changed email address.

- Version 1.0 (2009)

  - Initial release


