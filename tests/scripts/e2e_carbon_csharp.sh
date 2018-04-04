#!/bin/bash

function die {
    set +x
    echo -e $1
    exit 1
}

function run_test {
    echo "$1 recongition using base model:"
    (set -x; $CARBON_CSHARP $1 "$KeySpeech" $AUDIO_FILE) || exit $?
    echo -e "\n"
    
    echo "$1 recongition using CRIS model:"
    (set -x; $CARBON_CSHARP $1 "$KeyCris" $AUDIO_FILE model:$MODEL_ID) || exit $?
    echo -e "\n"

    echo "$1 recongition using speech endpoint:"
    (set -x; $CARBON_CSHARP $1 "$KeySpeech" $AUDIO_FILE endpoint:"$SPEECH_ENDPOINT") || exit $?
    echo -e "\n"
    
    echo "$1 recongition using CRIS endpoint:"
    (set -x; $CARBON_CSHARP $1 "$KeyCris" $AUDIO_FILE endpoint:"$CRIS_ENDPOINT") || exit $?
    echo -e "\n"
}

set -e

BINARY_DIR=$1
CARBON_CSHARP=$BINARY_DIR/carbon_csharp_console.exe

[ "$AUDIO_FILE" = "" ] && die "No audio input file specified."
[ "$MODEL_ID" = "" ] && die "No CRIS Model ID is specified."
[ "$SPEECH_ENDPOINT" = "" ] && die "No endpoint is specified."
[ "$CRIS_ENDPOINT" = "" ] && die "No endpoint is specified."

KeySpeech=$3
KeyCris=$4
KeyLuis=$5
KeySkyman=$6

if [ "$2" = "all" ]; then
 run_test speech
 run_test intent
else
 run_test $2
fi

echo -e "\nDone\n"
