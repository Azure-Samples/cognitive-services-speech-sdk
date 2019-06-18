#!/usr/bin/env bash

set -e -u -o pipefail -x

T="$(basename "$0" .sh)"
BUILD_DIR=`realpath "$1"`
PLATFORM="$2"
BINARY_DIR="$3"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

## assumes that build_dir is one level deeper than source

isOneOf "$PLATFORM" iOS-x64-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

# testing for iOS requires a development team id for signing
MICROSOFT_DEVELOPMENT_TEAM_ID=UBF8T346G9
xcodebuild test -workspace ${BUILD_DIR}/../tests/functional/objective-c/ios/SpeechSDKSwift/SpeechSDKSwift.xcworkspace \
    -scheme SpeechSDKSwift \
    -destination 'platform=iOS Simulator,name=iPhone 8,OS=12.1' \
    DEVELOPMENT_TEAM=${MICROSOFT_DEVELOPMENT_TEAM_ID} \
    SPEECHSDK_SPEECH_KEY="$SPEECHSDK_SPEECH_KEY" SPEECHSDK_SPEECH_REGION="$SPEECHSDK_SPEECH_REGION" \
    SPEECHSDK_LUIS_KEY="$SPEECHSDK_LUIS_KEY" SPEECHSDK_LUIS_REGION="$SPEECHSDK_LUIS_REGION" \
    SPEECHSDK_LUIS_APP_ID="$SPEECHSDK_LUIS_HOMEAUTOMATION_APPID" 2>&1 |
        tee "${OUTPUTDIRECTORY}/logs/xcodebuild-ios-swift-unittests.log" |
        xcpretty --report junit --output test-ios-swift-unittests.xml

