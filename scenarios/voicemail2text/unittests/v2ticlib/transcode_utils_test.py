import unittest
from unittest.mock import MagicMock, patch
import sys, os
sys.path.append(os.path.join(os.path.abspath(os.curdir)))
from v2ticlib.transcode_utils import TranscodeUtils

class TestTranscodeUtils(unittest.TestCase):

    def setUp(self):
        self.transcode_utils = TranscodeUtils()

    @patch('asyncio.subprocess.create_subprocess_shell')
    async def test_do_transcode_success(self, mock_create_subprocess_shell):
        mock_process = MagicMock()
        mock_process.returncode = 0
        mock_process.communicate.return_value = (b'', b'')
        mock_create_subprocess_shell.return_value = mock_process

        transcode_cmd = 'test_cmd'
        audio_bytes = b'test_audio_bytes'
        return_code, output, audio_output_bytes = await self.transcode_utils.exec_stdin(transcode_cmd, audio_bytes)

        mock_create_subprocess_shell.assert_called_once_with(transcode_cmd, stdin=-2, stdout=-1, stderr=-3)
        mock_process.communicate.assert_called_once_with(input=audio_bytes)
        self.assertEqual(return_code, 0)
        self.assertEqual(output, '')
        self.assertEqual(audio_output_bytes, b'')

    @patch('asyncio.subprocess.create_subprocess_shell')
    async def test_do_transcode_failure(self, mock_create_subprocess_shell):
        mock_process = MagicMock()
        mock_process.returncode = 1
        mock_process.communicate.return_value = (b'', b'test_error_output')
        mock_create_subprocess_shell.return_value = mock_process

        transcode_cmd = 'test_cmd'
        audio_bytes = b'test_audio_bytes'
        with self.assertRaises(Exception):
            await self.transcode_utils.exec_stdin(transcode_cmd, audio_bytes)

        mock_create_subprocess_shell.assert_called_once_with(transcode_cmd, stdin=-2, stdout=-1, stderr=-3)
        mock_process.communicate.assert_called_once_with(input=audio_bytes)

    def test_is_truncated_true(self):
        duration = 100
        max_audio_length = 50
        self.assertTrue(self.transcode_utils.is_truncated(duration, max_audio_length))

    def test_is_truncated_false(self):
        duration = 50
        max_audio_length = 100
        self.assertFalse(self.transcode_utils.is_truncated(duration, max_audio_length))

if __name__ == '__main__':
    unittest.main()