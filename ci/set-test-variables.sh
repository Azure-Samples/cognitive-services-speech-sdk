#!/bin/bash
function vsts_setvar {
  echo "Setting $1 to $2"
  echo "##vso[task.setvariable variable=$1]$2"
}

# Model on internal.cris.ai for Mark; Eldar's subscription for INT/PROD.
TEST_MODEL_ID=6264ab43-c854-4d9f-84fc-5f33621935f3
vsts_setvar TEST_MODEL_ID $TEST_MODEL_ID

vsts_setvar TEST_SPEECH_ENDPOINT "wss://westus.stt.speech.microsoft.com/speech/recognition/interactive/cognitiveservices/v1?format=simple&language=en-us"

vsts_setvar TEST_CRIS_ENDPOINT "wss://westus.stt.speech.microsoft.com/speech/recognition/interactive/cognitiveservices/v1?cid=$TEST_MODEL_ID"
