import asyncio
import logging
import time

from oai import ChatClient
from azuretts import Client as AzureTTSClient

chat_client = ChatClient()

class BotResponse:
    def __init__(
        self, human_turn, context, inf_url, inf_key, eleven_key, interrupted=False
    ):
        self.human_turn = human_turn
        self.context = context
        self.inf_url = inf_url
        self.inf_key = inf_key
        self.eleven_key = eleven_key
        self.out_audio_stream = None
        self.streaming = False
        self.ai_turn = None
        self.ts = int(time.time())
        self.ts_of_voice_start = 0
        self.interrupted = interrupted
        self.chat_client = ChatClient()
        self.azure_tts_client = AzureTTSClient()
        self._turn_id = None
        self.voice = "en-US-AvaNeural"

    async def start(self):
        # Add human turn to context
        self.context.append(
            {
                "text": self.human_turn,
                "type": "Human",
                "ts": self.ts,
            }
        )
        logging.info(f"Starting inference {time.time()}")
        self.ai_turn = await self._call_inference()
        logging.info(f"inference complete {time.time()}")

        # Add ai turn to context
        self.context.append(
            {
                "text": self.ai_turn,
                "type": "AI",
                "ts": int(time.time()),
            }
        )

        logging.info(f"Starting stream {self.human_turn}")

        self.out_audio_stream = await asyncio.to_thread(
            self.azure_tts_client.text_to_speech,
            text=self.ai_turn,
            voice=self.voice,
        )
        self.streaming = True

        return self.ai_turn

    @property
    def turn_id(self):
        return self._turn_id

    @turn_id.setter
    def turn_id(self, value):
        self._turn_id = value

    def stop(self, turn_id="all"):
        if turn_id == self._turn_id or turn_id == "all":

            human_turn = self.human_turn[:60]
            bot_response = self.ai_turn[:35] if self.ai_turn else "No response generated yet"
            logging.info(f"Stopping turn. Human: '{human_turn}' ai: '{bot_response}'")
            self.streaming = False
            self.out_audio_stream = None

    async def _call_inference(self):
        text_response = self.chat_client.chat(self.human_turn)
        if self.interrupted:
            text_response = f" {text_response}"

        logging.info(f"Pi's response {text_response}")
        return text_response
