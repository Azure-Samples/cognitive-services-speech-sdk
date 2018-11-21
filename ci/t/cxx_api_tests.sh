#!/usr/bin/env bash
T="$(basename "$0" .sh)"
BUILD_DIR="$1"
PLATFORM="$2"
BINARY_DIR="$3"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

isOneOf "$PLATFORM" {{Windows,Linux}-{x86,x64},Linux-arm32,OSX-x64}-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

TEST_CODE="$BINARY_DIR/cxx_api_tests"
[[ -x $TEST_CODE ]] ||
  exitWithSuccess "Test %s: skip, no test code\n" "$T"

. "$SCRIPT_DIR/../test-harness.sh" || exit 1

runCatchSuite \
  TESTRUNNER \
  "test-$T-$PLATFORM" \
  "$PLATFORM" \
  "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_LUIS_KEY" \
  "$T" \
  240 \
  "$TEST_CODE" \
    --keySpeech "$SPEECHSDK_SPEECH_KEY" \
    --regionId "$SPEECHSDK_SPEECH_REGION" \
    --keyLUIS "$SPEECHSDK_LUIS_KEY" \
    --regionIdLUIS "$SPEECHSDK_LUIS_REGION" \
    --luisAppId "$SPEECHSDK_LUIS_HOMEAUTOMATION_APPID" \
    --inputDir "$SPEECHSDK_INPUTDIR"
