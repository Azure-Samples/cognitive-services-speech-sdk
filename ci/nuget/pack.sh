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

USAGE="Usage: $0 <drop-dir> <version> <output-directory>"

DROP_DIR="${1?$USAGE}"
VERSION="${2?$USAGE}"
OUTPUT_DIR="${3?$USAGE}"

# Check that Windows drop directory exists and turn into Windows path.
WINDOWS_DROP_DIR="$(cygpath --unix --absolute "$DROP_DIR/Windows")"
[[ -e $WINDOWS_DROP_DIR ]]
WINDOWS_DROP_DIR="$(cygpath --windows --absolute "$WINDOWS_DROP_DIR")"

# Check that Linux drop directory exists and turn into Windows path.
LINUX_DROP_DIR="$(cygpath --unix --absolute "$DROP_DIR/Linux")"
[[ -e $LINUX_DROP_DIR ]]
LINUX_DROP_DIR="$(cygpath --windows --absolute "$LINUX_DROP_DIR")"

OUTPUT_DIR="$(cygpath --windows --absolute "$OUTPUT_DIR")"

"$NUGETEXETOOLPATH" pack ./ci/nuget/carbon.nuspec -Properties "WindowsDropDir=$WINDOWS_DROP_DIR;LinuxDropDir=$LINUX_DROP_DIR;Version=$VERSION" -OutputDirectory "$OUTPUT_DIR"
