#!/usr/bin/env bash
T="$(basename "$0" .sh)"
BUILD_DIR="$1"
PLATFORM="$2"
BINARY_DIR="$3"
TESTSET="${4:-dev}"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

isOneOf "$PLATFORM" {{Windows,Linux}-{x86,x64},Linux-arm32,OSX-x64}-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

TEST_CODE="$BINARY_DIR/cxx_api_tests"
[[ -x $TEST_CODE ]] ||
  exitWithSuccess "Test %s: skip, no test code\n" "$T"

. "$SCRIPT_DIR/../test-harness.sh" || exit 1

PATTERN=
case $TESTSET in
  prod|int)
    # All default (non-hidden) + the specified tag
    PATTERN="~[.],[$TESTSET]"
    ;;
esac

runCatchSuite \
  TESTRUNNER \
  "test-$T-$PLATFORM" \
  "$PLATFORM" \
  "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_LUIS_KEY $SPEECHSDK_BOT_SUBSCRIPTION $SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_KEY" \
  "$T" \
  240 \
  "$PATTERN" \
  "$TEST_CODE" \
    --keySpeech "$SPEECHSDK_SPEECH_KEY" \
    --regionId "$SPEECHSDK_SPEECH_REGION" \
    --keyLUIS "$SPEECHSDK_LUIS_KEY" \
    --regionIdLUIS "$SPEECHSDK_LUIS_REGION" \
    --luisAppId "$SPEECHSDK_LUIS_HOMEAUTOMATION_APPID" \
    --InroomEndpoint "$SPEECHSDK_PRINCETON_INROOM_ENDPOINT" \
    --OnlineEndpoint "$SPEECHSDK_PRINCETON_ONLINE_ENDPOINT" \
    --inputDir "$SPEECHSDK_INPUTDIR" \
    --keyBot "$SPEECHSDK_BOT_SUBSCRIPTION" \
    --regionIdBot "$SPEECHSDK_BOT_REGION" \
    --secretKeyBot "$SPEECHSDK_BOT_FUNCTIONALTESTBOT" \
    --keyConversationTranscriber "$SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_KEY"
