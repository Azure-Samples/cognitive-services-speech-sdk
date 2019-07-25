#!/usr/bin/env bash
set -e
set -o pipefail


function combine_dsym_bundles() {
    local USAGE="Usage: $0 OUTPUT_DIR SIMULATOR32_LIB SIMULATOR64_LIB DEVICE_LIB"

    local output_dir=${1?$USAGE}
    local simulator32_lib=${2?$USAGE}
    local simulator64_lib=${3?$USAGE}
    local device_lib=${4?$USAGE}

    set -u -e -o pipefail -x
    for lib in $simulator32_lib $simulator64_lib $device_lib; do
      dsymutil ${lib} -o ${lib}.dSYM
    done

    libname=$(basename ${simulator32_lib})
    # copy files from one of the frameworks
    cp -R "${simulator32_lib}.dSYM" "${output_dir}/${libname}.dSYM" && \
    # build a fat dSYM bundle
    lipo -create \
        -output "$output_dir/${libname}.dSYM/Contents/Resources/DWARF/${libname}" \
        "${device_lib}.dSYM/Contents/Resources/DWARF/${libname}" \
        "${simulator32_lib}.dSYM/Contents/Resources/DWARF/${libname}" \
        "${simulator64_lib}.dSYM/Contents/Resources/DWARF/${libname}"
}

