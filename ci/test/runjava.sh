#!/bin/bash

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

# runjava.sh ${BUILD_REPOSITORY_LOCALPATH} ./ $(BuildConfiguration) $(KeySpeech) $(KeyCris) $(KeyLuis) $(KeySkyman)
[[ $# -eq 7 ]] || {
  echo -e "Usage: $0 root_dir build_root build_configuration keySpeech keyCris keyLuis keySkyman"
  exit 1
}

SPEECH_SDK_SOURCE_ROOT=$1
BINARY_DIR=$2
BUILD_CONFIGURATION=$3
UserKeySpeech=$4
UserKeyCris=$5
UserKeyLuis=$6
UserKeySkyman=$7

set -e

# Get some helpers
. "$SCRIPT_DIR/../functions.sh"

pretty_print "ENTERING runjava.sh"

which java
pwd
ls

pretty_print "..SPEECH_SDK...................................................................................."

SPEECH_SDK_BUILD_ROOT=.
SPEECH_SDK_EXTERNAL=$SPEECH_SDK_SOURCE_ROOT/external

if [[ $OS == "Windows_NT" ]]; then
  CPSEP=\;
  PATH="$PATH:$SPEECH_SDK_BUILD_ROOT/bin/$BUILD_CONFIGURATION"
  TESTCLASS=tests.AllTests
else
  CPSEP=:
  # TODO add more when passing:
  TESTCLASS=tests.unit.ParameterCollectionTests
fi

print_vars = SPEECH_SDK_BUILD_ROOT SPEECH_SDK_EXTERNAL TESTCLASS =

ls -l $SPEECH_SDK_BUILD_ROOT/lib/com.microsoft.cognitiveservices.speech.jar
ls -l $SPEECH_SDK_BUILD_ROOT/bin/com.microsoft.cognitiveservices.speech.tests.jar
ls -l $SPEECH_SDK_EXTERNAL/junit/junit-4.12.jar
ls -l $SPEECH_SDK_EXTERNAL/junit/hamcrest-core-1.3.jar

pretty_print "..root...................................................................................."

ls ../..

pretty_print "..junit...................................................................................."

SPEECH_SDK_TEST_PROPERTIES=" \
  -DSpeechSubscriptionKey=$UserKeySpeech \
  -DSpeechRegion=westus \
  -DLuisRegion=westus2 \
  -DWaveFile=$SPEECH_SDK_BUILD_ROOT/tests/input/whatstheweatherlike.wav \
  -DKeyword=Computer \
  -DKeywordModel=/data/keyword/kws.table \
  -DTestOutputFilename=test-java-unittests.xml"

set -x
java \
  -Djava.library.path=$SPEECH_SDK_BUILD_ROOT/bin/$BUILD_CONFIGURATION \
  -cp $(printf "%s$CPSEP" \
    "$SPEECH_SDK_BUILD_ROOT/lib/com.microsoft.cognitiveservices.speech.jar" \
    "$SPEECH_SDK_BUILD_ROOT/bin/com.microsoft.cognitiveservices.speech.tests.jar" \
    "$SPEECH_SDK_EXTERNAL/junit/junit-4.12.jar" \
    "$SPEECH_SDK_EXTERNAL/junit/hamcrest-core-1.3.jar" ) \
  $SPEECH_SDK_TEST_PROPERTIES \
  tests.runner.Runner \
  $TESTCLASS

pretty_print "LEAVING runjava.sh"
