#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

"""
Embedded (offline) speech samples for the Speech SDK for Python.

This app displays a menu of embedded speech recognition, synthesis and translation
scenarios. Configure the embedded model paths and names in settings.py (or via the
corresponding environment variables) before running.
"""

import platform
from collections import OrderedDict

import speech_recognition_samples
import speech_synthesis_samples
import speech_translation_samples

eofkey = 'Ctrl-Z' if "Windows" == platform.system() else 'Ctrl-D'

samples = OrderedDict([
    (speech_recognition_samples, [
        speech_recognition_samples.embedded_recognition_list_models,
        speech_recognition_samples.embedded_recognition_from_wave_file,
        speech_recognition_samples.embedded_recognition_once_from_microphone,
        speech_recognition_samples.embedded_recognition_continuous_from_microphone,
        speech_recognition_samples.embedded_recognition_keyword_from_microphone,
    ]), (speech_synthesis_samples, [
        speech_synthesis_samples.embedded_synthesis_list_voices,
        speech_synthesis_samples.embedded_synthesis_to_speaker,
        speech_synthesis_samples.embedded_synthesis_to_wave_file,
    ]), (speech_translation_samples, [
        speech_translation_samples.embedded_translation_list_models,
        speech_translation_samples.embedded_translation_once_from_microphone,
        speech_translation_samples.embedded_translation_continuous_from_microphone,
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
        print("{}: {}\n\t{}".format(i, fun.__name__, fun.__doc__.strip()))

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


def main():
    while True:
        try:
            select()
        except EOFError:
            break


if __name__ == "__main__":
    main()
