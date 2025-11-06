import os
from openai import AzureOpenAI
import azure.cognitiveservices.speech as speechsdk

# setup AzureOpenAI client
gpt_client = AzureOpenAI(
    azure_endpoint=os.getenv("AZURE_OPENAI_API_ENDPOINT"),
    api_key=os.getenv("AZURE_OPENAI_API_KEY"),
    api_version="2024-02-01"
)

# setup speech synthesizer
# IMPORTANT: MUST use the websocket v2 endpoint
speech_config = speechsdk.SpeechConfig(
    endpoint=f"wss://{os.getenv('AZURE_TTS_REGION')}.tts.speech.microsoft.com/cognitiveservices/websocket/v2",
    subscription=os.getenv("AZURE_TTS_API_KEY")
)

# Configure the voice for speech synthesis. For a complete list of available voices,
# visit https://aka.ms/speech/voices/neural
#
# To use the latest LLM-based HD neural voice, set the voice name to "en-us-Ava:DragonHDLatestNeural"
# (available in regions such as East US). Alternatively, use standard neural voices like
# "en-US-AriaNeural" or "en-US-AvaMultilingualNeural".
#
# For HD neural voice region availability, see https://aka.ms/speech/regions
voice = "en-us-Ava:DragonHDLatestNeural"
speech_config.speech_synthesis_voice_name = voice
speech_synthesizer = speechsdk.SpeechSynthesizer(speech_config=speech_config)

speech_synthesizer.synthesizing.connect(lambda evt: print("[audio]", end=""))

# set timeout value to bigger ones to avoid sdk cancel the request when GPT latency too high
speech_config.set_property(speechsdk.PropertyId.SpeechSynthesis_FrameTimeoutInterval, "100000000")
speech_config.set_property(speechsdk.PropertyId.SpeechSynthesis_RtfTimeoutThreshold, "10")

# create request with TextStream input type
tts_request = speechsdk.SpeechSynthesisRequest(input_type=speechsdk.SpeechSynthesisRequestInputType.TextStream)
tts_task = speech_synthesizer.speak_async(tts_request)

# Get GPT output stream
completion = gpt_client.chat.completions.create(
    model="gpt-4-turbo",
    messages=[
        {"role": "system", "content": "You are a helpful assistant."},
        {"role": "user", "content": "tell me a joke in 100 words"}
    ],
    stream=True
)

for chunk in completion:
    if len(chunk.choices) > 0:
        chunk_text = chunk.choices[0].delta.content
        if chunk_text:
            print(chunk_text, end="")
            tts_request.input_stream.write(chunk_text)
print("[GPT END]", end="")

# close tts input stream when GPT finished
tts_request.input_stream.close()

# wait all tts audio bytes return
result = tts_task.get()
print("[TTS END]", end="")
