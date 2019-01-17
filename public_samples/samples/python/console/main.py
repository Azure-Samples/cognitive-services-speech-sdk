#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import speech_sample
import intent_sample
import translation_sample

from collections import OrderedDict
import platform

eofkey = 'Ctrl-Z' if "Windows" == platform.system() else 'Ctrl-D'
print('system: {}, eofkey: {}'.format(platform.system(), eofkey))

samples = OrderedDict({
    speech_sample: [
        speech_sample.speech_recognize_once_from_mic,
        speech_sample.speech_recognize_once_from_file,
        speech_sample.speech_recognize_once_from_file_with_customized_model,
        speech_sample.speech_recognize_once_from_file_with_custom_endpoint_parameters,
        speech_sample.speech_recognize_async_from_file,
        speech_sample.speech_recognize_continuous_from_file,
        speech_sample.speech_recognition_with_pull_stream,
        speech_sample.speech_recognition_with_push_stream,
    ], intent_sample: [
        intent_sample.recognize_intent_once_from_mic,
        intent_sample.recognize_intent_once_from_file,
        intent_sample.recognize_intent_continuous,
    ], translation_sample: [
        translation_sample.translation_once_from_mic,
        translation_sample.translation_once_from_file,
        translation_sample.translation_continuous,
    ]
})


def select():
    print('select sample module, {} to abort'.format(eofkey))
    modules = list(samples.keys())
    for i, module in enumerate(modules):
        print(i, module.__name__)

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
        print(i, fun.__name__)

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
