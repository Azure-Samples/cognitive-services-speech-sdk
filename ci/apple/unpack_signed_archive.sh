#! /usr/bin/env bash

# .zip from ESRP used to put the contents in a deep sub directory.  Now they seem not do the deep directory anymore, but drop our
# additional files (REDIST.txt etc.) on the floor.
#
# We try to re-zip and recreate any missing content.

usage="Usage: unpack_signed_archive.sh <output-directory> <framework-name> <versioned-framework-name> <source-dir>"

OutputDirectory="${1?$USAGE}"
FrameworkName="${2?$USAGE}"
VersionedFrameworkName="${3?$USAGE}"
SourceDir="${4?$USAGE}"

set -u -e -x -o pipefail
pwd
mkdir -p temp/nested
cd temp/nested
unzip "${OutputDirectory}/${VersionedFrameworkName}.zip"
FrameworkDir="$(find . -type d -name "${FrameworkName}.framework" -prune)"
if [[ -z $FrameworkDir ]]; then
    # Did not find it, framework content must be here or we can't continue.
    [[ -f "${FrameworkName}" ]]
    [[ -d Headers ]]
    cd ..
    mv nested "${FrameworkName}.framework"
    cp -p "${SourceDir}/"{REDIST.txt,license.md,ThirdPartyNotices.md} .
else
    cd "$FrameworkDir"
    cd ..
fi
# Be paranoid, check some stuff.
[[ "$( find . -maxdepth 1 | wc -l )" -eq 5 ]]
[[ -f license.md ]]
[[ -d "${FrameworkName}.framework" ]]
[[ -f REDIST.txt ]]
[[ -f ThirdPartyNotices.md ]]
[[ -f "${FrameworkName}.framework/${FrameworkName}" ]]
[[ -d "${FrameworkName}.framework/Headers" ]]
# check existence of code signing metadata for iOS/macOS
[[ -d "${FrameworkName}.framework/_CodeSignature" ]] || [[ -d "${FrameworkName}.framework/Versions/A/_CodeSignature" ]]
# Remove original package.
rm "${OutputDirectory}/${VersionedFrameworkName}.zip"
zip --symlinks -r "${OutputDirectory}/${VersionedFrameworkName}.zip" .
