# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license.

import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

import azure.cognitiveservices.speech as speechsdk

import live_interpreter


class LiveInterpreterTests(unittest.TestCase):
    def test_missing_configuration_exits_with_an_actionable_error(self):
        environment = os.environ.copy()
        environment.pop("SPEECH_KEY", None)
        result = subprocess.run(
            [sys.executable, str(Path(live_interpreter.__file__))],
            env=environment,
            capture_output=True,
            text=True,
            timeout=15,
        )
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("SPEECH_KEY", result.stderr)

    def test_empty_configuration_is_rejected(self):
        with patch.dict(os.environ, {"SPEECH_KEY": ""}):
            with self.assertRaisesRegex(ValueError, "SPEECH_KEY"):
                live_interpreter.required_environment("SPEECH_KEY")

    def test_configuration_preserves_llm_and_other_endpoint_parameters(self):
        endpoint = (
            "wss://example.cognitiveservices.azure.com/"
            "stt/speech/universal/v2?existing=value&setfeature=llm"
        )
        config = live_interpreter.create_translation_config("test-key", endpoint)
        self.assertEqual(config.voice_name, "personal-voice")
        self.assertEqual(config.target_languages, ("fr",))
        self.assertEqual(
            config.get_property(
                speechsdk.PropertyId.SpeechServiceConnection_SynthOutputFormat
            ),
            "riff-16khz-16bit-mono-pcm",
        )
        self.assertEqual(
            config.get_property(
                speechsdk.PropertyId.SpeechServiceConnection_Endpoint
            ),
            endpoint,
        )

    def test_synthesized_audio_is_written_without_overwriting(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "translation-1.wav"
            live_interpreter.write_audio(path, b"first")
            self.assertEqual(path.read_bytes(), b"first")
            with self.assertRaises(FileExistsError):
                live_interpreter.write_audio(path, b"second")
            self.assertEqual(path.read_bytes(), b"first")


if __name__ == "__main__":
    unittest.main()
