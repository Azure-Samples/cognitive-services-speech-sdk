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
Usage: $SCRIPT_NAME binary_dir action(all|speech|intent) keySkyman

Expected environment variables:
  - \$TEST_AUDIO_FILE - audio input file
  - \$TEST_MODEL_ID - CRIS Model ID
  - \$TEST_SPEECH_ENDPOINT - speech endpoint
  - \$TEST_CRIS_ENDPOINT - CRIS endpoint
"

[[ $# -eq 3 ]] || die "Error: wrong number of arguments.\n$USAGE"
[[ -n $TEST_AUDIO_FILE ]] || die "Error: \$TEST_AUDIO_FILE not set.\n$USAGE"
[[ -n $TEST_MODEL_ID ]] || die "Error: \$TEST_MODEL_ID not set.\n$USAGE"
[[ -n $TEST_SPEECH_ENDPOINT ]] || die "Error: \$TEST_SPEECH_ENDPOINT not set.\n$USAGE"
[[ -n $TEST_CRIS_ENDPOINT ]] || die "Error: \$TEST_CRIS_ENDPOINT not set.\n$USAGE"

BINARY_DIR=$1
Action=$2
UserKeySkyman=$3

CARBONX=$BINARY_DIR/carbonx

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
  "crisModel"       "--customSpeechModelId:$TEST_MODEL_ID"
  "speechEndpoint"  "--endpoint:$TEST_SPEECH_ENDPOINT"
  "crisEndpoint"    "--endpoint:$TEST_CRIS_ENDPOINT"
)

PLATFORMS_TO_RUN="$(joinArgs , {OSX-x64,Linux-x64,Windows-{x86,x64}}-{Debug,Release})"

PLATFORM=$SPEECHSDK_TARGET_PLATFORM-$SPEECHSDK_BUILD_CONFIGURATION

startTests TESTRUNNER test-carbonx "$PLATFORM" "$UserKeySkyman"

startSuite TESTRUNNER "$(basename "$CARBONX" .exe)"

# The continuous tests are sampling for 10 seconds. Waiting for 30 seconds should be enough.
TIMEOUT_SECONDS=30

for action in $Actions; do
  for ((modeIndex = 0; modeIndex < ${#modes[@]}; modeIndex += 2)); do
    mode="${modes[$modeIndex]}"
    modeArg="${modes[$modeIndex + 1]}"
    for ((targetIndex = 0; targetIndex < ${#targets[@]}; targetIndex += 2)); do
      target="${targets[$targetIndex]}"
      targetArg="${targets[$targetIndex + 1]}"
      TEST_NAME="$action $mode $target"

      # Disabling cris intent test due to flakiness
      # https://msasg.visualstudio.com/DefaultCollection/Skyman/_workitems/edit/1388581
      if [[ "$TEST_NAME" = "intent default crisEndpoint" ]]; then
        continue
      fi

      runTest TESTRUNNER "$TEST_NAME" "$PLATFORMS_TO_RUN" $TIMEOUT_SECONDS \
        $CARBONX --subscription:$UserKeySkyman --input $TEST_AUDIO_FILE --$action $modeArg $targetArg
    done
  done
done

endSuite TESTRUNNER
endTests TESTRUNNER
