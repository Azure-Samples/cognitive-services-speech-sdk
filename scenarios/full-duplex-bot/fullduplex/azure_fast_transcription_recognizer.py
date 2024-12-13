import json
import logging
import queue
import threading
from typing import Callable
from requests import Session
import wave
import torch
import numpy as np
from io import BytesIO

from VAD.vad_iterator import VADIterator, int2float, float2int

logger = logging.getLogger(__name__)


AzureADTokenProvider = Callable[[], str]

class VADHandler(threading.Thread):
    def __init__(self, model, threshold, sampling_rate, min_silence_duration_ms, speech_pad_ms, stop_event, input_queue, output_queue):
        super().__init__()
        self.stop_event = stop_event
        self.input_queue = input_queue
        self.output_queue = output_queue
        self.vad_iterator = VADIterator(
            model,
            threshold=threshold,
            sampling_rate=sampling_rate,
            min_silence_duration_ms=min_silence_duration_ms,
            speech_pad_ms=speech_pad_ms
        )


    def run(self) -> None:
        logger.info("VAD handler started")
        while not self.stop_event.is_set():
            chunk = self.input_queue.get()
            if chunk is None:
                break
            chunk = np.frombuffer(chunk, dtype=np.int16)
            vad_output = self.vad_iterator(torch.from_numpy(int2float(chunk)))
            if vad_output is not None and len(vad_output) != 0:
                logger.info(f"VAD output: {len(vad_output)}")
                array = np.concatenate(vad_output)
                self.output_queue.put(array)
        self.output_queue.put(None)


class AzureFastTranscriptionClient(threading.Thread):
    def __init__(self, endpoint: str, locale: str, key: str, token_provider: AzureADTokenProvider, stop_event, input_queue, callback):
        super().__init__()
        self.endpoint = f"{endpoint}/speechtotext/transcriptions:transcribe?api-version=2024-11-15"
        self.token_provider = token_provider
        self.key = key
        self.stop_event = stop_event
        self.input_queue = input_queue
        self.callback = callback
        self.session = Session()
        self.session.get(self.endpoint)
        self.data = {
                'definition': json.dumps({
                    'locales': [locale],
                    'profanityFilterMode': 'Masked',
                    'channels': [0]
                })
            }

    def run(self) -> None:
        # warm up connection
        self.session.get(self.endpoint)
        while not self.stop_event.is_set():
            array = self.input_queue.get()
            if array is None:
                break
            if self.token_provider is None:
                headers = {
                    'Ocp-Apim-Subscription-Key': self.key
                }
            else:
                headers = {
                    'Authorization': f'Bearer {self.token_provider()}'
                }
            array = float2int(array)
            # open a memory buffer
            tmp = BytesIO()
            with wave.open(tmp, "wb") as wf:
                wf.setnchannels(1)
                wf.setsampwidth(2)
                wf.setframerate(16000)
                wf.writeframes(array.tobytes())

            files = {
                'audio': tmp.getbuffer()
            }
            response = self.session.post(self.endpoint, headers=headers, files=files, data=self.data)
            if response.status_code >= 400:
                print(response.text)
            if self.callback is not None:
                self.callback(response.json())

class AzureFastTranscriptionRecognizer:
    def __init__(self, endpoint: str, token_provider: AzureADTokenProvider = None, key: str = None):
        self.endpoint = endpoint
        self.token_provider = token_provider
        self.key = key
        self.audio_queue = queue.Queue()
        self.vad_queue = queue.Queue()
        self.stop_event = threading.Event()
        self._partial_chunk = b""
        self._on_recognized = None
        vad_model, _ = torch.hub.load("snakers4/silero-vad", "silero_vad")
        self.vad_handler = VADHandler(
            model=vad_model,
            threshold=0.5,
            sampling_rate=16000,
            min_silence_duration_ms=150,
            speech_pad_ms=100,
            stop_event=self.stop_event,
            input_queue=self.audio_queue,
            output_queue=self.vad_queue
        )
        self._locale = "en-US"

    @classmethod
    def cache_model(cls):
        torch.hub.load("snakers4/silero-vad", "silero_vad")

    @property
    def locale(self):
        return self._locale

    @locale.setter
    def locale(self, value: str):
        self._locale = value

    def start(self):
        self.recognizer_client = AzureFastTranscriptionClient(
            endpoint=self.endpoint,
            locale=self.locale,
            key=self.key,
            token_provider=self.token_provider,
            stop_event=self.stop_event,
            input_queue=self.vad_queue,
            callback=self.on_recognized
        )
        threading.Thread(target=self.vad_handler.run).start()
        threading.Thread(target=self.recognizer_client.run).start()

    def start_continuous_recognition(self):
        self.start()

    def stop(self, force=False):
        if force:
            self.stop_event.set()
        self.audio_queue.put(None)

    def stop_continuous_recognition(self):
        self.stop()

    def __call__(self, chunk: bytes):
        self._partial_chunk += chunk
        while len(self._partial_chunk) >= 1024:
            self.audio_queue.put(self._partial_chunk[:1024])
            self._partial_chunk = self._partial_chunk[1024:]

    @property
    def on_recognized(self):
        return self._on_recognized

    @on_recognized.setter
    def on_recognized(self, callback: Callable[[str], None]):
        self._on_recognized = callback

if __name__ == '__main__':
    import os
    from azure.identity import DefaultAzureCredential, get_bearer_token_provider
    token_provider = get_bearer_token_provider(DefaultAzureCredential(), "https://cognitiveservices.azure.com/.default")
    recognizer = AzureFastTranscriptionRecognizer(
        endpoint=os.getenv("SPEECH_ENDPOINT"),
        key=os.getenv("SPEECH_KEY"))
    recognizer.on_recognized = lambda x: print(x)
    recognizer.start()
    with open("tests/4.wav", "rb") as f:
        f.read(44)
        while True:
            chunk = f.read(1600)
            if not chunk:
                break
            recognizer(chunk)
            # time.sleep(0.05)
    recognizer.stop()