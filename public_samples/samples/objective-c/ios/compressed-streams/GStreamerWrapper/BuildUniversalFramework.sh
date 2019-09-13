#!/usr/bin/env bash
#
# Copyright (c) Microsoft.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#
# Script that creates the Microsoft Cognitive Services Speech Universal Framework for iOS.

# Make the script err on failure
set -e -u
set -o pipefail
set -x

if [ "true" == ${ALREADYINVOKED:-false} ]
then
  echo "RECURSION: Detected, stopping"
else
  export ALREADYINVOKED="true"

  FRAMEWORK_NAME="${PRODUCT_NAME}"

  DEVICE_LIBRARY_DIR="${CONFIGURATION_BUILD_DIR}/${CONFIGURATION}-iphoneos"
  DEVICE_LIBRARY_PATH="${DEVICE_LIBRARY_DIR}/${FRAMEWORK_NAME}.framework"

  SIMULATOR32_LIBRARY_DIR="${CONFIGURATION_BUILD_DIR}/${CONFIGURATION}-iphonesimulator32"
  SIMULATOR32_LIBRARY_PATH="${SIMULATOR32_LIBRARY_DIR}/${FRAMEWORK_NAME}.framework"

  UNIVERSAL_LIBRARY_DIR="${CONFIGURATION_BUILD_DIR}/${CONFIGURATION}-iphoneuniversal"
  UNIVERSAL_FRAMEWORK="${UNIVERSAL_LIBRARY_DIR}/${FRAMEWORK_NAME}.framework"

  # Build the individual frameworks for device and iphonesimulator.

  xcodebuild -project ${PROJECT_FILE_PATH} -target ${PRODUCT_NAME} -sdk iphoneos -arch arm64 -arch armv7 \
    -configuration ${CONFIGURATION} build CONFIGURATION_BUILD_DIR=${DEVICE_LIBRARY_DIR} \
    2>&1

  xcodebuild -project ${PROJECT_FILE_PATH} -target ${PRODUCT_NAME} -sdk iphonesimulator -arch x86_64 -arch i386 \
    -configuration ${CONFIGURATION} build CONFIGURATION_BUILD_DIR=${SIMULATOR32_LIBRARY_DIR} \
    OTHER_LDFLAGS="-framework GStreamer -liconv -read_only_relocs suppress" 2>&1

  # Create universal directory and copy frameworks.

  rm -rf "${UNIVERSAL_LIBRARY_DIR}"
  mkdir -p "${UNIVERSAL_FRAMEWORK}"

  cp -R "${DEVICE_LIBRARY_PATH}/." "${UNIVERSAL_FRAMEWORK}"

  # Create universal framework.

  lipo "${DEVICE_LIBRARY_PATH}/${FRAMEWORK_NAME}" \
    "${SIMULATOR32_LIBRARY_PATH}/${FRAMEWORK_NAME}" \
    -create -output "${UNIVERSAL_FRAMEWORK}/${FRAMEWORK_NAME}"

  # clean up
  rm -rf ${DEVICE_LIBRARY_DIR}
  rm -rf ${SIMULATOR32_LIBRARY_DIR}
  # replace the default build product
  rm -rf ${CONFIGURATION_BUILD_DIR}/${FRAMEWORK_NAME}.framework
  mv ${UNIVERSAL_FRAMEWORK} ${CONFIGURATION_BUILD_DIR}/
  rmdir ${UNIVERSAL_LIBRARY_DIR}
fi

