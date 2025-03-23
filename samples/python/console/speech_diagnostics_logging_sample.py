#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
"""
Speech diagnostics API samples for the Microsoft Cognitive Services Speech SDK
"""

import threading
import sys

try:
    import azure.cognitiveservices.speech as speechsdk
    import azure.cognitiveservices.speech.diagnostics.logging as speechsdk_logging
except ImportError:
    print("""
    Importing the Speech SDK for Python failed.
    Refer to
    https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-python for
    installation instructions.
    """)
    sys.exit(1)

# Set up the subscription info for the Speech Service:
# Replace with your own subscription key and endpoint.
speech_key, speech_endpoint = "YourSubscriptionKey", "https://YourServiceRegion.api.cognitive.microsoft.com"

# Specify the path to an audio file containing speech (mono WAV / PCM with a sampling rate of 16
# kHz).
weatherfilename = "whatstheweatherlike.wav"


def speech_diagnostics_file_logger_without_filter():
    """Enable Speech SDK trace logging to a file"""
    # Start logging to the file with the full path on your local disk
    log_file = "speech-sdk-log.txt"
    speechsdk_logging.FileLogger.start(log_file)

    # Creates a speech recognizer
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)
    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config, language="en-US", audio_config=audio_config)

    # Start recognition and wait for a result
    result = speech_recognizer.recognize_once()
    print("Recognition result: {}".format(result.text))

    # Stop logging to the file
    speechsdk_logging.FileLogger.stop()


def speech_diagnostics_file_logger_with_filter():
    """Enable Speech SDK trace logging with filter to a file"""
    # Start logging to to the file with the full path on your local disk. But log only
    # traces that contain either one of the filter strings provided. By default it will
    # create a new file, but you have the option of appending to an existing one.
    log_file = "speech-sdk-log.txt"
    filters = {"SPX_DBG_TRACE_SCOPE_ENTER", "SPX_DBG_TRACE_SCOPE_EXIT"}
    speechsdk_logging.FileLogger.set_filters(filters)
    speechsdk_logging.FileLogger.start(log_file)

    # Creates a speech recognizer
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)
    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config, language="en-US", audio_config=audio_config)

    # Start recognition and wait for a result
    result = speech_recognizer.recognize_once()
    print("Recognition result: {}".format(result.text))

    # Stop logging to the file
    speechsdk_logging.FileLogger.stop()
    speechsdk_logging.FileLogger.set_filters()


def speech_diagnostics_event_logger_without_filter():
    """Enable Speech SDK trace logging to a subscribed event handler"""
    # Set up a callback to receive log messages
    messages = []
    lock = threading.Lock()

    def on_log(msg):
        with lock:
            messages.append(msg)
    speechsdk_logging.EventLogger.set_callback(on_log)

    # Creates a speech recognizer
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)
    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config, language="en-US", audio_config=audio_config)

    # Start recognition and wait for a result
    result = speech_recognizer.recognize_once()
    print("Recognition result: {}".format(result.text))

    # Unset the callback to stop log messages
    speechsdk_logging.EventLogger.set_callback()

    # Print the log messages
    for message in messages:
        print(message, end="")


def speech_diagnostics_event_logger_with_filter():
    """Enable Speech SDK trace logging with filter to a subscribed event handler"""
    # Set up a callback to receive log messages. But log only traces that contain either
    # one of the filter strings provided.
    messages = []
    lock = threading.Lock()

    def on_log(msg):
        with lock:
            messages.append(msg)
    filters = {"SPX_DBG_TRACE_SCOPE_ENTER", "SPX_DBG_TRACE_SCOPE_EXIT"}
    print("Setting filters to: {}".format(filters))
    speechsdk_logging.EventLogger.set_filters(filters)
    speechsdk_logging.EventLogger.set_callback(on_log)

    # Creates a speech recognizer
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)
    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config, language="en-US", audio_config=audio_config)

    # Start recognition and wait for a result
    result = speech_recognizer.recognize_once()
    print("Recognition result: {}".format(result.text))

    # Unset the callback to stop log messages
    speechsdk_logging.EventLogger.set_callback()
    speechsdk_logging.EventLogger.set_filters()

    # Print the log messages
    for message in messages:
        print(message, end="")


def speech_diagnostics_memory_logger_without_filter():
    """Enable Speech SDK trace logging to memory buffer"""
    # Start logging to the memory buffer
    speechsdk_logging.MemoryLogger.start()

    # Creates a speech recognizer
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)
    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config, language="en-US", audio_config=audio_config)

    # Start recognition and wait for a result
    result = speech_recognizer.recognize_once()
    print("Recognition result: {}".format(result.text))

    # At any time (while still logging or after logging is stopped) you can dump the
    # recent traces from memory to a file:
    speechsdk_logging.MemoryLogger.dump("speech-sdk-log.txt")

    # Or dump to any object that is derived from IOBase. For example, sys.stdout,
    # which will enable logging to your console windows:
    speechsdk_logging.MemoryLogger.dump_to_stream(sys.stdout)

    # Or dump to a list of strings, and see the results on the console:
    messages = speechsdk_logging.MemoryLogger.dump_to_list()
    for message in messages:
        print(message, end="")

    # Stop logging to the memory buffer
    speechsdk_logging.MemoryLogger.stop()


def speech_diagnostics_memory_logger_with_filter():
    """Enable Speech SDK trace logging with filter to memory buffer"""
    # Start logging with filter to the memory buffer
    filters = {"SPX_DBG_TRACE_SCOPE_ENTER", "SPX_DBG_TRACE_SCOPE_EXIT"}
    print("Setting filters to: {}".format(filters))
    speechsdk_logging.MemoryLogger.set_filters(filters)
    speechsdk_logging.MemoryLogger.start()

    # Creates a speech recognizer
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)
    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config, language="en-US", audio_config=audio_config)

    # Start recognition and wait for a result
    result = speech_recognizer.recognize_once()
    print("Recognition result: {}".format(result.text))

    # At any time (while still logging or after logging is stopped) you can dump the
    # recent traces from memory to a file:
    speechsdk_logging.MemoryLogger.dump("speech-sdk-log.txt")

    # Or dump to any object that is derived from IOBase. For example, sys.stdout,
    # which will enable logging to your console windows:
    speechsdk_logging.MemoryLogger.dump_to_stream(sys.stdout)

    # Or dump to a list of strings, and see the results on the console:
    messages = speechsdk_logging.MemoryLogger.dump_to_list()
    for message in messages:
        print(message, end="")

    # Stop logging to the memory buffer and unset the filters
    speechsdk_logging.MemoryLogger.stop()
    speechsdk_logging.MemoryLogger.set_filters()


def speech_diagnostics_self_defined_spx_trace_logging():
    """Ingest self-defined trace into Speech SDK trace and log the mixed trace to memory buffer"""
    # Start logging with filter to the memory buffer
    speechsdk_logging.MemoryLogger.start()

    speechsdk_logging.SpxTrace.trace_info("### This is my trace info -- START ###")

    # Creates a speech recognizer
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)
    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config, language="en-US", audio_config=audio_config)

    # Start recognition and wait for a result
    result = speech_recognizer.recognize_once()
    print("Recognition result: {}".format(result.text))

    speechsdk_logging.SpxTrace.trace_info("### This is my trace info -- STOP ###")

    # Stop logging to the memory buffer
    speechsdk_logging.MemoryLogger.stop()

    # Dump the recent traces from memory to a file
    speechsdk_logging.MemoryLogger.dump("speech-sdk-log.txt")
