"""Reusable backend Speech SDK adapter (Python 3.10+, validated with SDK 1.52.0).

No default credentials, service calls at import, speaker output or test-specific interfaces.
Copy this module into the application and adapt its small public facade.
"""
import asyncio
import inspect
import logging
import re
import threading
from urllib.parse import urlsplit

logger = logging.getLogger(__name__)

# Official text-stream samples raise both limits: with SDK defaults (RTF 2, 3 s frame gap)
# the SDK cancels the request when the LLM pauses for a few seconds mid-reply.
FRAME_TIMEOUT_MS = '100000000'
RTF_TIMEOUT_THRESHOLD = '10'


def _observe(task):
    if not task.cancelled():
        task.exception()


async def _best_effort(operation, action, seconds):
    """Bound one cleanup step, even if it does not cooperate with cancellation."""
    async def run():
        await operation()

    task = asyncio.create_task(run())
    task.add_done_callback(_observe)
    try:
        done, _ = await asyncio.wait({task}, timeout=seconds)
        if not done:
            task.cancel()
            raise TimeoutError()
        task.result()
        return True
    except asyncio.CancelledError:
        if not task.cancelled():
            task.cancel()
            raise
        logger.warning('Azure TTS cleanup could not %s: CancelledError', action)
    except Exception as error:
        logger.warning('Azure TTS cleanup could not %s: %s', action, type(error).__name__)
    return False


async def _finish_cleanup(awaitable):
    """Finish bounded cleanup before propagating caller cancellation, including repeats."""
    task = asyncio.create_task(awaitable)
    interrupted = False
    while not task.done():
        try:
            await asyncio.shield(task)
        except asyncio.CancelledError:
            interrupted = True
    result = task.result()
    if interrupted:
        raise asyncio.CancelledError()
    return result


def _failure(result):
    details = getattr(result, 'cancellation_details', None)
    code = getattr(getattr(details, 'error_code', None), 'name', None) or getattr(getattr(details, 'reason', None), 'name', None)
    return RuntimeError('Speech synthesis did not complete successfully' + (' (' + code + ')' if code else ''))


def _text(value, name):
    if not isinstance(value, str) or not value.strip():
        raise ValueError('Speech ' + name + ' is required')
    return value


class AzureTTS:
    """One instance per voice and identity; safe to share across concurrent calls on one event loop.

    Identity: exactly one of subscription (key) or credential (azure.identity TokenCredential).
    Location: exactly one of region (public region name) or endpoint (resource base URI such as
    https://<custom-domain>.cognitiveservices.azure.com/). Entra credentials require an endpoint.
    """

    def __init__(self, subscription=None, region=None, voice=None, *, endpoint=None, credential=None,
                 max_idle=4, max_pending=64, max_pending_bytes=1048576):
        self.voice = _text(voice, 'voice')
        if type(max_idle) is not int or max_idle < 0:
            raise ValueError('max_idle must be a nonnegative integer')
        if any(type(value) is not int or value <= 0 for value in (max_pending, max_pending_bytes)):
            raise ValueError('Audio admission limits must be positive integers')
        self._max_pending, self._max_pending_bytes = max_pending, max_pending_bytes
        self._closed = False
        if (subscription is None) == (credential is None):
            raise ValueError('Provide exactly one Speech identity: subscription key or credential')
        if subscription is not None:
            _text(subscription, 'subscription')
        else:
            getter = inspect.unwrap(getattr(credential, 'get_token', None))
            if not callable(getter) or any(
                check(inspect.unwrap(method))
                for method in (getter, getattr(getter, '__call__', None))
                for check in (inspect.iscoroutinefunction, inspect.isasyncgenfunction)
            ):
                raise ValueError('Speech credential must provide a synchronous get_token method')
        if (region is None) == (endpoint is None):
            raise ValueError('Provide exactly one Speech location: region or endpoint')
        if region is not None and not re.fullmatch(r'[A-Za-z0-9]+', _text(region, 'region')):
            raise ValueError('Speech region must be a public region name such as eastus')
        if endpoint is not None:
            endpoint = _text(endpoint, 'endpoint')
            try:
                if any(c.isspace() or ord(c) < 32 or ord(c) == 127 or c in '\\?#' for c in endpoint):
                    raise ValueError()
                parts = urlsplit(endpoint)
                port = parts.port  # Access validates numeric ports and the TCP port range.
                if parts.scheme != 'https' or not parts.hostname or parts.username is not None \
                        or parts.password is not None or parts.path not in ('', '/') \
                        or parts.netloc.endswith(':') or port == 0:
                    raise ValueError()
            except ValueError:
                raise ValueError('Speech endpoint must be a resource base HTTPS URI with a valid host and optional port') from None
        if credential is not None and endpoint is None:
            raise ValueError('Entra credentials require the resource custom-domain endpoint')
        self.subscription, self.credential, self.region, self.endpoint = subscription, credential, region, endpoint
        self._idle, self._max_idle, self._lock = {False: [], True: []}, max_idle, threading.Lock()

    def _stream_endpoint(self):
        if self.region:
            return 'wss://' + self.region + '.tts.speech.microsoft.com/cognitiveservices/websocket/v2'
        parts = urlsplit(self.endpoint)
        hostname = parts.hostname.lower()
        host = '[' + hostname + ']' if ':' in hostname else hostname
        if parts.port is not None:
            host += ':' + str(parts.port)
        prefix = '' if hostname.endswith('.tts.speech.microsoft.com') else '/tts'
        return 'wss://' + host + prefix + '/cognitiveservices/websocket/v2'

    def build_synthesizer(self, *, incremental=False):
        """New SDK synthesizer: no speakers; RIFF 24 kHz WAV, or raw 24 kHz PCM for text streaming."""
        import azure.cognitiveservices.speech as sdk
        identity = {'subscription': self.subscription} if self.subscription is not None else {'token_credential': self.credential}
        if incremental:
            route = {'endpoint': self._stream_endpoint()}
        else:
            route = {'region': self.region} if self.region else {'endpoint': self.endpoint}
        config = sdk.SpeechConfig(**identity, **route)
        config.speech_synthesis_voice_name = self.voice
        if incremental:
            config.set_property(sdk.PropertyId.SpeechSynthesis_FrameTimeoutInterval, FRAME_TIMEOUT_MS)
            config.set_property(sdk.PropertyId.SpeechSynthesis_RtfTimeoutThreshold, RTF_TIMEOUT_THRESHOLD)
        output = (sdk.SpeechSynthesisOutputFormat.Raw24Khz16BitMonoPcm if incremental
                  else sdk.SpeechSynthesisOutputFormat.Riff24Khz16BitMonoPcm)
        config.set_speech_synthesis_output_format(output)
        return sdk.SpeechSynthesizer(speech_config=config, audio_config=None)

    def _borrow(self, incremental):
        with self._lock:
            if self._closed:
                raise RuntimeError('AzureTTS is closed')
            if self._idle[incremental]:
                return self._idle[incremental].pop()
        return self.build_synthesizer(incremental=incremental)

    async def _close_connection(self, synth):
        import azure.cognitiveservices.speech as sdk
        return await _best_effort(
            lambda: asyncio.to_thread(lambda: sdk.Connection.from_speech_synthesizer(synth).close()),
            'close connection', 5)

    async def _release(self, synth, incremental):
        with self._lock:
            if not self._closed and len(self._idle[incremental]) < self._max_idle:
                self._idle[incremental].append(synth)
                return
        await self._close_connection(synth)

    async def aclose(self):
        """Reject new calls and close idle connections; active calls finish without re-pooling.

        Stop/cancel and await active calls first if immediate shutdown is needed.
        The caller owns the credential and must close it separately when appropriate.
        """
        with self._lock:
            self._closed = True
            idle = self._idle[False] + self._idle[True]
            self._idle = {False: [], True: []}

        async def close_idle():
            clean = True
            for synth in idle:
                clean &= await self._close_connection(synth)
            return clean
        if not await _finish_cleanup(close_idle()):
            raise RuntimeError('Some Azure TTS connections could not be closed')

    async def synthesize(self, text):
        """Complete text to WAV; cancellation requests native stop, then discards the worker."""
        import azure.cognitiveservices.speech as sdk
        _text(text, 'text')
        synth = self._borrow(False)
        completion = asyncio.create_task(asyncio.to_thread(lambda: synth.speak_text_async(text).get()))
        try:
            result = await asyncio.shield(completion)
        except BaseException:
            async def cleanup():
                await _best_effort(lambda: asyncio.to_thread(lambda: synth.stop_speaking_async().get()),
                                   'stop synthesis', 5)
                await _best_effort(lambda: asyncio.gather(completion, return_exceptions=True),
                                   'drain complete-text task', 5)
            await _finish_cleanup(cleanup())
            raise
        if result.reason != sdk.ResultReason.SynthesizingAudioCompleted:
            raise _failure(result)
        await self._release(synth, False)
        return result.audio_data

    async def speak_stream(self, text_source, on_audio, cancellation, *, cleanup_seconds=5):
        """Wait for first nonempty text before starting SDK synthesis; preserve whitespace.

        Pending PCM (scheduled callbacks, queued and in-flight sends) is bounded by
        max_pending chunks and max_pending_bytes. Overflow raises BufferError and stops
        synthesis, never silently drops audio. Sync/awaitable consumers are serialized.
        Cleanup deadlines are per step, not total teardown; underlying work may outlive
        cancellation. Event stop returns normally; task cancellation propagates.
        """
        if self._closed:
            raise RuntimeError('AzureTTS is closed')
        if cancellation.is_set():
            return
        iterator = text_source.__aiter__()

        async def first_text():
            async for delta in iterator:
                if not isinstance(delta, str):
                    raise TypeError('Text deltas must be strings')
                if delta:
                    return delta
            return None
        first = asyncio.create_task(first_text())
        stopped = asyncio.create_task(cancellation.wait())
        source_owned_by_stream = [False]
        try:
            await asyncio.wait({first, stopped}, return_when=asyncio.FIRST_COMPLETED)
            if cancellation.is_set():
                return
            delta = first.result()
            if delta is None:
                return
            await self._speak_stream(iterator, delta, on_audio, cancellation, cleanup_seconds, source_owned_by_stream)
        finally:
            for task in (first, stopped):
                if not task.done():
                    task.cancel()

            async def cleanup_prefetch():
                await _best_effort(lambda: asyncio.gather(first, stopped, return_exceptions=True),
                                   'drain first-text tasks', cleanup_seconds)
                if not source_owned_by_stream[0]:
                    closer = getattr(iterator, 'aclose', None)
                    if closer:
                        await _best_effort(closer, 'close text source', cleanup_seconds)
            await _finish_cleanup(cleanup_prefetch())

    async def _speak_stream(self, iterator, first_delta, on_audio, cancellation, cleanup_seconds, source_owned):
        import azure.cognitiveservices.speech as sdk
        if cancellation.is_set():
            return
        loop = asyncio.get_running_loop()
        synth = self._borrow(True)
        request = sdk.SpeechSynthesisRequest(input_type=sdk.SpeechSynthesisRequestInputType.TextStream)
        enabled, succeeded, closed = True, False, False
        callback_error = loop.create_future()
        outbox = asyncio.Queue()
        admission = threading.Lock()
        pending_chunks, pending_bytes, overloaded = 0, 0, False

        def release(data):
            nonlocal pending_chunks, pending_bytes
            with admission:
                pending_chunks -= 1
                pending_bytes -= len(data)

        def fail(error):
            if not callback_error.done():
                callback_error.set_result(error)

        def deliver(data):
            if enabled and not cancellation.is_set() and not callback_error.done():
                outbox.put_nowait(data)
            else:
                release(data)

        async def send_in_order():
            while True:
                data = await outbox.get()
                if data is None:
                    return
                try:
                    if cancellation.is_set() or callback_error.done():
                        return
                    result = on_audio(data)
                    if inspect.isawaitable(result):
                        await result
                except asyncio.CancelledError:
                    if enabled and not cancellation.is_set():
                        fail(ConnectionError('Audio consumer canceled delivery'))
                    raise
                except Exception as error:
                    fail(error)
                    return
                finally:
                    release(data)

        def audio(event):
            nonlocal pending_chunks, pending_bytes, overloaded
            raw = event.result.audio_data
            if not enabled or cancellation.is_set() or not raw:
                return
            with admission:
                if overloaded:
                    return
                if pending_chunks >= self._max_pending or pending_bytes + len(raw) > self._max_pending_bytes:
                    overloaded = True
                    overflow = True
                else:
                    pending_chunks += 1
                    pending_bytes += len(raw)
                    overflow = False
            try:
                if overflow:
                    loop.call_soon_threadsafe(fail, BufferError('Audio delivery capacity exceeded'))
                else:
                    loop.call_soon_threadsafe(deliver, bytes(raw))
            except RuntimeError:
                if not overflow:
                    release(raw)

        def close_input():
            nonlocal closed
            if not closed:
                closed = True
                request.input_stream.close()

        async def feed():
            request.input_stream.write(first_delta)
            async for delta in iterator:
                if cancellation.is_set():
                    return
                request.input_stream.write(delta)
            close_input()

        synth.synthesizing.connect(audio)
        producer = stop = completion = sender = None
        source_owned[0] = True
        try:
            sender = asyncio.create_task(send_in_order())
            future = synth.speak_async(request)
            completion = asyncio.create_task(asyncio.to_thread(future.get))
            producer = asyncio.create_task(feed())
            stop = asyncio.create_task(cancellation.wait())
            pending = {producer, completion}
            while pending:
                done, _ = await asyncio.wait(pending | {stop, callback_error}, return_when=asyncio.FIRST_COMPLETED)
                if cancellation.is_set():
                    return
                if callback_error in done:
                    raise callback_error.result()
                if completion in done and completion.result().reason != sdk.ResultReason.SynthesizingAudioCompleted:
                    raise _failure(completion.result())
                for task in pending & done:
                    task.result()
                pending -= done
            result = completion.result()
            if result.reason != sdk.ResultReason.SynthesizingAudioCompleted:
                raise _failure(result)
            await asyncio.sleep(0)  # Drain callbacks queued before completion.
            if sender is not None:
                outbox.put_nowait(None)  # Finish sending queued audio before returning.
                await asyncio.wait({sender, stop, callback_error}, return_when=asyncio.FIRST_COMPLETED)
                if cancellation.is_set():
                    return
            if callback_error.done():
                raise callback_error.result()
            succeeded = True
        finally:
            enabled = False
            for task in (producer, stop, sender):
                if task and not task.done():
                    task.cancel()
            callback_error.cancel()

            async def cleanup():
                clean = True
                if not succeeded:
                    clean &= await _best_effort(
                        lambda: asyncio.to_thread(lambda: synth.stop_speaking_async().get()),
                        'stop synthesis', cleanup_seconds)
                clean &= await _best_effort(lambda: asyncio.to_thread(close_input),
                                            'close input', cleanup_seconds)
                clean &= await _best_effort(lambda: asyncio.to_thread(synth.synthesizing.disconnect_all),
                                            'detach handlers', cleanup_seconds)
                owned = [task for task in (producer, stop, completion, sender) if task]
                if owned:
                    clean &= await _best_effort(lambda: asyncio.gather(*owned, return_exceptions=True),
                                                'drain tasks', cleanup_seconds)
                closer = getattr(iterator, 'aclose', None)
                if closer:
                    clean &= await _best_effort(closer, 'close text source', cleanup_seconds)
                return clean
            clean = await _finish_cleanup(cleanup())
            if succeeded and clean and not cancellation.is_set():
                await self._release(synth, True)
