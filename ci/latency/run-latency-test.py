#!/usr/bin/env python3

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import azure.cognitiveservices.speech as speechsdk
import time
import datetime
import uuid
import argparse
import os
import random
import string

done = False
end = 0


def recognize_file(cmd_line):
    if cmd_line.region == "local":
        template = "ws://localhost:5000/speech/recognition/dictation/cognitiveservices/v1?{}"
        speech_config = speechsdk.SpeechConfig(subscription=cmd_line.subscription,
                                               endpoint=template.format(str(uuid.uuid4())))
    else:
        speech_config = speechsdk.SpeechConfig(subscription=cmd_line.subscription, region=cmd_line.region)

    chars = string.ascii_lowercase + string.digits
    random_string = ''.join(random.choice(chars) for x in range(6))
    os.makedirs(cmd_line.log_directory, exist_ok=True)
    log_file = os.path.join(cmd_line.log_directory, "speech_" + random_string + ".log")
    speech_config.set_property(speechsdk.PropertyId.Speech_LogFilename, log_file)

    audio_config = speechsdk.audio.AudioConfig(filename=cmd_line.wav_file[0])
    speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config, audio_config=audio_config)

    def stop_cb(evt):
        """callback that stops continuous recognition upon receiving an event `evt`"""
        print('CLOSING on {}'.format(evt))
        speech_recognizer.stop_continuous_recognition()
        global done, end
        end = time.time()
        done = True

    # Connect callbacks to the events fired by the speech recognizer
    # speech_recognizer.recognizing.connect(lambda evt: print('RECOGNIZING: {}'.format(evt)))
    speech_recognizer.recognized.connect(lambda evt: print('RECOGNIZED: {}'.format(evt)))
    speech_recognizer.session_started.connect(lambda evt: print('SESSION STARTED: {0}, time: {1}'.format(evt, datetime.datetime.fromtimestamp(time.time()))))
    speech_recognizer.session_stopped.connect(lambda evt: print('SESSION STOPPED {0}, time: {1}'.format(evt, datetime.datetime.fromtimestamp(time.time()))))
    speech_recognizer.canceled.connect(lambda evt: print('CANCELED {}'.format(evt)))

    # Stop continuous recognition on either session stopped or canceled events
    speech_recognizer.session_stopped.connect(stop_cb)
    speech_recognizer.canceled.connect(stop_cb)

    # Start continuous speech recognition
    start = time.time()
    speech_recognizer.start_continuous_recognition()
    global done, end
    while not done:
        time.sleep(.5)
    print("Start: {0}, End: {1}, Latency: {2}".format(datetime.datetime.fromtimestamp(start),
                                                      datetime.datetime.fromtimestamp(end), end - start))


# ======================= Entry point =======================
if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Run latency test",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="\nexample use: \n\
    run-reco-cont -s <subscription> -r <azure region> <audiofile.wav>\n\n\n\
      "
    )

    parser.add_argument("-r", "--region", default="local", help="Pick an Azure region (default: %(default)s)")
    parser.add_argument("-s", "--subscription", default="00000000000000000000000000000000",
                        help="Pick speech subscription")
    parser.add_argument("-d", "--log-directory", default="/tmp/speech/",
                        help="Pick a location for storing logs (default: %(default)s)")
    parser.add_argument("wav_file", nargs=1, help="Wave file to recognize")
    cmd_line = parser.parse_args()
    recognize_file(cmd_line)
