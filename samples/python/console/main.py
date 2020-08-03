#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import speech_sample
import intent_sample
import translation_sample
import speech_synthesis_sample

from collections import OrderedDict
import platform

eofkey = 'Ctrl-Z' if "Windows" == platform.system() else 'Ctrl-D'

samples = OrderedDict([
    (speech_sample, [
        speech_sample.speech_recognize_once_from_mic,
        speech_sample.speech_recognize_once_from_file,
        speech_sample.speech_recognize_once_from_file_with_customized_model,
        speech_sample.speech_recognize_once_from_file_with_custom_endpoint_parameters,
        speech_sample.speech_recognize_async_from_file,
        speech_sample.speech_recognize_continuous_from_file,
        speech_sample.speech_recognition_with_pull_stream,
        speech_sample.speech_recognition_with_push_stream,
        speech_sample.speech_recognize_keyword_from_microphone,
    ]), (intent_sample, [
        intent_sample.recognize_intent_once_from_mic,
        intent_sample.recognize_intent_once_async_from_mic,
        intent_sample.recognize_intent_once_from_file,
        intent_sample.recognize_intent_continuous,
    ]), (translation_sample, [
        translation_sample.translation_once_from_mic,
        translation_sample.translation_once_from_file,
        translation_sample.translation_continuous,
    ]), (speech_synthesis_sample, [
        speech_synthesis_sample.speech_synthesis_to_speaker,
        speech_synthesis_sample.speech_synthesis_with_language,
        speech_synthesis_sample.speech_synthesis_with_voice,
        speech_synthesis_sample.speech_synthesis_to_wave_file,
        speech_synthesis_sample.speech_synthesis_to_mp3_file,
        speech_synthesis_sample.speech_synthesis_to_pull_audio_output_stream,
        speech_synthesis_sample.speech_synthesis_to_push_audio_output_stream,
        speech_synthesis_sample.speech_synthesis_to_result,
        speech_synthesis_sample.speech_synthesis_to_audio_data_stream,
        speech_synthesis_sample.speech_synthesis_events,
        speech_synthesis_sample.speech_synthesis_word_boundary_event,
        speech_synthesis_sample.speech_synthesis_with_auto_language_detection_to_speaker,
    ])
])


def select():
    print('select sample module, {} to abort'.format(eofkey))
    modules = list(samples.keys())
    for i, module in enumerate(modules):
        print("{}: {}\n\t{}".format(i, module.__name__, module.__doc__.strip()))

    try:
        num = int(input())
        selected_module = modules[num]
    except EOFError:
        raise
    except Exception as e:
        print(e)
        return

    print('select sample function, {} to abort'.format(eofkey))
    for i, fun in enumerate(samples[selected_module]):
        print("{}: {}\n\t{}".format(i, fun.__name__, fun.__doc__))

    try:
        num = int(input())
        selected_function = samples[selected_module][num]
    except EOFError:
        raise
    except Exception as e:
        print(e)
        return

    print('You selected: {}'.format(selected_function))
    try:
        selected_function()
    except Exception as e:
        print('Error running sample: {}'.format(e))

    print()


while True:
    try:
        select()
    except EOFError:
        break
