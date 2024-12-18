import logging
import os
import typing

import azure.cognitiveservices.speech as speechsdk
import numpy as np
from azure.identity import DefaultAzureCredential, get_bearer_token_provider

SPEECH_REGION = os.environ.get('SPEECH_REGION')
SPEECH_KEY = os.environ.get('SPEECH_KEY')
SPEECH_RESOURCE_ID = os.environ.get("SPEECH_RESOURCE_ID")

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


class Client:
    def __init__(self, synthesis_pool_size: int = 2):
        if synthesis_pool_size < 1:
            raise ValueError("synthesis_pool_size must be at least 1")
        if SPEECH_KEY:
            self.speech_config = speechsdk.SpeechConfig(subscription=SPEECH_KEY, region=SPEECH_REGION)
        else:
            auth_token = f"aad#{SPEECH_RESOURCE_ID}#{token_provider()}"
            self.speech_config = speechsdk.SpeechConfig(endpoint=f"wss://{SPEECH_REGION}.tts.speech.microsoft.com/cognitiveservices/websocket/v1")
        self.speech_config.set_speech_synthesis_output_format(speechsdk.SpeechSynthesisOutputFormat.Raw24Khz16BitMonoPcm)
        self.speech_synthesizers = [speechsdk.SpeechSynthesizer(speech_config=self.speech_config, audio_config=None) for _ in range(synthesis_pool_size)]
        if not SPEECH_KEY:
            for synthesizer in self.speech_synthesizers:
                synthesizer.authorization_token = auth_token
        self._counter = 0

    def text_to_speech(self, text: str, voice: str, speed: str = "medium") -> typing.Iterator[bytes]:
        self._counter = (self._counter + 1) % len(self.speech_synthesizers)
        current_synthesizer = self.speech_synthesizers[self._counter]
        current_synthesizer.properties.set_property(speechsdk.PropertyId.SpeechServiceConnection_SynthVoice, voice)
        ssml = f"<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xml:lang='en-US'><voice name='{voice}'><prosody rate='{speed}'>{text}</prosody></voice></speak>"
        result = current_synthesizer.start_speaking_ssml(ssml)
        stream = speechsdk.AudioDataStream(result)
        first = True
        leading_silence_skipped = False
        silence_detection_frames_size = int(50 * 24000 * 2 / 1000)  # 50 ms
        while True:
            if not leading_silence_skipped:
                if stream.position >= 3 * silence_detection_frames_size:
                    leading_silence_skipped = True
                    continue
                frame_data = bytes(silence_detection_frames_size)
                read_len = stream.read_data(frame_data)
                if read_len == 0:
                    break

                energy = calculate_energy(frame_data)
                if energy < 500:
                    print("Silence detected, skipping")
                    continue
                leading_silence_skipped = True
                stream.position = stream.position - silence_detection_frames_size
            chunk = bytes(2400*4)  # 200 ms duration
            read = stream.read_data(chunk)
            if read == 0:
                break
            yield chunk[:read]
        if stream.status != speechsdk.StreamStatus.AllData:
            logging.error(f"Speech synthesis failed: {stream.status}, details: {stream.cancellation_details.error_details}")


if __name__ == "__main__":
    client = Client()
    for chunk in client.text_to_speech("Hello, world!", "zh-CN-XiaoxiaoMultilingualNeural"):
        print(len(chunk))