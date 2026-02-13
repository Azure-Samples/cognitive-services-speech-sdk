# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import logging
import time
from enum import Enum
from pathlib import Path
from typing import List, NamedTuple, Tuple

import azure.cognitiveservices.speech as speechsdk
from pydub import AudioSegment

LOG = logging.getLogger(__name__)


class SegmentLabel(Enum):
    """
    Defines all available segment label:
        [failed, silence, speech]
    """

    failed = 0  # failed to get the segment label
    silence = 1  # voiced segment
    speech = 2  # silent segment


class Segment(NamedTuple):
    """Represents an audio segment."""

    idx: int  # The position of this segment among all segments
    start_time: int  # start time of this segment, in millisecond
    end_time: int  # end time of this segment, in millisecond
    label: SegmentLabel  # segment label of this segment


class AudioSplitter:
    def __init__(
        self, subscription: str, region: str, language: str = "zh-CN", padding_in_seconds: float = 0.1
    ) -> None:
        self.subscription = subscription
        self.region = region
        self.language = language
        self.padding_in_seconds = padding_in_seconds

    def split_audio(self, audio_path: Path, output_dir: Path) -> Tuple[List[Segment], Path]:
        output_dir.mkdir(parents=True, exist_ok=True)
        speech_segments = self._get_speech_segments(audio_path)
        segment_list = self._get_all_segments(audio_path, speech_segments)
        segment_output_dir = self.split_audio_by_segment_list(audio_path, segment_list, output_dir)
        return segment_list, segment_output_dir

    def _get_speech_segments(self, audio_path: Path) -> List[Tuple[int, int]]:
        LOG.info(f"Start recognition for {audio_path}.")
        audio_input = speechsdk.AudioConfig(filename=str(audio_path))
        speech_config = speechsdk.SpeechConfig(subscription=self.subscription, region=self.region)
        speech_config.speech_recognition_language = self.language
        speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config, audio_config=audio_input)

        done = False
        speech_segments = []

        def recognized(evt: speechsdk.SpeechRecognitionCanceledEventArgs) -> None:
            result = evt.result
            if result.reason == speechsdk.ResultReason.RecognizedSpeech:
                start = int(result.offset / self.audio_index_to_time_stamp)  # millisecond
                end = int((result.offset + result.duration) / self.audio_index_to_time_stamp)  # millisecond
                LOG.info(f"[{start/1000:.2f}s, {end/1000:.2f}s]({(end-start)/1000:.2f}s) {result.text}")
                speech_segments.append((start, end))
            elif result.reason == speechsdk.ResultReason.NoMatch:
                LOG.error("No speech could be recognized: {}".format(result.no_match_details))
            elif result.reason == speechsdk.ResultReason.Canceled:
                cancellation_details = result.cancellation_details
                LOG.error("Speech Recognition canceled: {}".format(cancellation_details.reason))
                if cancellation_details.reason == speechsdk.CancellationReason.Error:
                    LOG.error("Error details: {}".format(cancellation_details.error_details))
                    LOG.error("Did you set the speech resource key and region values?")

        def stop_cb(evt: speechsdk.SpeechRecognitionCanceledEventArgs) -> None:
            LOG.debug(f"CLOSING on {evt}")
            nonlocal speech_recognizer, done
            speech_recognizer.stop_continuous_recognition()
            done = True

        speech_recognizer.session_started.connect(lambda evt: LOG.debug(f"SESSION STARTED: {evt}"))
        speech_recognizer.recognized.connect(recognized)
        speech_recognizer.session_stopped.connect(lambda evt: LOG.debug(f"SESSION STOPPED: {evt}"))
        speech_recognizer.session_stopped.connect(stop_cb)
        speech_recognizer.canceled.connect(lambda evt: LOG.debug(f"CANCELED: {evt}"))
        speech_recognizer.canceled.connect(stop_cb)

        speech_recognizer.start_continuous_recognition()
        while not done:
            time.sleep(0.1)
        del speech_recognizer  # stop access to audio_path
        LOG.info("Finish recognition")

        return speech_segments

    def _get_all_segments(self, audio_path: Path, speech_segments: List[Tuple[int, int]]) -> List[Segment]:
        audio_segment: AudioSegment = AudioSegment.from_file(audio_path)
        audio_length = len(audio_segment)  # in milliseconds
        segment_list = []
        idx = 1
        if len(speech_segments) == 0:
            LOG.warning("ASR result is empty, and we treat the segmentation of this file was failed.")
            segment_list.append(Segment(idx, 0, audio_length, SegmentLabel.failed))
            return segment_list

        # get all voiced segments and silent segments
        last_end_time = 0
        padding = int(self.padding_in_seconds * self.second_to_audio_index)
        for i, (start_time, end_time) in enumerate(speech_segments):
            if start_time - last_end_time > padding:
                start_time -= padding
                segment_list.append(Segment(idx, last_end_time, start_time, SegmentLabel.silence))
                idx += 1
            else:
                start_time = last_end_time

            end_time = min(end_time, audio_length)  # in milliseconds
            next_start_time = audio_length if i == len(speech_segments) - 1 else speech_segments[i + 1][0]
            if next_start_time - end_time >= padding * 2:
                end_time += padding
            else:
                end_time += int((next_start_time - end_time) / 2)
            last_end_time = end_time

            segment_list.append(Segment(idx, start_time, end_time, SegmentLabel.speech))
            idx += 1

        if audio_length - last_end_time > 0:
            # fill out missing segment
            segment_list.append(Segment(idx, last_end_time, audio_length, SegmentLabel.silence))
        return segment_list

    def split_audio_by_segment_list(self, audio_path: Path, segment_list: List[Segment], output_dir: Path) -> Path:
        LOG.info(f"Start segmentation for {audio_path}.")
        audio_segment: AudioSegment = AudioSegment.from_file(audio_path)
        segment_output_dir = output_dir / "segments"
        segment_output_dir.mkdir(exist_ok=True)
        last_end_time = 0
        for segment in segment_list:
            idx = f"{segment.idx:06}"
            assert segment.start_time == last_end_time, "{} start_time({}) != last_end_time({})".format(
                idx, segment.start_time, last_end_time
            )
            last_end_time = segment.end_time
            split_segment_dir = segment_output_dir / segment.label.name
            split_segment_dir.mkdir(exist_ok=True)
            split_audio_output_path = split_segment_dir / f"{idx}.wav"
            split_aduio_segment = audio_segment[segment.start_time : segment.end_time]
            split_aduio_segment.export(split_audio_output_path, format="wav")
        LOG.info("Finish segmentation")
        return segment_output_dir

    @property
    def second_to_time_stamp(self) -> int:
        """Convert from seconds to ASR result timestamps(in 10^-7 s)"""
        return 10**7

    @property
    def second_to_audio_index(self) -> int:
        """Convert from seconds to AudioSegment index (in millisecond)"""
        return 10**3

    @property
    def audio_index_to_time_stamp(self) -> int:
        """Convert form AudioSegment index to ASR result timestamps"""
        return int(self.second_to_time_stamp / self.second_to_audio_index)
