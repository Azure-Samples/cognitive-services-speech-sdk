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

USAGE="Usage: $0 <drop-dir> <build-id> <output-directory>"

DROP_DIR="${1?$USAGE}"
BUILD_ID="${2?$USAGE}"
OUTPUT_DIR="${3?$USAGE}"

DROP_DIR="$(cygpath --unix --absolute "$DROP_DIR")"
[[ -e $DROP_DIR ]]

OUTPUT_DIR="$(cygpath --windows --absolute "$OUTPUT_DIR")"

DROP_DIR="$(cygpath --windows --absolute "$DROP_DIR")"

"$NUGETEXETOOLPATH" pack ./ci/nuget/carbon.nuspec -Properties "DropDir=$DROP_DIR;BuildNumber=$BUILD_ID" -OutputDirectory "$OUTPUT_DIR"
