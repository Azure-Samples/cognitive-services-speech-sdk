#!/bin/bash

set -e

[ $# -ne 5 ] && echo -e "Usage: e2e_uspconsole binary_dir keySpeech keyCris keyLuis keySkyman" && exit 1

# TODO: use cmake (configure_file) to automatically configure bin dir.
BINARY_DIR=$1
USPCONSOLE=$BINARY_DIR/uspclientconsole

[ "$TEST_AUDIO_FILE" = "" ] && die "No audio input file specified."
[ "$TEST_MODEL_ID" = "" ] && die "No CRIS Model ID is specified."
[ "$TEST_SPEECH_ENDPOINT" = "" ] && die "No endpoint is specified."
[ "$TEST_CRIS_ENDPOINT" = "" ] && die "No endpoint is specified."

UserKeySpeech=$2
UserKeyCris=$3
UserKeyLuis=$4
UserKeySkyman=$5


echo -e "\n Testing usp lib: \n"

echo "Testing speech base model in interactive mode:"
(set -x; $USPCONSOLE auth:$UserKeySkyman type:speech mode:interactive $TEST_AUDIO_FILE)
echo -e "\n"

echo "Testing speech base model in conversation mode:"
(set -x; $USPCONSOLE auth:$UserKeySkyman type:speech mode:conversation $TEST_AUDIO_FILE)
echo -e "\n"

echo "Testing speech base model in dictation mode:"
(set -x; $USPCONSOLE auth:$UserKeySkyman type:speech mode:dictation $TEST_AUDIO_FILE)
echo -e "\n"

echo "Testing CRIS model in interactive mode:"
(set -x; $USPCONSOLE auth:$UserKeySkyman type:speech mode:interactive model:$TEST_MODEL_ID $TEST_AUDIO_FILE)
echo -e "\n"

echo "Testing CRIS model in conversation mode:"
(set -x; $USPCONSOLE auth:$UserKeySkyman type:speech mode:conversation model:$TEST_MODEL_ID $TEST_AUDIO_FILE)
echo -e "\n"

echo "Testing CRIS model in dictation mode:"
(set -x; $USPCONSOLE auth:$UserKeySkyman type:speech mode:dictation model:$TEST_MODEL_ID $TEST_AUDIO_FILE)
echo -e "\n"

echo "Testing endpoint for speech model:"
(set -x; $USPCONSOLE auth:$UserKeySkyman type:speech url:$TEST_SPEECH_ENDPOINT $TEST_AUDIO_FILE)
echo -e "\n"

echo "Testing endpoint for CRIS model:"
(set -x; $USPCONSOLE auth:$UserKeySkyman type:speech url:$TEST_CRIS_ENDPOINT $TEST_AUDIO_FILE)
echo -e "\n"


echo -e "\n Done\n"