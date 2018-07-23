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

CARBON_CSHARP_CONSOLE=$BINARY_DIR/carbon_csharp_console

# Expand actions if all is specified
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
TOKEN="$(getAuthorizationToken "$UserKeySkyman" westus)" ||
  die "Error: could not obtain authorization token"

# Using array of pairs for deterministic test order

variants=(
  "baseModel"               "$UserKeySkyman $TEST_AUDIO_FILE"
  "crisModel"               "$UserKeySkyman $TEST_AUDIO_FILE model:$TEST_MODEL_ID"
  "speechEndpoint"          "$UserKeySkyman $TEST_AUDIO_FILE endpoint:"$TEST_SPEECH_ENDPOINT""
  "crisEndpoint"            "$UserKeySkyman $TEST_AUDIO_FILE endpoint:"$TEST_CRIS_ENDPOINT""
  "baseModelWithStream"     "$UserKeySkyman stream:$TEST_AUDIO_FILE"
)

PLATFORMS_TO_RUN="$(joinArgs , Windows-{x86,x64}-Release)"

PLATFORM=$SPEECHSDK_TARGET_PLATFORM-$SPEECHSDK_BUILD_CONFIGURATION

startTests TESTRUNNER TEST-carbon_csharp_console "$PLATFORM" "$UserKeySkyman $TOKEN"
startSuite TESTRUNNER "$(basename "$CARBON_CSHARP_CONSOLE" .exe)"

TIMEOUT_SECONDS=30

for action in $Actions; do
  for ((variantIndex = 0; variantIndex < ${#variants[@]}; variantIndex += 2)); do
    variant="${variants[$variantIndex]}"
    variantArg="${variants[$variantIndex + 1]}"
    TEST_NAME="$action $variant"

    runTest TESTRUNNER "$TEST_NAME" "$PLATFORMS_TO_RUN" $TIMEOUT_SECONDS \
      $CARBON_CSHARP_CONSOLE $action $variantArg
  done

  if [[ $action == speech ]]; then
    runTest TESTRUNNER "$action authTokenWithBaseModel" "$PLATFORMS_TO_RUN" $TIMEOUT_SECONDS \
      $CARBON_CSHARP_CONSOLE $action token:$TOKEN $TEST_AUDIO_FILE
  fi

done

endSuite TESTRUNNER
endTests TESTRUNNER
