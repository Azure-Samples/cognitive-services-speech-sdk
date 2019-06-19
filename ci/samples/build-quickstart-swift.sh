#!/usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
set -u -e -o pipefail

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SOURCE_ROOT="$SCRIPT_DIR/../.."

USAGE="Usage: $0 framework-dir logdir"
FRAMEWORK_DIR="${1?$USAGE}"
LOGDIR="${2?$USAGE}"  # path to the directory to store logs in

PROJECT_DIR=public_samples/quickstart/swift-ios/helloworld/
SCHEME=helloworld
XCODE_CONFIGURATION_BUILD_DIR=${PWD}/iosswiftqsbuild
mkdir -p ${XCODE_CONFIGURATION_BUILD_DIR}

sed -i '' "s/'~> 1.[[:digit:]]'/path: \"{{{PODSPEC_PATH}}}\"/" ${PROJECT_DIR}/Podfile
ci/apple/prepare-pods-for-swift-test.sh ${PROJECT_DIR} "$FRAMEWORK_DIR"

xcodebuild build -workspace ${PROJECT_DIR}/helloworld.xcworkspace \
    -destination "platform=iOS Simulator,name=iPhone X,OS=latest" \
    -scheme ${SCHEME} \
    CONFIGURATION_BUILD_DIR=${XCODE_CONFIGURATION_BUILD_DIR} \
    SPEECHSDK_SPEECH_KEY="$SPEECHSDK_SPEECH_KEY" SPEECHSDK_SPEECH_REGION="$SPEECHSDK_SPEECH_REGION" \
    SPEECHSDK_LUIS_KEY="$SPEECHSDK_LUIS_KEY" SPEECHSDK_LUIS_REGION="$SPEECHSDK_LUIS_REGION" 2>&1 |
      tee "${LOGDIR}/xcodebuild-ios-swift-quickstart.log" |
      xcpretty --report junit --output xcodebuild-ios-swift-quickstart.xml

