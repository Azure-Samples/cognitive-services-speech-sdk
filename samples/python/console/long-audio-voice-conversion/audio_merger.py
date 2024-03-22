# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import logging
from pathlib import Path
from typing import List

from audio_splitter import Segment, SegmentLabel
from pydub import AudioSegment

LOG = logging.getLogger(__name__)


def merge_audio(
    input_audio: Path, segment_list: List[Segment], vc_output_dir: Path, output_audio: Path, sample_rate: int = 24000
) -> None:
    input_audio_data = AudioSegment.from_file(input_audio)
    output_audio_data = AudioSegment.empty()
    for seg_idx, start_time, end_time, label in segment_list:
        if label is SegmentLabel.speech:
            try:
                audio_data = AudioSegment.from_file(vc_output_dir / f"{seg_idx:06}.wav")
            except Exception:
                LOG.error("VC failed for seg {}, use origional segment".format(seg_idx))
                audio_data = input_audio_data[start_time:end_time]
            audio_length = len(audio_data)
            duration = end_time - start_time
            if audio_length != duration:
                LOG.warning(
                    "{:06}.wav, duration was changed after VC, {} - {} = {}ms (increase)".format(
                        seg_idx, audio_length, duration, audio_length - duration
                    )
                )
                if audio_length < duration:
                    audio_data += AudioSegment.silent(duration=duration - audio_length, frame_rate=sample_rate)
                else:
                    audio_data = audio_data[:duration]
        else:
            audio_data = input_audio_data[start_time:end_time]
        output_audio_data += audio_data
    output_audio.absolute().parent.mkdir(exist_ok=True)
    output_audio_data.export(output_audio, format="wav")
