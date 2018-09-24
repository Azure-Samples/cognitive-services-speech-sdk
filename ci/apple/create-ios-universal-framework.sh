#!/bin/bash

USAGE="Usage: $0 repodir destination frameworkname"

REPO_DIR="${1?$USAGE}"
UNIVERSAL_LIBRARY_DIR="${2?$USAGE}"
FRAMEWORK_NAME="${3?$USAGE}"

set -e
set -o pipefail
set -o xtrace

DEVICE_LIBRARY_PATH=build_ios_device/lib
SIMULATOR_LIBRARY_PATH=build_ios_simulator/lib

UNIVERSAL_FRAMEWORK="${UNIVERSAL_LIBRARY_DIR}/${FRAMEWORK_NAME}.framework"

mkdir -p "${UNIVERSAL_LIBRARY_DIR}"

cp -R "${DEVICE_LIBRARY_PATH}/${FRAMEWORK_NAME}.framework" "${UNIVERSAL_LIBRARY_DIR}"

cp -R "${SIMULATOR_LIBRARY_PATH}/${FRAMEWORK_NAME}.framework" "${UNIVERSAL_LIBRARY_DIR}"

lipo \
    -create -output "${UNIVERSAL_FRAMEWORK}/${FRAMEWORK_NAME}" \
    "${DEVICE_LIBRARY_PATH}/${FRAMEWORK_NAME}.framework/${FRAMEWORK_NAME}" \
    "${SIMULATOR_LIBRARY_PATH}/${FRAMEWORK_NAME}.framework/${FRAMEWORK_NAME}"

cp -p "$REPO_DIR/"{REDIST.txt,license.md,ThirdPartyNotices.md} "$UNIVERSAL_LIBRARY_DIR"

pushd ${UNIVERSAL_LIBRARY_DIR}
# zip --symlinks -r "${FRAMEWORK_NAME}-iOS.zip" "license.md" "${FRAMEWORK_NAME}.framework"
zip --symlinks -r "${FRAMEWORK_NAME}-iOS.zip" REDIST.txt license.md ThirdPartyNotices.md "${FRAMEWORK_NAME}.framework"
popd

