#!/usr/bin/env bash

set -e -u -o pipefail -x

T="$(basename "$0" .sh)"
BUILD_DIR=`realpath "$1"`
PLATFORM="$2"
BINARY_DIR="$3"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

## assumes that build_dir is one level deeper than source

isOneOf "$PLATFORM" {iOS,OSX,macOS}-x64-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

if [[ ${PLATFORM} == macOS-* ]]; then
    xcodebuild test -project ${BUILD_DIR}/../tests/functional/objective-c/osx/SpeechSDK.xcodeproj \
        -scheme SpeechSDK \
        CODE_SIGN_IDENTITY="" CODE_SIGNING_REQUIRED=NO \
        SPEECHSDK_SPEECH_KEY="$SPEECHSDK_SPEECH_KEY" SPEECHSDK_SPEECH_REGION="$SPEECHSDK_SPEECH_REGION" \
        SPEECHSDK_LUIS_KEY="$SPEECHSDK_LUIS_KEY" SPEECHSDK_LUIS_REGION="$SPEECHSDK_LUIS_REGION" 2>&1 |
            tee "${OUTPUTDIRECTORY}/logs/xcodebuild-osx-unittests.log" |
            xcpretty --report junit --output test-osx-unittests.xml
elif [[ ${PLATFORM} == iOS-* ]]; then
    # testing for iOS requires a development team id for signing
    MICROSOFT_DEVELOPMENT_TEAM_ID=UBF8T346G9
    xcodebuild test -project ${BUILD_DIR}/../tests/functional/objective-c/ios/SpeechSDK_iOS.xcodeproj \
        -scheme SpeechSDK_iOS \
        -destination 'platform=iOS Simulator,name=iPhone 8,OS=12.1' \
        DEVELOPMENT_TEAM=${MICROSOFT_DEVELOPMENT_TEAM_ID} \
        SPEECHSDK_SPEECH_KEY="$SPEECHSDK_SPEECH_KEY" SPEECHSDK_SPEECH_REGION="$SPEECHSDK_SPEECH_REGION" \
        SPEECHSDK_LUIS_KEY="$SPEECHSDK_LUIS_KEY" SPEECHSDK_LUIS_REGION="$SPEECHSDK_LUIS_REGION" 2>&1 |
            tee "${OUTPUTDIRECTORY}/logs/xcodebuild-ios-unittests.log" |
            xcpretty --report junit --output test-ios-unittests.xml
fi

