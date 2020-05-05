#!/usr/bin/env bash

# Test runner for Objective-C/Swift tests to be used with test-harness.sh

set -e -u -o pipefail

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
XCODE_CONFIGURATION_BUILD_DIR=${PWD}/iostestbuild

USAGE="USAGE: objectivec-test.sh <xcodeproject> <xcodescheme> <testname> <opt:--extra-args xcode_extra_args> <opt:--redact strings_to_redact>"
PROJECT="${1?$USAGE}"  # Xcode project file
SCHEME="${2?$USAGE}"  # Xcode scheme name
TESTNAME="${3?$USAGE}"  # used for naming the logfile
shift 3

source $SCRIPT_DIR/../functions.sh

xcodeExtraArgs=()
redactStrings=
device=
sim_os=

inargs=("$@")
while [[ ${#inargs[@]} > 0 ]]
do
  key="${inargs[0]}"
  case $key in
  --extra-args)
    [[ -v inargs[1] ]] ||
        exitWithError "Error: expected argument for %s option\n" "$key"
    xcodeExtraArgs+=("${inargs[1]}")
    inargs=("${inargs[@]:1}")
    ;;
  --redact)
    [[ -v inargs[1] ]] ||
        exitWithError "Error: expected argument for %s option\n" "$key"
    redactStrings="${inargs[1]}"
    inargs=("${inargs[@]:1}")
    ;;
  --device)
    [[ -v inargs[1] ]] ||
        exitWithError "Error: expected argument for %s option\n" "$key"
    device="${inargs[1]}"
    inargs=("${inargs[@]:1}")
    ;;
  --os)
    [[ -v inargs[1] ]] ||
        exitWithError "Error: expected argument for %s option\n" "$key"
    sim_os="${inargs[1]}"
    inargs=("${inargs[@]:1}")
    ;;
  *)
    exitWithError "Error: unrecognized option '%s'\n" "$key"
    ;;
  esac
  inargs=("${inargs[@]:1}") # past argument or value
done

destinationSpec=()
if [[ -n $device ]] && [[ -n $sim_os ]]; then
  destinationSpec=("-destination" "platform=iOS Simulator,name=$device,OS=$sim_os")
fi

case $PROJECT in
  *"xcodeproj")
    projectArg=(-project ${PROJECT})
    ;;
  *"xcworkspace"*)
    projectArg=(-workspace ${PROJECT})
    ;;
    *) exitWithError "invalid project definition" "${PROJECT}"
    ;;
esac

function redact {
  # N.B. receiving stdin as first command in function.
  perl -MIO::Handle -lpe 'BEGIN { STDOUT->autoflush(1); STDERR->autoflush(1); if (@ARGV) { $re = sprintf "(?:%s)", (join "|", map { quotemeta $_ } splice @ARGV); $re = qr/$re/ } } $re and s/$re/***/gi' $@
}

runXcodeTest() {
  local i=0
  . ./ci/functions.sh

  while [[ -f "${OUTPUTDIRECTORY}/logs/${TESTNAME}-${i}.log" ]]; do
    i=$(( i + 1 ))
  done
  xcodebuild test "${projectArg[@]}" \
    -scheme ${SCHEME} "${xcodeExtraArgs[@]}" "${destinationSpec[@]}" \
    SPEECHSDK_SPEECH_KEY="$SPEECHSDK_SPEECH_KEY" SPEECHSDK_SPEECH_REGION="$SPEECHSDK_SPEECH_REGION" \
    SPEECHSDK_LUIS_KEY="$SPEECHSDK_LUIS_KEY" SPEECHSDK_LUIS_REGION="$SPEECHSDK_LUIS_REGION" 2>&1 |
      redact "$redactStrings" |
      tee "${OUTPUTDIRECTORY}/logs/${TESTNAME}-${i}.log" |
      xcpretty --report junit --output test-${TESTNAME}-${i}.xml
}

runXcodeTest

