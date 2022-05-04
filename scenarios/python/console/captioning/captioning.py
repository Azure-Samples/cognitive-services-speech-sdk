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

def user_config_from_args(usage : str) -> helper.Read_Only_Dict :
    key = helper.get_cmd_option("--key")
    if key is None:
        raise RuntimeError("Missing subscription key.{}{}".format(linesep, usage))
    region = helper.get_cmd_option("--region")
    if region is None:
        raise RuntimeError("Missing region.{}{}".format(linesep, usage))
        
    return helper.Read_Only_Dict({
        "use_compressed_audio" : helper.cmd_option_exists("--format"),
        "compressed_audio_format" : helper.get_compressed_audio_format(),
        "profanity_option" : helper.get_profanity_option(),
        "suppress_console_output" : helper.cmd_option_exists("--quiet"),
        "use_sub_rip_text_caption_format" : helper.cmd_option_exists("--srt"),
        "input_file" : helper.get_cmd_option("--input"),
        "output_file" : helper.get_cmd_option("--output"),
        "language_ID_languages" : helper.get_cmd_option("--languages"),
        "phrase_list" : helper.get_cmd_option("--phrases"),
        "show_recognizing_results" : helper.get_cmd_option("--recognizing"),
        "stable_partial_result_threshold" : helper.get_cmd_option("--threshold"),
        "subscription_key" : key,
        "region" : region,
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

def audio_config_from_user_config(user_config : helper.Read_Only_Dict) -> helper.Read_Only_Dict :
    if user_config["input_file"] is None :
        return speechsdk.AudioConfig(use_default_microphone = True), None, None, None
    else :
        audio_stream_format = None
        if not user_config["use_compressed_audio"] :
            reader = wave.open(user_config["input_file"], mode=None)
            audio_stream_format = speechsdk.audio.AudioStreamFormat(samples_per_second=reader.getframerate(), bits_per_sample=reader.getsampwidth() * 8, channels=reader.getnchannels())
            reader.close()
        else :
            audio_stream_format = speechsdk.audio.AudioStreamFormat(compressed_stream_format=user_config["compressed_audio_format"])
        callback = helper.BinaryFileReaderCallback(filename=user_config["input_file"])
        stream = speechsdk.audio.PullAudioInputStream(pull_stream_callback = callback, stream_format = audio_stream_format)
        # We return the BinaryFileReaderCallback, AudioStreamFormat, and PullAudioInputStream
        # because we need to keep them in scope until they are actually used.
        return helper.Read_Only_Dict({
            "audio_config" : speechsdk.audio.AudioConfig(stream=stream),
            "audio_stream_format" : audio_stream_format,
            "pull_input_audio_stream_callback" : callback,
            "pull_input_audio_stream" : stream,
        })

def speech_config_from_user_config(user_config : helper.Read_Only_Dict) -> speechsdk.SpeechConfig :
    speech_config = None
    if user_config["language_ID_languages"] is not None :
        endpoint = v2_endpoint_from_region(user_config["region"])
        speech_config = speechsdk.SpeechConfig(endpoint = endpoint, subscription = user_config["subscription_key"])
    else :
        speech_config = speechsdk.SpeechConfig(subscription = user_config["subscription_key"], region = user_config["region"])

    speech_config.set_profanity(user_config["profanity_option"])

    if user_config["stable_partial_result_threshold"] is not None :
        speech_config.set_property(property_id = speechsdk.PropertyId.SpeechServiceResponse_StablePartialResultThreshold, value = user_config["stable_partial_result_threshold"])

    speech_config.set_property(property_id = speechsdk.PropertyId.SpeechServiceResponse_PostProcessingOption, value = "TrueText")

    return speech_config

def speech_recognizer_from_user_config(user_config : helper.Read_Only_Dict) -> helper.Read_Only_Dict :
    audio_config_data = audio_config_from_user_config(user_config)
    speech_config = speech_config_from_user_config(user_config)
    speech_recognizer = None

    if user_config["language_ID_languages"] is not None :
        auto_detect_source_language_config = speechsdk.AutoDetectSourceLanguageConfig(user_config["language_ID_languages"].split(";"))
        speech_recognizer = speechsdk.SpeechRecognizer(speech_config = speech_config, audio_config = audio_config_data["audio_config"], auto_detect_source_language_config = auto_detect_source_language_config)
    else :
        speech_recognizer = speechsdk.SpeechRecognizer(speech_config = speech_config, audio_config = audio_config_data["audio_config"])

    if user_config["phrase_list"] is not None :
        grammar = speechsdk.PhraseListGrammar.from_recognizer(recognizer = speech_recognizer)
        grammar.addPhrase(user_config["phrase_list"])

    return helper.Read_Only_Dict({
        "speech_recognizer" : speech_recognizer,
        "audio_stream_format" : audio_config_data["audio_stream_format"],
        "pull_input_audio_stream_callback" : audio_config_data["pull_input_audio_stream_callback"],
        "pull_input_audio_stream" : audio_config_data["pull_input_audio_stream"],
    })

def recognize_continuous(speech_recognizer : speechsdk.SpeechRecognizer, user_config : helper.Read_Only_Dict, format : speechsdk.audio.AudioStreamFormat, callback : helper.BinaryFileReaderCallback, stream : speechsdk.audio.PullAudioInputStream) :
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

usage = """Usage: python captioning.py [...]

  HELP
    --help                        Show this help and stop.

  CONNECTION
    --key KEY                     Your Azure Speech service subscription key.
    --region REGION               Your Azure Speech service region.
                                  Examples: westus, eastus

  LANGUAGE
    --languages LANG1;LANG2       Enable language identification for specified languages.
                                  Example: en-US;ja-JP

  INPUT
    --input FILE                  Input audio from file (default input is the microphone.)
    --format FORMAT               Use compressed audio format.
                                  If this is not present, uncompressed format (wav) is assumed.
                                  Valid only with --file.
                                  Valid values: alaw, any, flac, mp3, mulaw, ogg_opus

  RECOGNITION
    --recognizing                 Output Recognizing results (default output is Recognized results only.)
                                  These are always written to the console, never to an output file.
                                  --quiet overrides this.

  ACCURACY
    --phrases PHRASE1;PHRASE2     Example: Constoso;Jessie;Rehaan

  OUTPUT
    --output FILE                 Output captions to text file.
    --srt                         Output captions in SubRip Text format (default format is WebVTT.)
    --quiet                       Suppress console output, except errors.
    --profanity OPTION            Valid values: raw, remove, mask
    --threshold NUMBER            Set stable partial result threshold.
                                  Default value: 3
"""

try :
    if helper.cmd_option_exists("--help") :
        print(usage)
    else :
        user_config = user_config_from_args(usage)
        initialize(user_config = user_config)
        speech_recognizer_data = speech_recognizer_from_user_config(user_config = user_config)
        recognize_continuous(speech_recognizer = speech_recognizer_data["speech_recognizer"], user_config = user_config, format = speech_recognizer_data["audio_stream_format"], callback = speech_recognizer_data["pull_input_audio_stream_callback"], stream = speech_recognizer_data["pull_input_audio_stream"])
except Exception as e:
    print(e)
