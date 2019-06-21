#!/usr/bin/env bash

set -e -u -o pipefail

T="$(basename "$0" .sh)"
BUILD_DIR=`realpath "$1"`
PLATFORM="$2"
BINARY_DIR="$3"

TEST_SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$TEST_SCRIPT_DIR/../functions.sh" || exit 1

## assumes that build_dir is one level deeper than source

isOneOf "$PLATFORM" iOS-x64-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

source $TEST_SCRIPT_DIR/../test-harness.sh

testCommand=$TEST_SCRIPT_DIR/../apple/xcodebuild-wrapper.sh

function runSwiftSuite {
  local usage testStateVarPrefix output platform redactStrings testsuiteName timeoutSeconds testCases objcExtraArgs
  usage="Usage: ${FUNCNAME[0]} <testStateVarPrefix> <output> <platform> <redactStrings> <testsuiteName> <timeoutSeconds>"
  testStateVarPrefix="${1?$usage}"
  output="${2?$usage}"
  platform="${3?$usage}"
  redactStrings="${4?$usage}"
  testsuiteName="${5?$usage}"
  timeoutSeconds="${6?$usage}"
  shift 6

  MICROSOFT_DEVELOPMENT_TEAM_ID=UBF8T346G9
  testCases=(
  "${BUILD_DIR}/../tests/functional/objective-c/ios/SpeechSDKSwift/SpeechSDKSwift.xcworkspace SpeechSDKSwift ${testsuiteName}
      --extra-args DEVELOPMENT_TEAM=${MICROSOFT_DEVELOPMENT_TEAM_ID} \
      --extra-args SPEECHSDK_LUIS_APP_ID=${SPEECHSDK_LUIS_HOMEAUTOMATION_APPID}"
  )
  deviceSettings=(--device "iPhone 8" --os "12.1")

  startTests "$testStateVarPrefix" "$output" "$platform" "$redactStrings"
  startSuite "$testStateVarPrefix" "$testsuiteName"

  for testCase in "${testCases[@]}"; do
    runTest "$testStateVarPrefix" "$testCase" "$timeoutSeconds" \
      $testCommand $testCase "${deviceSettings[@]}" --redact "$redactStrings" || true
  done

  endSuite "$testStateVarPrefix"
  endTests "$testStateVarPrefix"
}

runSwiftSuite \
  TESTRUNNER \
  swiftunittests-${PLATFORM} \
  ${PLATFORM} \
  " $SPEECHSDK_SPEECH_KEY $SPEECHSDK_LUIS_KEY $SPEECHSDK_BOT_SUBSCRIPTION $SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PPE_KEY $SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PROD_KEY" \
  swiftunittests-${PLATFORM} \
  150

