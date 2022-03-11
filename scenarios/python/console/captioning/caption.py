# Dependencies
# Note: abc = abstract base classes
from collections.abc import Mapping
import datetime
from os import linesep, remove
from os.path import exists
from pathlib import Path
from sys import argv
from time import sleep
from typing import Optional

# Notes:
# - Install the Speech SDK. Run:
# pip install azure-cognitiveservices-speech
# - The Python Speech SDK on Windows requires the Microsoft Visual C++ Redistributable for Visual Studio 2015, 2017, 2019, or 2022 on the system. See:
# https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/setup-platform?pivots=programming-language-python&tabs=dotnet%2Cwindows%2Cjre%2Cbrowser#prerequisites

import azure.cognitiveservices.speech as speechsdk

# Types

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

class Too_Few_Arguments(Exception) :
    pass

# Helper functions

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

def user_config_from_args() -> Read_Only_Dict :
    argc = len(argv)
    if argc < 3 :
        raise Too_Few_Arguments

    return Read_Only_Dict({
        "profanity_filter_remove_enabled" : cmd_option_exists("-f"),
        "profanity_filter_mask_enabled" : cmd_option_exists("-m"),
        "suppress_output_enabled" : cmd_option_exists("-q"),
        "srt_enabled" : cmd_option_exists("-s"),
        "true_text_enabled" : cmd_option_exists("-t"),
        "input_file" : get_cmd_option("-i"),
        "output_file" : get_cmd_option("-o"),
        "languages" : get_cmd_option("-l"),
        "phrases" : get_cmd_option("-p"),
        "partial_results_enabled" : get_cmd_option("-u"),
        "stable_partial_result_threshold" : get_cmd_option("-r"),
        "subscription_key" : argv[argc - 2],
        "region" : argv[argc - 1],
    })

# Note: Continuous language identification is supported only with v2 endpoints.
def v2_endpoint_from_region(region : str) -> str :
    return "wss://" + region + ".stt.speech.microsoft.com/speech/universal/v2"

# We cannot simply create datetime.time with ticks.
def time_from_ticks(ticks) -> datetime.time :
    microseconds_1 = ticks / 10
    microseconds_2 = microseconds_1 % 1000000
    seconds_1 = microseconds_1 / 1000000
    seconds_2 = seconds_1 % 60
    minutes_1 = seconds_1 / 60
    minutes_2 = minutes_1 % 60
    hours = minutes_1 / 60
    return datetime.time(int(hours), int(minutes_2), int(seconds_2), int(microseconds_2))

def timestamp_from_speech_recognition_result(result : speechsdk.SpeechRecognitionResult, user_config : Read_Only_Dict) -> str :
    start_time = time_from_ticks(ticks = result.offset)
    end_time = time_from_ticks(ticks = result.offset + result.duration)
    time_format = ""
    if user_config["srt_enabled"] :
# SRT format requires ',' as decimal separator rather than '.'.
        time_format = "%H:%M:%S,%f"
    else :
        time_format = "%H:%M:%S.%f"
# Truncate microseconds to milliseconds.
    return "{} --> {}".format(start_time.strftime(time_format)[:-3], end_time.strftime(time_format)[:-3])

def language_from_speech_recognition_result(result : speechsdk.SpeechRecognitionResult, user_config : Read_Only_Dict) -> str :
    if user_config["languages"] is not None :
        return "[{}] ".format(speechsdk.AutoDetectSourceLanguageResult(result).language)
    else :
        return ""

def caption_from_speech_recognition_result(sequence_number : int, result : speechsdk.SpeechRecognitionResult, user_config : Read_Only_Dict) -> str :
    caption = ""
    if not user_config["partial_results_enabled"] and user_config["srt_enabled"] :
        caption += str(sequenceNumber) + linesep
    caption += timestamp_from_speech_recognition_result(result = result, user_config = user_config) + linesep
    caption += language_from_speech_recognition_result(result = result, user_config = user_config)
    caption += result.text + linesep + linesep
    return caption

def write_to_console(text : str, user_config : Read_Only_Dict) :
    if not user_config["suppress_output_enabled"] :
        print(text, end = "")
    return

def write_to_console_or_file(text : str, user_config : Read_Only_Dict) :
    write_to_console(text = text, user_config = user_config)
    if user_config["output_file"] is not None :
        file_path = Path(user_config["output_file"])
        with open(file_path, mode = "a", newline = "") as f :
            f.write(text)
    return

def initialize(user_config : Read_Only_Dict) :
    if user_config["output_file"] is not None and exists(user_config["output_file"]) :
        remove(user_config["output_file"])

    if not user_config["srt_enabled"] :
        write_to_console_or_file(text = "WEBVTT{}{}".format(linesep, linesep), user_config = user_config)

    return

# Main functions

def audio_config_from_user_config(user_config : Read_Only_Dict) -> speechsdk.AudioConfig :
    if user_config["input_file"] is None :
        return speechsdk.AudioConfig(use_default_microphone = True)
    else :
        return speechsdk.AudioConfig(filename = user_config["input_file"])

def speech_config_from_user_config(user_config : Read_Only_Dict) -> speechsdk.SpeechConfig :
    speech_config = None
    if user_config["languages"] is not None :
        endpoint = v2_endpoint_from_region(user_config["region"])
        speech_config = speechsdk.SpeechConfig(endpoint = endpoint, subscription = user_config["subscription_key"])
    else :
        speech_config = speechsdk.SpeechConfig(subscription = user_config["subscription_key"], region = user_config["region"])

    if user_config["profanity_filter_remove_enabled"] :
        speech_config.set_profanity(speechsdk.ProfanityOption.Removed)
    elif user_config["profanity_filter_mask_enabled"] :
        speech_config.set_profanity(speechsdk.ProfanityOption.Masked)

    if user_config["stable_partial_result_threshold"] is not None :
        speech_config.set_property(property_id = speechsdk.PropertyId.SpeechServiceResponse_StablePartialResultThreshold, value = user_config["stable_partial_result_threshold"])

    if user_config["true_text_enabled"] :
        speech_config.set_property(property_id = speechsdk.PropertyId.SpeechServiceResponse_PostProcessingOption, value = "TrueText");

    return speech_config

def speech_recognizer_from_speech_config(speech_config : speechsdk.SpeechConfig, audio_config : speechsdk.AudioConfig, user_config : Read_Only_Dict) -> speechsdk.SpeechRecognizer :
    speech_recognizer = None

    if user_config["languages"] is not None :
        detect_language_config = speechsdk.AutoDetectSourceLanguageConfig(user_config["languages"].split(","))
        speech_recognizer = speechsdk.SpeechRecognizer(speech_config = speech_config, audio_config = audio_config, auto_detect_source_language_config = detect_language_config)
    else :
        speech_recognizer = speechsdk.SpeechRecognizer(speech_config = speech_config, audio_config = audio_config)

    if user_config["phrases"] is not None :
        grammar = speechsdk.PhraseListGrammar.from_recognizer(recognizer = speech_recognizer)
        grammar.addPhrase(user_config["phrases"]);

    return speech_recognizer

def speech_recognizer_from_user_config(user_config : Read_Only_Dict) -> speechsdk.SpeechRecognizer :
    audioConfig = audio_config_from_user_config(user_config)
    speechConfig = speech_config_from_user_config(user_config)
    return speech_recognizer_from_speech_config(speechConfig, audioConfig, user_config)

def recognize_continuous(speech_recognizer : speechsdk.SpeechRecognizer, user_config : Read_Only_Dict) :
    sequence_number = 0
    done = False

    def recognizing_handler(e : speechsdk.SpeechRecognitionEventArgs) :
        if speechsdk.ResultReason.RecognizingSpeech == e.result.reason and len(e.result.text) > 0 :
# We don't show sequence numbers for partial results.
            write_to_console_or_file(text = caption_from_speech_recognition_result(sequence_number = 0, result = e.result, user_config = user_config), user_config = user_config)
        elif speechsdk.ResultReason.NoMatch == e.result.reason :
            write_to_console(text = "NOMATCH: Speech could not be recognized.{}".format(linesep), user_config = user_config)

    def recognized_handler(e : speechsdk.SpeechRecognitionEventArgs) :
        if speechsdk.ResultReason.RecognizedSpeech == e.result.reason and len(e.result.text) > 0 :
            nonlocal sequence_number
            sequence_number += 1
            write_to_console_or_file(text = caption_from_speech_recognition_result(sequence_number = sequence_number, result = e.result, user_config = user_config), user_config = user_config)
        elif speechsdk.ResultReason.NoMatch == e.result.reason :
            write_to_console(text = "NOMATCH: Speech could not be recognized.{}".format(linesep), user_config = user_config)

    def canceled_handler(e : speechsdk.SpeechRecognitionCanceledEventArgs) :
        nonlocal done
# Notes:
# SpeechRecognitionCanceledEventArgs inherits the result property from SpeechRecognitionEventArgs. See:
# https://docs.microsoft.com/python/api/azure-cognitiveservices-speech/azure.cognitiveservices.speech.speechrecognitioncanceledeventargs?view=azure-python
# https://docs.microsoft.com/python/api/azure-cognitiveservices-speech/azure.cognitiveservices.speech.speechrecognitioneventargs?view=azure-python
# result is type SpeechRecognitionResult, which inherits the reason property from RecognitionResult. See:
# https://docs.microsoft.com/python/api/azure-cognitiveservices-speech/azure.cognitiveservices.speech.speechrecognitionresult?view=azure-python
# https://docs.microsoft.com/python/api/azure-cognitiveservices-speech/azure.cognitiveservices.speech.recognitionresult?view=azure-python
# e.result.reason is ResultReason.Canceled. To get the cancellation reason, see e.cancellation_details.reason.
        if speechsdk.CancellationReason.EndOfStream == e.cancellation_details.reason :
            write_to_console(text = "End of stream reached.{}".format(linesep), user_config = user_config)
            done = True
        elif speechsdk.CancellationReason.CancelledByUser == e.cancellation_details.reason :
            write_to_console(text = "User canceled request.{}".format(linesep), user_config = user_config)
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
        write_to_console(text = "Session stopped.".format(linesep), user_config = user_config);
        done = True

    speech_recognizer.recognizing.connect(recognizing_handler)
    speech_recognizer.recognized.connect(recognized_handler)
    speech_recognizer.session_stopped.connect(stopped_handler)
    speech_recognizer.canceled.connect(canceled_handler)

    speech_recognizer.start_continuous_recognition()
    while not done :
        sleep(1)
    speech_recognizer.stop_continuous_recognition()

    return

usage = """Usage: python caption.py [-f] [-h] [-i file] [-l] [-m] [-o file] [-p phrases] [-q] [-r number] [-s] [-t] [-u] <subscriptionKey> <region>
              -f: Enable profanity filter (remove profanity). Overrides -m.
              -h: Show this help and stop.
              -i: Input audio file *file* (default input is from the microphone.)
    -l languages: Enable language identification for specified *languages*.
                  Example: en-US,ja-JP
              -m: Enable profanity filter (mask profanity). -f overrides this.
         -o file: Output to *file*.
      -p phrases: Add specified *phrases*.
                  Example: Constoso;Jessie;Rehaan
              -q: Suppress console output (except errors).
       -r number: Set stable partial result threshold to *number*.
                  Example: 3
              -s: Emit SRT (default is WebVTT.)
              -t: Enable TrueText.
              -u: Emit partial results instead of finalized results."""

try :
    if cmd_option_exists("-h") :
        print(usage)
    else :
        user_config = user_config_from_args()
        initialize(user_config = user_config)
        speech_recognizer = speech_recognizer_from_user_config(user_config = user_config)
        recognize_continuous(speech_recognizer = speech_recognizer, user_config = user_config)
except Too_Few_Arguments :
    print("Too few arguments.")
    print(usage)
except Exception as e:
    print(e)
