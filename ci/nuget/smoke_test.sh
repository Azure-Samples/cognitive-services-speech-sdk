#!/bin/bash
# This is supposed to be run under Git Bash.

set -e -x -o pipefail

function getUniqueMatch {
  [[ $# -eq 1 && -f $1 ]]
  echo "$1"
}

SCRIPT_NAME="$(basename "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(readlink -f "$(dirname "${BASH_SOURCE[0]}")")"

USAGE="Usage: $0 <path-to-nuget-package-or-containing-directory> <speech-subscription-key>"

PACKAGE_PATH="${1?$USAGE}"
SPEECH_SUBSCRIPTION_KEY="${2?$USAGE}"

if [[ -n $NUGETEXETOOLPATH ]]; then
  NUGETEXETOOLPATH="$(cygpath --unix --absolute "$NUGETEXETOOLPATH")"
else
  # Must be in path
  NUGETEXETOOLPATH=nuget.exe
fi

PACKAGE_NAME=Microsoft.CognitiveServices.Speech

# Check existence
[[ -e "$PACKAGE_PATH" ]]

# If pointing to a directory, expand package inside
if [[ -d "$PACKAGE_PATH" ]]; then
  PACKAGE_PATH="$(getUniqueMatch "$PACKAGE_PATH/$PACKAGE_NAME."*)"
fi

# Parse package version from filename
PACKAGE_PATH="$(readlink -f "$PACKAGE_PATH")"
[[ -e $PACKAGE_PATH ]]
[[ "$(basename "$PACKAGE_PATH")" =~ ^${PACKAGE_NAME//./\\.}\.([0-9].*)\.nupkg$ ]]
PACKAGE_VERSION="${BASH_REMATCH[1]}"
echo Determined version as $PACKAGE_VERSION.

# Go to repo root
cd "$SCRIPT_DIR/../.."

MSBUILD15="/c/Program Files (x86)/Microsoft Visual Studio/2017/Enterprise/MSBuild/15.0/bin/msbuild.exe"
DOWNLOAD_AUDIO=tests/scripts/download_audio.sh
PACKAGES_CONFIG=samples/CxxHelloWorld/CxxHelloWorld/packages.config

# Check for dependencies
type $NUGETEXETOOLPATH
[[ -x $MSBUILD15 ]]
[[ -x $DOWNLOAD_AUDIO ]]
[[ -e $PACKAGES_CONFIG ]]

# Download audio (test dependency)
"$DOWNLOAD_AUDIO"

# Patch Speech SDK Version into packages.config file
powershell -NoProfile -NoLogo -ExecutionPolicy Bypass -NonInteractive -Command - <<PATCHPACKAGEVERSION
\$ErrorActionPreference = 'Stop'
\$cfgPath = Resolve-Path '$PACKAGES_CONFIG'
\$cfg = [xml](Get-Content \$cfgPath)
\$exitCode = 1
\$cfg.packages.package | Where-Object id -eq '$PACKAGE_NAME' | ForEach-Object { \$_.version = '$PACKAGE_VERSION'; \$exitCode = 0 }
\$cfg.Save(\$cfgPath)
Write-Host \$exitCode
exit \$exitCode
PATCHPACKAGEVERSION

$NUGETEXETOOLPATH restore -noninteractive "$PACKAGES_CONFIG" -packagesdirectory samples/CxxHelloWorld/packages -nocache -source "$(dirname "$PACKAGE_PATH")"

# TODO more samples, and C#
SOLUTION=samples/CxxHelloWorld/CxxHelloWorld.sln

# Build and test all configurations
for configuration in Debug Release; do
for platform in x86 x64; do
for useDebugLibs in 1 0; do
  MSYS_NO_PATHCONV=1 "$MSBUILD15" /m /p:Platform=$platform /p:Configuration=$configuration /p:SpeechSdkUseDebugLibs=$useDebugLibs /p:SpeechSdkVersion=$PACKAGE_VERSION $SOLUTION
  BINROOT=
  [[ $platform == x64 ]] && BINROOT=/$platform
  MSYS_NO_PATHCONV=1 ./samples/CxxHelloWorld$BINROOT/$configuration/CxxHelloWorld.exe $SPEECH_SUBSCRIPTION_KEY whatstheweatherlike.wav
done
done
done

echo All builds and tests passed.
