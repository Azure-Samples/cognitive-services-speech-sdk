#!/bin/sh
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
#    SIGN_CERTIFICATE_NAME="iPhone Developer: irgros@microsoft.com (9MMF92W679)"
#else
#    SIGN_CERTIFICATE_NAME="iPhone Developer: irgros@microsoft.com (9MMF92W679)"
#fi

MAC_LIBRARY_DIR="${BUILD_DIR}/${CONFIGURATION}-macos"

MAC_LIBRARY_PATH="${MAC_LIBRARY_DIR}/${FRAMEWORK_NAME}.framework"


# Build the individual frameworks for device and iphonesimulator.

# If we will change the project from .xcodeproj to .xcworkspace (e.g. we need to add CocoaPods to the project) we will need to use the below command
# instead of the current one.
#xcodebuild -workspace ${PROJECT_NAME}.xcworkspace -scheme ${PROJECT_NAME} -sdk macosx -configuration ${CONFIGURATION} clean build CONFIGURATION_BUILD_DIR=${BUILD_DIR}/${CONFIGURATION} 2>&1

xcodebuild -project ${PROJECT_NAME}.xcodeproj -target ${PROJECT_NAME} -sdk macosx -configuration ${CONFIGURATION} clean build CONFIGURATION_BUILD_DIR=${MAC_LIBRARY_DIR} 2>&1

# TODO: VSTS:1408395 - Investigating signing using ESRP, the code below needs to be updated once the signing method is decided.
#codesign --force -s "${SIGN_CERTIFICATE_NAME}" ${MAC_LIBRARY_PATH}

# No universal framework needed for Mac since it only contains one architecture.
OUTPUT_DIR="${PROJECT_DIR}/../../../../xcode-build-output/${FRAMEWORK_NAME}-${CONFIGURATION}-macos/"

rm -rf "${OUTPUT_DIR}"
mkdir -p "${OUTPUT_DIR}"

# Copy the Mac framework and license file to the output directory.
# Using -R so that symlinks are maintained.
cp -R "${MAC_LIBRARY_PATH}" "${OUTPUT_DIR}"
cp "${PROJECT_DIR}/../../../../license.md" "${OUTPUT_DIR}"

# Zipping the framework and license in order to create the archive we will make available as 'source' for the .podspec.
cd "${OUTPUT_DIR}"
zip --symlinks -r "${FRAMEWORK_NAME}-OSX.zip" "license.md" "${FRAMEWORK_NAME}.framework"
cd -
