#!/bin/bash
function vsts_setvar {
  echo "Setting $1 to $2"
  echo "##vso[task.setvariable variable=$1]$2"
}

vsts_setvar TEST_MODEL_ID 0f7118f0-b39c-4f28-baf0-6deebe25583a

vsts_setvar TEST_SPEECH_ENDPOINT "wss://westus.stt.speech.microsoft.com/speech/recognition/interactive/cognitiveservices/v1?format=simple&language=en-us"

vsts_setvar TEST_CRIS_ENDPOINT "wss://westus.stt.speech.microsoft.com/speech/recognition/interactive/cognitiveservices/v1?cid=0f7118f0-b39c-4f28-baf0-6deebe25583a"
