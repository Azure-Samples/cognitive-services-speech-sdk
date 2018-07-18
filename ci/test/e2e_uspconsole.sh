#!/bin/bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SCRIPT_NAME="$(basename "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh"
. "$SCRIPT_DIR/../test-harness.sh"

USAGE="
Usage: $SCRIPT_NAME binary_dir keySkyman

Expected environment variables:
  - \$TEST_AUDIO_FILE - audio input file
  - \$TEST_MODEL_ID - CRIS Model ID
  - \$TEST_SPEECH_ENDPOINT - speech endpoint
  - \$TEST_CRIS_ENDPOINT - CRIS endpoint
"

[[ $# -eq 2 ]] || die "Error: wrong number of arguments.\n$USAGE"
[[ -n $TEST_AUDIO_FILE ]] || die "Error: \$TEST_AUDIO_FILE not set.\n$USAGE"
[[ -n $TEST_MODEL_ID ]] || die "Error: \$TEST_MODEL_ID not set.\n$USAGE"
[[ -n $TEST_SPEECH_ENDPOINT ]] || die "Error: \$TEST_SPEECH_ENDPOINT not set.\n$USAGE"
[[ -n $TEST_CRIS_ENDPOINT ]] || die "Error: \$TEST_CRIS_ENDPOINT not set.\n$USAGE"

BINARY_DIR=$1
UserKeySkyman=$2

USPCONSOLE=$BINARY_DIR/uspclientconsole

# Using array of pairs for deterministic test order

tests=(
  "speech base model in interactive mode"   "mode:interactive"
  "speech base model in conversation mode"  "mode:conversation"
  "speech base model in dictation mode"     "mode:dictation"
  "CRIS model in interactive mode"          "mode:interactive model:$TEST_MODEL_ID"
  "CRIS model in conversation mode"         "mode:conversation model:$TEST_MODEL_ID"
  "CRIS model in dictation mode"            "mode:dictation model:$TEST_MODEL_ID"
  "endpoint for speech model"               "url:$TEST_SPEECH_ENDPOINT"
  "endpoint for CRIS model"                 "url:$TEST_CRIS_ENDPOINT"
)

PLATFORMS_TO_RUN="$(joinArgs , {OSX-x64,Linux-x64,Windows-{x86,x64}}-{Debug,Release})"

PLATFORM=$SPEECHSDK_TARGET_PLATFORM-$SPEECHSDK_BUILD_CONFIGURATION

startTests TESTRUNNER TEST-uspconsole "$PLATFORM" "$UserKeySkyman"
startSuite TESTRUNNER "$(basename "$USPCONSOLE" .exe)"

TIMEOUT_SECONDS=30

for ((testIndex = 0; testIndex < ${#tests[@]}; testIndex += 2)); do
  t="${tests[$testIndex]}"
  testArg="${tests[$testIndex + 1]}"

  runTest TESTRUNNER "$t" "$PLATFORMS_TO_RUN" $TIMEOUT_SECONDS \
    $USPCONSOLE auth:$UserKeySkyman type:speech $testArg $TEST_AUDIO_FILE
done

endSuite TESTRUNNER
endTests TESTRUNNER
