#!/usr/bin/env bash
T="$(basename "$0" .sh)"
BUILD_DIR="$1"
PLATFORM="$2"
BINARY_DIR="$3"
TESTSET="${4:-dev}"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

isOneOf "$PLATFORM" {{Windows,Linux}-{x86,x64},Linux-{arm32,arm64},OSX-x64}-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

TEST_CODE="$BINARY_DIR/uspclientconsole"
[[ -x $TEST_CODE ]] ||
  exitWithSuccess "Test %s: skip, no test code\n" "$T"

. "$SCRIPT_DIR/../test-harness.sh" || exit 1

# Apparently need to override the normally blank endpointid
SPEECHSDK_SPEECH_ENDPOINTID_ENUS=$( getSetting './tests/test.defaults.json' 'OnlineAudioEndpoint' )

speechEndpoint="$(speechWebSocketsEndpoint "$SPEECHSDK_SPEECH_REGION" interactive simple en-us)"
crisEndpoint="$(crisWebSocketsEndpoint "$SPEECHSDK_SPEECH_REGION" interactive "$SPEECHSDK_SPEECH_ENDPOINTID_ENUS")"
audioFile="$SPEECHSDK_INPUTDIR/audio/whatstheweatherlike.wav"

# Using array of pairs for deterministic test order

tests=(
  "speech base model in interactive mode" "mode:interactive"
  "speech base model in conversation mode"  "mode:conversation"
  "speech base model in dictation mode" "mode:dictation"
  "CRIS model in interactive mode" "mode:interactive model:$SPEECHSDK_SPEECH_ENDPOINTID_ENUS"
  "CRIS model in conversation mode" "mode:conversation model:$SPEECHSDK_SPEECH_ENDPOINTID_ENUS"
  "CRIS model in dictation mode" "mode:dictation model:$SPEECHSDK_SPEECH_ENDPOINTID_ENUS"
  "endpoint for speech model" "url:$speechEndpoint"
  "endpoint for CRIS model" "url:$crisEndpoint"
)

startConstructedTestRunOutput TESTRUNNER "test-$T-$PLATFORM" "$PLATFORM" "$SPEECHSDK_SPEECH_KEY"
startConstructedSuiteOutput TESTRUNNER "$T"

TIMEOUT_SECONDS=30

for ((testIndex = 0; testIndex < ${#tests[@]}; testIndex += 2)); do
  t="${tests[$testIndex]}"
  testArg="${tests[$testIndex + 1]}"

  runTest TESTRUNNER "$t" $TIMEOUT_SECONDS \
    "$TEST_CODE" auth:$SPEECHSDK_SPEECH_KEY region:$SPEECHSDK_SPEECH_REGION type:speech $testArg $audioFile
done

endConstructedSuiteOutput TESTRUNNER
endConstructedTestRunOutput TESTRUNNER