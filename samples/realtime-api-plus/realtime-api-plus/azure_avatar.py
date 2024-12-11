# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import asyncio
import json
import logging
import os
import time
from typing import Tuple

import azure.cognitiveservices.speech as speechsdk
from aiohttp import ClientSession
from data_models import IceServer
from azure_tts import AioOutputStream, InputTextStream, InputTextStreamFromQueue, token_provider

logger = logging.getLogger(__name__)

SPEECH_REGION = os.environ.get("SPEECH_REGION")
SPEECH_KEY = os.environ.get('SPEECH_KEY')
SPEECH_RESOURCE_ID = os.environ.get("SPEECH_RESOURCE_ID")
CNV_DEPLOYMENT_ID = os.environ.get("CNV_DEPLOYMENT_ID", "bfab6398-3c4a-4b85-8e7b-c155ca8bfa50")

if not SPEECH_REGION:
    raise ValueError("SPEECH_REGION must be set")

if not (SPEECH_KEY or SPEECH_RESOURCE_ID):
    raise ValueError("SPEECH_KEY or SPEECH_RESOURCE_ID must be set")


class Client:
    def __init__(self, synthesis_pool_size: int = 2):
        if synthesis_pool_size < 1:
            raise ValueError("synthesis_pool_size must be at least 1")
        self.synthesis_pool_size = synthesis_pool_size
        self._counter = 0
        self.voice = None
        self.interruption_time = 0
        self.tts_host = f"https://{SPEECH_REGION}.tts.speech.microsoft.com"

    def configure(self, voice: str):
        logger.info(f"Configuring voice: {voice}")
        endpoint_id = ""
        endpoint_prefix = "tts"
        if voice.startswith("CNV:"):
            voice = voice[4:]
            endpoint_id = CNV_DEPLOYMENT_ID
            endpoint_prefix = "voice"

        self.voice = voice
        endpoint = f"wss://{SPEECH_REGION}.{endpoint_prefix}.speech.microsoft.com/cognitiveservices/websocket/v1?trafficType=RealtimePlus&enableTalkingAvatar=true"

        if SPEECH_KEY:
            self.speech_config = speechsdk.SpeechConfig(subscription=SPEECH_KEY, endpoint=endpoint)
        else:
            auth_token = f"aad#{SPEECH_RESOURCE_ID}#{token_provider()}"
            self.speech_config = speechsdk.SpeechConfig(endpoint=endpoint)
        self.speech_config.speech_synthesis_voice_name = voice
        self.speech_config.endpoint_id = endpoint_id
        self.speech_config.set_speech_synthesis_output_format(
            speechsdk.SpeechSynthesisOutputFormat.Raw24Khz16BitMonoPcm
        )
        # self.speech_config.set_property(speechsdk.PropertyId.Speech_LogFilename, "speechsdk-avatar.log")
        self.speech_synthesizer = speechsdk.SpeechSynthesizer(speech_config=self.speech_config, audio_config=None)
        self.speech_synthesizer.synthesis_started.connect(
            lambda evt: logger.info(f"Synthesis started: {evt.result.reason}")
        )
        self.speech_synthesizer.synthesis_completed.connect(
            lambda evt: logger.info(f"Synthesis completed: {evt.result.reason}")
        )
        self.speech_synthesizer.synthesis_canceled.connect(
            lambda evt: logger.error(f"Synthesis canceled: {evt.result.reason}")
        )
        if not SPEECH_KEY:
            self.speech_synthesizer.authorization_token = auth_token

    def text_to_speech(
        self, voice: str, speed: str = "medium"
    ) -> Tuple[InputTextStream, AioOutputStream]:
        # input_stream = sp
        logger.info(f"Synthesizing text with voice: {voice}")
        # synthesis_request.rate = speed
        current_synthesizer = self.speech_synthesizer
        start_time = time.time()

        aio_stream = AioOutputStream()
        input_stream = InputTextStreamFromQueue()

        async def read_from_data_stream():
            inputs = []
            async for chunk in input_stream:
                inputs.append(chunk)
            contents = "".join(inputs)
            if start_time < self.interruption_time:
                logger.info("Synthesis interrupted, ignoring this turn")
                return
            result = current_synthesizer.start_speaking_text(contents)
            stream = speechsdk.AudioDataStream(result)
            loop = asyncio.get_running_loop()
            while True:
                chunk = bytes(2400 * 4)
                read = await loop.run_in_executor(None, stream.read_data, chunk)
                if read == 0:
                    break
                aio_stream.write_data(chunk[:read])
            if stream.status == speechsdk.StreamStatus.Canceled:
                logger.error(
                    f"Speech synthesis failed: {stream.status}, details: {stream.cancellation_details.error_details}"
                )
            aio_stream.end_of_stream()

        asyncio.create_task(read_from_data_stream())
        return input_stream, aio_stream

    async def interrupt(self):
        self.interruption_time = time.time()
        # todo: uncomment this when the service supports it
        # try:
        #     connection = speechsdk.Connection.from_speech_synthesizer(self.speech_synthesizer)
        #     sending = connection.send_message_async('synthesis.control', '{"action":"stop"}')
        #     loop = asyncio.get_running_loop()
        #     await loop.run_in_executor(None, sending.get)
        # except Exception as e:
        #     logger.error(f"Error interrupting synthesis: {e}")

    async def get_ice_servers(self):
        headers = {}
        if SPEECH_KEY:
            headers["Ocp-Apim-Subscription-Key"] = SPEECH_KEY
        else:
            headers["Authorization"] = f"Bearer aad#{SPEECH_RESOURCE_ID}#{token_provider()}"
        async with ClientSession(headers=headers) as session:
            async with session.get(f"{self.tts_host}/cognitiveservices/avatar/relay/token/v1") as response:
                response.raise_for_status()
                j = await response.text()
                return IceServer.model_validate_json(j)

    async def connect(self, client_description: str, ice_server: IceServer) -> str:
        avatar_config = {
            "synthesis": {
                "video": {
                    "protocol": {
                        "name": "WebRTC",
                        "webrtcConfig": {
                            "clientDescription": client_description,
                            "iceServers": [
                                {
                                    "urls": [ice_server.urls[0]],
                                    "username": ice_server.username,
                                    "credential": ice_server.credential,
                                }
                            ],
                        },
                    },
                    "talkingAvatar": {
                        "customized": False,
                        "character": "meg",
                        "style": "formal",
                    },
                }
            }
        }

        connection = speechsdk.Connection.from_speech_synthesizer(self.speech_synthesizer)
        connection.set_message_property("speech.config", "context", json.dumps(avatar_config))

        loop = asyncio.get_running_loop()
        speech_synthesis_result = await loop.run_in_executor(None, self.speech_synthesizer.speak_text, "")
        if speech_synthesis_result.reason == speechsdk.ResultReason.Canceled:
            cancellation_details = speech_synthesis_result.cancellation_details
            logger.error(
                f"Speech synthesis canceled: {cancellation_details.reason}, result id: {speech_synthesis_result.result_id}"
            )
            if cancellation_details.reason == speechsdk.CancellationReason.Error:
                logger.error(f"Error details: {cancellation_details.error_details}")
                raise Exception(cancellation_details.error_details)
        turn_start_message = self.speech_synthesizer.properties.get_property_by_name(
            "SpeechSDKInternal-ExtraTurnStartMessage"
        )
        if not turn_start_message:
            logger.error("No turn start message")
            raise Exception("No turn start message")
        remote_sdp = json.loads(turn_start_message).get("webrtc", {}).get("connectionString")
        if not remote_sdp:
            logger.error("No remote SDP")
            raise Exception("No remote SDP")
        return remote_sdp
        # self.speech_synthesizer.start_speaking_text_async("hello world")

    def close(self):
        del self.speech_synthesizer


if __name__ == "__main__":

    async def main():
        logging.basicConfig(level=logging.INFO)
        client = Client()
        client.configure("en-US-Andrew:DragonHDLatestNeural")
        token = await client.get_ice_servers()
        print("token", token.model_dump_json())
        # add header to the wave file

    asyncio.run(main())
