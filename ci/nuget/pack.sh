#!/bin/bash
# This is supposed to be run under Git Bash.

set -e -x -o pipefail

ls -l "/c/Program Files (x86)/Microsoft SDKs/Windows/v10.0A/bin/"

if [[ -n $NUGETEXETOOLPATH ]]; then
  NUGETEXETOOLPATH="$(cygpath --unix --absolute "$NUGETEXETOOLPATH")"
else
  # Must be in path
  NUGETEXETOOLPATH=nuget.exe
fi

SCRIPT_NAME="$(basename "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(readlink -f "$(dirname "${BASH_SOURCE[0]}")")"

USAGE="Usage: $0 <redist-dir> <drop-dir> <version> <output-directory>"

REDIST_DIR="${1?$USAGE}"
DROP_DIR="${2?$USAGE}"
VERSION="${3?$USAGE}"
OUTPUT_DIR="${4?$USAGE}"

REDIST_DIR="$(cygpath --unix --absolute "$REDIST_DIR")"
[[ -e $REDIST_DIR ]]
REDIST_DIR="$(cygpath --windows --absolute "$REDIST_DIR")"

# Check that Windows drop directory exists and turn into Windows path.
WINDOWS_DROP_DIR="$(cygpath --unix --absolute "$DROP_DIR/Windows")"
[[ -e $WINDOWS_DROP_DIR ]]
WINDOWS_DROP_DIR="$(cygpath --windows --absolute "$WINDOWS_DROP_DIR")"

# Check that Windows UWP drop directory exists and turn into Windows path.
WINDOWS_UWP_DROP_DIR="$(cygpath --unix --absolute "$DROP_DIR/WindowsUwp")"
[[ -e $WINDOWS_UWP_DROP_DIR ]]
WINDOWS_UWP_DROP_DIR="$(cygpath --windows --absolute "$WINDOWS_UWP_DROP_DIR")"

# Check that Linux drop directory exists and turn into Windows path.
LINUX_DROP_DIR="$(cygpath --unix --absolute "$DROP_DIR/Linux")"
[[ -e $LINUX_DROP_DIR ]]
LINUX_DROP_DIR="$(cygpath --windows --absolute "$LINUX_DROP_DIR")"

OUTPUT_DIR="$(cygpath --windows --absolute "$OUTPUT_DIR")"

"$NUGETEXETOOLPATH" pack ./ci/nuget/carbon.nuspec \
  -Properties "RedistDir=$REDIST_DIR;WindowsDropDir=$WINDOWS_DROP_DIR;WindowsUwpDropDir=$WINDOWS_UWP_DROP_DIR;LinuxDropDir=$LINUX_DROP_DIR;Version=$VERSION" \
  -OutputDirectory "$OUTPUT_DIR"
