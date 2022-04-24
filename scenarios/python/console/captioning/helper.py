#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#

# Note: abc = abstract base classes
from collections.abc import Mapping
from datetime import time
from sys import argv
from typing import Optional

import azure.cognitiveservices.speech as speechsdk

# See speech_recognize_once_compressed_input() in:
# https://github.com/Azure-Samples/cognitive-services-speech-sdk/blob/master/samples/python/console/speech_sample.py
class BinaryFileReaderCallback(speechsdk.audio.PullAudioInputStreamCallback):
    def __init__(self, filename: str):
        super().__init__()
        self._file_h = open(filename, "rb")

    def read(self, buffer: memoryview) -> int:
        try:
            size = buffer.nbytes
            frames = self._file_h.read(size)
            buffer[:len(frames)] = frames
            return len(frames)
        except Exception as ex:
            print('Exception in `read`: {}'.format(ex))
            raise

    def close(self) -> None:
        print('closing file')
        try:
            self._file_h.close()
        except Exception as ex:
            print('Exception in `close`: {}'.format(ex))
            raise

# See
# https://stackoverflow.com/a/28452633
class Read_Only_Dict(Mapping):
    def __init__(self, data):
        self._data = data
    def __getitem__(self, key): 
        return self._data[key]
    def __len__(self):
        return len(self._data)
    def __iter__(self):
        return iter(self._data)

def get_cmd_option(option : str) -> Optional[str] :
    argc = len(argv)
    if option in argv :
        index = argv.index(option)
        if index < argc - 2 :
            # We found the option (for example, "-o"), so advance from that to the value (for example, "filename").
            return argv[index + 1]
        else :
            return None
    else :
        return None

def cmd_option_exists(option : str) -> bool :
    return option in argv

def get_compressed_audio_format() -> speechsdk.AudioStreamContainerFormat :
    value = get_cmd_option("-c")
    if value is None :
        return speechsdk.AudioStreamContainerFormat.ANY
    else :
        value = value.lower()
        if value == "alaw" : return speechsdk.AudioStreamContainerFormat.ALAW
        elif value == "flac" : return speechsdk.AudioStreamContainerFormat.FLAC
        elif value == "mp3" : return speechsdk.AudioStreamContainerFormat.MP3
        elif value == "mulaw" : return speechsdk.AudioStreamContainerFormat.MULAW
        elif value == "ogg_opus" : return speechsdk.AudioStreamContainerFormat.OGG_OPUS
        else : return speechsdk.AudioStreamContainerFormat.ANY;

# We cannot simply create time with ticks.
def time_from_ticks(ticks) -> time :
    microseconds_1 = ticks / 10
    microseconds_2 = microseconds_1 % 1000000
    seconds_1 = microseconds_1 / 1000000
    seconds_2 = seconds_1 % 60
    minutes_1 = seconds_1 / 60
    minutes_2 = minutes_1 % 60
    hours = minutes_1 / 60
    return time(int(hours), int(minutes_2), int(seconds_2), int(microseconds_2))

def write_to_console(text : str, user_config : Read_Only_Dict) :
    if not user_config["suppress_console_output"] :
        print(text, end = "", flush = True)
    return

def write_to_console_or_file(text : str, user_config : Read_Only_Dict) :
    write_to_console(text = text, user_config = user_config)
    if user_config["output_file"] is not None :
        file_path = Path(user_config["output_file"])
        with open(file_path, mode = "a", newline = "") as f :
            f.write(text)
    return
