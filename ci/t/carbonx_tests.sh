#!/usr/bin/env bash
T="$(basename "$0" .sh)"
BUILD_DIR="$1"
PLATFORM="$2"
BINARY_DIR="$3"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

isOneOf "$PLATFORM" {{Windows,Linux}-{x86,x64},Linux-arm32,OSX-x64}-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

TEST_CODE="$BINARY_DIR/carbonx"
[[ -x $TEST_CODE ]] ||
  exitWithSuccess "Test %s: skip, no test code\n" "$T"

. "$SCRIPT_DIR/../test-harness.sh" || exit 1

speechEndpoint="$(speechWebSocketsEndpoint "$SPEECHSDK_SPEECH_REGION" interactive simple en-us)"
crisEndpoint="$(crisWebSocketsEndpoint "$SPEECHSDK_SPEECH_REGION" interactive "$SPEECHSDK_SPEECH_ENDPOINTID_ENUS")"

# TODO
Action=all

# Expand actions if all is specified
if [[ $Action == all ]]; then
  Actions="speech intent"
else
  Actions="$Action"
fi

# Validate actions
for action in $Actions; do
  case $action in
    speech|intent)
      ;;
    *)
      echo Unknown action: $action 1>&2
      exit 1
  esac
done

# Using array of pairs for deterministic test order

modes=(
  "default"        ""
  "single"         "--single"
  "continuous:10"  "--continuous:10"
)

targets=(
  "baseModel"       ""
  "crisModel"       "--customSpeechModelId:$SPEECHSDK_SPEECH_ENDPOINTID_ENUS"
  "speechEndpoint"  "--endpoint:$speechEndpoint"
  "crisEndpoint"    "--endpoint:$crisEndpoint"
)

startTests TESTRUNNER "test-$T-$PLATFORM" "$PLATFORM" "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_LUIS_KEY"

startSuite TESTRUNNER "$T"

# The continuous tests are sampling for 10 seconds. Waiting for 30 seconds should be enough.
TIMEOUT_SECONDS=30

for action in $Actions; do
  # Intent disabled due to service issues.
  # https://msasg.visualstudio.com/Skyman/_workitems/edit/1550174
  [[ $action != intent ]] || continue

  for ((modeIndex = 0; modeIndex < ${#modes[@]}; modeIndex += 2)); do
    mode="${modes[$modeIndex]}"
    modeArg="${modes[$modeIndex + 1]}"
    for ((targetIndex = 0; targetIndex < ${#targets[@]}; targetIndex += 2)); do
      target="${targets[$targetIndex]}"
      targetArg="${targets[$targetIndex + 1]}"
      TEST_NAME="$action $mode $target"

      if [[ $action == intent ]]; then

        # Only do intent for base model
        if [[ $target != baseModel ]]; then
          continue
        fi

        EXTRA_ARGS="--subscription:$SPEECHSDK_LUIS_KEY --region $SPEECHSDK_LUIS_REGION --input $SPEECHSDK_INPUTDIR/audio/TurnOnTheLamp.wav --intentNames HomeAutomation.TurnOn --intentAppId $SPEECHSDK_LUIS_HOMEAUTOMATION_APPID"
      else
        EXTRA_ARGS="--subscription:$SPEECHSDK_SPEECH_KEY --region $SPEECHSDK_SPEECH_REGION --input $SPEECHSDK_INPUTDIR/audio/whatstheweatherlike.wav"
      fi

      runTest TESTRUNNER "$TEST_NAME" $TIMEOUT_SECONDS \
        "$TEST_CODE" $EXTRA_ARGS --$action $modeArg $targetArg
    done
  done
done

endSuite TESTRUNNER
endTests TESTRUNNER
