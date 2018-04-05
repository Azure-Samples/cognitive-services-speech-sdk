#!/bin/bash

function die {
    set +x
    echo -e $1
    exit 1
}

function run_test {
    echo "$1 recongition using base model:"
    (set -x; $CARBONX --$1 --subscription:$UserKeySpeech --input $TEST_AUDIO_FILE $2) || exit $?
    echo -e "\n"

    echo "$1 recongition using CRIS model:"
    (set -x; $CARBONX --$1 --subscription:$UserKeyCris --input $TEST_AUDIO_FILE  --customSpeechModelId:$TEST_MODEL_ID $2) || exit $?
    echo -e "\n"

    echo "$1 recongition using speech endpoint:"
    (set -x; $CARBONX --$1 --subscription:$UserKeySpeech --input $TEST_AUDIO_FILE --endpoint:"$TEST_SPEECH_ENDPOINT" $2) || exit $?
    echo -e "\n"

    echo "$1 recongition using CRIS endpoint:"
    (set -x; $CARBONX --$1 --subscription:$UserKeyCris --input $TEST_AUDIO_FILE --endpoint:"$TEST_CRIS_ENDPOINT" $2) || exit $?
    echo -e "\n"
}

set -e

[ $# -ne 6 ] && die "Usage: e2e_carbonx binary_dir action(all|speech|intent) keySpeech keyCris keyLuis keySkyman"

BINARY_DIR=$1
CARBONX=$BINARY_DIR/carbonx

[ "$TEST_AUDIO_FILE" = "" ] && die "No audio input file specified."
[ "$TEST_MODEL_ID" = "" ] && die "No CRIS Model ID is specified."
[ "$TEST_SPEECH_ENDPOINT" = "" ] && die "No endpoint is specified."
[ "$TEST_CRIS_ENDPOINT" = "" ] && die "No endpoint is specified."

Action=$2
UserKeySpeech=$3
UserKeyCris=$4
UserKeyLuis=$5
UserKeySkyman=$6

echo -e "\n Testing carbonx: \n"

echo $*

if [ "$Action" = "all" ]; then
    run_test speech
    run_test speech --single
    run_test speech --continuous:10
    run_test intent
    run_test intent --single
    run_test intent --continuous:10
elif [ "$Action" = "speech" ]; then
    run_test speech
    run_test speech --single
    run_test speech --continuous:10
elif [ "$Action" = "intent" ]; then
    run_test intent
    run_test intent --single
    run_test intent --continuous:10
else
    die "Unknow action: $Action"
fi

echo -e "\nDone\n"
