#!/usr/bin/env bash
T="$(basename "$0" .sh)"
BUILD_DIR="$1"
PLATFORM="$2"
BINARY_DIR="$3"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

isOneOf "$PLATFORM" {{Windows,Linux}-{x86,x64},Linux-arm32,OSX-x64}-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

TEST_CODE="$BINARY_DIR/usp_tests"
[[ -x $TEST_CODE ]] ||
  exitWithSuccess "Test %s: skip, no test code\n" "$T"

set -x
"$TEST_CODE" \
  --keySpeech "$SPEECHSDK_SPEECH_KEY" \
  --regionId "$SPEECHSDK_SPEECH_REGION" \
  --inputDir "$SPEECHSDK_INPUTDIR" \
  --reporter junit --out "test-$T-$PLATFORM.xml" --success
