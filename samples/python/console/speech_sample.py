#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
"""
Speech recognition samples for the Microsoft Cognitive Services Speech SDK
"""

import json
import time
import threading
import wave
import utils
import sys
import io

try:
    import azure.cognitiveservices.speech as speechsdk
    from azure.identity import DefaultAzureCredential
except ImportError:
    print(
        """
    Importing the Speech SDK for Python failed.
    Refer to
    https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-python for
    installation instructions.

    For AAD authentication, install the azure-identity package:
    pip install azure-identity
    """
    )
    sys.exit(1)

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8")

# Set up the subscription info for the Speech Service:
# Replace with your own subscription key and endpoint.
speech_key, speech_endpoint = "YourSubscriptionKey", "https://YourServiceRegion.api.cognitive.microsoft.com"

# Set up endpoint with custom domain. This is required when using AAD token credential to authenticate.
# For details on setting up a custom domain with private links, see:
# https://learn.microsoft.com/azure/ai-services/speech-service/speech-services-private-link?tabs=portal#create-a-custom-domain-name
speech_endpoint_with_custom_domain = "YourServiceEndpointWithCustomDomain"

# Specify the path to an audio file containing speech (mono WAV / PCM with a sampling rate of 16
# kHz).
weatherfilename = "whatstheweatherlike.wav"
weatherfilenamemp3 = "whatstheweatherlike.mp3"
weatherfilenamemulaw = "whatstheweatherlike-mulaw.wav"


def speech_recognize_once_from_mic():
    """performs one-shot speech recognition from the default microphone"""
    # <SpeechRecognitionWithMicrophone>
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    # Creates a speech recognizer using microphone as audio input.
    # The default language is "en-us".
    speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config)

    # Starts speech recognition, and returns after a single utterance is recognized. The end of a
    # single utterance is determined by listening for silence at the end or until a maximum of about 30
    # seconds of audio is processed. It returns the recognition text as result.
    # Note: Since recognize_once() returns only a single utterance, it is suitable only for single
    # shot recognition like command or query.
    # For long-running multi-utterance recognition, use start_continuous_recognition() instead.
    result = speech_recognizer.recognize_once()

    # Check the result
    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print("Recognized: {}".format(result.text))
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized")
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = result.cancellation_details
        print("Speech Recognition canceled: {}".format(cancellation_details.reason))
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(cancellation_details.error_details))
    # </SpeechRecognitionWithMicrophone>


def speech_recognize_once_from_file():
    """performs one-shot speech recognition with input from an audio file"""
    # <SpeechRecognitionWithFile>
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)
    # Creates a speech recognizer using a file as audio input, also specify the speech language
    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config, language="de-DE", audio_config=audio_config
    )

    # Starts speech recognition, and returns after a single utterance is recognized. The end of a
    # single utterance is determined by listening for silence at the end or until a maximum of about 30
    # seconds of audio is processed. It returns the recognition text as result.
    # Note: Since recognize_once() returns only a single utterance, it is suitable only for single
    # shot recognition like command or query.
    # For long-running multi-utterance recognition, use start_continuous_recognition() instead.
    result = speech_recognizer.recognize_once()

    # Check the result
    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print("Recognized: {}".format(result.text))
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized: {}".format(result.no_match_details))
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = result.cancellation_details
        print("Speech Recognition canceled: {}".format(cancellation_details.reason))
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(cancellation_details.error_details))
    # </SpeechRecognitionWithFile>


def speech_recognize_once_from_file_with_detailed_recognition_results():
    """performs one-shot speech recognition with input from an audio file, showing detailed recognition results
    including word-level timing"""
    # <SpeechRecognitionFromFileWithDetailedRecognitionResults>
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)

    # Ask for detailed recognition result
    speech_config.output_format = speechsdk.OutputFormat.Detailed

    # If you also want word-level timing in the detailed recognition results, set the following.
    # Note that if you set the following, you can omit the previous line
    #   "speech_config.output_format = speechsdk.OutputFormat.Detailed",
    # since word-level timing implies detailed recognition results.
    speech_config.request_word_level_timestamps()

    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)

    # Creates a speech recognizer using a file as audio input, also specify the speech language
    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config, language="en-US", audio_config=audio_config
    )

    # Starts speech recognition, and returns after a single utterance is recognized. The end of a
    # single utterance is determined by listening for silence at the end or until a maximum of about 30
    # seconds of audio is processed. It returns the recognition text as result.
    # Note: Since recognize_once() returns only a single utterance, it is suitable only for single
    # shot recognition like command or query.
    # For long-running multi-utterance recognition, use start_continuous_recognition() instead.
    result = speech_recognizer.recognize_once()

    # Check the result
    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print("Recognized: {}".format(result.text))

        # Time units are in hundreds of nanoseconds (HNS), where 10000 HNS equals 1 millisecond
        print("Offset: {}".format(result.offset))
        print("Duration: {}".format(result.duration))

        # Now get the detailed recognition results from the JSON
        json_result = json.loads(result.json)

        # The first cell in the NBest list corresponds to the recognition results
        # (NOT the cell with the highest confidence number!)
        print("Detailed results - Lexical: {}".format(json_result["NBest"][0]["Lexical"]))
        # ITN stands for Inverse Text Normalization
        print("Detailed results - ITN: {}".format(json_result["NBest"][0]["ITN"]))
        print("Detailed results - MaskedITN: {}".format(json_result["NBest"][0]["MaskedITN"]))
        print("Detailed results - Display: {}".format(json_result["NBest"][0]["Display"]))

        # Print word-level timing. Time units are HNS.
        words = json_result["NBest"][0]["Words"]
        print("Detailed results - Word timing:\nWord:\tOffset:\tDuration:")
        for word in words:
            print(f"{word['Word']}\t{word['Offset']}\t{word['Duration']}")

        # You can access alternative recognition results through json_result['NBest'][i], i=1,2,..

    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized: {}".format(result.no_match_details))
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = result.cancellation_details
        print("Speech Recognition canceled: {}".format(cancellation_details.reason))
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(cancellation_details.error_details))
    # </SpeechRecognitionFromFileWithDetailedRecognitionResults>


def speech_recognize_once_compressed_input():
    """performs one-shot speech recognition with compressed input from an audio file"""

    # <SpeechRecognitionWithCompressedFile>
    class BinaryFileReaderCallback(speechsdk.audio.PullAudioInputStreamCallback):
        def __init__(self, filename: str):
            super().__init__()
            self._file_h = open(filename, "rb")

        def read(self, buffer: memoryview) -> int:
            try:
                size = buffer.nbytes
                frames = self._file_h.read(size)

                buffer[: len(frames)] = frames

                return len(frames)
            except Exception as ex:
                print("Exception in `read`: {}".format(ex))
                raise

        def close(self) -> None:
            print("closing file")
            try:
                self._file_h.close()
            except Exception as ex:
                print("Exception in `close`: {}".format(ex))
                raise

    # Creates an audio stream format. For an example we are using MP3 compressed file here
    compressed_format = speechsdk.audio.AudioStreamFormat(
        compressed_stream_format=speechsdk.AudioStreamContainerFormat.MP3
    )
    callback = BinaryFileReaderCallback(filename=weatherfilenamemp3)
    stream = speechsdk.audio.PullAudioInputStream(stream_format=compressed_format, pull_stream_callback=callback)

    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    audio_config = speechsdk.audio.AudioConfig(stream=stream)

    # Creates a speech recognizer using a file as audio input, also specify the speech language
    speech_recognizer = speechsdk.SpeechRecognizer(speech_config, audio_config)

    # Starts speech recognition, and returns after a single utterance is recognized. The end of a
    # single utterance is determined by listening for silence at the end or until a maximum of about 30
    # seconds of audio is processed. It returns the recognition text as result.
    # Note: Since recognize_once() returns only a single utterance, it is suitable only for single
    # shot recognition like command or query.
    # For long-running multi-utterance recognition, use start_continuous_recognition() instead.
    result = speech_recognizer.recognize_once()

    # Check the result
    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print("Recognized: {}".format(result.text))
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized: {}".format(result.no_match_details))
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = result.cancellation_details
        print("Speech Recognition canceled: {}".format(cancellation_details.reason))
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(cancellation_details.error_details))
    # </SpeechRecognitionWithCompressedFile>


def speech_recognize_once_from_file_with_customized_model():
    """performs one-shot speech recognition with input from an audio file, specifying a custom
    model"""
    # <SpeechRecognitionUsingCustomizedModel>
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)

    # Create source language configuration with the speech language and the endpoint ID of your customized model
    # Replace with your speech language and CRIS endpoint ID.
    source_language_config = speechsdk.languageconfig.SourceLanguageConfig("zh-CN", "YourEndpointId")

    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)
    # Creates a speech recognizer using a file as audio input and specify the source language config
    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config, source_language_config=source_language_config, audio_config=audio_config
    )

    # Starts speech recognition, and returns after a single utterance is recognized. The end of a
    # single utterance is determined by listening for silence at the end or until a maximum of about 30
    # seconds of audio is processed. It returns the recognition text as result.
    # Note: Since recognize_once() returns only a single utterance, it is suitable only for single
    # shot recognition like command or query.
    # For long-running multi-utterance recognition, use start_continuous_recognition() instead.
    result = speech_recognizer.recognize_once()

    # Check the result
    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print("Recognized: {}".format(result.text))
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized: {}".format(result.no_match_details))
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = result.cancellation_details
        print("Speech Recognition canceled: {}".format(cancellation_details.reason))
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(cancellation_details.error_details))
    # </SpeechRecognitionUsingCustomizedModel>


def speech_recognize_once_from_file_with_custom_endpoint_parameters():
    """performs one-shot speech recognition with input from an audio file, specifying an
    endpoint with custom parameters"""
    initial_silence_timeout_ms = 15 * 1e3
    template = (
        "wss://{}.stt.speech.microsoft.com/speech/recognition"
        "/conversation/cognitiveservices/v1?initialSilenceTimeoutMs={:d}"
    )
    speech_config = speechsdk.SpeechConfig(
        subscription=speech_key, endpoint=template.format("YourServiceRegion", int(initial_silence_timeout_ms))
    )
    print("Using endpoint", speech_config.get_property(speechsdk.PropertyId.SpeechServiceConnection_Endpoint))
    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)
    # Creates a speech recognizer using a file as audio input.
    # The default language is "en-us".
    speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config, audio_config=audio_config)

    # Starts speech recognition, and returns after a single utterance is recognized. The end of a
    # single utterance is determined by listening for silence at the end or until a maximum of about 30
    # seconds of audio is processed. It returns the recognition text as result.
    # Note: Since recognize_once() returns only a single utterance, it is suitable only for single
    # shot recognition like command or query.
    # For long-running multi-utterance recognition, use start_continuous_recognition() instead.
    result = speech_recognizer.recognize_once()

    # Check the result
    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print("Recognized: {}".format(result.text))
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized: {}".format(result.no_match_details))
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = result.cancellation_details
        print("Speech Recognition canceled: {}".format(cancellation_details.reason))
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(cancellation_details.error_details))


def speech_recognize_async_from_file():
    """performs one-shot speech recognition asynchronously with input from an audio file"""
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)
    # Creates a speech recognizer using a file as audio input.
    # The default language is "en-us".
    speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config, audio_config=audio_config)

    # Perform recognition. `recognize_async` does not block until recognition is complete,
    # so other tasks can be performed while recognition is running.
    # However, recognition stops when the first utterance has been recognized.
    # For long-running recognition, use continuous recognitions instead.
    result_future = speech_recognizer.recognize_once_async()

    print("recognition is running....")
    # Other tasks can be performed here...

    # Retrieve the recognition result. This blocks until recognition is complete.
    result = result_future.get()

    # Check the result
    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print("Recognized: {}".format(result.text))
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized: {}".format(result.no_match_details))
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = result.cancellation_details
        print("Speech Recognition canceled: {}".format(cancellation_details.reason))
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(cancellation_details.error_details))


def speech_recognize_continuous_from_file():
    """performs continuous speech recognition with input from an audio file"""
    # <SpeechContinuousRecognitionWithFile>
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)

    speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config, audio_config=audio_config)

    done = False

    def stop_cb(evt: speechsdk.SessionEventArgs):
        """callback that signals to stop continuous recognition upon receiving an event `evt`"""
        print("CLOSING on {}".format(evt))
        nonlocal done
        done = True

    # Connect callbacks to the events fired by the speech recognizer
    speech_recognizer.recognizing.connect(lambda evt: print("RECOGNIZING: {}".format(evt)))
    speech_recognizer.recognized.connect(lambda evt: print("RECOGNIZED: {}".format(evt)))
    speech_recognizer.session_started.connect(lambda evt: print("SESSION STARTED: {}".format(evt)))
    speech_recognizer.session_stopped.connect(lambda evt: print("SESSION STOPPED {}".format(evt)))
    speech_recognizer.canceled.connect(lambda evt: print("CANCELED {}".format(evt)))
    # Stop continuous recognition on either session stopped or canceled events
    speech_recognizer.session_stopped.connect(stop_cb)
    speech_recognizer.canceled.connect(stop_cb)

    # Start continuous speech recognition
    speech_recognizer.start_continuous_recognition()
    while not done:
        time.sleep(0.5)

    speech_recognizer.stop_continuous_recognition()
    # </SpeechContinuousRecognitionWithFile>


def speech_recognize_continuous_async_from_microphone():
    """performs continuous speech recognition asynchronously with input from microphone"""
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    # The default language is "en-us".
    speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config)

    done = False

    def recognizing_cb(evt: speechsdk.SpeechRecognitionEventArgs):
        print("RECOGNIZING: {}".format(evt))

    def recognized_cb(evt: speechsdk.SpeechRecognitionEventArgs):
        print("RECOGNIZED: {}".format(evt))

    def stop_cb(evt: speechsdk.SessionEventArgs):
        """callback that signals to stop continuous recognition"""
        print("CLOSING on {}".format(evt))
        nonlocal done
        done = True

    # Connect callbacks to the events fired by the speech recognizer
    speech_recognizer.recognizing.connect(recognizing_cb)
    speech_recognizer.recognized.connect(recognized_cb)
    speech_recognizer.session_stopped.connect(stop_cb)
    speech_recognizer.canceled.connect(stop_cb)

    # Perform recognition. `start_continuous_recognition_async asynchronously initiates continuous recognition operation,
    # Other tasks can be performed on this thread while recognition starts...
    # wait on result_future.get() to know when initialization is done.
    # Call stop_continuous_recognition_async() to stop recognition.
    result_future = speech_recognizer.start_continuous_recognition_async()

    result_future.get()  # wait for voidfuture, so we know engine initialization is done.
    print("Continuous Recognition is now running, say something.")

    while not done:
        # No real sample parallel work to do on this thread, so just wait for user to type stop.
        # Can't exit function or speech_recognizer will go out of scope and be destroyed while running.
        print('type "stop" then enter when done')
        stop = input()
        if stop.lower() == "stop":
            print("Stopping async recognition.")
            speech_recognizer.stop_continuous_recognition_async()
            break

    print("recognition stopped, main thread can exit now.")


# <SpeechRecognitionUsingKeywordModel>
def speech_recognize_keyword_from_microphone():
    """performs keyword-triggered speech recognition with input microphone"""
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)

    # Creates an instance of a keyword recognition model. Update this to
    # point to the location of your keyword recognition model.
    model = speechsdk.KeywordRecognitionModel("YourKeywordRecognitionModelFile.table")

    # The phrase your keyword recognition model triggers on.
    keyword = "YourKeyword"

    speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config)

    done = False

    def stop_cb(evt: speechsdk.SessionEventArgs):
        """callback that signals to stop continuous recognition upon receiving an event `evt`"""
        print("CLOSING on {}".format(evt))
        nonlocal done
        done = True

    def recognizing_cb(evt: speechsdk.SpeechRecognitionEventArgs):
        """callback for recognizing event"""
        if evt.result.reason == speechsdk.ResultReason.RecognizingKeyword:
            print("RECOGNIZING KEYWORD: {}".format(evt))
        elif evt.result.reason == speechsdk.ResultReason.RecognizingSpeech:
            print("RECOGNIZING: {}".format(evt))

    def recognized_cb(evt: speechsdk.SpeechRecognitionEventArgs):
        """callback for recognized event"""
        if evt.result.reason == speechsdk.ResultReason.RecognizedKeyword:
            print("RECOGNIZED KEYWORD: {}".format(evt))
        elif evt.result.reason == speechsdk.ResultReason.RecognizedSpeech:
            print("RECOGNIZED: {}".format(evt))
        elif evt.result.reason == speechsdk.ResultReason.NoMatch:
            print("NOMATCH: {}".format(evt))

    # Connect callbacks to the events fired by the speech recognizer
    speech_recognizer.recognizing.connect(recognizing_cb)
    speech_recognizer.recognized.connect(recognized_cb)
    speech_recognizer.session_started.connect(lambda evt: print("SESSION STARTED: {}".format(evt)))
    speech_recognizer.session_stopped.connect(lambda evt: print("SESSION STOPPED {}".format(evt)))
    speech_recognizer.canceled.connect(lambda evt: print("CANCELED {}".format(evt)))
    # Stop continuous recognition on either session stopped or canceled events
    speech_recognizer.session_stopped.connect(stop_cb)
    speech_recognizer.canceled.connect(stop_cb)

    # Start keyword recognition
    speech_recognizer.start_keyword_recognition(model)
    print('Say something starting with "{}" followed by whatever you want...'.format(keyword))
    while not done:
        time.sleep(0.5)

    speech_recognizer.stop_keyword_recognition()
# </SpeechRecognitionUsingKeywordModel>


def speech_recognition_with_pull_stream():
    """gives an example how to use a pull audio stream to recognize speech from a custom audio
    source"""

    class WavFileReaderCallback(speechsdk.audio.PullAudioInputStreamCallback):
        """Example class that implements the Pull Audio Stream interface to recognize speech from
        an audio file"""

        def __init__(self, filename: str):
            super().__init__()
            self._file_h = wave.open(filename, mode=None)

            self.sample_width = self._file_h.getsampwidth()

            assert self._file_h.getnchannels() == 1
            assert self._file_h.getsampwidth() == 2
            assert self._file_h.getframerate() == 16000
            assert self._file_h.getcomptype() == "NONE"

        def read(self, buffer: memoryview) -> int:
            """read callback function"""
            size = buffer.nbytes
            frames = self._file_h.readframes(size // self.sample_width)

            buffer[: len(frames)] = frames

            return len(frames)

        def close(self):
            """close callback function"""
            self._file_h.close()

    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)

    # Specify the audio format
    wave_format = speechsdk.audio.AudioStreamFormat(samples_per_second=16000, bits_per_sample=16, channels=1)

    # Setup the audio stream
    callback = WavFileReaderCallback(weatherfilename)
    stream = speechsdk.audio.PullAudioInputStream(callback, wave_format)
    audio_config = speechsdk.audio.AudioConfig(stream=stream)

    # Instantiate the speech recognizer with pull stream input
    speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config, audio_config=audio_config)

    done = False

    def stop_cb(evt: speechsdk.SessionEventArgs):
        """callback that signals to stop continuous recognition upon receiving an event `evt`"""
        print("CLOSING on {}".format(evt))
        nonlocal done
        done = True

    # Connect callbacks to the events fired by the speech recognizer
    speech_recognizer.recognizing.connect(lambda evt: print("RECOGNIZING: {}".format(evt)))
    speech_recognizer.recognized.connect(lambda evt: print("RECOGNIZED: {}".format(evt)))
    speech_recognizer.session_started.connect(lambda evt: print("SESSION STARTED: {}".format(evt)))
    speech_recognizer.session_stopped.connect(lambda evt: print("SESSION STOPPED {}".format(evt)))
    speech_recognizer.canceled.connect(lambda evt: print("CANCELED {}".format(evt)))
    # Stop continuous recognition on either session stopped or canceled events
    speech_recognizer.session_stopped.connect(stop_cb)
    speech_recognizer.canceled.connect(stop_cb)

    # Start continuous speech recognition
    speech_recognizer.start_continuous_recognition()

    while not done:
        time.sleep(0.5)

    speech_recognizer.stop_continuous_recognition()


def push_stream_writer(stream):
    # The number of bytes to push per buffer
    n_bytes = 3200
    wav_fh = wave.open(weatherfilename)
    # Start pushing data until all data has been read from the file
    try:
        while True:
            frames = wav_fh.readframes(n_bytes // 2)
            print("read {} bytes".format(len(frames)))
            if not frames:
                break
            stream.write(frames)
            time.sleep(0.1)
    finally:
        wav_fh.close()
        stream.close()  # must be done to signal the end of stream


def speech_recognition_with_push_stream():
    """gives an example how to use a push audio stream to recognize speech from a custom audio
    source"""
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)

    # Setup the audio stream
    stream = speechsdk.audio.PushAudioInputStream()
    audio_config = speechsdk.audio.AudioConfig(stream=stream)

    # Instantiate the speech recognizer with push stream input
    speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config, audio_config=audio_config)
    recognition_done = threading.Event()

    # Connect callbacks to the events fired by the speech recognizer
    def session_stopped_cb(evt):
        """callback that signals to stop continuous recognition upon receiving an event `evt`"""
        print("SESSION STOPPED: {}".format(evt))
        recognition_done.set()

    speech_recognizer.recognizing.connect(lambda evt: print("RECOGNIZING: {}".format(evt)))
    speech_recognizer.recognized.connect(lambda evt: print("RECOGNIZED: {}".format(evt)))
    speech_recognizer.session_started.connect(lambda evt: print("SESSION STARTED: {}".format(evt)))
    speech_recognizer.session_stopped.connect(session_stopped_cb)
    speech_recognizer.canceled.connect(lambda evt: print("CANCELED {}".format(evt)))

    # Start push stream writer thread
    push_stream_writer_thread = threading.Thread(target=push_stream_writer, args=[stream])
    push_stream_writer_thread.start()

    # Start continuous speech recognition
    speech_recognizer.start_continuous_recognition()

    # Wait until all input processed
    recognition_done.wait()

    # Stop recognition and clean up
    speech_recognizer.stop_continuous_recognition()
    push_stream_writer_thread.join()


def speech_recognition_with_push_stream_mulaw():
    """gives an example how to use a push mulaw audio stream to recognize speech from a custom audio
    source"""
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)

    # Setup the audio stream
    audio_format = speechsdk.audio.AudioStreamFormat(
        samples_per_second=16000,
        bits_per_sample=8,
        channels=1,
        wave_stream_format=speechsdk.AudioStreamWaveFormat.MULAW,
    )
    stream = speechsdk.audio.PushAudioInputStream(stream_format=audio_format)
    audio_config = speechsdk.audio.AudioConfig(stream=stream)

    # Instantiate the speech recognizer with push stream input
    speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config, audio_config=audio_config)
    recognition_done = threading.Event()

    # Connect callbacks to the events fired by the speech recognizer
    def session_stopped_cb(evt):
        """callback that signals to stop continuous recognition upon receiving an event `evt`"""
        print("SESSION STOPPED: {}".format(evt))
        recognition_done.set()

    speech_recognizer.recognizing.connect(lambda evt: print("RECOGNIZING: {}".format(evt)))
    speech_recognizer.recognized.connect(lambda evt: print("RECOGNIZED: {}".format(evt)))
    speech_recognizer.session_started.connect(lambda evt: print("SESSION STARTED: {}".format(evt)))
    speech_recognizer.session_stopped.connect(session_stopped_cb)
    speech_recognizer.canceled.connect(lambda evt: print("CANCELED {}".format(evt)))

    # Start continuous speech recognition
    speech_recognizer.start_continuous_recognition()

    # Open the wav file and push it to the push stream.
    # NOTE the wav header must be skipped before pushing the data to the stream.
    with open(weatherfilenamemulaw, "rb") as audio_file:
        wav_header_size = utils.get_wav_header_size(weatherfilenamemulaw)
        # Read the wave header
        header = audio_file.read(wav_header_size)  # noqa: F841 # pylint: disable=unused-variable
        # Read the audio data
        audio_data = audio_file.read()
        stream.write(audio_data)
        stream.close()

    # Wait until all input processed
    recognition_done.wait()

    # Stop recognition and clean up
    speech_recognizer.stop_continuous_recognition()


def speech_recognize_once_with_auto_language_detection_from_mic():
    """performs one-shot speech recognition from the default microphone with auto language detection"""
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)

    # Create the auto detection language configuration with the potential source language candidates
    auto_detect_source_language_config = speechsdk.languageconfig.AutoDetectSourceLanguageConfig(
        languages=["de-DE", "en-US"]
    )
    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config, auto_detect_source_language_config=auto_detect_source_language_config
    )
    result = speech_recognizer.recognize_once()

    # Check the result
    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        auto_detect_source_language_result = speechsdk.AutoDetectSourceLanguageResult(result)
        print("Recognized: {} in language {}".format(result.text, auto_detect_source_language_result.language))
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized")
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = result.cancellation_details
        print("Speech Recognition canceled: {}".format(cancellation_details.reason))
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(cancellation_details.error_details))


def speech_recognize_with_auto_language_detection_UsingCustomizedModel():
    """performs speech recognition from the audio file with auto language detection, using customized model"""
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)

    # Replace the languages with your languages in BCP-47 format, e.g. fr-FR.
    # Please see https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support
    # for all supported languages
    en_language_config = speechsdk.languageconfig.SourceLanguageConfig("en-US")
    # Replace the languages with your languages in BCP-47 format, e.g. zh-CN.
    # Set the endpoint ID of your customized mode that will be used for fr-FR.
    # Replace with your own CRIS endpoint ID.
    fr_language_config = speechsdk.languageconfig.SourceLanguageConfig("fr-FR", "myendpointId")
    # Create the auto detection language configuration with the source language configurations
    auto_detect_source_language_config = speechsdk.languageconfig.AutoDetectSourceLanguageConfig(
        sourceLanguageConfigs=[en_language_config, fr_language_config]
    )

    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config,
        auto_detect_source_language_config=auto_detect_source_language_config,
        audio_config=audio_config,
    )
    result = speech_recognizer.recognize_once()

    # Check the result
    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        auto_detect_source_language_result = speechsdk.AutoDetectSourceLanguageResult(result)
        print("Recognized: {} in language {}".format(result.text, auto_detect_source_language_result.language))
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized")
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = result.cancellation_details
        print("Speech Recognition canceled: {}".format(cancellation_details.reason))
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(cancellation_details.error_details))


def speech_recognize_keyword_locally_from_microphone():
    """runs keyword spotting locally, with direct access to the result audio"""

    # Creates an instance of a keyword recognition model. Update this to
    # point to the location of your keyword recognition model.
    model = speechsdk.KeywordRecognitionModel("YourKeywordRecognitionModelFile.table")

    # The phrase your keyword recognition model triggers on.
    keyword = "YourKeyword"

    # Create a local keyword recognizer with the default microphone device for input.
    keyword_recognizer = speechsdk.KeywordRecognizer()

    done = False

    def recognized_cb(evt: speechsdk.SpeechRecognitionEventArgs):
        # Only a keyword phrase is recognized. The result cannot be 'NoMatch'
        # and there is no timeout. The recognizer runs until a keyword phrase
        # is detected or recognition is canceled (by stop_recognition_async()
        # or due to the end of an input file or stream).
        result = evt.result
        if result.reason == speechsdk.ResultReason.RecognizedKeyword:
            print("RECOGNIZED KEYWORD: {}".format(result.text))
        nonlocal done
        done = True

    def canceled_cb(evt: speechsdk.SpeechRecognitionCanceledEventArgs):
        result = evt.result
        if result.reason == speechsdk.ResultReason.Canceled:
            print("CANCELED: {}".format(result.cancellation_details.reason))
        nonlocal done
        done = True

    # Connect callbacks to the events fired by the keyword recognizer.
    keyword_recognizer.recognized.connect(recognized_cb)
    keyword_recognizer.canceled.connect(canceled_cb)

    # Start keyword recognition.
    result_future = keyword_recognizer.recognize_once_async(model)
    print('Say something starting with "{}" followed by whatever you want...'.format(keyword))
    result = result_future.get()

    # Read result audio (incl. the keyword).
    if result.reason == speechsdk.ResultReason.RecognizedKeyword:
        time.sleep(2)  # give some time so the stream is filled
        result_stream = speechsdk.AudioDataStream(result)
        result_stream.detach_input()  # stop any more data from input getting to the stream

        save_future = result_stream.save_to_wav_file_async("AudioFromRecognizedKeyword.wav")
        print("Saving file...")
        save_future.get()

    # If active keyword recognition needs to be stopped before results, it can be done with
    #
    #   stop_future = keyword_recognizer.stop_recognition_async()
    #   print('Stopping...')
    #   stopped = stop_future.get()


def speech_recognize_once_from_file_with_aad():
    """
    Performs one-shot speech recognition from a file using Azure AD authentication.

    This sample shows how to use DefaultAzureCredential with a custom domain endpoint
    for speech recognition. This is useful for enterprise applications where centralized
    authentication is required instead of subscription keys.

    You can use other credential types from azure.identity based on your scenario:
    - InteractiveBrowserCredential: Good for interactive applications
    - ClientSecretCredential: Good for service-to-service authentication
    - ManagedIdentityCredential: Good for Azure-hosted applications
    - ChainedTokenCredential: Combine multiple credential types

    For details, see: https://learn.microsoft.com/python/api/azure-identity/azure.identity
    """
    print("Speech recognition with Azure AD authentication...")

    try:
        # Create DefaultAzureCredential which tries multiple authentication methods
        auth_credential = DefaultAzureCredential()

        # Create the speech config with endpoint and authentication
        # Note: You must replace 'YourServiceEndpointWithCustomDomain' with a properly configured custom domain endpoint
        # when using token authentication in production to avoid misconfiguration issues.
        speech_config = speechsdk.SpeechConfig(endpoint=speech_endpoint_with_custom_domain, token_credential=auth_credential)
        speech_config.speech_recognition_language = "en-US"

        # Create an audio configuration using a file
        audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)

        # Creates a speech recognizer using file as audio input and the authentication config
        speech_recognizer = speechsdk.SpeechRecognizer(
            speech_config=speech_config,
            audio_config=audio_config
        )

        # Start speech recognition
        print(f"Recognizing speech from file: {weatherfilename}")
        result = speech_recognizer.recognize_once()

        # Process and print the result
        if result.reason == speechsdk.ResultReason.RecognizedSpeech:
            print(f"Recognized: {result.text}")
        elif result.reason == speechsdk.ResultReason.NoMatch:
            print(f"No speech could be recognized: {result.no_match_details}")
        elif result.reason == speechsdk.ResultReason.Canceled:
            cancellation_details = result.cancellation_details
            print(f"Speech recognition canceled: {cancellation_details.reason}")
            if cancellation_details.reason == speechsdk.CancellationReason.Error:
                print(f"Error details: {cancellation_details.error_details}")
                print(f"Error code: {cancellation_details.error_code}")

    except Exception as e:
        print(f"Error using Azure AD authentication: {e}")
        print("Make sure you have:")
        print("1. Installed azure-identity: pip install azure-identity")
        print("2. Set up a custom domain endpoint for your Speech resource")
        print("3. Proper permissions to access the Speech resource")


def speech_recognize_continuous_from_file_with_aad():
    """
    Performs continuous speech recognition from a file using Azure AD authentication.

    This sample shows how to use DefaultAzureCredential with a custom domain endpoint
    for continuous speech recognition. This is useful for enterprise applications where centralized
    authentication is required instead of subscription keys.

    You can use other credential types from azure.identity based on your scenario:
    - InteractiveBrowserCredential: Good for interactive applications
    - ClientSecretCredential: Good for service-to-service authentication
    - ManagedIdentityCredential: Good for Azure-hosted applications
    - ChainedTokenCredential: Combine multiple credential types

    For details, see: https://learn.microsoft.com/python/api/azure-identity/azure.identity
    """
    print("Continuous speech recognition with Azure AD authentication...")

    try:
        # Create DefaultAzureCredential which tries multiple authentication methods
        auth_credential = DefaultAzureCredential()

        # Create the speech config with endpoint and authentication
        # Note: You must use a custom domain endpoint when using token authentication
        speech_config = speechsdk.SpeechConfig(endpoint=speech_endpoint_with_custom_domain, token_credential=auth_credential)
        speech_config.speech_recognition_language = "en-US"

        # Create an audio configuration using a file
        audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)

        # Creates a speech recognizer using file as audio input and the authentication config
        speech_recognizer = speechsdk.SpeechRecognizer(
            speech_config=speech_config,
            audio_config=audio_config
        )

        done = False

        def stop_cb(evt: speechsdk.SessionEventArgs):
            """callback that signals to stop continuous recognition upon receiving an event `evt`"""
            print("CLOSING on {}".format(evt))
            nonlocal done
            done = True

        # Connect callbacks to the events fired by the speech recognizer
        speech_recognizer.recognizing.connect(lambda evt: print("RECOGNIZING: {}".format(evt)))
        speech_recognizer.recognized.connect(lambda evt: print("RECOGNIZED: {}".format(evt)))
        speech_recognizer.session_started.connect(lambda evt: print("SESSION STARTED: {}".format(evt)))
        speech_recognizer.session_stopped.connect(lambda evt: print("SESSION STOPPED {}".format(evt)))
        speech_recognizer.canceled.connect(lambda evt: print("CANCELED {}".format(evt)))

        # Stop continuous recognition on either session stopped or canceled events
        speech_recognizer.session_stopped.connect(stop_cb)
        speech_recognizer.canceled.connect(stop_cb)

        # Start continuous speech recognition
        print(f"Starting continuous recognition from file: {weatherfilename}")
        speech_recognizer.start_continuous_recognition()

        timeout = 60  # Set a timeout of 60 seconds
        start_time = time.time()
        while not done and (time.time() - start_time) < timeout:
            time.sleep(0.5)

        if not done:
            print("Timeout reached, stopping recognition.")
            speech_recognizer.stop_continuous_recognition()

        speech_recognizer.stop_continuous_recognition()

    except Exception as e:
        print(f"Error using Azure AD authentication: {e}")
        print("Make sure you have:")
        print("1. Installed azure-identity: pip install azure-identity")
        print("2. Set up a custom domain endpoint for your Speech resource")
        print("3. Proper permissions to access the Speech resource")
