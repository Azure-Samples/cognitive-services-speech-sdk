# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import asyncio
import base64
import logging
import os
from typing import Optional

import aiohttp
import rtclient
from azure.core.credentials import AzureKeyCredential
from azure.identity.aio import DefaultAzureCredential
from azure_avatar import Client as AzureAvatarClient
from data_models import Session
from realtime_audio_session_handler import RealtimeAudioSessionHandler
from rtclient import RTClient
from rtclient import models as rt_models

logger = logging.getLogger(__name__)
endpoint = os.getenv("AZURE_OPENAI_ENDPOINT")
key = os.getenv("AZURE_OPENAI_KEY")
deployment = os.getenv("AZURE_OPENAI_DEPLOYMENT")
# note: gpt4o real-time + avatar (audio driven) is in preview
enable_avatar = os.getenv("ENABLE_AVATAR")
avatar_service_host = os.getenv("AVATAR_SERVICE_HOST", "")
if enable_avatar and not avatar_service_host:
    raise ValueError("Avatar service host must be set")
if not avatar_service_host.startswith("ws"):
    avatar_service_host = f"wss://{avatar_service_host}"

ice_servers = [None]


class GPT4OClient:
    def __init__(
        self,
        realtime_handler: RealtimeAudioSessionHandler,
        url: Optional[str] = None,
        key_credential: Optional[AzureKeyCredential] = None,
        azure_deployment: Optional[str] = None,
    ):

        self._realtime_handler = realtime_handler
        self._token_credential = DefaultAzureCredential()

        if url is None:
            if endpoint is None:
                raise ValueError("No gpt4o endpoint provided")
            logger.info("Using gpt4o endpoint from environment variables")
            if key:
                self._client = RTClient(url=endpoint, azure_deployment=deployment, key_credential=AzureKeyCredential(key))
            else:
                self._client = RTClient(url=endpoint, azure_deployment=deployment, token_credential=self._token_credential)
        else:
            logger.info("Using gpt4o deployment from user input")
            self._client = RTClient(url=url, azure_deployment=azure_deployment, key_credential=key_credential)

        self.avatar_ws_client = None

    async def connect(self):
        await self._client.connect()
        await self._realtime_handler.on_session_created(self._client.session)
        if enable_avatar:
            auth = await self._get_auth()
            self.avatar_ws_client_session = aiohttp.ClientSession(headers=auth)
            self.avatar_ws_client = await self.avatar_ws_client_session.ws_connect(
                f"{avatar_service_host}/talking-avatar/live"
            )
            if self.avatar_ws_client.closed:
                raise ConnectionError("Failed to connect to the avatar WebSocket service")

    async def _get_auth(self):
        scope = "https://cognitiveservices.azure.com/.default"
        token = await self._token_credential.get_token(scope)
        return {"Authorization": f"Bearer {token.token}"}

    async def configure(self, session_config: rt_models.SessionUpdateParams) -> Session:
        response = await self._client.configure(
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
            max_response_output_tokens=session_config.max_response_output_tokens,
        )
        if enable_avatar:
            if not ice_servers[0]:
                ice_servers[0] = await AzureAvatarClient().get_ice_servers()
            response = Session(**response.model_dump())
            self.ice_server = ice_servers[0]
            response.ice_servers = [self.ice_server]
        return response

    async def send_audio(self, audio_message: rt_models.InputAudioBufferAppendMessage):
        return await self._client._client.send(audio_message)

    async def connect_avatar(self, client_description: str) -> str:
        await self.avatar_ws_client.send_json(
            {
                "type": "session.update",
                "session": {
                    "avatar": {
                        "character": "meg",
                        "style": "formal",
                    },
                    "ice_servers": [
                        {
                            "urls": [self.ice_server.urls[0]],
                            "username": self.ice_server.username,
                            "credential": self.ice_server.credential,
                        }
                    ],
                    "input_audio": {
                        "sample_rate": 24000,
                    },
                },
            }
        )
        r = await self.avatar_ws_client.receive()
        logger.info(f"Received message: {r.data}")
        await self.avatar_ws_client.send_json(
            {"type": "webrtc.local.description", "local_description": client_description}
        )
        msg = await self.avatar_ws_client.receive()
        return msg.json()["remote_description"]

    async def receive_message_item(self, message: rt_models.MessageItem, response_id: str):
        item_id = message.id
        async for m in message:
            if type(m) is rtclient.RTAudioContent:
                await self._realtime_handler.on_response_content_part_added(
                    response_id, item_id, m.content_index, m._part
                )

                async def on_audio_chunk(chunks):
                    if enable_avatar:
                        await self.avatar_ws_client.send_json({"type": "audio.start", "turn_id": item_id})
                    async for a in chunks:
                        if enable_avatar:
                            await self.avatar_ws_client.send_json(
                                {"type": "audio.delta", "chunk": base64.b64encode(a).decode(), "turn_id": item_id}
                            )
                        else:
                            await self._realtime_handler.on_audio_chunk(response_id, item_id, m.content_index, a)
                    if enable_avatar:
                        await self.avatar_ws_client.send_json({"type": "audio.end", "turn_id": item_id})

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
        if enable_avatar:
            await self.avatar_ws_client.send_json({"type": "input.interrupt"})
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
        if enable_avatar:
            await self.avatar_ws_client.close()
            await self.avatar_ws_client_session.close()
