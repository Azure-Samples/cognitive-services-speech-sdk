#!/bin/bash

set -e -x -o pipefail
USAGE="Usage: $0 <path-to-nuget-package-path> <sln-path>"
PACKAGE_PATH="${1?$USAGE}"
SLN_PATH="${2?$USAGE}"
if [[ -n $NUGETEXETOOLPATH ]]; then
  NUGETEXETOOLPATH="$(cygpath --unix --absolute "$NUGETEXETOOLPATH")"
else
  # Must be in path
  NUGETEXETOOLPATH=nuget.exe
fi
type $NUGETEXETOOLPATH

[[ -d $PACKAGE_PATH ]]
[[ -d $SLN_PATH ]]
PACKAGE_PATH="$(cygpath --windows --absolute "$PACKAGE_PATH")"

find "$SLN_PATH" -name \*.sln -print0 |
  xargs -0 -n1 --verbose $NUGETEXETOOLPATH restore -noninteractive -NonInteractive -NoCache -Source "$(dirname "$PACKAGE_PATH")" -Source https://api.nuget.org/v3/index.json
