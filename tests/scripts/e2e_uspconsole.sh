#!/bin/bash

set -e

# TODO: use cmake (configure_file) to automatically configure bin dir.
BINARY_DIR=$1
USPCONSOLE=$BINARY_DIR/uspclientconsole
AUDIO_FILE=whatstheweatherlike.wav
SPEECH_SUB_KEY=$2
CRIS_SUB_KEY=$3
CRIS_MODEL_ID=3ae61ac26c534775b2606a61acd052bd


echo -e "\n Testing usp lib: \n"

ARGS="audio $AUDIO_FILE false $SPEECH_SUB_KEY speech"

echo "Launching: "
(set -x; $USPCONSOLE $ARGS interactive)
echo -e "\n\n\n"

echo "Launching: "
(set -x; $USPCONSOLE $ARGS conversation en-us)
echo -e "\n\n\n"

echo "Launching: "
(set -x; $USPCONSOLE $ARGS dictation en-us simple)
echo -e "\n\n\n"

echo "Launching: "
(set -x; $USPCONSOLE audio $AUDIO_FILE false $CRIS_SUB_KEY cris interactive $CRIS_MODEL_ID)
echo -e "\n\n\n"

echo -e "\n Done\n"