#!/bin/bash
function vsts_setvar {
  echo "Setting $1 to $2"
  echo "##vso[task.setvariable variable=$1]$2"
}

# Model on internal.cris.ai for Mark; Eldar's subscription for INT/PROD.
vsts_setvar TEST_MODEL_ID 2c225b03-6216-4b71-9db7-2403c0eb08b3

vsts_setvar TEST_SPEECH_ENDPOINT "wss://westus.stt.speech.microsoft.com/speech/recognition/interactive/cognitiveservices/v1?format=simple&language=en-us"

vsts_setvar TEST_CRIS_ENDPOINT "wss://westus.stt.speech.microsoft.com/speech/recognition/interactive/cognitiveservices/v1?cid=2c225b03-6216-4b71-9db7-2403c0eb08b3"
