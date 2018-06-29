#!/bin/bash
# This is supposed to be run under Git Bash.

# This script assumes that NuGet restore was already done.

set -e -x -o pipefail

function getUniqueMatch {
  [[ $# -eq 1 && -f $1 ]]
  echo "$1"
}

SCRIPT_NAME="$(basename "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(readlink -f "$(dirname "${BASH_SOURCE[0]}")")"

USAGE="Usage: $0 <speech-subscription-key> [<region>]"

SPEECH_SUBSCRIPTION_KEY="${1?$USAGE}"
REGION="${2:-westus}"

# Go to repo root
cd "$SCRIPT_DIR/../.."

MSBUILD15="/c/Program Files (x86)/Microsoft Visual Studio/2017/Enterprise/MSBuild/15.0/bin/msbuild.exe"

# Check for dependencies
[[ -x $MSBUILD15 ]]

WAV_PATH=tests/input/audio/whatstheweatherlike.wav
[[ -e $WAV_PATH ]]

# Native sample build and test

NATIVE_SAMPLE_SLN=public_samples/quickstart/cpp-windows/helloworld.sln

# Build and test all configurations
for configuration in Release; do # TODO add-back Debug
for platform in x86 x64; do
# N.B. run non-debug first. Debug (for now) may trigger pop-up windows and time-out
for useDebugLibs in 0; do # TODO add back 1
  MSYS_NO_PATHCONV=1 "$MSBUILD15" /m /p:Platform=$platform /p:Configuration=$configuration /p:SpeechSdkUseDebugLibs=$useDebugLibs $NATIVE_SAMPLE_SLN
  BINROOT=
  [[ $platform == x64 ]] && BINROOT=/$platform
  [[ $useDebugLibs == 1 ]] && echo ::: Running with debug libraries - if this hangs there may be a debug assertion with a pop-up window that causes a timeout.
  ./public_samples/quickstart/cpp-windows$BINROOT/$configuration/helloworld.exe $SPEECH_SUBSCRIPTION_KEY $REGION $WAV_PATH
done
done
done

# Managed sample build and test

MANAGED_SAMPLE_SLN=public_samples/quickstart/csharp-windows/helloworld.sln

# Build and test all configurations
for configuration in Release; do # TODO add back Debug
for platform in x86 x64; do
#for useDebugLibs in 1 0; do
  MSYS_NO_PATHCONV=1 "$MSBUILD15" /m /p:Platform=$platform /p:Configuration=$configuration /p:SpeechSdkUseDebugLibs=$useDebugLibs $MANAGED_SAMPLE_SLN
  ./public_samples/quickstart/csharp-windows/helloworld/bin/$platform/$configuration/helloworld.exe $SPEECH_SUBSCRIPTION_KEY $REGION $WAV_PATH
#done
done
done

echo All builds and tests passed.
