#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import copy
import json
import logging
import time
import xml.etree.ElementTree as ET
from multiprocessing.pool import ThreadPool
from pathlib import Path
from typing import List, Tuple

import azure.cognitiveservices.speech as speechsdk
import nltk
from nltk.tokenize import sent_tokenize
from tqdm import tqdm

from synthesizer_pool import SynthesizerPool

# Only needed for first run
nltk.download('punkt')

logger = logging.getLogger(__name__)


class LongTextSynthesizer:
    def __init__(self, subscription: str, region: str, language: str = 'english',
                 voice: str = 'en-US-AvaMultilingualNeural', parallel_threads: int = 8) -> None:
        self.is_ssml = None
        self.subscription = subscription
        self.region = region
        self.language = language
        self.voice = voice
        self.parallel_threads = parallel_threads
        self.synthesizer_pool = SynthesizerPool(self._create_synthesizer, self.parallel_threads)

    def _create_synthesizer(self) -> speechsdk.SpeechSynthesizer:
        config = speechsdk.SpeechConfig(subscription=self.subscription, region=self.region)
        config.set_speech_synthesis_output_format(speechsdk.SpeechSynthesisOutputFormat.Audio24Khz48KBitRateMonoMp3)
        config.set_property(
            speechsdk.PropertyId.SpeechServiceResponse_RequestSentenceBoundary, 'true')
        config.speech_synthesis_voice_name = self.voice
        return speechsdk.SpeechSynthesizer(config, audio_config=None)

    def synthesize_text_once(self, text: str) -> Tuple[speechsdk.SpeechSynthesisResult,
                                                       List[speechsdk.SpeechSynthesisWordBoundaryEventArgs]]:
        logger.debug("Synthesis started %s", text)
        text_boundaries = []
        finished = []

        def word_boundary_cb(evt: speechsdk.SpeechSynthesisWordBoundaryEventArgs) -> None:
            text_boundaries.append(evt)

        with self.synthesizer_pool.borrow_synthesizer() as synthesizer:
            synthesizer.synthesis_word_boundary.connect(word_boundary_cb)
            synthesizer.synthesis_completed.connect(lambda _: finished.append(True))
            synthesizer.synthesis_canceled.connect(lambda _: finished.append(True))
            for _ in range(3):  # retry count
                text_boundaries = []
                finished = []
                result = synthesizer.speak_ssml_async(text).get() if self.is_ssml else \
                    synthesizer.speak_text_async(text).get()
                if result.reason == speechsdk.ResultReason.SynthesizingAudioCompleted:
                    logger.debug("Synthesis completed %s", text)
                    while not finished:
                        time.sleep(0.1)
                    return result, text_boundaries
                elif result.reason == speechsdk.ResultReason.Canceled:
                    cancellation_details = result.cancellation_details
                    logger.warning("Synthesis canceled, error details %s", cancellation_details.error_details)
                    if cancellation_details.error_code in \
                        [speechsdk.CancellationErrorCode.ConnectionFailure,
                         speechsdk.CancellationErrorCode.ServiceUnavailable,
                         speechsdk.CancellationErrorCode.ServiceTimeout]:
                        logger.info("Synthesis canceled with connection failure, retrying.")
                        continue
                    break
            logger.error("Synthesizer failed to synthesize text")
            return None, None

    def synthesize_text(self, text: str = None, ssml_path: Path = None, output_path: Path = Path.cwd()) -> None:
        output_path.mkdir(parents=True, exist_ok=True)
        all_word_boundaries, all_sentence_boundaries = [], []
        if text is not None:
            sentences = self.split_text(text)
            self.is_ssml = False
        elif ssml_path is not None:
            sentences = self.read_and_split_ssml(ssml_path)
            self.is_ssml = True
        else:
            raise ValueError('Either text or ssml_path must be provided')
        offset = 0
        with ThreadPool(processes=self.parallel_threads) as pool:
            audio_path = output_path / 'audio.mp3'
            with audio_path.open("wb") as f:
                for result, text_boundaries in tqdm(
                        pool.imap(self.synthesize_text_once, sentences), total=len(sentences)):
                    if result is not None:
                        f.write(result.audio_data)
                        for text_boundary in text_boundaries:
                            text_boundary_dict = {
                                'audio_offset': offset + text_boundary.audio_offset / 10000,
                                'duration': text_boundary.duration.total_seconds() * 1000,
                                'text': text_boundary.text
                            }
                            if text_boundary.boundary_type == speechsdk.SpeechSynthesisBoundaryType.Sentence:
                                all_sentence_boundaries.append(text_boundary_dict)
                            else:
                                all_word_boundaries.append(text_boundary_dict)
                        # Calculate the offset for the next sentence,
                        offset += len(result.audio_data) / (48 / 8)
            with (output_path / "word_boundaries.json").open("w", encoding="utf-8") as f:
                json.dump(all_word_boundaries, f, indent=4, ensure_ascii=False)
            with (output_path / "sentence_boundaries.json").open("w", encoding="utf-8") as f:
                json.dump(all_sentence_boundaries, f, indent=4, ensure_ascii=False)

    def split_text(self, text: str) -> List[str]:
        sentences = sent_tokenize(text, language=self.language)
        logger.info(f'Splitting into {len(sentences)} sentences')
        logger.debug(sentences)
        return sentences

    @staticmethod
    def read_and_split_ssml(ssml_path: Path) -> List[str]:
        namespaces = dict([node for _, node in ET.iterparse(ssml_path, events=['start-ns'])])
        for ns in namespaces:
            ET.register_namespace(ns, namespaces[ns])
        root = ET.parse(ssml_path).getroot()
        sentences = []
        speak_element = copy.deepcopy(root)

        for child in list(speak_element):
            _, _, tag = child.tag.rpartition('}')
            if tag != 'voice':
                raise ValueError(f'Only voice element is supported, got {tag}')
            speak_element.remove(child)
        for child in root:
            single_voice = copy.deepcopy(speak_element)
            single_voice.append(child)
            sentences.append(ET.tostring(single_voice, encoding='unicode'))
        return sentences


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    s = LongTextSynthesizer(subscription="YourSubscriptionKey", region="YourServiceRegion")
    with Path('./Gatsby-chapter1.txt').open('r', encoding='utf-8') as r:
        s.synthesize_text(r.read(), output_path=Path('./gatsby'))
    s.synthesize_text(ssml_path=Path('multi-role.xml'), output_path=Path('./multi-role'))
