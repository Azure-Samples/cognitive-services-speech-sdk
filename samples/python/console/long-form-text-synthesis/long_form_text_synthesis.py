#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import copy
import io
import json
import logging
import re
import time
import xml.etree.ElementTree as ET
from multiprocessing.pool import ThreadPool
from pathlib import Path
from typing import List, Tuple
from xml.parsers import expat

import azure.cognitiveservices.speech as speechsdk
from tqdm import tqdm

from synthesizer_pool import SynthesizerPool

logger = logging.getLogger(__name__)

_SENTENCE_END = re.compile(
    r'[.!?]+(?:["\'\u2019\u201D)\]}]+)?(?=\s|$)'
    r'|[\u3002\uFF01\uFF1F]+(?:["\'\u2019\u201D)\]}]+)?'
    r'|(?:\r?\n){2,}'
)
_ABBREVIATIONS = frozenset({
    'dr.', 'e.g.', 'etc.', 'i.e.', 'jr.', 'mr.', 'mrs.', 'ms.',
    'prof.', 'sr.', 'st.', 'u.k.', 'u.s.', 'vs.',
})
_INITIAL_OR_ACRONYM = re.compile(r'(?:[a-z]\.)+', re.IGNORECASE)
_TRAILING_CLOSERS = '"\'\u2019\u201D)]}'


class LongTextSynthesizer:
    def __init__(self, subscription: str, endpoint: str,
                 voice: str = 'en-US-JennyNeural', parallel_threads: int = 8) -> None:
        self.is_ssml = None
        self.subscription = subscription
        self.endpoint = endpoint
        self.voice = voice
        self.parallel_threads = parallel_threads
        self.synthesizer_pool = SynthesizerPool(self._create_synthesizer, self.parallel_threads)

    def _create_synthesizer(self) -> speechsdk.SpeechSynthesizer:
        config = speechsdk.SpeechConfig(subscription=self.subscription, endpoint=self.endpoint)
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

    @staticmethod
    def split_text(text: str) -> List[str]:
        sentences = []
        start = 0
        for match in _SENTENCE_END.finditer(text):
            sentence = text[start:match.end()].strip()
            if sentence:
                # A segment may consist solely of closing punctuation (for example a
                # stray quote or bracket on its own line). Stripping the trailing
                # closers then leaves an empty string, so guard against rsplit()
                # returning no tokens before indexing into it.
                tokens = sentence.rstrip(_TRAILING_CLOSERS).rsplit(maxsplit=1)
                last_token = tokens[-1].lower() if tokens else ''
                if match.group(0).startswith('.') and (
                        last_token in _ABBREVIATIONS
                        or _INITIAL_OR_ACRONYM.fullmatch(last_token)):
                    continue
                sentences.append(sentence)
            start = match.end()

        remainder = text[start:].strip()
        if remainder:
            sentences.append(remainder)

        logger.info(f'Splitting into {len(sentences)} sentences')
        logger.debug(sentences)
        return sentences

    @staticmethod
    def read_and_split_ssml(ssml_path: Path) -> List[str]:
        ssml = ssml_path.read_bytes()
        parser = expat.ParserCreate()

        def reject_doctype(*_args) -> None:
            raise ValueError('DOCTYPE declarations are not allowed in SSML')

        parser.StartDoctypeDeclHandler = reject_doctype
        parser.Parse(ssml, True)

        namespaces = dict([
            node for _, node in ET.iterparse(io.BytesIO(ssml), events=['start-ns'])
        ])
        for ns in namespaces:
            ET.register_namespace(ns, namespaces[ns])
        root = ET.fromstring(ssml)
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
    s = LongTextSynthesizer(subscription="YourSubscriptionKey", endpoint="YourServiceEndpoint")
    with Path('./Gatsby-chapter1.txt').open('r', encoding='utf-8') as r:
        s.synthesize_text(r.read(), output_path=Path('./gatsby'))
    s.synthesize_text(ssml_path=Path('multi-role.xml'), output_path=Path('./multi-role'))
