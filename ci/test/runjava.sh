#!/bin/bash

# runjava.sh ${BUILD_REPOSITORY_LOCALPATH} ./ $(BuildConfiguration) $(KeySpeech) $(KeyCris) $(KeyLuis) $(KeySkyman)
[ $# -ne 7 ] && echo -e "Usage: runjava root_dir build_root build_configuration keySpeech keyCris keyLuis keySkyman" && exit 1

SPEECH_SDK_SOURCE_ROOT=$1
BINARY_DIR=$2
BUILD_CONFIGURATION=$3
UserKeySpeech=$4
UserKeyCris=$5
UserKeyLuis=$6
UserKeySkyman=$7

set -e

pretty_print() {
  printf '%.0s=' {1..78}; echo
  echo $1
  printf '%.0s=' {1..78}; echo
}

pretty_print "ENTERING runjava.sh"

which java
pwd
ls

pretty_print "..SPEECH_SDK...................................................................................."

SPEECH_SDK_BUILD_ROOT=.
SPEECH_SDK_EXTERNAL=$SPEECH_SDK_SOURCE_ROOT/external
PATH="$PATH;$SPEECH_SDK_BUILD_ROOT/bin/$BUILD_CONFIGURATION"

pretty_print "SPEECH_SDK_BUILD_ROOT $SPEECH_SDK_BUILD_ROOT"
pretty_print "SPEECH_SDK_EXTERNAL $SPEECH_SDK_EXTERNAL"
pretty_print "LD_LIBRARY_PATH $LD_LIBRARY_PATH"


ls -l $SPEECH_SDK_BUILD_ROOT/lib/com.microsoft.cognitiveservices.speech.jar
ls -l $SPEECH_SDK_BUILD_ROOT/bin/com.microsoft.cognitiveservices.speech.tests.jar
ls -l $SPEECH_SDK_EXTERNAL/junit/junit-4.12.jar
ls -l $SPEECH_SDK_EXTERNAL/junit/hamcrest-core-1.3.jar

pretty_print "..root...................................................................................."

ls ../..


pretty_print "..junit...................................................................................."

SPEECH_SDK_TEST_PROPERTIES="-DSpeechSubscriptionKey=$UserKeySpeech -DSpeechRegion=westus -DLuisRegion=westus2 -DWaveFile=$SPEECH_SDK_BUILD_ROOT/tests/input/whatstheweatherlike.wav -DKeyword=Computer -DKeywordModel=/data/keyword/kws.table"

java -cp "$SPEECH_SDK_BUILD_ROOT/lib/com.microsoft.cognitiveservices.speech.jar;$SPEECH_SDK_BUILD_ROOT/bin/com.microsoft.cognitiveservices.speech.tests.jar;$SPEECH_SDK_EXTERNAL/junit/junit-4.12.jar;$SPEECH_SDK_EXTERNAL/junit/hamcrest-core-1.3.jar" $SPEECH_SDK_TEST_PROPERTIES org.junit.runner.JUnitCore tests.AllTests

pretty_print "LEAVING runjava.sh"
