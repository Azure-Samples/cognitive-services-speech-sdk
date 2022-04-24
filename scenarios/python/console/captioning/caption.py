#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#

# Notes:
# - Install the Speech SDK. Run:
# pip install azure-cognitiveservices-speech
# - The Python Speech SDK on Windows requires the Microsoft Visual C++ Redistributable for Visual Studio 2015, 2017, 2019, or 2022 on the system. See:
# https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/setup-platform
# - Install gstreamer:
# https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-use-codec-compressed-audio-input-streams

from datetime import datetime
from os import linesep, remove
from os.path import exists
from pathlib import Path
from sys import argv
from time import sleep
from typing import Any
import wave

import azure.cognitiveservices.speech as speechsdk
import helper

def user_config_from_args() -> helper.Read_Only_Dict :
    argc = len(argv)
    if argc < 3 :
        raise RuntimeError("Too few arguments. Usage:{}{}".format(linesep, usage))

    return helper.Read_Only_Dict({
        "compressed_audio_format" : helper.get_compressed_audio_format(),
        "remove_profanity" : helper.cmd_option_exists("-f"),
        "disable_masking_profanity" : helper.cmd_option_exists("-m"),
        "suppress_console_output" : helper.cmd_option_exists("-q"),
        "use_sub_rip_text_caption_format" : helper.cmd_option_exists("-s"),
        "input_file" : helper.get_cmd_option("-i"),
        "output_file" : helper.get_cmd_option("-o"),
        "language_ID_languages" : helper.get_cmd_option("-l"),
        "phrase_list" : helper.get_cmd_option("-p"),
        "show_recognizing_results" : helper.get_cmd_option("-u"),
        "stable_partial_result_threshold" : helper.get_cmd_option("-t"),
        "subscription_key" : argv[argc - 2],
        "region" : argv[argc - 1],
    })

# Note: Continuous language identification is supported only with v2 endpoints.
def v2_endpoint_from_region(region : str) -> str :
    return "wss://{}.stt.speech.microsoft.com/speech/universal/v2".format(region);

def timestamp_from_speech_recognition_result(result : speechsdk.SpeechRecognitionResult, user_config : helper.Read_Only_Dict) -> str :
    start_time = helper.time_from_ticks(ticks = result.offset)
    end_time = helper.time_from_ticks(ticks = result.offset + result.duration)
    time_format = ""
    if user_config["use_sub_rip_text_caption_format"] :
        # SRT format requires ',' as decimal separator rather than '.'.
        time_format = "%H:%M:%S,%f"
    else :
        time_format = "%H:%M:%S.%f"
    # Truncate microseconds to milliseconds.
    return "{} --> {}".format(start_time.strftime(time_format)[:-3], end_time.strftime(time_format)[:-3])

def language_from_speech_recognition_result(result : speechsdk.SpeechRecognitionResult, user_config : helper.Read_Only_Dict) -> str :
    if user_config["language_ID_languages"] is not None :
        return "[{}] ".format(speechsdk.AutoDetectSourceLanguageResult(result).language)
    else :
        return ""

def caption_from_speech_recognition_result(sequence_number : int, result : speechsdk.SpeechRecognitionResult, user_config : helper.Read_Only_Dict) -> str :
    caption = ""
    if not user_config["show_recognizing_results"] and user_config["use_sub_rip_text_caption_format"] :
        caption += str(sequenceNumber) + linesep
    caption += timestamp_from_speech_recognition_result(result = result, user_config = user_config) + linesep
    caption += language_from_speech_recognition_result(result = result, user_config = user_config)
    caption += result.text + linesep + linesep
    return caption

def initialize(user_config : helper.Read_Only_Dict) :
    if user_config["output_file"] is not None and exists(user_config["output_file"]) :
        remove(user_config["output_file"])
    if not user_config["use_sub_rip_text_caption_format"] :
        helper.write_to_console_or_file(text = "WEBVTT{}{}".format(linesep, linesep), user_config = user_config)
    return

def audio_config_from_user_config(user_config : helper.Read_Only_Dict) -> tuple[speechsdk.AudioConfig, Any, speechsdk.audio.AudioStreamFormat, speechsdk.audio.PullAudioInputStream] :
    if user_config["input_file"] is None :
        return speechsdk.AudioConfig(use_default_microphone = True)
    else :
        format = None
        if user_config["input_file"].endswith(".wav") :        
            reader = wave.open(user_config["input_file"], mode=None)
            format = speechsdk.audio.AudioStreamFormat(samples_per_second=reader.getframerate(), bits_per_sample=reader.getsampwidth() * 8, channels=reader.getnchannels())
            reader.close()        
        else :
            format = speechsdk.audio.AudioStreamFormat(compressed_stream_format=user_config["compressed_audio_format"])
        callback = helper.BinaryFileReaderCallback(filename=user_config["input_file"])
        stream = speechsdk.audio.PullAudioInputStream(pull_stream_callback = callback, stream_format = format)
        # TODO1 Need to return callback, format, stream to keep them in scope.
        return speechsdk.audio.AudioConfig(stream=stream), callback, format, stream

def speech_config_from_user_config(user_config : helper.Read_Only_Dict) -> speechsdk.SpeechConfig :
    speech_config = None
    if user_config["language_ID_languages"] is not None :
        endpoint = v2_endpoint_from_region(user_config["region"])
        speech_config = speechsdk.SpeechConfig(endpoint = endpoint, subscription = user_config["subscription_key"])
    else :
        speech_config = speechsdk.SpeechConfig(subscription = user_config["subscription_key"], region = user_config["region"])

    if user_config["remove_profanity"] :
        speech_config.set_profanity(speechsdk.ProfanityOption.Removed)
    elif user_config["disable_masking_profanity"] :
        speech_config.set_profanity(speechsdk.ProfanityOption.Raw)

    if user_config["stable_partial_result_threshold"] is not None :
        speech_config.set_property(property_id = speechsdk.PropertyId.SpeechServiceResponse_StablePartialResultThreshold, value = user_config["stable_partial_result_threshold"])

    speech_config.set_property(property_id = speechsdk.PropertyId.SpeechServiceResponse_PostProcessingOption, value = "TrueText")

    return speech_config

def speech_recognizer_from_user_config(user_config : helper.Read_Only_Dict) -> tuple[speechsdk.SpeechRecognizer, Any, speechsdk.audio.AudioStreamFormat, speechsdk.audio.PullAudioInputStream] :
    (audio_config, callback, format, stream) = audio_config_from_user_config(user_config)
    speech_config = speech_config_from_user_config(user_config)
    speech_recognizer = None

    if user_config["language_ID_languages"] is not None :
        auto_detect_source_language_config = speechsdk.AutoDetectSourceLanguageConfig(user_config["language_ID_languages"].split(","))
        speech_recognizer = speechsdk.SpeechRecognizer(speech_config = speech_config, audio_config = audio_config, auto_detect_source_language_config = auto_detect_source_language_config)
    else :
        speech_recognizer = speechsdk.SpeechRecognizer(speech_config = speech_config, audio_config = audio_config)

    if user_config["phrase_list"] is not None :
        grammar = speechsdk.PhraseListGrammar.from_recognizer(recognizer = speech_recognizer)
        grammar.addPhrase(user_config["phrase_list"])

    return (speech_recognizer, callback, format, stream)

def recognize_continuous(speech_recognizer : speechsdk.SpeechRecognizer, user_config : helper.Read_Only_Dict, callback : Any, format : speechsdk.audio.AudioStreamFormat, stream : speechsdk.audio.PullAudioInputStream) :
    sequence_number = 0
    done = False

    def recognizing_handler(e : speechsdk.SpeechRecognitionEventArgs) :
        if speechsdk.ResultReason.RecognizingSpeech == e.result.reason and len(e.result.text) > 0 :
            # We don't show sequence numbers for partial results.
            helper.write_to_console(text = caption_from_speech_recognition_result(sequence_number = 0, result = e.result, user_config = user_config), user_config = user_config)
        elif speechsdk.ResultReason.NoMatch == e.result.reason :
            helper.write_to_console(text = "NOMATCH: Speech could not be recognized.{}".format(linesep), user_config = user_config)

    def recognized_handler(e : speechsdk.SpeechRecognitionEventArgs) :
        if speechsdk.ResultReason.RecognizedSpeech == e.result.reason and len(e.result.text) > 0 :
            nonlocal sequence_number
            sequence_number += 1
            helper.write_to_console_or_file(text = caption_from_speech_recognition_result(sequence_number = sequence_number, result = e.result, user_config = user_config), user_config = user_config)
        elif speechsdk.ResultReason.NoMatch == e.result.reason :
            helper.write_to_console(text = "NOMATCH: Speech could not be recognized.{}".format(linesep), user_config = user_config)

    def canceled_handler(e : speechsdk.SpeechRecognitionCanceledEventArgs) :
        nonlocal done
        # Notes:
        # SpeechRecognitionCanceledEventArgs inherits the result property from SpeechRecognitionEventArgs. See:
        # https://docs.microsoft.com/python/api/azure-cognitiveservices-speech/azure.cognitiveservices.speech.speechrecognitioncanceledeventargs
        # https://docs.microsoft.com/python/api/azure-cognitiveservices-speech/azure.cognitiveservices.speech.speechrecognitioneventargs
        # result is type SpeechRecognitionResult, which inherits the reason property from RecognitionResult. See:
        # https://docs.microsoft.com/python/api/azure-cognitiveservices-speech/azure.cognitiveservices.speech.speechrecognitionresult
        # https://docs.microsoft.com/python/api/azure-cognitiveservices-speech/azure.cognitiveservices.speech.recognitionresult
        # e.result.reason is ResultReason.Canceled. To get the cancellation reason, see e.cancellation_details.reason.
        if speechsdk.CancellationReason.EndOfStream == e.cancellation_details.reason :
            helper.write_to_console(text = "End of stream reached.{}".format(linesep), user_config = user_config)
            done = True
        elif speechsdk.CancellationReason.CancelledByUser == e.cancellation_details.reason :
            helper.write_to_console(text = "User canceled request.{}".format(linesep), user_config = user_config)
            done = True
        elif speechsdk.CancellationReason.Error == e.cancellation_details.reason :
            # Error output should not be suppressed, even if suppress output flag is set.
            print("Encountered error. Cancellation details: {}{}".format(e.cancellation_details, linesep))
            done = True
        else :
            print("Request was cancelled for an unrecognized reason. Cancellation details: {}{}".format(e.cancellation_details, linesep))
            done = True

    def stopped_handler(e : speechsdk.SessionEventArgs) :
        nonlocal done
        helper.write_to_console(text = "Session stopped.{}".format(linesep), user_config = user_config)
        done = True

    if user_config["show_recognizing_results"] :
        speech_recognizer.recognizing.connect(recognizing_handler)
    speech_recognizer.recognized.connect(recognized_handler)
    speech_recognizer.session_stopped.connect(stopped_handler)
    speech_recognizer.canceled.connect(canceled_handler)

    speech_recognizer.start_continuous_recognition()

    while not done :
        sleep(5)
    speech_recognizer.stop_continuous_recognition()

    return

usage = """Usage: python caption.py [-c ALAW|ANY|FLAC|MP3|MULAW|OGG_OPUS] [-f] [-h] [-i file] [-l languages] [-m] [-o file] [-p phrases] [-q] [-s] [-t number] [-u] <subscriptionKey> <region>
       -c format: Use compressed audio format.
                  Valid values: ALAW, ANY, FLAC, MP3, MULAW, OGG_OPUS.
                  Default value: ANY.
              -f: Remove profanity (default behavior is to mask profanity). Overrides -m.
              -h: Show this help and stop.
              -i: Input audio file *file* (default input is from the microphone.)
    -l languages: Enable language identification for specified *languages*.
                  Example: en-US,ja-JP
              -m: Disable masking profanity (default behavior). -f overrides this.
         -o file: Output to *file*.
      -p phrases: Add specified *phrases*.
                  Example: Constoso;Jessie;Rehaan
              -q: Suppress console output (except errors).
              -s: Output captions in SRT format (default is WebVTT format.)
       -t number: Set stable partial result threshold to *number*.
                  Example: 3
              -u: Output partial results. These are always written to the console, never to an output file. -q overrides this."""

try :
    if helper.cmd_option_exists("-h") :
        print(usage)
    else :
        user_config = user_config_from_args()
        initialize(user_config = user_config)
        (speech_recognizer, stream, format, callback) = speech_recognizer_from_user_config(user_config = user_config)
        recognize_continuous(speech_recognizer = speech_recognizer, user_config = user_config, stream = stream, format = format, callback = callback)
except Exception as e:
    print(e)
