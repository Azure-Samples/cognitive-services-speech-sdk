#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import logging
from pathlib import Path

import librosa
import soundfile as sf
from audio_merger import merge_audio
from audio_splitter import AudioSplitter
from voice_changer import VoiceChanger

LOG = logging.getLogger(__name__)


def resample_wav(input_file: Path, output_file: Path, sample_rate: int = 16000) -> Path:
    audio_data, _sr = librosa.load(input_file, sr=sample_rate, mono=True)
    sf.write(file=output_file, data=audio_data, samplerate=sample_rate, subtype="PCM_16", format="WAV")
    return output_file


class LongAudioVoiceChanger:
    def __init__(
        self,
        subscription: str,
        region: str,
        container_url_with_sas: str,
        language: str,
        name: str,
    ) -> None:
        self.audio_splitter = AudioSplitter(subscription, region, language)
        self.voice_changer = VoiceChanger(subscription, region, container_url_with_sas, language, name)

    def convert_one_audio(self, input_audio: Path, output_audio: Path, output_dir: Path) -> None:
        # process input
        audio_16k = resample_wav(input_audio, output_dir / "16k.wav", 16000)
        audio_24k = resample_wav(input_audio, output_dir / "24k.wav", 24000)

        # split long audio
        segment_list, segment_output_dir = self.audio_splitter.split_audio(audio_16k, output_dir)

        # call VC service for each voiced segment in parallel
        speech_segments_dir = segment_output_dir / "speech"
        if speech_segments_dir.is_dir():
            vc_output_dir = output_dir / "speech_vc"
            vc_output_dir.mkdir(parents=True, exist_ok=True)
            input_audio_list = []
            output_audio_list = []
            for speech_segment_file in speech_segments_dir.iterdir():
                input_audio_list.append(speech_segment_file)
                output_audio_list.append(vc_output_dir / speech_segment_file.name)
            self.voice_changer.convert_audios(input_audio_list, output_audio_list)

        # merge back into long audio
        merge_audio(audio_24k, segment_list, vc_output_dir, output_audio, sample_rate=24000)


# Demo for long audio voice conversion
if __name__ == "__main__":
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s\t%(name)s\t%(levelname)s\tP%(process)d\t%(message)s",
    )

    # Replace with your own subscription key, service region (e.g., "westus") and container URL(with SAS token).
    subscription = "YourSubscriptionKey"
    region = "YourServiceRegion"
    container_url_with_sas = "YourContainerURLWithSAS"
    language = "zh-CN"
    name = "zh-CN-XiaoxiaoNeural"
    long_audio_vc = LongAudioVoiceChanger(subscription, region, container_url_with_sas, language, name)

    # Define input and output
    input_audio = Path("src.wav")  # input long audio file
    output_audio = Path("output/vc.wav")  # output long audio file
    output_dir = Path("output/debug")  # temporary output directory for debug
    output_dir.mkdir(parents=True, exist_ok=True)

    long_audio_vc.convert_one_audio(input_audio, output_audio, output_dir)
