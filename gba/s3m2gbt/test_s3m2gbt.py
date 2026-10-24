import unittest

import s3m2gbt


class ChannelFourNoteCutTests(unittest.TestCase):
    def test_note_cut_with_instrument_reports_conversion_error(self):
        with self.assertRaises(s3m2gbt.RowConversionError) as raised:
            s3m2gbt.convert_channel4(0xFE, 1, -1, None, None)

        self.assertEqual(
            raised.exception.message,
            "Note cut + Sample in same row: Not supported in channel 4",
        )

    def test_note_cut_without_instrument_remains_supported(self):
        self.assertEqual(
            s3m2gbt.convert_channel4(0xFE, 0, -1, None, None),
            [0x80, 0xFE],
        )


if __name__ == "__main__":
    unittest.main()
