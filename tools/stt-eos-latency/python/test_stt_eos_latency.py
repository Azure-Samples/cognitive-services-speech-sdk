import contextlib
import importlib.util
import io
import sys
import types
import unittest
from pathlib import Path
from unittest import mock


SCRIPT_PATH = Path(__file__).with_name("stt-eos-latency.py")
SPEC = importlib.util.spec_from_file_location("stt_eos_latency", SCRIPT_PATH)
stt_eos_latency = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(stt_eos_latency)


class EventHook:
    def __init__(self):
        self._callbacks = []

    def connect(self, callback):
        self._callbacks.append(callback)

    def fire(self, event):
        for callback in self._callbacks:
            callback(event)


class FakeWave:
    def __init__(self):
        self.closed = False
        self._read_count = 0

    def getframerate(self):
        return 10

    def getnchannels(self):
        return 1

    def getsampwidth(self):
        return 2

    def getnframes(self):
        return 1

    def readframes(self, frame_count):
        del frame_count
        self._read_count += 1
        return b"\x00\x00" if self._read_count == 1 else b""

    def close(self):
        self.closed = True


def create_fake_speech_sdk(error_details):
    speechsdk = types.ModuleType("azure.cognitiveservices.speech")

    class SpeechConfig:
        def __init__(self, subscription, region=None, endpoint=None):
            del subscription, region, endpoint
            self.output_format = None
            self.speech_recognition_language = None

        def request_word_level_timestamps(self):
            pass

        def set_property(self, property_id, value):
            del property_id, value

    class PushAudioInputStream:
        instances = []

        def __init__(self, stream_format):
            del stream_format
            self.closed = False
            self.__class__.instances.append(self)

        def write(self, data):
            del data

        def close(self):
            self.closed = True

    class SpeechRecognizer:
        instances = []

        def __init__(self, **kwargs):
            del kwargs
            self.recognized = EventHook()
            self.session_started = EventHook()
            self.session_stopped = EventHook()
            self.canceled = EventHook()
            self.stopped = False
            self.__class__.instances.append(self)

        def start_continuous_recognition(self):
            self.session_started.fire(types.SimpleNamespace())
            details = types.SimpleNamespace(
                reason=speechsdk.CancellationReason.Error,
                error_details=error_details,
            )
            result = types.SimpleNamespace(cancellation_details=details)
            self.canceled.fire(types.SimpleNamespace(result=result))

        def stop_continuous_recognition(self):
            self.stopped = True

    speechsdk.SpeechConfig = SpeechConfig
    speechsdk.SpeechRecognizer = SpeechRecognizer
    speechsdk.OutputFormat = types.SimpleNamespace(Detailed="Detailed")
    speechsdk.PropertyId = types.SimpleNamespace(
        SpeechServiceConnection_LanguageIdMode="language-id-mode",
        Speech_SegmentationSilenceTimeoutMs="silence-timeout",
        SpeechServiceResponse_PostProcessingOption="post-processing",
        Speech_SegmentationStrategy="segmentation-strategy",
    )
    speechsdk.CancellationReason = types.SimpleNamespace(Error="Error")
    speechsdk.ResultReason = types.SimpleNamespace(RecognizedSpeech="RecognizedSpeech")
    speechsdk.audio = types.SimpleNamespace(
        AudioStreamFormat=lambda **kwargs: kwargs,
        PushAudioInputStream=PushAudioInputStream,
        AudioConfig=lambda **kwargs: kwargs,
    )
    return speechsdk, SpeechRecognizer, PushAudioInputStream


class SttEosLatencyTests(unittest.TestCase):
    def test_run_file_raises_after_cleanup_on_error_cancellation(self):
        fake_wave = FakeWave()
        speechsdk, recognizer_type, stream_type = create_fake_speech_sdk(
            "Authentication failed"
        )
        azure = types.ModuleType("azure")
        cognitive_services = types.ModuleType("azure.cognitiveservices")
        azure.cognitiveservices = cognitive_services
        cognitive_services.speech = speechsdk

        modules = {
            "azure": azure,
            "azure.cognitiveservices": cognitive_services,
            "azure.cognitiveservices.speech": speechsdk,
        }
        with (
            mock.patch.dict(sys.modules, modules),
            mock.patch.object(stt_eos_latency.wave, "open", return_value=fake_wave),
            mock.patch.object(stt_eos_latency.time, "sleep"),
            mock.patch.object(stt_eos_latency, "TRAILING_SILENCE_SEC", 0),
            contextlib.redirect_stdout(io.StringIO()),
        ):
            with self.assertRaisesRegex(RuntimeError, "Authentication failed"):
                stt_eos_latency.run_file(
                    "input.wav",
                    ["en-US"],
                    "westus",
                    "invalid-key",
                )

        self.assertTrue(fake_wave.closed)
        self.assertTrue(stream_type.instances[-1].closed)
        self.assertTrue(recognizer_type.instances[-1].stopped)

    def test_parallel_csv_write_failures_are_reported(self):
        argv = [
            "stt-eos-latency.py",
            "--files",
            "first.wav",
            "second.wav",
            "--workers",
            "2",
            "--key",
            "key",
            "--region",
            "region",
        ]
        output = io.StringIO()
        with (
            mock.patch.object(sys, "argv", argv),
            mock.patch.object(stt_eos_latency, "run_file", return_value=[]),
            mock.patch.object(
                stt_eos_latency,
                "write_csv",
                side_effect=PermissionError("read-only output"),
            ),
            contextlib.redirect_stdout(output),
        ):
            with self.assertRaises(SystemExit) as exit_error:
                stt_eos_latency.main()

        self.assertEqual(exit_error.exception.code, 1)
        self.assertEqual(output.getvalue().count("ERROR"), 2)
        self.assertIn("Failed to process 2 of 2 file(s).", output.getvalue())

    def test_parallel_unhandled_worker_exception_is_not_silenced(self):
        class FailedFuture:
            def __init__(self, message):
                self.message = message
                self.inspected = False

            def result(self):
                self.inspected = True
                raise RuntimeError(self.message)

        class FakeExecutor:
            instances = []

            def __init__(self, max_workers):
                del max_workers
                self.futures = [
                    FailedFuture("first worker failed"),
                    FailedFuture("second worker failed"),
                ]
                self.pending = list(self.futures)
                self.__class__.instances.append(self)

            def __enter__(self):
                return self

            def __exit__(self, exc_type, exc_value, traceback):
                del exc_type, exc_value, traceback

            def submit(self, function, item):
                del function, item
                return self.pending.pop(0)

        argv = [
            "stt-eos-latency.py",
            "--files",
            "first.wav",
            "second.wav",
            "--workers",
            "2",
            "--key",
            "key",
            "--region",
            "region",
        ]
        with (
            mock.patch.object(sys, "argv", argv),
            mock.patch.object(
                stt_eos_latency,
                "ThreadPoolExecutor",
                FakeExecutor,
            ),
            mock.patch.object(
                stt_eos_latency,
                "as_completed",
                side_effect=lambda futures: futures,
            ),
        ):
            with self.assertRaisesRegex(RuntimeError, "first worker failed"):
                stt_eos_latency.main()
            self.assertTrue(
                all(
                    future.inspected
                    for future in FakeExecutor.instances[-1].futures
                )
            )


if __name__ == "__main__":
    unittest.main()
