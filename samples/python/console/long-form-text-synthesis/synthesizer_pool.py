#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import logging
import queue
from contextlib import contextmanager
from queue import LifoQueue
from typing import Callable

import azure.cognitiveservices.speech as speechsdk

logger = logging.getLogger(__name__)


class SynthesizerPool(object):
    def __init__(self, create_fn: Callable[[], speechsdk.SpeechSynthesizer], max_size: int = 32) -> None:
        self.create_fn = create_fn
        self._queue = LifoQueue(maxsize=max_size)

    def _create_synthesizer(self) -> speechsdk.SpeechSynthesizer:
        synthesizer = self.create_fn()
        if isinstance(synthesizer, speechsdk.SpeechSynthesizer):
            return synthesizer
        raise TypeError("create_fn should return a SpeechSynthesizer")

    def _borrow(self) -> speechsdk.SpeechSynthesizer:
        try:
            return self._queue.get(block=False)
        except queue.Empty:
            return self._create_synthesizer()
        except Exception as e:
            logger.error(e)
            raise e

    def _return(self, synthesizer: speechsdk.SpeechSynthesizer) -> None:
        try:
            synthesizer.synthesis_word_boundary.disconnect_all()
            synthesizer.synthesis_completed.disconnect_all()
            synthesizer.synthesis_canceled.disconnect_all()
            self._queue.put(synthesizer, block=False)
        except queue.Full:
            logger.warning("Synthesizer pool is full, discard the synthesizer")

    @contextmanager
    def borrow_synthesizer(self) -> speechsdk.SpeechSynthesizer:
        obj = self._borrow()
        try:
            yield obj
        except Exception as e:
            yield None
            raise e
        finally:
            self._return(obj)
