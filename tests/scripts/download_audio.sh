#!/bin/bash

set -e

# TODO: move audio files used for tests into a git submodule (carbon_test).
ASSETS_URL=https://raw.githubusercontent.com/Azure-Samples/Cognitive-Speech-STT-Android/master/samples/SpeechRecoExample/assets
AUDIO_FILE=whatstheweatherlike.wav
(set -x; curl -fsSO $ASSETS_URL/$AUDIO_FILE)

ls -l
if [ -f $AUDIO_FILE ] 
then
    exit 0
else
    echo "$AUDIO_FILE not found"
    exit 1
fi