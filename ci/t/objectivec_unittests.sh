#!/usr/bin/env bash
set -e -u -o pipefail

T="$(basename "$0" .sh)"
BUILD_DIR=`realpath "$1"`
PLATFORM="$2"
BINARY_DIR="$3"

TEST_SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$TEST_SCRIPT_DIR/../functions.sh" || exit 1

isOneOf "$PLATFORM" {iOS,OSX,macOS}-x64-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

source $TEST_SCRIPT_DIR/../test-harness.sh

testCommand=$TEST_SCRIPT_DIR/../apple/xcodebuild-wrapper.sh

function runObjcSuite {
  local usage testStateVarPrefix output platform redactStrings testsuiteName timeoutSeconds testCases objcExtraArgs
  usage="Usage: ${FUNCNAME[0]} <testStateVarPrefix> <output> <platform> <redactStrings> <testsuiteName> <timeoutSeconds>"
  testStateVarPrefix="${1?$usage}"
  output="${2?$usage}"
  platform="${3?$usage}"
  redactStrings="${4?$usage}"
  testsuiteName="${5?$usage}"
  timeoutSeconds="${6?$usage}"
  shift 6

  if [[ ${PLATFORM} == macOS-* ]]; then
    testCases=(
      "${BUILD_DIR}/../tests/functional/objective-c/osx/SpeechSDK.xcodeproj SpeechSDK ${testsuiteName}
        --extra-args CODE_SIGN_IDENTITY= --extra-args CODE_SIGNING_REQUIRED=NO" \
    )
    deviceSettings=()
  elif [[ ${PLATFORM} == iOS-* ]]; then
    MICROSOFT_DEVELOPMENT_TEAM_ID=UBF8T346G9
    testCases=(
    "${BUILD_DIR}/../tests/functional/objective-c/ios/SpeechSDK_iOS.xcodeproj SpeechSDK_iOS ${testsuiteName}
        --extra-args DEVELOPMENT_TEAM=${MICROSOFT_DEVELOPMENT_TEAM_ID}"   \
    )
    deviceSettings=(--device "iPhone 8" --os "12.4")
  fi

  startTests "$testStateVarPrefix" "$output" "$platform" "$redactStrings"
  startSuite "$testStateVarPrefix" "$testsuiteName"

  for testCase in "${testCases[@]}"; do
    runTest "$testStateVarPrefix" "$testCase" "$timeoutSeconds" \
      $testCommand $testCase "${deviceSettings[@]}" --redact "$redactStrings" || true
  done

  endSuite "$testStateVarPrefix"
  endTests "$testStateVarPrefix"
}

runObjcSuite \
  TESTRUNNER \
  test-objcunittests-${PLATFORM} \
  ${PLATFORM} \
  " $SPEECHSDK_SPEECH_KEY $SPEECHSDK_LUIS_KEY $SPEECHSDK_BOT_SUBSCRIPTION $SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PPE_KEY $SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PROD_KEY" \
  objcunittests-${PLATFORM} \
  420

