# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import asyncio
import logging

import data_models as custom_models
import rtclient
import rtclient.low_level_client
from aiohttp import WSMsgType
from aiohttp.web import Request, WebSocketResponse
from azure.core.credentials import AzureKeyCredential
from gpt4o_azure_voice_client import GPT4oAzureVoiceClient
from gpt4o_azure_avatar_client import GPT4oAzureAvatarClient
from gpt4o_client import GPT4OClient
from realtime_audio_session_handler import RealtimeAudioSessionHandlerImpl
from rtclient import models as rt_models

logger = logging.getLogger(__name__)


class RealtimeAudioSession(RealtimeAudioSessionHandlerImpl):
    def __init__(self, deployment: str = "gpt4o-realtime", gpt4o_endpoint: str = None, aoai_api_key: str = None):
        if deployment == "gpt4o-realtime":
            if gpt4o_endpoint is None:
                self._client = GPT4OClient(realtime_handler=self)
            else:
                self._client = GPT4OClient(realtime_handler=self, url=gpt4o_endpoint, key_credential=AzureKeyCredential(aoai_api_key))
        elif deployment == "gpt4o-realtime-azure-voice":
            self._client = GPT4oAzureVoiceClient(realtime_handler=self)
        elif deployment == "gpt4o-realtime-azure-avatar":
            self._client = GPT4oAzureAvatarClient(realtime_handler=self)
        super().__init__()

    async def connect(self):
        await self._client.connect()

    async def configure(self, session_config: custom_models.SessionUpdateParams) -> rt_models.Session:
        return await self._client.configure(session_config)

    async def send_audio(self, audio_message: rt_models.InputAudioBufferAppendMessage):
        return await self._client.send_audio(audio_message)

    async def send_item(self, item: rt_models.Item, previous_item_id: str):
        return await self._client.send_item(item, previous_item_id)

    async def generate_response(self) -> rtclient.RTResponse:
        return await self._client.generate_response()

    async def clear_audio(self) -> None:
        await self._client.clear_audio()

    async def receive_messages(self):
        asyncio.gather(self._client.receive_messages())

    async def connect_avatar(self, client_description: str):
        return await self._client.connect_avatar(client_description)

    async def close(self):
        await self._client.close()


class WebSocketServer:
    def __init__(self):
        self._websockets = set()

    async def websocket_handler(self, request: Request):
        # get query params from request
        params = request.query
        logger.info(f"Received request with query params: {params}")
        session = RealtimeAudioSession(deployment=params.get("deployment"), gpt4o_endpoint=params.get("gpt4o_endpoint"), aoai_api_key=params.get("aoai_api_key"))
        try:
            await session.connect()
        except rtclient.low_level_client.ConnectionError as e:
            logger.error(f"Error connecting to session: {e}, {e.headers}")
            ws = WebSocketResponse()
            # todo: check if this is the correct status code
            ws.close(code=429, message="Too many requests")
            return ws

        ws = WebSocketResponse()
        await ws.prepare(request)
        self._websockets.add(ws)
        config = await ws.receive_json()
        logger.info(f"Received config: {config}")
        session_config = custom_models.SessionUpdateParams.model_validate(config.get("session"))
        configure_response = await session.configure(session_config)
        configure_response_message = custom_models.SessionUpdatedMessage(
            session=configure_response,
            event_id="event_AIsUqs9MsJYCeO1U0rGEY"
        )
        await ws.send_str(configure_response_message.model_dump_json(exclude_none=True))
        asyncio.gather(session.receive_messages())

        async def send_output_message():
            while True:
                message = await session.output_queue.get()
                await ws.send_str(message)
        asyncio.create_task(send_output_message())

        async for msg in ws:
            if msg.type == WSMsgType.TEXT:
                payload = msg.json()
                match payload.get("type"):
                    case "input_audio_buffer.append":
                        audio_message = rt_models.InputAudioBufferAppendMessage.model_validate(payload)
                        await session.send_audio(audio_message)
                    case "conversation.item.create":
                        item_create_message = rt_models.ItemCreateMessage.model_validate(payload)
                        response = await session.send_item(item_create_message.item, item_create_message.previous_item_id)
                        await session.on_conversation_item_created(previous_id=item_create_message.previous_item_id, item=response)
                    case "response.create":
                        await session.generate_response()
                    case "input_audio_buffer.clear":
                        await session.clear_audio()
                    case "extension.avatar.connect":
                        client_description = payload.get("client_description")
                        async def connect_avatar():
                            remote_sdp = await session.connect_avatar(client_description)
                            await ws.send_str(custom_models.AvatarConnectingMessage(
                                server_description=remote_sdp,
                                event_id="event_AIsUqs9MsJYCeO1U0rGEY"
                            ).model_dump_json(exclude_none=True))
                        asyncio.create_task(connect_avatar())
                    case _:
                        logger.warning(f"Unknown message type: {payload.get('type')}")
            elif msg.type == WSMsgType.ERROR:
                break
        await session.close()
        self._websockets.remove(ws)
        return ws


if __name__ == '__main__':
    from aiohttp import web

    app = web.Application()
    server = WebSocketServer()
    app.router.add_get('/openai/realtime', server.websocket_handler)
    web.run_app(app)