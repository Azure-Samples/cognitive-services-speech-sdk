# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import asyncio
import copy
import logging
from rtclient import RTClient
import os
from rtclient import models as rt_models
from azure.identity.aio import DefaultAzureCredential
from azure.core.credentials import AzureKeyCredential
from realtime_audio_session_handler import RealtimeAudioSessionHandler
import rtclient
from azure_avatar import Client as AzureAvatarClient
from data_models import Session
endpoint = os.getenv("AZURE_OPENAI_ENDPOINT")
key = os.getenv("AZURE_OPENAI_KEY")
deployment = os.getenv("AZURE_OPENAI_DEPLOYMENT")
logger = logging.getLogger(__name__)


class GPT4oAzureAvatarClient:
    def __init__(self, realtime_handler: RealtimeAudioSessionHandler):
        if key:
            self._rt_client = self._client = RTClient(
                url=endpoint, azure_deployment=deployment, key_credential=AzureKeyCredential(key))
        else:
            self._rt_client = self._client = RTClient(
                url=endpoint, azure_deployment=deployment, token_credential=DefaultAzureCredential())
        self._realtime_handler = realtime_handler
        self._tts_client = AzureAvatarClient()
        self._voice = None

    async def connect(self):
        await self._rt_client.connect()
        await self._realtime_handler.on_session_created(self._rt_client.session)

    async def configure(self, session_config: rt_models.SessionUpdateParams) -> Session:
        self._voice = session_config.voice or "en-US-AvaNeural"
        self._tts_client.configure(voice=self._voice)
        ice_server = self._tts_client.get_ice_servers()
        response = await self._rt_client.configure(
            model=session_config.model,
            modalities=set(['text']),
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
        response = Session(**response.model_dump())
        self.ice_server = await ice_server
        logger.warning(f"ICE server: {self.ice_server}")
        response.ice_servers = [self.ice_server]
        return response

    async def send_audio(self, audio_message: rt_models.InputAudioBufferAppendMessage):
        return await self._rt_client._client.send(audio_message)

    async def send_item(self, item: rt_models.Item, previous_item_id: str):
        return await self._rt_client.send_item(item, previous_item_id)

    async def generate_response(self) -> None:
        return await self._rt_client._client.send(rt_models.ResponseCreateMessage())

    async def clear_audio(self) -> None:
        return await self._rt_client.clear_audio()

    async def connect_avatar(self, client_description: str) -> str:
        return await self._tts_client.connect(client_description, self.ice_server)

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
                audio_content_part = rt_models.ResponseItemAudioContentPart(
                    transcript=""
                )
                tts_input, tts_output = self._tts_client.text_to_speech(voice=self._voice)
                await self._realtime_handler.on_response_content_part_added(response_id, item_id, m.content_index, audio_content_part)
                async def receive_tts_output():
                    async for a in tts_output:
                        await self._realtime_handler.on_audio_chunk(response_id, item_id, m.content_index, a)
                    await self._realtime_handler.on_audio_done(response_id, item_id, m.content_index)
                async def receive_text_chunks(chunks):
                    async for t in chunks:
                        # todo: hack: replace double \n with single \n
                        if t == "\n\n":
                            t = "\n"
                        tts_input.write(t)
                        await self._realtime_handler.on_transcript_chunk(response_id, item_id, m.content_index, t)
                    logger.warning("End of transcript")
                    tts_input.close()
                await asyncio.gather(receive_tts_output(), receive_text_chunks(m.text_chunks()))
        audio_part = rt_models.ResponseItemAudioContentPart(
            transcript=m._part.text
        )
        await self._realtime_handler.on_response_content_part_done(response_id, item_id, m.content_index, audio_part)
        if len(message._item.content) == 0:
            return
        audio_item = copy.deepcopy(message._item)
        audio_item.content[0] = audio_part
        await self._realtime_handler.on_response_output_item_done(response_id, audio_item)

    async def receive_function_call_item(self, item: rtclient.RTFunctionCallItem, response_id: str):
        await self._realtime_handler.on_response_output_item_added(item.id, item._item)
        async for m in item:
            await self._realtime_handler.on_response_function_call_arguments_delta(response_id, item.id, call_id=item.call_id, delta=m)
        await self._realtime_handler.on_response_function_call_arguments_done(
            response_id, item.id, call_id=item.call_id, name=item.function_name, arguments=item.arguments)
        await self._realtime_handler.on_response_output_item_done(item.id, item._item)

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
            elif type(item) is rtclient.RTFunctionCallItem:
                await self._realtime_handler.on_response_output_item_added(item.id, item._item)
                await self._realtime_handler.on_conversation_item_created(item.previous_id, item._item)
                t = asyncio.create_task(self.receive_function_call_item(item, response_id=response_id))
                tasks.append(t)
        await asyncio.gather(*tasks)
        await self._realtime_handler.on_response_done(response._response)

    async def receive_input_item(self, item: rtclient.RTInputAudioItem):
        await self._realtime_handler.on_input_audio_buffer_speech_started(item.id, item.audio_start_ms)
        await self._tts_client.interrupt()
        await item
        await self._realtime_handler.on_input_audio_buffer_speech_stopped(item.id, item.audio_end_ms)
        if item.transcript:
            await self._realtime_handler.on_conversation_item_input_audio_transcript_completed(item.id, item.transcript)

    async def receive_events(self):
        async for event in self._rt_client.events():
            if type(event) is rtclient.RTInputAudioItem:
                asyncio.create_task(self.receive_input_item(event))
            if type(event) is rtclient.RTResponse:
                asyncio.create_task(self.receive_response(event))

    async def receive_messages(self):
        await asyncio.gather(
            self.receive_events(),
        )

    async def close(self):
        self._tts_client.close()
        await self._rt_client.close()
