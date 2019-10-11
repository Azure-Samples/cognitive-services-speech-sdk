#!/usr/bin/env bash
T="$(basename "$0" .sh)"
BUILD_DIR="$1"
PLATFORM="$2"
BINARY_DIR="$3"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

isOneOf "$PLATFORM" {Windows,Linux,OSX}-x64-Release ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

TEST_CODE="$BUILD_DIR/bin/com.microsoft.cognitiveservices.speech.tests.jar"
[[ -f $TEST_CODE ]] ||
  exitWithSuccess "Test %s: skip, no test code\n" "$T"

TESTCLASS=tests.AllTests

if [[ $OS == "Windows_NT" ]]; then
  CPSEP=\;
  PATH="$PATH:$BINARY_DIR"
else
  CPSEP=:
fi

SPEECH_SDK_EXTERNAL="$SCRIPT_DIR/../../external"

# deactivated because libraries need to be signed before testing. To run this
# test e.g. on a locally built .jar, remove the NativeLibraryLoader.class file
# from the .jar before running the tests.

set -x
# java \
#   -Djava.library.path="$BINARY_DIR" \
#   -cp $(printf "%s$CPSEP" \
#     "$BUILD_DIR/lib/com.microsoft.cognitiveservices.speech.jar" \
#     "$TEST_CODE" \
#     "$SPEECH_SDK_EXTERNAL/junit/junit-4.12.jar" \
#     "$SPEECH_SDK_EXTERNAL/junit/hamcrest-core-1.3.jar" \
#     "$SPEECH_SDK_EXTERNAL/json/javax.json-api-1.0.jar" \
#     "$SPEECH_SDK_EXTERNAL/json/javax.json-1.1.jar" ) \ 
#   -DSpeechSubscriptionKey=$SPEECHSDK_SPEECH_KEY \
#   -DSpeechRegion=$SPEECHSDK_SPEECH_REGION \
#   -DLuisRegion=$SPEECHSDK_LUIS_REGION \
#   -DWaveFile=$SPEECHSDK_INPUTDIR/audio/whatstheweatherlike.wav \
#   -DKeyword=Computer \
#   -DKeywordModel=/data/keyword/kws.table \
#   -DTestOutputFilename=test-$T-$PLATFORM-$RANDOM.xml \
#   tests.runner.Runner \
#   $TESTCLASS
