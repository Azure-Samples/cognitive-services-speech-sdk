#!/bin/bash

USAGE="Usage: $0 destination "

UNIVERSAL_LIBRARY_DIR="${1?$USAGE}"

set -e
set -o pipefail
set -o xtrace

DEVICE_LIBRARY_PATH=build_ios_device/lib
SIMULATOR_LIBRARY_PATH=build_ios_simulator/lib

# TODO: fix framework name
FRAMEWORK_NAME=MicrosoftCognitiveServicesSpeech

UNIVERSAL_FRAMEWORK="${UNIVERSAL_LIBRARY_DIR}/${FRAMEWORK_NAME}.framework"

mkdir -p "${UNIVERSAL_LIBRARY_DIR}"

cp -R "${DEVICE_LIBRARY_PATH}/${FRAMEWORK_NAME}.framework" "${UNIVERSAL_LIBRARY_DIR}"

cp -R "${SIMULATOR_LIBRARY_PATH}/${FRAMEWORK_NAME}.framework" "${UNIVERSAL_LIBRARY_DIR}"

lipo \
    -create -output "${UNIVERSAL_FRAMEWORK}/${FRAMEWORK_NAME}" \
    "${DEVICE_LIBRARY_PATH}/${FRAMEWORK_NAME}.framework/${FRAMEWORK_NAME}" \
    "${SIMULATOR_LIBRARY_PATH}/${FRAMEWORK_NAME}.framework/${FRAMEWORK_NAME}"

# TODO: copy license
pushd ${UNIVERSAL_LIBRARY_DIR}
# zip --symlinks -r "${FRAMEWORK_NAME}-iOS.zip" "license.md" "${FRAMEWORK_NAME}.framework"
zip --symlinks -r "${FRAMEWORK_NAME}-iOS.zip" "${FRAMEWORK_NAME}.framework"
popd

