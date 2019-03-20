#!/usr/bin/env bash
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

USAGE="Usage: $0 <nuspec-file> <redist-dir> <drop-dir> <version> <output-directory> [<is-os-build>]"

NUSPEC_FILE_PATH="${1?$USAGE}"
REDIST_DIR="${2?$USAGE}"
DROP_DIR="${3?$USAGE}"
VERSION="${4?$USAGE}"
OUTPUT_DIR="${5?$USAGE}"
IS_OS_BUILD="${6:false}"

REDIST_DIR="$(cygpath --unix --absolute "$REDIST_DIR")"
[[ -e $REDIST_DIR ]]
REDIST_DIR="$(cygpath --windows --absolute "$REDIST_DIR")"

# Check that Windows drop directory exists and turn into Windows path.
WINDOWS_DROP_DIR="$(cygpath --unix --absolute "$DROP_DIR/Windows")"
[[ -e $WINDOWS_DROP_DIR ]]
WINDOWS_DROP_DIR="$(cygpath --windows --absolute "$WINDOWS_DROP_DIR")"

# Check that Windows OS drop directory exists and turn into Windows Path (only if OS build is present).
if [[ "$IS_OS_BUILD" = "true" ]]; then
  WINDOWS_OS_DROP_DIR="$(cygpath --unix --absolute "$DROP_DIR/WindowsOS")"
  [[ -e $WINDOWS_OS_DROP_DIR ]]
  WINDOWS_OS_DROP_DIR="$(cygpath --windows --absolute "$WINDOWS_OS_DROP_DIR")"
fi

# Check that Windows UWP drop directory exists and turn into Windows path.
WINDOWS_UWP_DROP_DIR="$(cygpath --unix --absolute "$DROP_DIR/WindowsUwp")"
[[ -e $WINDOWS_UWP_DROP_DIR ]]
WINDOWS_UWP_DROP_DIR="$(cygpath --windows --absolute "$WINDOWS_UWP_DROP_DIR")"

# Check that Linux drop directory exists and turn into Windows path.
LINUX_DROP_DIR="$(cygpath --unix --absolute "$DROP_DIR/Linux")"
[[ -e $LINUX_DROP_DIR ]]
LINUX_DROP_DIR="$(cygpath --windows --absolute "$LINUX_DROP_DIR")"

# Check that OSX drop directory exists and turn into Windows path.
OSX_DROP_DIR="$(cygpath --unix --absolute "$DROP_DIR/macOS")"
[[ -e $OSX_DROP_DIR ]]
OSX_DROP_DIR="$(cygpath --windows --absolute "$OSX_DROP_DIR")"

OUTPUT_DIR="$(cygpath --windows --absolute "$OUTPUT_DIR")"

"$NUGETEXETOOLPATH" pack "$NUSPEC_FILE_PATH" \
  -Properties "RedistDir=$REDIST_DIR;WindowsDropDir=$WINDOWS_DROP_DIR;WindowsOSDropDir=$WINDOWS_OS_DROP_DIR;WindowsUwpDropDir=$WINDOWS_UWP_DROP_DIR;LinuxDropDir=$LINUX_DROP_DIR;OSXDropDir=$OSX_DROP_DIR;Version=$VERSION" \
  -OutputDirectory "$OUTPUT_DIR" \
  -Symbols

