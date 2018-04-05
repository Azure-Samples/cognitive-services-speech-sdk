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

ARGS="audio $TEST_AUDIO_FILE false"

echo "Testing speech base model in interactive mode:"
(set -x; $USPCONSOLE $ARGS $UserKeySpeech speech interactive)
echo -e "\n"

echo "Testing speech base model in conversation mode:"
(set -x; $USPCONSOLE $ARGS $UserKeySpeech speech conversation)
echo -e "\n"

echo "Testing speech base model in dictation mode:"
(set -x; $USPCONSOLE $ARGS $UserKeySpeech speech dictation)
echo -e "\n"

echo "Testing CRIS model in interactive mode:"
(set -x; $USPCONSOLE $ARGS $UserKeyCris cris interactive $TEST_MODEL_ID)
echo -e "\n"

echo "Testing CRIS model in conversation mode:"
(set -x; $USPCONSOLE $ARGS $UserKeyCris cris conversation $TEST_MODEL_ID)
echo -e "\n"

echo "Testing CRIS model in dictation mode:"
(set -x; $USPCONSOLE $ARGS $UserKeyCris cris dictation $TEST_MODEL_ID)
echo -e "\n"

echo "Testing endpoint for speech model:"
(set -x; $USPCONSOLE $ARGS $UserKeySpeech url $TEST_SPEECH_ENDPOINT)
echo -e "\n"

echo "Testing endpoint for CRIS model:"
(set -x; $USPCONSOLE $ARGS $UserKeyCris url $TEST_CRIS_ENDPOINT)
echo -e "\n"


echo -e "\n Done\n"