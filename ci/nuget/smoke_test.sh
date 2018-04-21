#!/bin/bash
# This is supposed to be run under Git Bash.

set -e -x -o pipefail

function getUniqueMatch {
  [[ $# -eq 1 && -f $1 ]]
  echo "$1"
}

function patchPackageVersion {
  local PACKAGE_NAME PACKAGE_VERSION PACKAGES_CONFIG

  PACKAGE_NAME="$1"
  PACKAGE_VERSION="$2"
  PACKAGES_CONFIG="$3"

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

# Check for dependencies
type $NUGETEXETOOLPATH
[[ -x $MSBUILD15 ]]

WAV_PATH=tests/input/audio/whatstheweatherlike.wav
[[ -e $WAV_PATH ]]

# Native sample build and test

NATIVE_SAMPLE_PACKAGES_DIR=samples/CxxHelloWorld/packages 
NATIVE_SAMPLE_PACKAGES_CONFIG=samples/CxxHelloWorld/CxxHelloWorld/packages.config
[[ -e $NATIVE_SAMPLE_PACKAGES_CONFIG ]]

patchPackageVersion "$PACKAGE_NAME" "$PACKAGE_VERSION" "$NATIVE_SAMPLE_PACKAGES_CONFIG"

$NUGETEXETOOLPATH restore -noninteractive "$NATIVE_SAMPLE_PACKAGES_CONFIG" -packagesdirectory "$NATIVE_SAMPLE_PACKAGES_DIR" -nocache -source "$(dirname "$PACKAGE_PATH")"

NATIVE_SAMPLE_SLN=samples/CxxHelloWorld/CxxHelloWorld.sln

# Build and test all configurations
for configuration in Release; do # TODO add-back Debug
for platform in x86 x64; do
# N.B. run non-debug first. Debug (for now) may trigger pop-up windows and time-out
for useDebugLibs in 0; do # TODO add back 1
  MSYS_NO_PATHCONV=1 "$MSBUILD15" /m /p:Platform=$platform /p:Configuration=$configuration /p:SpeechSdkUseDebugLibs=$useDebugLibs /p:SpeechSdkVersion=$PACKAGE_VERSION $NATIVE_SAMPLE_SLN
  BINROOT=
  [[ $platform == x64 ]] && BINROOT=/$platform
  [[ $useDebugLibs == 1 ]] && echo ::: Running with debug libraries - if this hangs there may be a debug assertion with a pop-up window that causes a timeout.
  ./samples/CxxHelloWorld$BINROOT/$configuration/CxxHelloWorld.exe $SPEECH_SUBSCRIPTION_KEY $WAV_PATH
done
done
done

# Managed sample build and test

# TODO WIP

MANAGED_SAMPLE_SLN=samples/CsharpHelloWorld/CsharpHelloWorld.sln
MANAGED_SAMPLE_PACKAGES_DIR=samples/CsharpHelloWorld/packages
MANAGED_SAMPLE_PACKAGES_CONFIG=samples/CsharpHelloWorld/CsharpHelloWorld/packages.config
[[ -e $MANAGED_SAMPLE_PACKAGES_CONFIG ]]

patchPackageVersion "$PACKAGE_NAME" "$PACKAGE_VERSION" "$MANAGED_SAMPLE_PACKAGES_CONFIG"
$NUGETEXETOOLPATH restore -noninteractive "$MANAGED_SAMPLE_PACKAGES_CONFIG" -packagesdirectory "$MANAGED_SAMPLE_PACKAGES_DIR" -nocache -source "$(dirname "$PACKAGE_PATH")"

# Build and test all configurations
for configuration in Release; do # TODO add back Debug
for platform in x86 x64; do
#for useDebugLibs in 1 0; do
  MSYS_NO_PATHCONV=1 "$MSBUILD15" /m /p:Platform=$platform /p:Configuration=$configuration /p:SpeechSdkUseDebugLibs=$useDebugLibs /p:SpeechSdkVersion=$PACKAGE_VERSION $MANAGED_SAMPLE_SLN
  ./samples/CsharpHelloWorld/CsharpHelloWorld/bin/$platform/$configuration/CsharpHelloWorld.exe $SPEECH_SUBSCRIPTION_KEY $WAV_PATH
#done
done
done

echo All builds and tests passed.
