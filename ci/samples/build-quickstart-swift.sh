#!/usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
set -u -e -o pipefail

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SOURCE_ROOT="$SCRIPT_DIR/../.."

USAGE="Usage: $0 project-dir framework-dir logdir testname build-platform"
PROJECT_DIR="${1?$USAGE}"  # directory containing the quickstart Xcode project and the Podfile
FRAMEWORK_DIR="${2?$USAGE}"  # where to install the framework from
LOGDIR="${3?$USAGE}"  # path to the directory to store logs in
TESTNAME="${4?$USAGE}"  # the name to identify the test in the log filenames
BUILD_PLATFORM="${5?$USAGE}"  # the platform to build for (iOS/macOS)

QUICKSTART_NAME=helloworld
XCODE_CONFIGURATION_BUILD_DIR=${PWD}/swiftqsbuild
mkdir -p ${XCODE_CONFIGURATION_BUILD_DIR}

sed -i '' -E "s/'~> 1.[[:digit:]]\.?[[:digit:]]?'/path: \"{{{PODSPEC_PATH}}}\"/" ${PROJECT_DIR}/Podfile
ci/apple/prepare-pods-for-swift-test.sh ${PROJECT_DIR} "$FRAMEWORK_DIR"

case ${BUILD_PLATFORM} in
iOS)
    extra_args=(-destination "platform=iOS Simulator,name=iPhone X,OS=latest")
    ;;
macOS)
    extra_args=(CODE_SIGN_IDENTITY="-")
    ;;
*)
    echo "Error: unrecognized build platform '%s'\n" "$BUILD_PLATFORM"
    exit -1
    ;;
esac

. ./ci/functions.sh
cp ./ci/jsonsettings.py .

xcodebuild build -workspace ${PROJECT_DIR}/${QUICKSTART_NAME}.xcworkspace \
    -scheme ${QUICKSTART_NAME} \
    "${extra_args[@]}" \
    CONFIGURATION_BUILD_DIR=${XCODE_CONFIGURATION_BUILD_DIR} \
    SPEECHSDK_SPEECH_KEY="$SPEECHSDK_SPEECH_KEY" SPEECHSDK_SPEECH_REGION="$SPEECHSDK_SPEECH_REGION" \
    SPEECHSDK_LUIS_KEY="$SPEECHSDK_LUIS_KEY" SPEECHSDK_LUIS_REGION="$SPEECHSDK_LUIS_REGION" 2>&1 |
      tee "${LOGDIR}/xcodebuild-${TESTNAME}.log" |
      xcpretty --report junit --output test-xcodebuild-${TESTNAME}.xml

