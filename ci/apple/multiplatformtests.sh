#!/usr/bin/env bash
set -e -u -o pipefail

APPLE_SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$APPLE_SCRIPT_DIR"/..

USAGE="USAGE: multiplatformtests.sh <projectfile> <xcodescheme> <appname> <logdir> <testname> <devteamid> <opt:--usegui>"
PROJECT="${1?$USAGE}"  # Xcode project file
SCHEME="${2?$USAGE}"  # Xcode scheme name
APPNAME="${3?$USAGE}"  # name of the app (without .app extension)
LOGDIR="${4?$USAGE}"  # path to the directory to store logs in
TESTNAME="${5?$USAGE}"  # name of the kind of test (unittest/quickstart) to use for naming the logfile
DEVTEAMID="${6?$USAGE}"  # the developer team id to use for signing the test app
USEGUI=
if [[ ${7:-} = --usegui ]]; then
  USEGUI=$7
  shift
fi

# make sure the simulator is not running.
if [[ -n ${USEGUI} ]]; then
    osascript -e 'quit app "Simulator"'
fi

test_command=($APPLE_SCRIPT_DIR/xcodeunittests.sh $PROJECT $SCHEME $APPNAME $LOGDIR $TESTNAME)

source $SCRIPT_DIR/test-harness.sh

function runXcodeSuite {
  local usage testStateVarPrefix output platform redactStrings testsuiteName timeoutSeconds testCases xcodeExtraArgs
  usage="Usage: ${FUNCNAME[0]} <testStateVarPrefix> <output> <platform> <redactStrings> <testsuiteName> <timeoutSeconds> <opt:--extra-args xcode_extra_args>"
  testStateVarPrefix="${1?$usage}"
  output="${2?$usage}"
  platform="${3?$usage}"
  redactStrings="${4?$usage}"
  testsuiteName="${5?$usage}"
  timeoutSeconds="${6?$usage}"
  shift 6

  xcodeExtraArgs=()
  while [[ $# > 0 ]]
  do
    key="$1"
    case $key in
    --extra-args)
      [[ -n $2 ]] ||
        exitWithError "Error: expected argument for %s option\n" "$key"
      xcodeExtraArgs+=("$2")
      shift
      ;;
    *)
      exitWithError "Error: unrecognized option '%s'\n" "$key"
      ;;
    esac
    shift # past argument or value
  done

  xcodeExtraArgs=( ${xcodeExtraArgs[@]/#/"--extra-args "} )

  testCases=(
    "iPhone-X iOS-12-2"
    "iPhone-6 iOS-9-3"
  )

  # deactivated test cases:
  # iPhone-8 iOS-11-4  # not installed in Xcode 10.1 toolchain
  # iPhone-6 iOS-10-2  # simulator times out, unrelated to test suite
  # iPhone-5 iOS-10-2  # simulator times out, unrelated to test suite
  # these simulators have been too unstable to run the tests reliably:
  # "iPhone-5 iOS-9-2 --extra-args ARCHS=\"i386\" --extra-args IPHONEOS_DEPLOYMENT_TARGET=9.2"
  # "iPhone-5 iOS-9-3 --extra-args ARCHS=\"i386\" --extra-args IPHONEOS_DEPLOYMENT_TARGET=9.2"
  # "iPhone-6s iOS-10-0"
  # "iPhone-5 iOS-10-2"
  # "iPhone-6 iOS-10-2"
  # "iPhone-6 iOS-9-2"

  testCases=( "${testCases[@]/%/" ${xcodeExtraArgs[@]}"}" )

  startTests "$testStateVarPrefix" "$output" "$platform" "$redactStrings"
  startSuite "$testStateVarPrefix" "$testsuiteName"

  # Remove individual catch output files, ignoring errors
  rm -f "catch$output-"*.{xml,txt}

  for testCase in "${testCases[@]}"; do
    runTest "$testStateVarPrefix" "$testCase" "$timeoutSeconds" \
      "${test_command[@]}" $testCase ${USEGUI} --redact "$redactStrings" || true
  done

  endSuite "$testStateVarPrefix"
  endTests "$testStateVarPrefix"
}

runXcodeSuite \
  TESTRUNNER \
  test-${TESTNAME} \
  iOS \
  "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_LUIS_KEY $SPEECHSDK_BOT_SUBSCRIPTION $SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PPE_KEY $SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PROD_KEY" \
  ${TESTNAME} \
  240 \
  --extra-args DEVELOPMENT_TEAM=${DEVTEAMID}

