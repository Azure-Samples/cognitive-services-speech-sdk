# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import asyncio
import base64
from rtclient import models as rt_models

class RealtimeAudioSessionHandler:
    def __init__(self) -> None:
        pass

    async def on_session_created(self, session: rt_models.Session):
        raise NotImplementedError

    async def on_response_created(self, response: rt_models.Response):
        raise NotImplementedError

    async def on_response_done(self, response: rt_models.Response):
        raise NotImplementedError

    async def on_response_output_item_added(self, response_id: str, item: rt_models.ResponseItem):
        raise NotImplementedError

    async def on_response_output_item_done(self, response_id: str, item: rt_models.ResponseItem):
        raise NotImplementedError

    async def on_conversation_item_created(self, response_id: str, item: rt_models.ResponseItem):
        raise NotImplementedError

    async def on_transcript_start(self, response_id: str, item_id: str, content_index: int, content: rt_models.ResponseItemContentPart):
        raise NotImplementedError

    async def on_transcript_end(self, response_id: str, item_id: str, content_index: int, content: rt_models.ResponseItemContentPart):
        raise NotImplementedError

    async def on_audio_chunk(self, response_id: str, item_id: str, content_index: str, audio_chunk: bytes):
        raise NotImplementedError

    async def on_audio_done(self, response_id: str, item_id: str, content_index: str):
        raise NotImplementedError

    async def on_transcript_chunk(self, response_id: str, item_id: str, content_index: str, transcript_chunk: str):
        raise NotImplementedError

    async def on_response_content_part_added(self, response_id: str, item_id: str, content_index: int, content: rt_models.ResponseItemContentPart):
        raise NotImplementedError

    async def on_response_content_part_done(self, response_id: str, item_id: str, content_index: int, content: rt_models.ResponseItemContentPart):
        raise NotImplementedError

    ## input audio
    async def on_input_audio_buffer_speech_started(self, item_id: str, audio_start_ms: int):
        raise NotImplementedError
    async def on_input_audio_buffer_speech_stopped(self, item_id: str, audio_end_ms: int):
        raise NotImplementedError
    async def on_input_audio_buffer_committed(self, item_id: str, previous_item_id: str):
        raise NotImplementedError
    async def on_conversation_item_input_audio_transcript_completed(self, item_id: str, transcript: str):
        raise NotImplementedError

    ## function call
    async def on_response_function_call_arguments_delta(self, response_id: str, item_id: str, call_id: str, delta: str):
        raise NotImplementedError
    async def on_response_function_call_arguments_done(self, response_id: str, item_id: str, call_id: str, name: str, arguments: str):
        raise NotImplementedError

class AsyncMsgQueue:
    def __init__(self):
        self.queue = asyncio.Queue()

    async def put(self, frame):
        await self.queue.put(frame)

    async def get(self):
        return await self.queue.get()

    def size(self):
        return self.queue.qsize()


class RealtimeAudioSessionHandlerImpl(RealtimeAudioSessionHandler):
    def __init__(self) -> None:
        self._output_queue = AsyncMsgQueue()

    @property
    def output_queue(self):
        return self._output_queue

    def generate_event_id(self):
        return "event_AIsUqs9MsJYCeO1U0rGEY"

    async def on_session_created(self, session: rt_models.Session):
        session_created = rt_models.SessionCreatedMessage(
            event_id=self.generate_event_id(),
            session=session
        )
        await self.output_queue.put(session_created.model_dump_json(exclude_none=True))

    async def on_response_created(self, response: rt_models.Response):
        response_created = rt_models.ResponseCreatedMessage(
            event_id=self.generate_event_id(),
            response=response
        )
        await self.output_queue.put(response_created.model_dump_json(exclude_none=True))

    async def on_response_done(self, response: rt_models.Response):
        response_done = rt_models.ResponseDoneMessage(
            event_id=self.generate_event_id(),
            response=response
        )
        await self.output_queue.put(response_done.model_dump_json(exclude_none=True))

    async def on_response_output_item_added(self, response_id: str, item: rt_models.ResponseItem):
        response_output_item_added = rt_models.ResponseOutputItemAddedMessage(
            event_id=self.generate_event_id(),
            response_id=response_id,
            output_index=0,
            item=item
        )
        await self.output_queue.put(response_output_item_added.model_dump_json(exclude_none=True))

    async def on_response_output_item_done(self, response_id: str, item: rt_models.ResponseItem):
        response_output_item_done = rt_models.ResponseOutputItemDoneMessage(
            event_id=self.generate_event_id(),
            response_id=response_id,
            output_index=0,
            item=item
        )
        await self.output_queue.put(response_output_item_done.model_dump_json(exclude_none=True))

    async def on_conversation_item_created(self, previous_id: str, item: rt_models.ResponseItem):
        conversation_item_created = rt_models.ItemCreatedMessage(
            event_id=self.generate_event_id(),
            item=item,
            previous_item_id=previous_id,
        )
        await self.output_queue.put(conversation_item_created.model_dump_json(exclude_none=True))

    async def on_response_content_part_added(self, response_id: str, item_id: str, content_index: int, content: rt_models.ResponseItemContentPart):
        response_content_part_added = rt_models.ResponseContentPartAddedMessage(
            event_id=self.generate_event_id(),
            response_id=response_id,
            item_id=item_id,
            output_index=0,
            content_index=content_index,
            content=content
        )
        await self.output_queue.put(response_content_part_added.model_dump_json(exclude_none=True))

    async def on_response_content_part_done(self, response_id: str, item_id: str, content_index: int, content: rt_models.ResponseItemContentPart):
        response_content_part_done = rt_models.ResponseContentPartDoneMessage(
            event_id=self.generate_event_id(),
            response_id=response_id,
            item_id=item_id,
            output_index=0,
            content_index=content_index,
            content=content
        )
        await self.output_queue.put(response_content_part_done.model_dump_json(exclude_none=True))

    async def on_audio_chunk(self, response_id: str, item_id: str, content_index: str, audio_chunk: bytes):
        response_audio_delta = rt_models.ResponseAudioDeltaMessage(
            event_id=self.generate_event_id(),
            response_id=response_id,
            item_id=item_id,
            output_index=0,
            content_index=content_index,
            delta=base64.b64encode(audio_chunk)
        )
        await self.output_queue.put(response_audio_delta.model_dump_json(exclude_none=True))

    async def on_audio_done(self, response_id, item_id, content_index):
        response_audio_done = rt_models.ResponseAudioDoneMessage(
            event_id=self.generate_event_id(),
            response_id=response_id,
            item_id=item_id,
            output_index=0,
            content_index=content_index
        )
        await self.output_queue.put(response_audio_done.model_dump_json(exclude_none=True))

    async def on_transcript_chunk(self, response_id: str, item_id: str, content_index: str, transcript_chunk: str):
        response_transcript_delta = rt_models.ResponseAudioTranscriptDeltaMessage(
            event_id=self.generate_event_id(),
            response_id=response_id,
            item_id=item_id,
            output_index=0,
            content_index=content_index,
            delta=transcript_chunk
        )
        await self.output_queue.put(response_transcript_delta.model_dump_json(exclude_none=True))

    ## input audio
    async def on_input_audio_buffer_speech_started(self, item_id: str, audio_start_ms: int):
        input_audio_buffer_speech_started = rt_models.InputAudioBufferSpeechStartedMessage(
            event_id=self.generate_event_id(),
            audio_start_ms=audio_start_ms,
            item_id=item_id
        )
        await self.output_queue.put(input_audio_buffer_speech_started.model_dump_json(exclude_none=True))

    async def on_input_audio_buffer_speech_stopped(self, item_id: str, audio_end_ms: int):
        input_audio_buffer_speech_stopped = rt_models.InputAudioBufferSpeechStoppedMessage(
            event_id=self.generate_event_id(),
            audio_end_ms=audio_end_ms,
            item_id=item_id
        )
        await self.output_queue.put(input_audio_buffer_speech_stopped.model_dump_json(exclude_none=True))

    async def on_input_audio_buffer_committed(self, item_id: str, previous_item_id: str):
        input_audio_buffer_committed = rt_models.InputAudioBufferCommittedMessage(
            event_id=self.generate_event_id(),
            item_id=item_id,
            previous_item_id=previous_item_id
        )
        await self.output_queue.put(input_audio_buffer_committed.model_dump_json(exclude_none=True))

    async def on_conversation_item_input_audio_transcript_completed(self, item_id: str, transcript: str):
        conversation_item_input_audio_transcript_completed = rt_models.ItemInputAudioTranscriptionCompletedMessage(
            event_id=self.generate_event_id(),
            item_id=item_id,
            transcript=transcript,
            content_index=0
        )
        await self.output_queue.put(conversation_item_input_audio_transcript_completed.model_dump_json(exclude_none=True))

    ## function call
    async def on_response_function_call_arguments_delta(self, response_id: str, item_id: str, call_id:str, delta: str):
        response_function_call_arguments_delta = rt_models.ResponseFunctionCallArgumentsDeltaMessage(
            event_id=self.generate_event_id(),
            response_id=response_id,
            item_id=item_id,
            output_index=0,
            call_id=call_id,
            delta=delta
        )
        await self.output_queue.put(response_function_call_arguments_delta.model_dump_json(exclude_none=True))

    # response.function_call_arguments.done"
    async def on_response_function_call_arguments_done(self, response_id: str, item_id: str, call_id: str, name: str, arguments: str):
        response_function_call_arguments_done = rt_models.ResponseFunctionCallArgumentsDoneMessage(
            event_id=self.generate_event_id(),
            response_id=response_id,
            item_id=item_id,
            output_index=0,
            call_id=call_id,
            name=name,
            arguments=arguments
        )
        await self.output_queue.put(response_function_call_arguments_done.model_dump_json(exclude_none=True))