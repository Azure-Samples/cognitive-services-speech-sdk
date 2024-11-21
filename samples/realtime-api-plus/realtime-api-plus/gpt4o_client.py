# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import asyncio
import logging
import os
from typing import Optional

import rtclient
from azure.core.credentials import AzureKeyCredential
from azure.identity.aio import DefaultAzureCredential
from realtime_audio_session_handler import RealtimeAudioSessionHandler
from rtclient import RTClient
from rtclient import models as rt_models

logger = logging.getLogger(__name__)
endpoint = os.getenv("AZURE_OPENAI_ENDPOINT")
deployment = os.getenv("AZURE_OPENAI_DEPLOYMENT")

class GPT4OClient:
    def __init__(self,
                 realtime_handler: RealtimeAudioSessionHandler,
        url: Optional[str] = None,
        key_credential: Optional[AzureKeyCredential] = None,
        azure_deployment: Optional[str] = None,):

        self._realtime_handler = realtime_handler

        if url is None:
            if endpoint is None:
                raise ValueError("No gpt4o endpoint provided")
            self._client = RTClient(url=endpoint, azure_deployment=deployment, token_credential=DefaultAzureCredential())
        else:
            logger.info("Using gpt4o deployment from user input")
            self._client = RTClient(url=url, azure_deployment=azure_deployment, key_credential=key_credential)

    async def connect(self):
        await self._client.connect()
        await self._realtime_handler.on_session_created(self._client.session)

    async def configure(self, session_config: rt_models.SessionUpdateParams):
        return await self._client.configure(
            model=session_config.model,
            modalities=session_config.modalities,
            voice=session_config.voice,
            instructions=session_config.instructions,
            input_audio_format=session_config.input_audio_format,
            output_audio_format=session_config.output_audio_format,
            input_audio_transcription=session_config.input_audio_transcription,
            turn_detection=session_config.turn_detection,
            tools=session_config.tools,
            tool_choice=session_config.tool_choice,
            temperature=session_config.temperature,
            max_response_output_tokens=session_config.max_response_output_tokens
        )

    async def send_audio(self, audio_message: rt_models.InputAudioBufferAppendMessage):
        return await self._client._client.send(audio_message)

    async def receive_message_item(self, message: rt_models.MessageItem, response_id: str):
        item_id = message.id
        async for m in message:
            if type(m) is rtclient.RTAudioContent:
                await self._realtime_handler.on_response_content_part_added(response_id, item_id, m.content_index, m._part)
                async def on_audio_chunk(chunks):
                    async for a in chunks:
                        await self._realtime_handler.on_audio_chunk(response_id, item_id, m.content_index, a)
                async def on_transcript_chunk(chunks):
                    async for t in chunks:
                        await self._realtime_handler.on_transcript_chunk(response_id, item_id, m.content_index, t)
                await asyncio.gather(on_audio_chunk(m.audio_chunks()), on_transcript_chunk(m.transcript_chunks()))
            elif type(m) is rtclient.RTTextContent:
                raise NotImplementedError
        await self._realtime_handler.on_response_content_part_done(response_id, item_id, m.content_index, m._part)
        await self._realtime_handler.on_response_output_item_done(response_id, message._item)


    async def receive_response(self, response: rtclient.RTResponse):
        await self._realtime_handler.on_response_created(response._response)
        response_id = response.id
        tasks = []
        async for item in response:
            if item.type == "message":
                await self._realtime_handler.on_response_output_item_added(response_id, item._item)
                await self._realtime_handler.on_conversation_item_created(item.previous_id, item._item)
                t = asyncio.create_task(self.receive_message_item(item, response_id))
                tasks.append(t)
            elif item.type == "audio":
                pass
        await asyncio.gather(*tasks)
        await self._realtime_handler.on_response_done(response._response)

    async def receive_input_item(self, item: rtclient.RTInputAudioItem):
        await self._realtime_handler.on_input_audio_buffer_speech_started(item.id, item.audio_start_ms)
        await item
        await self._realtime_handler.on_input_audio_buffer_speech_stopped(item.id, item.audio_end_ms)
        if item.transcript:
            await self._realtime_handler.on_conversation_item_input_audio_transcript_completed(item.id, item.transcript)

    async def receive_events(self):
        async for event in self._client.events():
            if type(event) is rtclient.RTInputAudioItem:
                asyncio.create_task(self.receive_input_item(event))
            if type(event) is rtclient.RTResponse:
                asyncio.create_task(self.receive_response(event))

    async def receive_messages(self):
        await asyncio.gather(
            self.receive_events(),
        )

    async def close(self):
        await self._client.close()