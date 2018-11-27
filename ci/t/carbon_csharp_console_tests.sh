#!/usr/bin/env bash
T="$(basename "$0" .sh)"
BUILD_DIR="$1"
PLATFORM="$2"
BINARY_DIR="$3"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

isOneOf "$PLATFORM" Windows-{x86,x64}-Release ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

TEST_CODE="$BINARY_DIR/carbon_csharp_console"
[[ -x $TEST_CODE ]] ||
  exitWithSuccess "Test %s: skip, no test code\n" "$T"

. "$SCRIPT_DIR/../test-harness.sh" || exit 1

speechEndpoint="$(speechWebSocketsEndpoint "$SPEECHSDK_SPEECH_REGION" interactive simple en-us)"
crisEndpoint="$(crisWebSocketsEndpoint "$SPEECHSDK_SPEECH_REGION" interactive "$SPEECHSDK_SPEECH_ENDPOINTID_ENUS")"
audioFile="$SPEECHSDK_INPUTDIR/audio/whatstheweatherlike.wav"

# Expand actions if all is specified
Action=all
if [[ $Action == all ]]; then
  Actions="speech intent translation"
else
  Actions="$Action"
fi

# Validate actions
for action in $Actions; do
  case $action in
    speech|intent|translation)
      ;;
    *)
      echo Unknown action: $action 1>&2
      exit 1
  esac
done

# TODO: skip auth token tests if this step fails
TOKEN="$(getAuthorizationToken "$SPEECHSDK_SPEECH_KEY" "$SPEECHSDK_SPEECH_REGION")" ||
  die "Error: could not obtain authorization token"

# Using array of pairs for deterministic test order

variants=(
  "baseModel"               "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_SPEECH_REGION $audioFile"
  "crisModel"               "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_SPEECH_REGION $audioFile model:$SPEECHSDK_SPEECH_ENDPOINTID_ENUS"
  "speechEndpoint"          "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_SPEECH_REGION $audioFile endpoint:"$speechEndpoint""
  "crisEndpoint"            "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_SPEECH_REGION $audioFile endpoint:"$crisEndpoint""
  "baseModelWithStream"     "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_SPEECH_REGION stream:$audioFile"
)

startTests TESTRUNNER "test-$T-$PLATFORM" "$PLATFORM" "$SPEECHSDK_SPEECH_KEY $TOKEN"
startSuite TESTRUNNER "$T"

TIMEOUT_SECONDS=30

for action in $Actions; do
  for ((variantIndex = 0; variantIndex < ${#variants[@]}; variantIndex += 2)); do
    variant="${variants[$variantIndex]}"
    variantArg="${variants[$variantIndex + 1]}"
    TEST_NAME="$action $variant"

    # Filter out unsupported combos
    if [[ ($action == translation && $variant == crisModel) ||
          ($action == intent && ($variant == crisModel || $variant == base*)) ]]; then
      continue
    fi

    # TODO intent should use different subscriptions / regions / endpoints

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
