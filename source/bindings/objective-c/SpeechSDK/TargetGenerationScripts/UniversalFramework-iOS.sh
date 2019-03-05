#!/usr/bin/env bash
#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#
# Script that creates the Microsoft Cognitive Services Speech Universal Framework for iOS.

# Make the script err on failure
set -e
set -o pipefail

FRAMEWORK_NAME="${PRODUCT_NAME}"

# If the configuration is 'Release' we need to sign it with the Microsoft Distribution Certificate.
# TODO: VSTS:1408395 - Investigating signing using ESRP, the code below needs to be updated once the signing method is decided.
#if [ ${CONFIGURATION} = "Release" ]
#then
#    SIGN_CERTIFICATE_NAME="<CERTIFICATE_NAME_DISTRIBUTION>"
#else
#    SIGN_CERTIFICATE_NAME="<CERTIFICATE_NAME_DEVELOPMENT>"
#fi


DEVICE_LIBRARY_DIR="${BUILD_DIR}/${CONFIGURATION}-iphoneos"

DEVICE_LIBRARY_PATH="${DEVICE_LIBRARY_DIR}/${FRAMEWORK_NAME}.framework"

SIMULATOR_LIBRARY_DIR="${BUILD_DIR}/${CONFIGURATION}-iphonesimulator"

SIMULATOR_LIBRARY_PATH="${SIMULATOR_LIBRARY_DIR}/${FRAMEWORK_NAME}.framework"

UNIVERSAL_LIBRARY_DIR="${BUILD_DIR}/${CONFIGURATION}-iphoneuniversal"

UNIVERSAL_FRAMEWORK="${UNIVERSAL_LIBRARY_DIR}/${FRAMEWORK_NAME}.framework"


# Build the individual frameworks for device and iphonesimulator.

# If we will change the project from .xcodeproj to .xcworkspace (e.g. we need to add CocoaPods to the project) we will need to use the below command
# instead of the current one.
#xcodebuild -workspace ${PROJECT_NAME}.xcworkspace -scheme ${PROJECT_NAME} -sdk macosx -configuration ${CONFIGURATION} clean build CONFIGURATION_BUILD_DIR=${BUILD_DIR}/${CONFIGURATION} 2>&1

xcodebuild -project ${PROJECT_NAME}.xcodeproj -target ${PROJECT_NAME} -sdk iphonesimulator -arch x86_64  -configuration ${CONFIGURATION} clean build CONFIGURATION_BUILD_DIR=${BUILD_DIR}/${CONFIGURATION}-iphonesimulator 2>&1

# TODO: VSTS:1408395 - Investigating signing using ESRP, the code below needs to be updated once the signing method is decided.
#codesign --force -s "${SIGN_CERTIFICATE_NAME}" ${SIMULATOR_LIBRARY_PATH}

xcodebuild -project ${PROJECT_NAME}.xcodeproj -target ${PROJECT_NAME} -sdk iphoneos -arch arm64 -configuration ${CONFIGURATION} clean build CONFIGURATION_BUILD_DIR=${DEVICE_LIBRARY_DIR} 2>&1

# TODO: VSTS:1408395 - Investigating signing using ESRP, the code below needs to be updated once the signing method is decided.
#codesign --force -s "${SIGN_CERTIFICATE_NAME}" ${DEVICE_LIBRARY_PATH}


# Create univesal directory and copy frameworks.

rm -rf "${UNIVERSAL_LIBRARY_DIR}"

mkdir "${UNIVERSAL_LIBRARY_DIR}"

mkdir "${UNIVERSAL_FRAMEWORK}"

cp -R "${DEVICE_LIBRARY_PATH}/." "${UNIVERSAL_FRAMEWORK}"

cp -R "${SIMULATOR_LIBRARY_PATH}/." "${UNIVERSAL_FRAMEWORK}"

# Create universal framework.

lipo "${DEVICE_LIBRARY_PATH}/${FRAMEWORK_NAME}" "${SIMULATOR_LIBRARY_PATH}/${FRAMEWORK_NAME}" -create -output "${UNIVERSAL_FRAMEWORK}/${FRAMEWORK_NAME}" | echo

# TODO: VSTS:1408395 - Investigating signing using ESRP, the code below needs to be updated once the signing method is decided.
#codesign --force -s "${SIGN_CERTIFICATE_NAME}" "${UNIVERSAL_FRAMEWORK}/${FRAMEWORK_NAME}"

# Copy only the universal framework to the output direcotry.

OUTPUT_DIR="${PROJECT_DIR}/../../../../xcode-build-output/${FRAMEWORK_NAME}-${CONFIGURATION}-iosuniversal/"

rm -rf "${OUTPUT_DIR}"
mkdir -p "${OUTPUT_DIR}"

# Copy the universal framework and license file to the output directory.
cp -R "${UNIVERSAL_FRAMEWORK}" "${OUTPUT_DIR}"
cp "${PROJECT_DIR}/../../../../license.md" "${OUTPUT_DIR}"

# Zipping the framework and license in order to create the archive we will make available as 'source' for the .podspec.
cd "${OUTPUT_DIR}"
zip --symlinks -r "${FRAMEWORK_NAME}-iOS.zip" "license.md" "${FRAMEWORK_NAME}.framework"
cd -
