import asyncio
import json
import logging
import os
import time
import traceback
import uuid
from datetime import datetime

import azure.cognitiveservices.speech as speechsdk
from azure.identity import DefaultAzureCredential, get_bearer_token_provider
from azure_fast_transcription_recognizer import AzureFastTranscriptionRecognizer
from oai import ChatClient
from bot_response import BotResponse

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s - %(levelname)s - %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S",
)

logger = logging.Logger(__name__)

token_provider = get_bearer_token_provider(DefaultAzureCredential(), "https://cognitiveservices.azure.com/.default")


SPEECH_REGION = os.getenv("SPEECH_REGION")
SPEECH_RESOURCE_ID = os.getenv("SPEECH_RESOURCE_ID")

if not SPEECH_REGION and not SPEECH_RESOURCE_ID:
    raise EnvironmentError("SPEECH_REGION or SPEECH_RESOURCE_ID environment variables must be set")

loop = asyncio.new_event_loop()

class AzureChatServer:
    def __init__(self, in_queue, out_queue, language: str = "en-US", provider=None):
        self.in_queue = in_queue
        self.out_queue = out_queue
        self.chat_client = ChatClient()
        self.audio_input_stream = None

        if provider == "azure-fast-transcription":
            self.speech_recognizer = AzureFastTranscriptionRecognizer(
                endpoint=f"https://{SPEECH_REGION}.api.cognitive.microsoft.com",
                key=os.getenv("SPEECH_KEY"),
            )
            self.speech_recognizer.locale = language
            self.speech_recognizer.on_recognized = self.on_fast_transcription_recognized
        else:
            self.init_speech_recognizer(language)

        self.recognized_text = ""
        self.voice = "en-US-AvaNeural"
        self.pis_response = None
        self.interrupted = False
        self.last_latency = -1

    def init_speech_recognizer(self, language: str):
        auth_token = f"aad#{SPEECH_RESOURCE_ID}#{token_provider()}"
        # speech_config = speechsdk.SpeechConfig(endpoint=SPEECH_ENDPOINT_URL)
        # speech_config.authorization_token = auth_token
        # logger.info(f"ASR Endpoint: {SPEECH_ENDPOINT_URL}")
        speech_config = speechsdk.SpeechConfig(region=SPEECH_REGION, auth_token=auth_token)

        self.audio_input_stream = speechsdk.audio.PushAudioInputStream()
        audio_config = speechsdk.audio.AudioConfig(stream=self.audio_input_stream)
        # speech_config.set_property(speechsdk.PropertyId.Speech_LogFilename, "speechsdk.log")

        # The default language is "en-us".
        # Set all TimeoutSettings
        speech_config.speech_recognition_language = language
        speech_config.set_property(speechsdk.PropertyId.Speech_SegmentationSilenceTimeoutMs, "200")
        # speech_config.set_service_property("trafficType", "voice-chat-demo", speechsdk.ServicePropertyChannel.UriQueryParameter)
        # speech_config.set_service_property("SpeechContext-phraseDetection.TrailingSilenceTimeout", "3000", speechsdk.ServicePropertyChannel.UriQueryParameter)
        # speech_config.set_service_property("SpeechContext-PhraseDetection.InitialSilenceTimeout", "10000", speechsdk.ServicePropertyChannel.UriQueryParameter)
        # speech_config.set_service_property("SpeechContext-PhraseDetection.Dictation.Segmentation.Mode", "Custom", speechsdk.ServicePropertyChannel.UriQueryParameter)
        # speech_config.set_service_property("SpeechContext-PhraseDetection.Dictation.Segmentation.SegmentationSilenceTimeoutMs", "200", speechsdk.ServicePropertyChannel.UriQueryParameter)
        # speech_config.set_service_property("SpeechContext-phraseOutput.interimResults.resultType", "Hypothesis", speechsdk.ServicePropertyChannel.UriQueryParameter)

        # https://aka.ms/csspeech/timeouts.
        self.speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config, audio_config=audio_config)
        self.recognized_text = ""
        self.speech_recognizer.session_started.connect(lambda evt: logger.info(f"Session started: {evt}"))
        self.speech_recognizer.session_stopped.connect(lambda evt: logger.info(f"Session stopped: {evt}"))
        # uncomment the following line to interrupt the TTS when intermediate text is detected, this is too sensitive in the testing.
        # self.speech_recognizer.recognizing.connect(self.on_recognizing)
        self.speech_recognizer.recognized.connect(self.on_recognized)
        self.speech_recognizer.canceled.connect(self.on_cancelled)
        self.speech_recognizer.speech_start_detected.connect(lambda evt: logger.info(f"Speech start detected: {evt}"))
        self.speech_recognizer.speech_end_detected.connect(lambda evt: logger.info(f"Speech end detected: {evt}"))
        self.speech_recognizer.recognizing.connect(lambda evt: logger.info(f"Recognizing: {evt}"))

    async def worker(self):
        self.speech_recognizer.start_continuous_recognition()
        self._total_audio_bytes = 0

        try:
            while True:
                chunk = await self.in_queue.get()
                if chunk:
                    if isinstance(chunk, str):
                        logger.info(f"Inbound control msg: {chunk}")
                        control = json.loads(chunk)
                        if control.get("type") == "voice":
                            self.voice = control.get("value")
                    else:
                        if self.audio_input_stream:
                            self.audio_input_stream.write(chunk)
                        else:
                            self.speech_recognizer(chunk)
                        self._total_audio_bytes += len(chunk)
                    await asyncio.sleep(0)
                if self.recognized_text:
                    await self.out_queue.put(json.dumps(
                        {"type": "recognized", "text": self.recognized_text,
                        "time": datetime.utcnow().strftime('%F %T.%f')[:-3],
                        "latency": int(self.last_latency)
                          }))
                    logger.info(f"Recognized text x: {self.recognized_text}")
                    asyncio.ensure_future(self.recognized_handler(self.recognized_text, self.out_queue))
                    self.recognized_text = ""

        except Exception as e:
            print(f"An eleven error occurred: {e}")
        finally:
            self.speech_recognizer.stop_continuous_recognition()

    def on_recognized(self, args: speechsdk.SpeechRecognitionEventArgs):
        logger.info(f"Recognized: {args.result.text}")
        self.interrupted = False
        self.last_latency = self._total_audio_bytes / 2 / 16 - (args.result.offset + args.result.duration)/1000/10
        self.recognized_text = args.result.text

    def on_fast_transcription_recognized(self, response):
        texts = [phrase['text'] for phrase in response['phrases']]
        self.interrupted = False
        print(f"Recognized: '{response}'")
        self.recognized_text = ' '.join(texts)

    def on_cancelled(self, args: speechsdk.SpeechRecognitionCanceledEventArgs):
        logger.info(f"Cancelled: {args}")
        cancellation_reason = args.cancellation_details
        logger.info(f"Cancellation reason: %s, error details: %s", cancellation_reason.reason, cancellation_reason.error_details)

    def on_recognizing(self, args):
        logger.info(f"Recognizing: {args.result.text}")
        if not self.interrupted:
            self.interrupted = True
            self.out_queue.queue.put_nowait(json.dumps({"type": "interrupted", "reason": "intermediate text detected", "time": datetime.utcnow().strftime('%F %T.%f')[:-3]}))

    async def recognized_handler(self, text: str, out_queue):
        sentence = text
        if len(sentence) == 0:
            return
        logger.info(f"speaker: '{sentence}'")
        try:
            inf_url = os.getenv("CHAT_SERVER_URL")
            inf_key = os.getenv("CHAT_SERVER_API_KEY")
            eleven_key = os.getenv("ELEVEN_LABS")

            # Is this stream interrupting?
            interrupting = False

            if self.pis_response and self.pis_response.streaming:
                logger.info("User interrupting, bailing")

                interrupting = True
                self.pis_response.stop()
                await asyncio.sleep(0.01)

                if sentence.strip().lower() == "stop.":  # replace with model
                    logger.info("User said stop")
                    return

                time_delta = int(time.time()) - self.pis_response.ts_of_voice_start
                logger.info(f"Time delta since pi started talking is {time_delta}s")
                if time_delta < 5:
                    # It's been less than 5 seconds since the user stopped speaking
                    # so we're going to assume that they wanted to continue their thought
                    # and we'll amend their last turn
                    sentence = f"{self.pis_response.human_turn} {sentence}"
                    logger.info(
                        f"Interrupted: {self.pis_response.human_turn} => {sentence}"
                    )

            context = self.pis_response.context if self.pis_response else []

            self.pis_response = BotResponse(
                sentence, context, inf_url, inf_key, eleven_key, interrupting
            ) if not self.pis_response else self.pis_response

            self.pis_response.interrupted = interrupting
            self.pis_response.human_turn = sentence

            turn_id = uuid.uuid4().hex
            self.pis_response.turn_id = turn_id

            logger.info("Calling BotResponse.start()")
            llm_start_time = datetime.utcnow()
            msg = {"type": "thinking", "time": llm_start_time.strftime('%F %T.%f')[:-3]}
            await out_queue.put(json.dumps(msg))
            self.pis_response.voice = self.voice
            ai_turn = await self.pis_response.start()

            # time format, e.g. 2021-09-01T12:00:00.010Z
            llm_finish_time = datetime.utcnow()
            msg = {
                "type": "ai response", "text": ai_turn,
                "time": llm_finish_time.strftime('%F %T.%f')[:-3],
                "latency": int((llm_finish_time - llm_start_time).total_seconds() * 1000)}
            await out_queue.put(json.dumps(msg))


            logger.info(f"Sending audio chunks for {self.pis_response.human_turn}")
            # Send control message to start
            # await out_queue.put('{ "type": "stream_start" }')

            # Has this stream been interrupted?
            interrupted = False

            first = True

            try:
                self.pis_response.ts_of_voice_start = int(time.time())
                logger.info(f"Voice start time: {self.pis_response.ts_of_voice_start}")
                for chunk in self.pis_response.out_audio_stream:
                    if first:
                        tts_first_chunk_time = datetime.utcnow()
                        await out_queue.put(json.dumps({
                            "type": "stream_start", "reason": "TTS started",
                            "time": tts_first_chunk_time.strftime('%F %T.%f')[:-3], "latency": int((tts_first_chunk_time - llm_finish_time).total_seconds() * 1000)}))
                        await out_queue.put(chunk)
                        first = False
                        last_update = time.time()
                    else:
                        # logger.info(f"Sending chunk of audio")
                        for i in range(0, len(chunk), 1200):
                            if self.pis_response.streaming and self.pis_response.turn_id == turn_id and not self.interrupted:
                                # Ensure we're still streaming this response
                                await out_queue.put(chunk[i : i + 1200])
                                current_time = time.time()
                                if current_time - last_update < 0.024:
                                    await asyncio.sleep(last_update + 0.024 - current_time)
                                else:
                                    logging.warning(f"We got unexpected delay: {current_time - last_update - 0.024}")
                                    # await out_queue.put(json.dumps({"type": "service underrun", "time": current_time - last_update - 0.024}))
                                last_update = time.time()
                            else:
                                interrupted = True
                                logger.info(
                                    "Stream has stopped. No longer writing to outqueue"
                                )
                                return
                    await asyncio.sleep(0)
            except TypeError as e:
                interrupted = True
                logger.info(
                    "Stream has stopped and response gone. No longer writing to outqueue"
                )
            finally:
                # Indicate response is complete
                self.pis_response.stop(turn_id)
                msg = {
                    "type": "stream_stop",
                    "reason": "interrupted" if interrupted else "done",
                }
                await out_queue.put(json.dumps(msg))
                logger.info("Stream complete")

        except Exception as e:
            print(f"Blowing up write writing to outqueue: {e}")
            traceback.print_exc()
