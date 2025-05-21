#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
"""
Chatting with Azure OAI and Azure Pronunciation Assessment samples for the Microsoft Cognitive Services Speech SDK
"""

import json
import math
import os
import requests
import sys
import time
import threading
import wave
import string
from typing import Literal

import numpy as np
import soundfile as sf

try:
    import azure.cognitiveservices.speech as speechsdk
except ImportError:
    print("""
    Importing the Speech SDK for Python failed.
    Refer to
    https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-python for
    installation instructions.
    """)
    sys.exit(1)


# Set up the subscription info for the Speech Service
# Replace with your own subscription key and endpoint.
speech_key, speech_endpoint = "YourSubscriptionKey", "https://YourServiceRegion.api.cognitive.microsoft.com"

# Set up the parameters for Azure OAI Services
oai_resource_name = "YourOaiResourceName"
oai_deployment_name = "YourOaiDeploymentName"
oai_api_version = "YourOaiApiVersion"
oai_api_key = "YourOaiApiKey"

sample_width = 2
sample_rate = 16000
channels = 1
reduced_unit = 10000000


def read_wave_header(file_path):
    with wave.open(file_path, 'rb') as audio_file:
        framerate = audio_file.getframerate()
        bits_per_sample = audio_file.getsampwidth() * 8
        num_channels = audio_file.getnchannels()
        return framerate, bits_per_sample, num_channels


def push_stream_writer(stream, filenames, merged_audio_path):
    byte_data = b""
    # The number of bytes to push per buffer
    n_bytes = 3200
    try:
        for filename in filenames:
            wav_fh = wave.open(filename)
            # Start pushing data until all data has been read from the file
            try:
                while True:
                    frames = wav_fh.readframes(n_bytes // 2)
                    if not frames:
                        break
                    stream.write(frames)
                    byte_data += frames
                    time.sleep(.1)
            finally:
                wav_fh.close()
        with wave.open(merged_audio_path, 'wb') as wave_file:
            wave_file.setnchannels(channels)
            wave_file.setsampwidth(sample_width)
            wave_file.setframerate(sample_rate)
            wave_file.writeframes(byte_data)
    finally:
        stream.close()


def merge_wav(audio_list, output_path, tag=None):
    combined_audio = np.empty((0,))
    for audio in audio_list:
        y, _ = sf.read(audio, dtype="float32")
        combined_audio = np.concatenate((combined_audio, y))
        os.remove(audio)
    sf.write(output_path, combined_audio, sample_rate)
    if tag:
        print(f"Save {tag} to {output_path}")


def get_mispronunciation_clip(offset, duration, save_path, merged_audio_path):
    y, _ = sf.read(
        merged_audio_path,
        start=int((offset) / reduced_unit * sample_rate),
        stop=int((offset + duration) / reduced_unit * sample_rate),
        dtype=np.float32
    )
    sf.write(save_path, y, sample_rate)


def strip_end_silence(file_path):
    y, _ = sf.read(file_path, start=0, stop=-int(sample_rate * 0.8), dtype=np.float32)
    sf.write(file_path, y, sample_rate)


def chatting_from_file():
    """Performs chatting with Azure OAI and Azure Pronunciation Assessment asynchronously from audio files.
        See more information at https://aka.ms/csspeech/pa"""

    input_files = ["resources/chat_input_1.wav", "resources/chat_input_2.wav"]
    reference_text = ""
    chat_history = [
        {
            "role": "system",
            "content": (
                "You are a voice assistant, and when you answer questions,"
                "your response should not exceed 25 words."
            )
        }
    ]
    sample_sentence1 = "OK the movie i like to talk about is the cove it is very say phenomenal sensational documentary about adopting hunting practices in japan i think the director is called well i think the name escapes me anyway but well let's talk about the movie basically it's about dolphin hunting practices in japan there's a small village where where villagers fisherman Q almost twenty thousand dolphins on a yearly basis which is brutal and just explain massacre this book has influenced me a lot i still remember the first time i saw this movie i think it was in middle school one of my teachers showed it to all the class or the class and i remember we were going through some really boring topics like animal protection at that time it was really boring to me but right before everyone was going to just sleep in the class the teacher decided to put the textbook down and show us a clear from this document documentary we were shocked speechless to see the has of the dolphins chopped off and left on the beach and the C turning bloody red with their blood which is i felt sick i couldn't need fish for a whole week and it was lasting impression if not scarring impression and i think this movie is still very meaningful and it despite me a lot especially on wildlife protection dolphins or search beautiful intelligent animals of the sea and why do villagers and fishermen in japan killed it i assume there was a great benefit to its skin or some scientific research but the ironic thing is that they only kill them for the meat because the meat taste great that sickens me for awhile and i think the book inspired me to do a lot of different to do a lot of things about well i protection i follow news like"
    sample_sentence2 = "yes i can speak how to this movie is it is worth young wolf young man this is this movie from korea it's a crime movies the movies on the movies speaker speaker or words of young man love hello a cow are you saying they end so i have to go to the go to the america or ha ha ha lots of years a go on the woman the woman is very old he talk to korea he carpool i want to go to the this way this whole home house this house is a is hey so what's your man and at the end the girl cause so there's a woman open open hum finally finds other wolf so what's your young man so the young man don't so yeah man the young man remember he said here's a woman also so am i it's very it's very very sad she is she is a crack credit thank you "
    sample_sentence3 = "yes i want i want to talk about the TV series are enjoying watching a discount name is a friends and it's uh accommodate in the third decades decades an it come out the third decades and its main characters about a six friends live in the NYC but i watched it a long time ago i can't remember the name of them and the story is about what they are happening in their in their life and there are many things treating them and how the friendship are hard friendship and how the french how the strong strongly friendship they obtain them and they always have some funny things happen they only have happened something funny things and is a comedy so that was uh so many and i like this be cause of first adult cause it has a funding it has a farming serious and it can improve my english english words and on the other hand it can i can know about a lot of cultures about the united states and i i first hear about death TV series it's come out of a website and i took into and i watch it after my after my finish my studies and when i was a bad mood when i when i'm in a bad mood or i "
    type_of_feedback = {
        "comment_on_vocabulary": "lexical",
        "comment_on_grammar": "grammatical",
    }

    def get_request_data(send_text, scenario=Literal["chat", "content_assess", "comment_on_vocabulary", "comment_on_grammar"]):
        promptForFeedback = (
            'From a professional perspective, provide a {type_of_feedback} evaluation of the following passage: "{passage}" First, give a summary evaluation, then list the issues and provide suggestions, keeping it within 50 words. output format as '
            '"Summary Evaluation: *'
            'Issues and Suggestions:'
            '  1. *'
            '  ..."'
        )
        if scenario == "content_assess":
            data = [
                {
                    "role": "system",
                    "content":
                        'You are an English teacher and please help to grade a student\'s essay from vocabulary and grammar relevance on how well the essay aligns, and output format as: {"vocabulary": *.**(0-100), "grammar": *.**(0-100)}.',
                },
                {
                    "role": "user",
                    "content": (
                        f'Example1: this essay: "{sample_sentence1}" has vocabulary and grammar scores of 80 and 80, respectively.'
                        f'Example2: this essay: "{sample_sentence2}" has vocabulary and grammar scores of 40 and 43, respectively.'
                        f'Example3: this essay: "{sample_sentence3}" has vocabulary and grammar scores of 50 and 50, respectively.'
                        f'The essay for you to score is "{send_text}".'
                        'The script is from speech recognition so that please first add punctuations when needed, remove duplicates and unnecessary un uh from oral speech, then find all the misuse of words and grammar errors in this essay, find advanced words and grammar usages, and finally give scores based on this information. Please only response as this format {"vocabulary": *.**(0-100), "grammar": *.**(0-100)}.'
                    )
                }
            ]
            return data
        elif scenario == "comment_on_vocabulary":
            return [{
                "role": "user",
                "content": promptForFeedback.format(type_of_feedback=type_of_feedback["comment_on_vocabulary"], passage=send_text)
            }]
        elif scenario == "comment_on_grammar":
            return [{
                "role": "user",
                "content": promptForFeedback.format(type_of_feedback=type_of_feedback["comment_on_grammar"], passage=send_text)
            }]
        else:
            chat_history.append(
                {
                    "role": "user",
                    "content": send_text
                }
            )
            return chat_history

    def stt(filename):
        result_text = []

        speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
        audio_config = speechsdk.audio.AudioConfig(filename=filename)

        speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config, audio_config=audio_config)

        done = False

        def stop_cb(evt: speechsdk.SessionEventArgs):
            """callback that signals to stop continuous recognition upon receiving an event `evt`"""
            nonlocal done
            done = True

        # Connect callbacks to the events fired by the speech recognizer
        speech_recognizer.recognized.connect(lambda evt: result_text.append(evt.result.text))
        # Stop continuous recognition on either session stopped or canceled events
        speech_recognizer.session_stopped.connect(stop_cb)
        speech_recognizer.canceled.connect(stop_cb)

        # Start continuous speech recognition
        speech_recognizer.start_continuous_recognition()
        while not done:
            time.sleep(.5)

        nonlocal reference_text
        speech_recognizer.stop_continuous_recognition()
        text = " ".join(result_text)
        reference_text += text + " "
        print("YOU: ", text)
        return text

    def call_gpt(send_text, scenario=Literal["chat", "content_assess", "comment_on_vocabulary", "comment_on_grammar"]):
        url = (
            f"https://{oai_resource_name}.openai.azure.com/openai/deployments/{oai_deployment_name}/"
            f"chat/completions?api-version={oai_api_version}"
        )
        headers = {"Content-Type": "application/json", "api-key": oai_api_key}
        data = {"messages": get_request_data(send_text, scenario)}

        text = json.loads(
            requests.post(url=url, headers=headers, data=json.dumps(data)).content
        )["choices"][0]["message"]["content"]
        if scenario == "chat":
            chat_history.append(
                {
                    "role": "assistant",
                    "content": text
                }
            )
            print("GPT: ", text)
        return text

    def tts(text, output_path, tag=None):
        file_config = speechsdk.audio.AudioOutputConfig(filename=output_path)
        speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
        speech_synthesizer = speechsdk.SpeechSynthesizer(speech_config=speech_config, audio_config=file_config)

        ssml_text = (
            '<speak version="1.0" xmlns="http://www.w3.org/2001/10/synthesis" xml:lang="en-US">'
            f'<voice name="en-US-JennyNeural">{text}</voice>'
            '</speak>'
        )
        result = speech_synthesizer.speak_ssml_async(ssml_text).get()
        # Check result
        if result.reason == speechsdk.ResultReason.SynthesizingAudioCompleted:
            strip_end_silence(output_path)
            if tag:
                print(f"Save {tag} to {output_path}")
        elif result.reason == speechsdk.ResultReason.Canceled:
            cancellation_details = result.cancellation_details
            print("Speech synthesis canceled: {}".format(cancellation_details.reason))
            if cancellation_details.reason == speechsdk.CancellationReason.Error:
                print("Error details: {}".format(cancellation_details.error_details))

    def pronunciation_assessment():
        # Setup the audio stream
        framerate, bits_per_sample, num_channels = read_wave_header(input_files[0])
        format = speechsdk.audio.AudioStreamFormat(
            samples_per_second=framerate,
            bits_per_sample=bits_per_sample,
            channels=num_channels
        )
        stream = speechsdk.audio.PushAudioInputStream(format)
        audio_config = speechsdk.audio.AudioConfig(stream=stream)

        speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)

        # Must disable miscue for the chatting scenario.
        json_string = {
            "GradingSystem": "HundredMark",
            "Granularity": "Phoneme",
            "EnableMiscue": False,
            "phonemeAlphabet": "IPA",
        }
        pronunciation_config = speechsdk.PronunciationAssessmentConfig(json_string=json.dumps(json_string))
        pronunciation_config.enable_prosody_assessment()
        pronunciation_config.reference_text = reference_text.strip()

        # Creates a speech recognizer using a file as audio input.
        language = 'en-US'
        speech_recognizer = speechsdk.SpeechRecognizer(
            speech_config=speech_config,
            language=language,
            audio_config=audio_config
        )
        # Apply pronunciation assessment config to speech recognizer
        pronunciation_config.apply_to(speech_recognizer)

        done = False
        recognized_words = []
        prosody_scores = []
        fluency_scores = []
        durations = []
        json_words = []
        display_text = ""
        startOffset = 0
        endOffset = 0
        # The speaking rate is the number of words per minute.
        speaking_rate = 0

        def stop_cb(evt):
            """callback that signals to stop continuous recognition upon receiving an event `evt`"""
            nonlocal done
            done = True

        def recognized(evt):
            pronunciation_result = speechsdk.PronunciationAssessmentResult(evt.result)
            nonlocal recognized_words, prosody_scores, fluency_scores, durations, json_words, display_text, startOffset, endOffset
            recognized_words += pronunciation_result.words
            fluency_scores.append(pronunciation_result.fluency_score)
            json_result = evt.result.properties.get(speechsdk.PropertyId.SpeechServiceResponse_JsonResult)
            jo = json.loads(json_result)
            nb = jo["NBest"][0]
            display_text += nb["Display"] + " "
            json_words += nb["Words"]
            prosody_scores.append(pronunciation_result.prosody_score)
            durations.extend([int(w["Duration"]) + 100000 for w in nb["Words"]])
            if startOffset == 0:
                startOffset = nb["Words"][0]["Offset"]
            endOffset = nb["Words"][-1]["Offset"] + nb["Words"][-1]["Duration"] + 100000

        # Connect callbacks to the events fired by the speech recognizer
        speech_recognizer.recognized.connect(recognized)
        # stop continuous recognition on either session stopped or canceled events
        speech_recognizer.session_stopped.connect(stop_cb)
        speech_recognizer.canceled.connect(stop_cb)

        # Start push stream writer thread
        merged_audio_path = "output/merged_audio.wav"
        push_stream_writer_thread = threading.Thread(target=push_stream_writer, args=[stream, input_files, merged_audio_path])
        push_stream_writer_thread.start()
        # Start continuous pronunciation assessment
        speech_recognizer.start_continuous_recognition()
        while not done:
            time.sleep(.5)

        speech_recognizer.stop_continuous_recognition()
        push_stream_writer_thread.join()

        # Get content scores
        content_result = json.loads(call_gpt(display_text, "content_assess"))

        durations_sum = sum([d for w, d in zip(recognized_words, durations) if w.error_type == "None"])

        # We can calculate whole accuracy by averaging
        final_accuracy_scores = []
        for word in recognized_words:
            if word.error_type == 'Insertion':
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

        sorted_scores = sorted([accuracy_score, prosody_score, fluency_score])
        pron_score = sorted_scores[0] * 0.6 + sorted_scores[1] * 0.2 + sorted_scores[2] * 0.2
        print(f"Pronunciation score: {pron_score:.1f}")
        print(f"Accuracy Score: {accuracy_score:.1f}")
        if not math.isnan(prosody_score):
            print(f"Prosody Score: {prosody_score:.1f}")
        print(f"Fluency Score: {fluency_score:.1f}")
        print(f"Vocabulary score: {content_result['vocabulary']:.1f}")
        print(f"Grammar score: {content_result['grammar']:.1f}")

        top_mispronunciation_words = sorted(
            [
                word for word in json_words
                if word["PronunciationAssessment"]["ErrorType"] == "Mispronunciation"
                or word["PronunciationAssessment"]["AccuracyScore"] < 60
            ],
            key=lambda x: x["PronunciationAssessment"]["AccuracyScore"],
        )[:3]

        speaking_rate = len(json_words) // (((endOffset - startOffset) / reduced_unit) / 60)

        comment_result(
            {
                "accuracy score": accuracy_score,
                "fluency score": fluency_score,
                "prosody score": prosody_score,
                "vocabulary score": content_result["vocabulary"],
                "grammar score": content_result["grammar"],
            },
            set_punctuation(json_words, display_text),
            top_mispronunciation_words,
            merged_audio_path,
            display_text,
            speaking_rate
        )

    def set_punctuation(json_words, display_text):
        for idx, word in enumerate(display_text.split(" ")):
            if word.strip() and word.strip()[-1] in string.punctuation:
                json_words[idx]["has_punctuation"] = True
        return json_words

    def comment_result(scores_dict, json_words, mis_pronunciation_words, merged_audio_path, content, speaking_rate):
        message_dict = {
            "Excellent": [],
            "Good": [],
            "Fair": [],
            "Poor": [],
            "Bad": [],
        }
        error_dict = {
            "Missing break": [],
            "Unexpected break": [],
            "Monotone": [],
        }
        speed_of_speaking_rule = {
            "a bit slowly": range(150 - 200),
            "slowly": range(100 - 150),
            "too slowly": range(0, 100)
        }

        def set_message_dict(score, score_name):
            if score >= 80:
                return message_dict["Excellent"].append(score_name)
            elif score < 80 and score >= 60:
                return message_dict["Good"].append(score_name)
            elif score < 60 and score >= 40:
                return message_dict["Fair"].append(score_name)
            elif score < 40 and score >= 20:
                return message_dict["Poor"].append(score_name)
            else:
                return message_dict["Bad"].append(score_name)

        def get_prosody_error(error_type, word, last_word):
            threshold = 0.1
            if error_type == "MissingBreak" or error_type == "UnexpectedBreak":
                break_error_info = \
                    word["PronunciationAssessment"]["Feedback"]["Prosody"]["Break"].get(error_type, "null")
                if break_error_info == "null":
                    return False
                if error_type == "MissingBreak":
                    if break_error_info["Confidence"] >= threshold and last_word.get("has_punctuation", False):
                        return True
                if error_type == "UnexpectedBreak":
                    if break_error_info["Confidence"] >= threshold and not last_word.get("has_punctuation", False):
                        return True
                return False
            elif error_type == "Monotone" and \
                    error_type in word["PronunciationAssessment"]["Feedback"]["Prosody"]["Intonation"]["ErrorTypes"]:
                return True
            else:
                return False

        def set_error_dict(json_words):
            for idx, word in enumerate(json_words):
                if get_prosody_error("MissingBreak", word, json_words[idx - 1]):
                    error_dict["Missing break"].append(word)
                elif get_prosody_error("UnexpectedBreak", word, json_words[idx - 1]):
                    error_dict["Unexpected break"].append(word)
                elif get_prosody_error("Monotone", word, json_words[idx - 1]):
                    error_dict["Monotone"].append(word)

        def get_error_message(error_types):
            message = ""
            for error_type in error_types:
                if len(error_dict[error_type]) != 0:
                    message += f"{error_type} count is {len(error_dict[error_type])}. near the word: "
                    message += f"{', '.join([word['Word'].strip() for word in error_dict[error_type]])}. "

            return message

        def get_report(json_words, mis_pronunciation_words, merged_audio_path, content, speaking_rate):
            set_error_dict(json_words)

            report_audio_list = []
            report_path = "output/chat_report.wav"
            text_to_write = ""
            if len(mis_pronunciation_words) != 0:
                accuracy_report_audio_list = []
                accuracy_report_path = "output/accuracy_report.wav"
                for idx, mis_word in enumerate(mis_pronunciation_words):
                    origin_content = ""
                    report_clip_path = f"output/accuracy_report_clip_{idx+1}.wav"
                    mis_word_clip_path = f"output/mis_word_clip_{idx+1}.wav"
                    if idx == 0:
                        origin_content += "Accuracy report:"
                    origin_content += f' word {mis_word["Word"]}'
                    origin_content += f' correct pronunciation is {mis_word["Word"]}, your pronunciation is'

                    tts(origin_content, report_clip_path)
                    text_to_write += origin_content + f' {mis_word["Word"]}.'
                    get_mispronunciation_clip(
                        mis_word["Offset"],
                        mis_word["Duration"],
                        mis_word_clip_path,
                        merged_audio_path
                    )
                    accuracy_report_audio_list.append(report_clip_path)
                    accuracy_report_audio_list.append(mis_word_clip_path)
                merge_wav(accuracy_report_audio_list, accuracy_report_path)
                report_audio_list.append(accuracy_report_path)
                os.remove(merged_audio_path)

            if scores_dict["fluency score"] < 60 or scores_dict["prosody score"] < 60:
                origin_content = ""
                fluency_prosody_report_path = "output/fluency_prosody_report.wav"
                if scores_dict["fluency score"] < 60:
                    origin_content += "Fluency "
                if scores_dict["prosody score"] < 60:
                    origin_content += "Prosody "
                origin_content += "report: "
                origin_content += get_error_message(["Missing break", "Unexpected break", "Monotone"])

                tts(origin_content, fluency_prosody_report_path)
                text_to_write += origin_content
                report_audio_list.append(fluency_prosody_report_path)

            if content.strip() != "":
                vocabulary_feedback_path = "output/vocabulary_feedback.wav"
                grammar_feedback_path = "output/grammar_feedback.wav"
                tts(call_gpt(content, "comment_on_vocabulary"), vocabulary_feedback_path, "vocabulary feedback")
                tts(call_gpt(content, "comment_on_grammar"), grammar_feedback_path, "grammar feedback")

            # Comments on speaking speed.
            for k, r in speed_of_speaking_rule.items():
                if speaking_rate in r:
                    speed_of_speaking_report_path = "output/speed_of_speaking_report.wav"
                    comments_on_speed_of_speaking = f"You're speaking {k}"
                    tts(comments_on_speed_of_speaking, speed_of_speaking_report_path)
                    text_to_write += comments_on_speed_of_speaking
                    report_audio_list.append(speed_of_speaking_report_path)

            with open("output/chat_report.txt", "w", encoding="utf-8") as f:
                f.write(text_to_write)
            merge_wav(report_audio_list, report_path, "report")

        def get_score_comment(scores_dict):
            for score_key in scores_dict:
                set_message_dict(scores_dict[score_key], score_key)

            messages = ""
            for message_key in message_dict:
                if message_dict[message_key] != []:
                    is_or_are = "is" if len(message_dict[message_key]) == 1 else "are"
                    messages += f"{', '.join(message_dict[message_key])} {is_or_are} {message_key}. "

            tts(messages, "output/chat_score_comment.wav", "score comment")

        get_score_comment(scores_dict)
        get_report(json_words, mis_pronunciation_words, merged_audio_path, content, speaking_rate)

    if not os.path.exists("output"):
        os.makedirs("output")
    for idx, file in enumerate(input_files):
        tts(call_gpt(stt(file), "chat"), f"output/gpt_output_{idx+1}.wav", "GPT output")
    print("Generate the final report ......")
    pronunciation_assessment()


if __name__ == "__main__":
    chatting_from_file()
