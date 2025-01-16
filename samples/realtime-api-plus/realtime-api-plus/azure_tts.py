# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

from abc import ABC, abstractmethod
import asyncio
import logging
import os
import time
from typing import AsyncIterator, Tuple

import azure.cognitiveservices.speech as speechsdk
import numpy as np
from azure.identity import DefaultAzureCredential, get_bearer_token_provider

logger = logging.getLogger(__name__)

SPEECH_REGION = os.environ.get('SPEECH_REGION')
SPEECH_KEY = os.environ.get('SPEECH_KEY')
SPEECH_RESOURCE_ID = os.environ.get("SPEECH_RESOURCE_ID")
CNV_DEPLOYMENT_ID = os.environ.get("CNV_DEPLOYMENT_ID", "bfab6398-3c4a-4b85-8e7b-c155ca8bfa50")

if not SPEECH_REGION:
    raise ValueError("SPEECH_REGION must be set")

if not (SPEECH_KEY or SPEECH_RESOURCE_ID):
    raise ValueError("SPEECH_KEY or SPEECH_RESOURCE_ID must be set")

token_provider = get_bearer_token_provider(DefaultAzureCredential(), "https://cognitiveservices.azure.com/.default")


def calculate_energy(frame_data):
    # Convert the byte data to a numpy array for easier processing (assuming 16-bit PCM)
    data = np.frombuffer(frame_data, dtype=np.int16)
    # Calculate the energy as the sum of squares of the samples
    energy = np.sum(data**2) / len(data)
    return energy

class AioOutputStream:
    def __init__(self):
        self._queue = asyncio.Queue()

    def write_data(self, data: bytes):
        self._queue.put_nowait(data)

    def end_of_stream(self):
        self._queue.put_nowait(None)

    async def read(self) -> bytes:
        chunk = await self._queue.get()
        if chunk is None:
            raise StopAsyncIteration
        return chunk

    def __aiter__(self) -> AsyncIterator[bytes]:
        return self

    async def __anext__(self):
        return await self.read()

class InputTextStream(ABC):
    @abstractmethod
    def write(self, data: str):
        pass

    @abstractmethod
    def close(self):
        pass

class InputTextStreamFromSDK(InputTextStream):
    def __init__(self, sdk_stream: speechsdk.SpeechSynthesisRequest.InputStream):
        self.sdk_stream = sdk_stream

    def write(self, data: str):
        self.sdk_stream.write(data)

    def close(self):
        self.sdk_stream.close()

class InputTextStreamFromQueue(InputTextStream):
    def __init__(self):
        self._queue = asyncio.Queue()

    def write(self, data: str):
        self._queue.put_nowait(data)

    def close(self):
        self._queue.put_nowait(None)

    async def read(self) -> str:
        chunk = await self._queue.get()
        if chunk is None:
            raise StopAsyncIteration
        return chunk

    def __aiter__(self) -> AsyncIterator[str]:
        return self

    async def __anext__(self):
        return await self.read()

class Client:
    def __init__(self, synthesis_pool_size: int = 2):
        if synthesis_pool_size < 1:
            raise ValueError("synthesis_pool_size must be at least 1")
        self.synthesis_pool_size = synthesis_pool_size
        self._counter = 0
        self.voice = None
        self.interruption_time = 0

    def configure(self, voice: str):
        logger.info(f"Configuring voice: {voice}")
        self.endpoint_id = ""
        endpoint_prefix = "tts"
        endpoint_version = "v2"
        if voice.startswith("CNV:"):
            voice = voice[4:]
            self.endpoint_id = CNV_DEPLOYMENT_ID
            endpoint_prefix = "voice"
            endpoint_version = "v1"

        self.voice = voice

        endpoint = endpoint=f"wss://{SPEECH_REGION}.{endpoint_prefix}.speech.microsoft.com/cognitiveservices/websocket/{endpoint_version}?debug=3&trafficType=AzureSpeechRealtime"

        if SPEECH_KEY:
            self.speech_config = speechsdk.SpeechConfig(subscription=SPEECH_KEY, endpoint=endpoint)
        else:
            auth_token = f"aad#{SPEECH_RESOURCE_ID}#{token_provider()}"
            self.speech_config = speechsdk.SpeechConfig(endpoint=endpoint)
        self.speech_config.speech_synthesis_voice_name = voice
        self.speech_config.endpoint_id = self.endpoint_id
        self.speech_config.set_speech_synthesis_output_format(speechsdk.SpeechSynthesisOutputFormat.Raw24Khz16BitMonoPcm)
        self.speech_synthesizers = [speechsdk.SpeechSynthesizer(speech_config=self.speech_config, audio_config=None) for _ in range(self.synthesis_pool_size)]
        for s in self.speech_synthesizers:
            s.synthesis_started.connect(lambda evt: logger.info(f"Synthesis started: {evt.result.reason}"))
            s.synthesis_completed.connect(lambda evt: logger.info(f"Synthesis completed: {evt.result.reason}"))
            s.synthesis_canceled.connect(lambda evt: logger.error(f"Synthesis canceled: {evt.result.reason}"))
            if not SPEECH_KEY:
                s.authorization_token = auth_token

    def interrupt(self):
        self.interruption_time = time.time()

    def text_to_speech(self, voice: str, speed: str = "medium") -> Tuple[InputTextStream, AioOutputStream]:
        start_time = time.time()
        if self.endpoint_id:
            # CNV does not support input streaming
            return self.text_to_speech_non_streaming(voice, speed)
        logger.info(f"Synthesizing text with voice: {voice}")
        synthesis_request = speechsdk.SpeechSynthesisRequest(
            input_type=speechsdk.SpeechSynthesisRequestInputType.TextStream)
        # synthesis_request.rate = speed
        self._counter = (self._counter + 1) % len(self.speech_synthesizers)
        current_synthesizer = self.speech_synthesizers[self._counter]

        result = current_synthesizer.start_speaking(synthesis_request)
        stream = speechsdk.AudioDataStream(result)
        aio_stream = AioOutputStream()
        async def read_from_data_stream():
            leading_silence_skipped = False
            silence_detection_frames_size = int(50 * 24000 * 2 / 1000)  # 50 ms
            loop = asyncio.get_running_loop()
            while True:
                if not leading_silence_skipped:
                    if stream.position >= 3 * silence_detection_frames_size:
                        leading_silence_skipped = True
                        continue
                    frame_data = bytes(silence_detection_frames_size)
                    lenx = await loop.run_in_executor(None, stream.read_data, frame_data)
                    if lenx == 0:
                        if stream.status != speechsdk.StreamStatus.AllData:
                            logger.error(f"Speech synthesis failed: {stream.status}, details: {stream.cancellation_details.error_details}")
                        break
                    energy = await loop.run_in_executor(None, calculate_energy, frame_data)
                    if energy < 500:
                        logger.info("Silence detected, skipping")
                        continue
                    leading_silence_skipped = True
                    stream.position = stream.position - silence_detection_frames_size
                chunk = bytes(2400*4)
                read = await loop.run_in_executor(None, stream.read_data, chunk)
                if read == 0:
                    break
                if start_time < self.interruption_time:
                    logger.warning("Interruption detected, stopping synthesis")
                    break
                aio_stream.write_data(chunk[:read])
            if stream.status == speechsdk.StreamStatus.Canceled:
                logger.error(f"Speech synthesis failed: {stream.status}, details: {stream.cancellation_details.error_details}")
            aio_stream.end_of_stream()

        asyncio.create_task(read_from_data_stream())
        return InputTextStreamFromSDK(synthesis_request.input_stream), aio_stream

    def text_to_speech_non_streaming(self, voice: str, speed: str = "medium") -> Tuple[InputTextStream, AioOutputStream]:
        """
        This method is to support non-streaming TTS synthesis. e.g., CNV
        """
        start_time = time.time()
        logger.warning("non-streaming TTS synthesis is being used")
        logger.info(f"Synthesizing text with voice: {voice}")
        self._counter = (self._counter + 1) % len(self.speech_synthesizers)
        current_synthesizer = self.speech_synthesizers[self._counter]

        input_stream = InputTextStreamFromQueue()
        aio_stream = AioOutputStream()
        async def read_from_data_stream():
            inputs = []
            async for chunk in input_stream:
                inputs.append(chunk)
            result = current_synthesizer.start_speaking_text("".join(inputs))
            stream = speechsdk.AudioDataStream(result)
            leading_silence_skipped = False
            silence_detection_frames_size = int(50 * 24000 * 2 / 1000)  # 50 ms
            loop = asyncio.get_running_loop()
            while True:
                if not leading_silence_skipped:
                    if stream.position >= 3 * silence_detection_frames_size:
                        leading_silence_skipped = True
                        continue
                    frame_data = bytes(silence_detection_frames_size)
                    lenx = await loop.run_in_executor(None, stream.read_data, frame_data)
                    if lenx == 0:
                        if stream.status != speechsdk.StreamStatus.AllData:
                            logger.error(f"Speech synthesis failed: {stream.status}, details: {stream.cancellation_details.error_details}")
                        break
                    energy = await loop.run_in_executor(None, calculate_energy, frame_data)
                    if energy < 500:
                        logger.info("Silence detected, skipping")
                        continue
                    leading_silence_skipped = True
                    stream.position = stream.position - silence_detection_frames_size
                chunk = bytes(2400*4)
                read = await loop.run_in_executor(None, stream.read_data, chunk)
                if read == 0:
                    break
                if start_time < self.interruption_time:
                    logger.warning("Interruption detected, stopping synthesis")
                    break
                aio_stream.write_data(chunk[:read])
            if stream.status == speechsdk.StreamStatus.Canceled:
                logger.error(f"Speech synthesis failed: {stream.status}, details: {stream.cancellation_details.error_details}")
            aio_stream.end_of_stream()

        asyncio.create_task(read_from_data_stream())
        return input_stream, aio_stream

if __name__ == "__main__":
    async def main():
        logging.basicConfig(level=logging.INFO)
        client = Client()
        print("client", client)
        input, output = client.text_to_speech("en-US-Andrew:DragonHDLatestNeural")

        async def read_output():
            audio = b''
            async for chunk in output:
                print(len(chunk))
                audio += chunk
            with open("output.wav", "wb") as f:
                f.write(b'RIFF')
                f.write((36 + len(audio)).to_bytes(4, 'little'))
                f.write(b'WAVE')
                f.write(b'fmt ')
                f.write((16).to_bytes(4, 'little'))
                f.write((1).to_bytes(2, 'little'))
                f.write((1).to_bytes(2, 'little'))
                f.write((24000).to_bytes(4, 'little'))
                f.write((48000).to_bytes(4, 'little'))
                f.write((2).to_bytes(2, 'little'))
                f.write((16).to_bytes(2, 'little'))
                f.write(b'data')
                f.write((len(audio)).to_bytes(4, 'little'))
                f.write(audio)
        async def put_input():
            for c in ['Hello,', ' world!', '我', '是', '好问题！这方面的确内容丰富。简而言之，中国的主要传统节日包括春节（农历新年）、元宵节、清明节、端午节和中秋节。每个节日都有其独特的庆祝方式和传统，比如春节时人们会放鞭炮、吃年夜饭、发红包；端午节则有赛龙舟和吃粽子的习俗。这些节日不仅是家庭团聚的时刻，也蕴含着丰富的历史和文化意义。你对哪个节日最感兴趣？', '。']:
                input.write(c)
            input.close()
                # await asyncio.sleep(1)
        await asyncio.gather(read_output(), put_input())
        # add header to the wave file

    asyncio.run(main())
