import unittest
from datetime import datetime, timedelta
from v2ticlib.rtf_utils import *

class TestRtfUtils(unittest.TestCase):

    def test_get_rtf(self):
        # Test case 1: deposit time is 10 seconds before current time, final audio length is 5 seconds
        deposit_time = datetime.now() - timedelta(seconds=10)
        final_audio_length_secs = 5
        expected_rtf = 2.0
        self.assertAlmostEqual(get_rtf(deposit_time, final_audio_length_secs), expected_rtf, places=2)

        # Test case 2: deposit time is 1 minute before current time, final audio length is 30 seconds
        deposit_time = datetime.now() - timedelta(minutes=1)
        final_audio_length_secs = 30
        expected_rtf = 2.0
        self.assertAlmostEqual(get_rtf(deposit_time, final_audio_length_secs), expected_rtf, places=2)

        # Test case 3: deposit time is 5 minutes before current time, final audio length is 2 minutes
        deposit_time = datetime.now() - timedelta(minutes=5)
        final_audio_length_secs = 120
        expected_rtf = 2.5
        self.assertAlmostEqual(get_rtf(deposit_time, final_audio_length_secs), expected_rtf, places=2)

        # Test case 4: deposit time is 1 hour before current time, final audio length is 30 seconds
        deposit_time = datetime.now() - timedelta(hours=1)
        final_audio_length_secs = 30
        expected_rtf = 120.0
        self.assertAlmostEqual(get_rtf(deposit_time, final_audio_length_secs), expected_rtf, places=2)

if __name__ == '__main__':
    unittest.main()