#!/bin/bash

function die {
    set +x
    echo -e $1
    exit 1
}

function run_test {
    echo "$1 recognition using base model:"
    (set -x; $CARBON_CSHARP $1 "$UserKeySkyman" $TEST_AUDIO_FILE) || exit $?
    echo -e "\n"

    echo "$1 recognition using CRIS model:"
    (set -x; $CARBON_CSHARP $1 "$UserKeySkyman" $TEST_AUDIO_FILE model:$TEST_MODEL_ID) || exit $?
    echo -e "\n"

    echo "$1 recognition using speech endpoint:"
    (set -x; $CARBON_CSHARP $1 "$UserKeySkyman" $TEST_AUDIO_FILE endpoint:"$TEST_SPEECH_ENDPOINT") || exit $?
    echo -e "\n"
    
    echo "$1 recognition using CRIS endpoint:"
    (set -x; $CARBON_CSHARP $1 "$UserKeySkyman" $TEST_AUDIO_FILE endpoint:"$TEST_CRIS_ENDPOINT") || exit $?
    echo -e "\n"

    echo "$1 recognition using stream input:"
    (set -x; $CARBON_CSHARP $1 "$UserKeySkyman" stream:$TEST_AUDIO_FILE) || exit $?
    echo -e "\n"
    
    #echo "Test AuthToken: $1 recognition using base model:"
    #(set -x; $CARBON_CSHARP $1 token:"$UserKeySkyman" $TEST_AUDIO_FILE) || exit $?
    #echo -e "\n"

    #echo "Test AuthToken: $1 recognition using speech endpoint:"
    #(set -x; $CARBON_CSHARP $1 token:"$UserKeySkyman" $TEST_AUDIO_FILE endpoint:"$TEST_SPEECH_ENDPOINT") || exit $?
    #echo -e "\n"
}

set -e

[ $# -ne 6 ] && die "Usage: e2e_carbon_csharp binary_dir action(all|speech|intent) keySpeech keyCris keyLuis keySkyman"

BINARY_DIR=$1
CARBON_CSHARP=$BINARY_DIR/carbon_csharp_console

[ "$TEST_AUDIO_FILE" = "" ] && die "No audio input file specified."
[ "$TEST_MODEL_ID" = "" ] && die "No CRIS Model ID is specified."
[ "$TEST_SPEECH_ENDPOINT" = "" ] && die "No endpoint is specified."
[ "$TEST_CRIS_ENDPOINT" = "" ] && die "No endpoint is specified."

Action=$2
UserKeySpeech=$3
UserKeyCris=$4
UserKeyLuis=$5
UserKeySkyman=$6

if [ "$Action" = "all" ]; then
 run_test speech
 run_test intent
elif [ "$Action" = "speech" ]; then
 run_test speech
elif [ "$Action" = "intent" ]; then
 run_test intent
elif [ "$Action" = "translation" ]; then
 run_test translation
else
    die "Unknown action: $Action"
fi

echo -e "\nDone\n"
