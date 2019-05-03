#!/usr/bin/env bash
T="$(basename "$0" .sh)"
BUILD_DIR="$1"
PLATFORM="$2"
BINARY_DIR="$3"
TESTSET="${4:-dev}"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

isOneOf "$PLATFORM" Windows-{x86,x64}-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

TEST_CODE="$BINARY_DIR/carbon_csharp_console"
[[ -x $TEST_CODE ]] ||
  exitWithSuccess "Test %s: skip, no test code\n" "$T"

. "$SCRIPT_DIR/../test-harness.sh" || exit 1

speechEndpoint="$(speechWebSocketsEndpoint "$SPEECHSDK_SPEECH_REGION" interactive simple en-us)"
crisEndpoint="$(crisWebSocketsEndpoint "$SPEECHSDK_SPEECH_REGION" interactive "$SPEECHSDK_SPEECH_ENDPOINTID_ENUS")"
audioFile="$SPEECHSDK_INPUTDIR/audio/whatstheweatherlike.wav"

# TODO: skip auth token tests if this step fails
TOKEN="$(getAuthorizationToken "$SPEECHSDK_SPEECH_KEY" "$SPEECHSDK_SPEECH_REGION")" ||
  die "Error: could not obtain authorization token"

actions=(speech intent translation)

# Using array of pairs for deterministic test order

variants=(
  baseModel "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_SPEECH_REGION $audioFile"
)

if [[ $TESTSET != dev ]]; then
  # Additional tests for prod and int builds.
  actions+=(intent translation)

  variants+=(
    crisModel "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_SPEECH_REGION $audioFile model:$SPEECHSDK_SPEECH_ENDPOINTID_ENUS"
    speechEndpoint "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_SPEECH_REGION $audioFile endpoint:"$speechEndpoint""
    crisEndpoint "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_SPEECH_REGION $audioFile endpoint:"$crisEndpoint""
    baseModelWithStream "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_SPEECH_REGION stream:$audioFile"
  )
fi

startTests TESTRUNNER "test-$T-$PLATFORM" "$PLATFORM" "$SPEECHSDK_SPEECH_KEY $TOKEN"
startSuite TESTRUNNER "$T"

TIMEOUT_SECONDS=30

for action in "${actions[@]}"; do
  for ((variantIndex = 0; variantIndex < ${#variants[@]}; variantIndex += 2)); do
    variant="${variants[$variantIndex]}"
    variantArg="${variants[$variantIndex + 1]}"
    TEST_NAME="$action $variant"

    # TODO intent should use different subscriptions / regions / endpoints
    [[ $action != intent ]] || continue

    runTest TESTRUNNER "$TEST_NAME" $TIMEOUT_SECONDS \
      "$TEST_CODE" $action $variantArg
  done

  if [[ $action == speech ]]; then
    runTest TESTRUNNER "$action authTokenWithBaseModel" $TIMEOUT_SECONDS \
      "$TEST_CODE" $action token:$TOKEN $SPEECHSDK_SPEECH_REGION $audioFile
  fi

done

endSuite TESTRUNNER
endTests TESTRUNNER
