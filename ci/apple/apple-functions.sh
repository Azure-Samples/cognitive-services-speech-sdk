#!/bin/bash
set -e
set -o pipefail


function build_ios_universal_framework {
    local USAGE="Usage: $0 destination frameworkname"

    local UNIVERSAL_LIBRARY_DIR="${1?$USAGE}"
    local FRAMEWORK_NAME="${2?$USAGE}"

    local DEVICE_LIBRARY_PATH=build_ios_device/lib
    local SIMULATOR32_LIBRARY_PATH=build_ios_simulator32/lib
    local SIMULATOR64_LIBRARY_PATH=build_ios_simulator64/lib

    local UNIVERSAL_FRAMEWORK="${UNIVERSAL_LIBRARY_DIR}/${FRAMEWORK_NAME}.framework"

    mkdir -p "${UNIVERSAL_LIBRARY_DIR}" && \
    cp -R "${DEVICE_LIBRARY_PATH}/${FRAMEWORK_NAME}.framework" "${UNIVERSAL_LIBRARY_DIR}" && \
    cp -R "${SIMULATOR32_LIBRARY_PATH}/${FRAMEWORK_NAME}.framework" "${UNIVERSAL_LIBRARY_DIR}" && \
    cp -R "${SIMULATOR64_LIBRARY_PATH}/${FRAMEWORK_NAME}.framework" "${UNIVERSAL_LIBRARY_DIR}" && \
    lipo \
        -create -output "${UNIVERSAL_FRAMEWORK}/${FRAMEWORK_NAME}" \
        "${DEVICE_LIBRARY_PATH}/${FRAMEWORK_NAME}.framework/${FRAMEWORK_NAME}" \
        "${SIMULATOR32_LIBRARY_PATH}/${FRAMEWORK_NAME}.framework/${FRAMEWORK_NAME}" \
        "${SIMULATOR64_LIBRARY_PATH}/${FRAMEWORK_NAME}.framework/${FRAMEWORK_NAME}"
}

function build_dsym_bundle() {
    local USAGE="Usage: $0 FRAMEWORK_DIR FRAMEWORK_NAME"

    local framework_dir=${1?$USAGE}
    local FRAMEWORK_NAME="${2?$USAGE}"

    dsymutil "${framework_dir}/${FRAMEWORK_NAME}.framework/${FRAMEWORK_NAME}" \
        -o "${framework_dir}/${FRAMEWORK_NAME}.framework.dSYM"
}

function combine_dsym_bundles() {
    local USAGE="Usage: $0 OUTPUT_DIR SIMULATOR32_DSYM_BUNDLE SIMULATOR64_DSYM_BUNDLE DEVICE_DSYM_BUNDLE FRAMEWORK_NAME"

    local output_dir=${1?$USAGE}
    local simulator32_dsym_bundle=${2?$USAGE}
    local simulator64_dsym_bundle=${3?$USAGE}
    local device_dsym_bundle=${4?$USAGE}
    local framework_name=${5?USAGE}

    # copy files from one of the frameworks
    # build a fat dSYM bundle
    cp -R "${simulator32_dsym_bundle}" "${output_dir}/${framework_name}.framework.dSYM" && \
    lipo -create \
        -output "$output_dir/${framework_name}.framework.dSYM/Contents/Resources/DWARF/${framework_name}" \
        "${device_dsym_bundle}/Contents/Resources/DWARF/${framework_name}" \
        "${simulator32_dsym_bundle}/Contents/Resources/DWARF/${framework_name}" \
        "${simulator64_dsym_bundle}/Contents/Resources/DWARF/${framework_name}"
}

