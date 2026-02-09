import os
import struct
import argparse
from openai import AzureOpenAI
import azure.cognitiveservices.speech as speechsdk

# Parse command-line arguments
parser = argparse.ArgumentParser(description="Azure TTS Text Streaming with GPT")
parser.add_argument("--voice", type=str, default="en-us-Ava:DragonHDLatestNeural",
                    help="Voice name for synthesis (default: en-us-Ava:DragonHDLatestNeural)")
parser.add_argument("--region", type=str, default=None,
                    help="Azure TTS region (default: from AZURE_TTS_REGION env)")
parser.add_argument("--key", type=str, default=None,
                    help="Azure TTS API key (default: from AZURE_TTS_API_KEY env)")
parser.add_argument("--deployment", type=str, default=None,
                    help="Custom deployment ID for custom voice")
parser.add_argument("--output", type=str, default="output_audio.wav",
                    help="Output WAV file path (default: output_audio.wav)")
parser.add_argument("--prompt", type=str, default="tell me a joke in 100 words",
                    help="Prompt for GPT (default: tell me a joke in 100 words)")
args = parser.parse_args()

# Get configuration from args or environment variables
tts_region = args.region or os.getenv('AZURE_TTS_REGION')
tts_key = args.key or os.getenv('AZURE_TTS_API_KEY')
voice = args.voice
deployment = args.deployment
output_file = args.output

# setup AzureOpenAI client
gpt_client = AzureOpenAI(
    azure_endpoint=os.getenv("AZURE_OPENAI_API_ENDPOINT"),
    api_key=os.getenv("AZURE_OPENAI_API_KEY"),
    api_version=os.getenv("AZURE_OPENAI_API_VERSION"),
)

# setup speech synthesizer
# IMPORTANT: MUST use the websocket v2 endpoint
endpoint = f"wss://{tts_region}.tts.speech.microsoft.com/cognitiveservices/websocket/v2"
speech_config = speechsdk.SpeechConfig(
    endpoint=endpoint,
    subscription=tts_key
)

# Set deployment ID if provided (for custom voice)
if deployment:
    speech_config.endpoint_id = deployment

# Configure the voice for speech synthesis
speech_config.speech_synthesis_voice_name = voice

print("=" * 70)
print("Azure TTS Text Streaming with GPT")
print("=" * 70)
print(f"Endpoint: {endpoint}")
print(f"Voice: {voice}")
if deployment:
    print(f"Deployment: {deployment}")
print(f"Output file: {output_file}")
print(f"GPT Prompt: {args.prompt}")
print("=" * 70)

# Create synthesizer with default audio output (sound card)
speech_synthesizer = speechsdk.SpeechSynthesizer(speech_config=speech_config)

# Audio format parameters (16-bit PCM, mono, 24kHz by default for Azure TTS)
sample_rate = 24000
bits_per_sample = 16
channels = 1

# List to collect audio chunks for saving to file
audio_chunks = []


def synthesizing_callback(evt):
    """Called when audio chunks are available during synthesis."""
    if evt.result.audio_data:
        audio_chunk = evt.result.audio_data
        audio_chunks.append(audio_chunk)
        print(f"[audio chunk: {len(audio_chunk)} bytes]\n", end="")


speech_synthesizer.synthesizing.connect(synthesizing_callback)

# set timeout value to bigger ones to avoid sdk cancel the request when GPT latency too high
speech_config.set_property(speechsdk.PropertyId.SpeechSynthesis_FrameTimeoutInterval, "100000000")
speech_config.set_property(speechsdk.PropertyId.SpeechSynthesis_RtfTimeoutThreshold, "10")

# create request with TextStream input type
tts_request = speechsdk.SpeechSynthesisRequest(input_type=speechsdk.SpeechSynthesisRequestInputType.TextStream)
tts_task = speech_synthesizer.speak_async(tts_request)

# Get GPT output stream
completion = gpt_client.chat.completions.create(
    model=os.getenv("AZURE_OPENAI_DEPLOYMENT_NAME"),
    messages=[
        {"role": "system", "content": "You are a helpful assistant."},
        {"role": "user", "content": args.prompt}
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
print("[TTS END]")


# Write WAV file with header
def write_wav_file(filename, audio_data, sample_rate, bits_per_sample, channels):
    """Write WAV file with proper header."""
    byte_rate = sample_rate * channels * bits_per_sample // 8
    block_align = channels * bits_per_sample // 8
    data_size = len(audio_data)

    with open(filename, 'wb') as wav_file:
        # RIFF header
        wav_file.write(b'RIFF')
        wav_file.write(struct.pack('<I', data_size + 36))  # File size - 8
        wav_file.write(b'WAVE')

        # fmt subchunk
        wav_file.write(b'fmt ')
        wav_file.write(struct.pack('<I', 16))  # Subchunk size
        wav_file.write(struct.pack('<H', 1))   # Audio format (1 = PCM)
        wav_file.write(struct.pack('<H', channels))
        wav_file.write(struct.pack('<I', sample_rate))
        wav_file.write(struct.pack('<I', byte_rate))
        wav_file.write(struct.pack('<H', block_align))
        wav_file.write(struct.pack('<H', bits_per_sample))

        # data subchunk
        wav_file.write(b'data')
        wav_file.write(struct.pack('<I', data_size))
        wav_file.write(audio_data)


# Combine all audio chunks
audio_data = b''.join(audio_chunks)
write_wav_file(output_file, audio_data, sample_rate, bits_per_sample, channels)

print(f"\nAudio saved to: {output_file}")
print(f"Total audio size: {len(audio_data)} bytes ({len(audio_data) / (sample_rate * 2):.2f} seconds)")
