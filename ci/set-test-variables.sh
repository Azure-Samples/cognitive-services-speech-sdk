#!/bin/bash

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

# Get some helpers
. "$SCRIPT_DIR/functions.sh"

# Model on internal.cris.ai for Mark; Eldar's subscription for INT/PROD.
TEST_MODEL_ID=f039fdc7-5a4a-4e28-9c62-c8c0fbc0da39
vsts_setvar TEST_MODEL_ID $TEST_MODEL_ID

vsts_setvar TEST_SPEECH_ENDPOINT "wss://westus.stt.speech.microsoft.com/speech/recognition/interactive/cognitiveservices/v1?format=simple&language=en-us"

vsts_setvar TEST_CRIS_ENDPOINT "wss://westus.stt.speech.microsoft.com/speech/recognition/interactive/cognitiveservices/v1?cid=$TEST_MODEL_ID"

vsts_setvar TEST_AUDIO_FILE $BUILD_REPOSITORY_LOCALPATH/tests/input/audio/whatstheweatherlike.wav

# App based on LUIS' Home Automation pre-build domain (see https://docs.microsoft.com/azure/cognitive-services/LUIS/luis-get-started-create-app)
# This specific one was created under mahilleb's LUIS account and published to
# csspeechsdk-luis-westus-s0-201809 in the Custom Speech Development
# subscription with the following settings:
# External services settings
#   [ ] Enable Sentiment Analysis
#   [X] Enable Speech Priming
# Endpoint url settings
#   Timezone: PST
#   [X] Include all predicted intent scores
#   [ ] Enable Bing spell checker
vsts_setvar TEST_INTENT_HOMEAUTOMATION_APPID b687b851-56c5-4d31-816f-35a741a3f0be
vsts_setvar TEST_INTENT_HOMEAUTOMATION_TURNON_INTENT HomeAutomation.TurnOn
vsts_setvar TEST_INTENT_HOMEAUTOMATION_TURNON_AUDIO $BUILD_REPOSITORY_LOCALPATH/tests/input/audio/TurnOnTheLamp.wav
