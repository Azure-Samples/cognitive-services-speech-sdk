#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
"""
Pronunciation Assessment samples for the Microsoft Cognitive Services Speech SDK and REST API.
"""

import base64
import json
import math
import string
import threading
import time
import uuid

import azure.cognitiveservices.speech as speechsdk
import requests

from utils import read_wave_header, push_stream_writer, get_reference_words, WaveHeader16K16BitMono, align_lists_with_diff_handling

with open("config.json", "r") as config_file:
    config = json.load(config_file)

speech_key = config.get("SubscriptionKey")
service_region = config.get("ServiceRegion")
speech_endpoint = f"https://{service_region}.api.cognitive.microsoft.com"


WEATHER_WAVE_FILE = "resources/whats_the_weather_like.wav"
ZHCN_SHORT_WAVE_FILE = "resources/zhcn_short_dummy_sample.wav"
ZHCN_LONG_WAVE_FILE = "resources/zhcn_continuous_mode_sample.wav"
ZHCN_LONG_TEXT_FILE = "resources/zhcn_continuous_mode_sample.txt"
AUDIO_PCM_FILE = "resources/good_morning.pcm"


def pronunciation_assessment_from_microphone():
    """Performs one-shot pronunciation assessment asynchronously with input from microphone.
    See more information at https://aka.ms/csspeech/pa"""

    # Creates an instance of a speech config with specified subscription key and endpoint.
    config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)

    # The pronunciation assessment service has a longer default end silence timeout (5 seconds) than normal STT
    # as the pronunciation assessment is widely used in education scenario where kids have longer break in reading.
    # You can adjust the end silence timeout based on your real scenario.
    config.set_property(speechsdk.PropertyId.SpeechServiceConnection_EndSilenceTimeoutMs, "3000")

    reference_text = ""
    pronunciation_config = speechsdk.PronunciationAssessmentConfig(
        reference_text=reference_text,
        grading_system=speechsdk.PronunciationAssessmentGradingSystem.HundredMark,
        granularity=speechsdk.PronunciationAssessmentGranularity.Phoneme,
        enable_miscue=True,
    )
    pronunciation_config.enable_prosody_assessment()

    # Create a speech recognizer, also specify the speech language
    language = "en-US"
    recognizer = speechsdk.SpeechRecognizer(speech_config=config, language=language)
    while True:
        # Receives reference text from console input.
        print("Enter reference text you want to assess, or enter empty text to exit.")
        print("> ", end="")

        try:
            reference_text = input()
        except EOFError:
            break

        if not reference_text:
            break

        pronunciation_config.reference_text = reference_text
        # (Optional) get the session ID
        recognizer.session_started.connect(lambda evt: print(f"SESSION ID: {evt.session_id}"))
        pronunciation_config.apply_to(recognizer)

        # Starts recognizing.
        print(f"Read out '{reference_text}' for pronunciation assessment ...")

        # Note: Since recognize_once() returns only a single utterance, it is suitable only for single shot evaluation.
        # For long-running multi-utterance pronunciation evaluation, use start_continuous_recognition() instead.
        result = recognizer.recognize_once_async().get()

        # Check the result
        if result.reason == speechsdk.ResultReason.RecognizedSpeech:
            print(f"Recognized: {result.text}")
            print("  Pronunciation Assessment Result:")

            pronunciation_result = speechsdk.PronunciationAssessmentResult(result)
            print(
                f"    Accuracy score: {pronunciation_result.accuracy_score}, "
                f"Prosody score: {pronunciation_result.prosody_score}, "
                f"FluencyScore: {pronunciation_result.fluency_score}, "
                f"Completeness score : {pronunciation_result.completeness_score}, "
                f"Pronunciation score: {pronunciation_result.pronunciation_score}"
            )
            print("  Word-level details:")
            for idx, word in enumerate(pronunciation_result.words):
                print(
                    f"    {idx + 1}: word: {word.word}\taccuracy score: {word.accuracy_score}\t"
                    f"error type: {word.error_type}"
                )
        elif result.reason == speechsdk.ResultReason.NoMatch:
            print("No speech could be recognized")
        elif result.reason == speechsdk.ResultReason.Canceled:
            cancellation_details = result.cancellation_details
            print(f"Speech Recognition canceled: {cancellation_details.reason}")
            if cancellation_details.reason == speechsdk.CancellationReason.Error:
                print(f"Error details: {cancellation_details.error_details}")


def pronunciation_assessment_from_stream():
    """Performs pronunciation assessment asynchronously with input from a short audio stream.
    See more information at https://aka.ms/csspeech/pa"""

    # Creates an instance of a speech config with specified subscription key and endpoint.
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)

    # Setup the audio stream
    framerate, bits_per_sample, num_channels = read_wave_header(WEATHER_WAVE_FILE)
    format = speechsdk.audio.AudioStreamFormat(
        samples_per_second=framerate, bits_per_sample=bits_per_sample, channels=num_channels
    )
    stream = speechsdk.audio.PushAudioInputStream(format)
    audio_config = speechsdk.audio.AudioConfig(stream=stream)

    reference_text = "What's the weather like?"
    pronunciation_config = speechsdk.PronunciationAssessmentConfig(
        reference_text=reference_text,
        grading_system=speechsdk.PronunciationAssessmentGradingSystem.HundredMark,
        granularity=speechsdk.PronunciationAssessmentGranularity.Phoneme,
        enable_miscue=True,
    )
    pronunciation_config.enable_prosody_assessment()

    language = "en-US"
    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config, language=language, audio_config=audio_config
    )
    # (Optional) get the session ID
    speech_recognizer.session_started.connect(lambda evt: print(f"SESSION ID: {evt.session_id}"))
    # Apply pronunciation assessment config to speech recognizer
    pronunciation_config.apply_to(speech_recognizer)

    # Start push stream writer thread
    push_stream_writer_thread = threading.Thread(target=push_stream_writer, args=[stream, WEATHER_WAVE_FILE])
    push_stream_writer_thread.start()
    result = speech_recognizer.recognize_once_async().get()
    push_stream_writer_thread.join()

    # Check the result
    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print(f"pronunciation assessment for: {result.text}")
        pronunciation_result = speechsdk.PronunciationAssessmentResult(result)
        print(
            f"    Accuracy score: {pronunciation_result.accuracy_score}, "
            f"Prosody score: {pronunciation_result.prosody_score}, "
            f"Fluency score: {pronunciation_result.fluency_score}, "
            f"Completeness score : {pronunciation_result.completeness_score}, "
            f"Pronunciation score: {pronunciation_result.pronunciation_score}"
        )
        print("  Word-level details:")
        for idx, word in enumerate(pronunciation_result.words):
            print(
                f"    {idx + 1}: word: {word.word}\taccuracy score: {word.accuracy_score}\t"
                f"error type: {word.error_type}"
            )
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized")
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = result.cancellation_details
        print(f"Speech Recognition canceled: {cancellation_details.reason}")
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print(f"Error details: {cancellation_details.error_details}")


def pronunciation_assessment_configured_with_json():
    """Performs pronunciation assessment asynchronously with input from a short audio file.
    See more information at https://aka.ms/csspeech/pa"""

    # Creates an instance of a speech config with specified subscription key and endpoint.
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    audio_config = speechsdk.audio.AudioConfig(filename=WEATHER_WAVE_FILE)

    reference_text = "What's the weather like?"
    # Create pronunciation assessment config with json string (JSON format is not recommended)
    enable_miscue, enable_prosody = True, True
    config_json = {
        "GradingSystem": "HundredMark",
        "Granularity": "Phoneme",
        "Dimension": "Comprehensive",
        "ScenarioId": "",  # "" is the default scenario or ask product team for a customized one
        "EnableMiscue": enable_miscue,
        "EnableProsodyAssessment": enable_prosody,
        "NBestPhonemeCount": 0,  # > 0 to enable "spoken phoneme" mode, 0 to disable
    }
    pronunciation_config = speechsdk.PronunciationAssessmentConfig(json_string=json.dumps(config_json))
    pronunciation_config.reference_text = reference_text

    # Create a speech recognizer using a file as audio input.
    language = "en-US"
    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config, language=language, audio_config=audio_config
    )
    # (Optional) get the session ID
    speech_recognizer.session_started.connect(lambda evt: print(f"SESSION ID: {evt.session_id}"))
    # Apply pronunciation assessment config to speech recognizer
    pronunciation_config.apply_to(speech_recognizer)

    result = speech_recognizer.recognize_once_async().get()

    # Check the result
    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print(f"pronunciation assessment for: {result.text}")
        pronunciation_result = json.loads(result.properties.get(speechsdk.PropertyId.SpeechServiceResponse_JsonResult))
        print(f"assessment results:\n{json.dumps(pronunciation_result, indent=2)}")
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized")
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = result.cancellation_details
        print(f"Speech Recognition canceled: {cancellation_details.reason}")
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print(f"Error details: {cancellation_details.error_details}")


def pronunciation_assessment_continuous_from_file():
    """Performs continuous pronunciation assessment asynchronously with input from a long audio file.
    See more information at https://aka.ms/csspeech/pa"""

    import difflib
    import json

    # Creates an instance of a speech config with specified subscription key and endpoint.
    # Note: The sample is for en-US language.
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    audio_config = speechsdk.audio.AudioConfig(filename=ZHCN_LONG_WAVE_FILE)

    # You can adjust the segmentation silence timeout based on your real scenario.
    speech_config.set_property(speechsdk.PropertyId.Speech_SegmentationSilenceTimeoutMs, "1500")

    with open(ZHCN_LONG_TEXT_FILE, "r", encoding="utf-8") as t:
        reference_text = t.readline()

    language = "zh-CN"
    enable_miscue = True
    enable_prosody_assessment = True
    unscripted_scenario = False if len(reference_text) > 0 else True

    # We need to convert the reference text to lower case, and split to words, then remove the punctuations.
    if language.lower() == "zh-cn":
        # Word segmentation for Chinese using the reference text and any short wave file
        # Remove the blank characters in the reference text
        reference_text = reference_text.replace(" ", "")
        reference_words = get_reference_words(
            ZHCN_SHORT_WAVE_FILE, reference_text, language, speech_key, speech_endpoint
        )
    else:
        reference_words = [w.strip(string.punctuation) for w in reference_text.lower().split()]
    # Remove empty words
    reference_words = [w for w in reference_words if len(w.strip()) > 0]
    reference_text = " ".join(reference_words)
    print("Reference text:", reference_text)

    pronunciation_config = speechsdk.PronunciationAssessmentConfig(
        reference_text=reference_text,
        grading_system=speechsdk.PronunciationAssessmentGradingSystem.HundredMark,
        granularity=speechsdk.PronunciationAssessmentGranularity.Phoneme,
        enable_miscue=enable_miscue,
    )
    if enable_prosody_assessment:
        pronunciation_config.enable_prosody_assessment()

    # Creates a speech recognizer using a file as audio input.
    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config, language=language, audio_config=audio_config
    )
    # Apply pronunciation assessment config to speech recognizer
    pronunciation_config.apply_to(speech_recognizer)

    done = False
    recognized_words = []
    prosody_scores = []
    fluency_scores = []
    durations = []
    startOffset = 0
    endOffset = 0

    def stop_cb(evt: speechsdk.SessionEventArgs):
        """callback that signals to stop continuous recognition upon receiving an event `evt`"""
        print(f"CLOSING on {evt}")
        nonlocal done
        done = True

    def recognized(evt: speechsdk.SpeechRecognitionEventArgs):
        print(f"\npronunciation assessment for: {evt.result.text}")
        pronunciation_result = speechsdk.PronunciationAssessmentResult(evt.result)
        print(
            f"    Accuracy score: {pronunciation_result.accuracy_score}, "
            f"Prosody score: {pronunciation_result.prosody_score}, "
            f"Fluency score: {pronunciation_result.fluency_score}, "
            f"Completeness score : {pronunciation_result.completeness_score}, "
            f"Pronunciation score: {pronunciation_result.pronunciation_score}\n"
        )
        nonlocal recognized_words, startOffset, endOffset
        recognized_words += pronunciation_result.words
        fluency_scores.append(pronunciation_result.fluency_score)
        if pronunciation_result.prosody_score is not None:
            prosody_scores.append(pronunciation_result.prosody_score)
        json_result = evt.result.properties.get(speechsdk.PropertyId.SpeechServiceResponse_JsonResult)
        jo = json.loads(json_result)
        nb = jo["NBest"][0]
        durations.extend(
            [int(w["Duration"]) + 100000 for w in nb["Words"] if w["PronunciationAssessment"]["ErrorType"] == "None"]
        )
        if startOffset == 0:
            startOffset = nb["Words"][0]["Offset"]
        endOffset = nb["Words"][-1]["Offset"] + nb["Words"][-1]["Duration"] + 100000

    # (Optional) get the session ID
    speech_recognizer.session_started.connect(lambda evt: print(f"SESSION ID: {evt.session_id}"))

    # Connect callbacks to the events fired by the speech recognizer
    speech_recognizer.recognizing.connect(lambda evt: print(f"Recognizing: {evt.result.text}"))
    speech_recognizer.recognized.connect(recognized)
    speech_recognizer.session_stopped.connect(lambda evt: print(f"SESSION STOPPED {evt}"))
    speech_recognizer.canceled.connect(lambda evt: print(f"CANCELED {evt.cancellation_details}"))
    # Stop continuous recognition on either session stopped or canceled events
    speech_recognizer.session_stopped.connect(stop_cb)
    speech_recognizer.canceled.connect(stop_cb)

    # Start continuous pronunciation assessment
    speech_recognizer.start_continuous_recognition()
    while not done:
        time.sleep(0.5)

    speech_recognizer.stop_continuous_recognition()

    # For continuous pronunciation assessment mode, the service won't return the words with `Insertion` or `Omission`
    # even if miscue is enabled.
    # We need to compare with the reference text after received all recognized words to get these error words.
    if enable_miscue and not unscripted_scenario:
        # align the reference words basing on recognized words.
        reference_words = align_lists_with_diff_handling(reference_words, [x.word.lower() for x in recognized_words])

        diff = difflib.SequenceMatcher(None, reference_words, [x.word.lower() for x in recognized_words])
        final_words = []
        for tag, i1, i2, j1, j2 in diff.get_opcodes():
            if tag in ["insert", "replace"]:
                for word in recognized_words[j1:j2]:
                    word._error_type = "Insertion"
                    final_words.append(word)
            if tag in ["delete", "replace"]:
                for word_text in reference_words[i1:i2]:
                    word = speechsdk.PronunciationAssessmentWordResult(
                        {
                            "Word": word_text,
                            "PronunciationAssessment": {
                                "ErrorType": "Omission",
                            },
                        }
                    )
                    final_words.append(word)
            if tag == "equal":
                final_words += recognized_words[j1:j2]
    else:
        final_words = recognized_words

    # If accuracy score is below 60, mark as mispronunciation
    for idx, word in enumerate(final_words):
        if word.accuracy_score < 60 and word.error_type == "None":
            word._error_type = "Mispronunciation"

    durations_sum = sum([d for w, d in zip(recognized_words, durations) if w.error_type == "None"])

    # We can calculate whole accuracy by averaging
    final_accuracy_scores = []
    for word in final_words:
        if word.error_type == "Insertion":
            continue
        else:
            final_accuracy_scores.append(word.accuracy_score)
    accuracy_score = sum(final_accuracy_scores) / len(final_accuracy_scores)
    # Re-calculate the prosody score by averaging
    if len(prosody_scores) == 0:
        prosody_score = float("nan")
    else:
        prosody_score = sum(prosody_scores) / len(prosody_scores)
    # Re-calculate fluency score
    fluency_score = 0
    if startOffset > 0:
        fluency_score = durations_sum / (endOffset - startOffset) * 100
    # Calculate whole completeness score
    if not unscripted_scenario:
        handled_final_words = [w.word for w in final_words if w.error_type != "Insertion"]
        completeness_score = len([w for w in final_words if w.error_type == "None"]) / len(handled_final_words) * 100
        completeness_score = completeness_score if completeness_score <= 100 else 100
    else:
        completeness_score = 100

    if not unscripted_scenario:
        # Scripted scenario
        if enable_prosody_assessment and not math.isnan(prosody_score):
            sorted_scores = sorted([accuracy_score, prosody_score, completeness_score, fluency_score])
            pronunciation_score = (
                sorted_scores[0] * 0.4 + sorted_scores[1] * 0.2 + sorted_scores[2] * 0.2 + sorted_scores[3] * 0.2
            )
        else:
            sorted_scores = sorted([accuracy_score, fluency_score, completeness_score])
            pronunciation_score = sorted_scores[0] * 0.6 + sorted_scores[1] * 0.2 + sorted_scores[2] * 0.2
    else:
        # Unscripted scenario
        if enable_prosody_assessment and not math.isnan(prosody_score):
            sorted_scores = sorted([accuracy_score, prosody_score, fluency_score])
            pronunciation_score = sorted_scores[0] * 0.6 + sorted_scores[1] * 0.2 + sorted_scores[2] * 0.2
        else:
            sorted_scores = sorted([accuracy_score, fluency_score])
            pronunciation_score = sorted_scores[0] * 0.6 + sorted_scores[1] * 0.4

    print(
        f"    Paragraph accuracy score: {accuracy_score:.0f}, prosody score: {prosody_score:.0f}, "
        f"fluency score: {fluency_score:.0f}, completeness score: {completeness_score:.0f}, "
        f"pronunciation score: {pronunciation_score:.0f}"
    )

    for idx, word in enumerate(final_words):
        print(
            f"    {idx + 1:03d}: word: {word.word}\taccuracy score: {word.accuracy_score:.0f}\t"
            f"error type: {word.error_type}"
        )


def pronunciation_assessment_with_rest_api():
    """Performs pronunciation assessment asynchronously with REST API for a short audio file.
    See more information at https://learn.microsoft.com/azure/ai-services/speech-service/rest-speech-to-text-short
    """

    # A generator which reads audio data chunk by chunk.
    # The audio_source can be any audio input stream which provides read() method,
    # e.g. audio file, microphone, memory stream, etc.
    def get_chunk(audio_source, chunk_size=1024):
        yield WaveHeader16K16BitMono
        while True:
            time.sleep(chunk_size / 32000)  # to simulate human speaking rate
            chunk = audio_source.read(chunk_size)
            if not chunk:
                global upload_finish_time
                upload_finish_time = time.time()
                break
            yield chunk

    # Build pronunciation assessment parameters
    locale = "en-US"
    audio_file = open(AUDIO_PCM_FILE, "rb")
    reference_text = "Good morning."
    enable_prosody_assessment = True
    phoneme_alphabet = "SAPI"  # IPA or SAPI
    enable_miscue = True
    nbest_phoneme_count = 5
    pron_assessment_params_json = (
        '{"GradingSystem":"HundredMark","Dimension":"Comprehensive","ReferenceText":"%s",'
        '"EnableProsodyAssessment":"%s","PhonemeAlphabet":"%s","EnableMiscue":"%s",'
        '"NBestPhonemeCount":"%s"}'
        % (reference_text, enable_prosody_assessment, phoneme_alphabet, enable_miscue, nbest_phoneme_count)
    )
    pron_assessment_params_base64 = base64.b64encode(bytes(pron_assessment_params_json, "utf-8"))
    pron_assessment_params = str(pron_assessment_params_base64, "utf-8")

    # https://learn.microsoft.com/azure/ai-services/speech-service/how-to-get-speech-session-id#provide-session-id-using-rest-api-for-short-audio
    session_id = uuid.uuid4().hex

    # Build request
    url = f"https://{service_region}.stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1"
    url = f"{url}?format=detailed&language={locale}&X-ConnectionId={session_id}"
    headers = {
        "Accept": "application/json;text/xml",
        "Connection": "Keep-Alive",
        "Content-Type": "audio/wav; codecs=audio/pcm; samplerate=16000",
        "Ocp-Apim-Subscription-Key": speech_key,
        "Pronunciation-Assessment": pron_assessment_params,
        "Transfer-Encoding": "chunked",
        "Expect": "100-continue",
    }

    print(f"II URL: {url}")
    print(f"II Config: {pron_assessment_params_json}")

    # Send request with chunked data
    response = requests.post(url=url, data=get_chunk(audio_file), headers=headers)
    get_response_time = time.time()
    audio_file.close()

    # Show Session ID
    print(f"II Session ID: {session_id}")

    if response.status_code != 200:
        print(f"EE Error code: {response.status_code}")
        print(f"EE Error message: {response.text}")
        exit()
    else:
        print(f"II Response: {response.json()}")

    latency = get_response_time - upload_finish_time
    print(f"II Latency: {int(latency * 1000)}ms")
