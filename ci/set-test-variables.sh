#!/bin/bash
function vsts_setvar {
  echo "Setting $1 to $2"
  echo "##vso[task.setvariable variable=$1]$2"
}

vsts_setvar TEST_MODEL_ID ebe91752-a5ca-47b0-9ef8-7ddd533a34ef
vsts_setvar TEST_SPEECH_ENDPOINT "wss://fedev.develop2.cris.ai/speech/recognition/interactive/cognitiveservices/v1?format=simple&language=en-us"
vsts_setvar TEST_CRIS_ENDPOINT "wss://fedev.develop2.cris.ai/speech/recognition/interactive/cognitiveservices/v1?cid=ebe91752-a5ca-47b0-9ef8-7ddd533a34ef"
