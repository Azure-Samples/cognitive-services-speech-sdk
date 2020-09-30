# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.

import os
import sys

from .diagnostics import Diagnostics


class SpxTest():
    """
    Utils for tracing and logging from pytest cases.
    """
    _SPX_TRACE_LEVEL_ERROR   = 0x02
    _SPX_TRACE_LEVEL_WARNING = 0x04
    _SPX_TRACE_LEVEL_INFO    = 0x08
    _SPX_TRACE_LEVEL_VERBOSE = 0x10

    def __init__(self):
        logging = os.environ.get("SPEECHSDK_TEST_LOGGING")
        self.logging = not logging or "memory" in logging

        if self.logging:
            self._impl = self._get_impl()
        else:
            sys.stderr.write("SpxTest logging disabled\n")

    @staticmethod
    def _get_impl():
        return Diagnostics()

    def start_logging(self):
        if self.logging:
            self._impl.diagnostics_log_memory_start_logging()

    def stop_logging(self):
        if self.logging:
            self._impl.diagnostics_log_memory_stop_logging()

    # SPX_TRACE_ERROR
    def trace_error(self, file: str, line: int, message: str):
        if self.logging:
            self._impl.diagnostics_log_trace_string(self._SPX_TRACE_LEVEL_ERROR, "SPX_TRACE_ERROR: ", file, line, message);

    # SPX_TRACE_INFO
    def trace_info(self, file: str, line: int, message: str):
        if self.logging:
            self._impl.diagnostics_log_trace_string(self._SPX_TRACE_LEVEL_INFO, "SPX_TRACE_INFO: ", file, line, message);

    def dump_log(self):
        if self.logging:
            self._impl.diagnostics_log_memory_dump_to_stderr()
