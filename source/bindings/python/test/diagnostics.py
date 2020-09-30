# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.

import azure.cognitiveservices.speech as msspeech
import os
import platform

from ctypes import *


class Diagnostics():
    """
    Interface to the native diagnostics memory logger. Loads the platform
    specific native library and implements select calls to diagnostics for
    logging.
    """
    def __init__(self):
        self._impl = self._get_impl()

    @staticmethod
    def _get_impl():
        system_name = platform.system()

        if system_name == "Windows":
            dll_name = "Microsoft.CognitiveServices.Speech.core.dll"
        elif system_name == "Linux":
            dll_name = "libMicrosoft.CognitiveServices.Speech.core.so"
        elif system_name == "Darwin":
            dll_name = "libMicrosoft.CognitiveServices.Speech.core.dylib"
        else:
            raise OSError("Unsupported system '{}'".format(system_name))

        try:
            dir_name = os.path.dirname(os.path.realpath(msspeech.__file__))
            dll_path = os.path.join(dir_name, dll_name)

            if system_name == "Windows":
                native_dll = WinDLL(dll_path)
            else:
                native_dll = CDLL(dll_path)

        except Exception as error:
            raise OSError("Cannot load {}: {}".format(dll_path, error))

        return native_dll

    def diagnostics_log_memory_start_logging(self):
        self._impl.diagnostics_log_memory_start_logging()

    def diagnostics_log_memory_stop_logging(self):
        self._impl.diagnostics_log_memory_stop_logging()

    def diagnostics_log_trace_string(self, level: int, title: str, file: str, line: int, message: str):
        self._impl.diagnostics_log_trace_string(
            c_int(level),
            c_char_p(title.encode('utf-8')),
            c_char_p(file.encode('utf-8')),
            c_int(line),
            c_char_p(message.encode('utf-8')))

    def diagnostics_log_memory_dump_to_stderr(self):
        self._impl.diagnostics_log_memory_dump_to_stderr()
