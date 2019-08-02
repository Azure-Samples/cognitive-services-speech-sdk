#!/usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
set -u -e -x -o pipefail

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

# Get some helpers
. "$SCRIPT_DIR/../functions.sh"

set -x -e -o pipefail
USAGE="Usage: $0 drop-dir"
DROP_DIR="${1?$USAGE}"
MAC_ARTIFACT_DIR=${2:-macOS}

# Make absolute
DROP_DIR="$(readlink -f "$DROP_DIR")"

# Before continuing check that we can find the base JAR
# TODO which one should we pick (also during compile)? What about compatibility.
BASE_JAR="$DROP_DIR/Windows/x64/Release/public/lib/com.microsoft.cognitiveservices.speech.jar"
[[ -f $BASE_JAR ]]

JAR_DIR="$SCRIPT_DIR/jar"

# Clean output
[[ -d $JAR_DIR ]] && rm -rf "$JAR_DIR"

# Unzip the base JAR
unzip -q -o "$BASE_JAR" -d "$JAR_DIR"

# Check that we weren't signed yet.
[[ -n "$(find "$JAR_DIR/META-INF" -type f -not -path "$JAR_DIR/META-INF/MANIFEST.MF")" ]] && {
  echo Unexpected files in the META-INF directory - was the JAR already signed?
  exit 1
}

# Used for timestamping all published files
NOW=$(date -Iseconds)

# (Shipping) platform list
platforms=({Linux,Windows,OSX}-x64-Release)

# Will need extension to support more archs / debug flavor (in sync with NativeLibraryLoader.java).
for platformString in "${platforms[@]}"; do
  platform=(${platformString//-/ })
  os="${platform[0]}"
  arch="${platform[1]}"
  flavor="${platform[2]}"

  assetDir=ASSETS
  dropPrefix="$DROP_DIR"
  case $os in
    Windows)
      dropPrefix+="/$os/$([[ $arch == x86 ]] && echo Win32 || echo x64)/$flavor"
      libPrefix= # empty
      libSuffix=.dll
      jnilibSuffix=.dll
      assetDir+=/windows
      ;;
    Linux)
      dropPrefix+="/$os/$os-$arch/$flavor"
      libPrefix=lib
      libSuffix=.so
      jnilibSuffix=.so
      assetDir+=/linux
      ;;
    OSX)
      dropPrefix+="/${MAC_ARTIFACT_DIR}/$flavor"
      libPrefix=lib
      libSuffix=.dylib
      jnilibSuffix=.jnilib
      assetDir+=/mac
      ;;
  esac

  dropPrefix+=/public/lib

  bitness="$([[ $arch == x86 ]] && echo 32 || echo 64)"
  assetDir+="$bitness"

  mkdir -p "$JAR_DIR/$assetDir"
  if [[ $os == "Windows" ]] || [[ $os == "Linux" ]] || [[ $os == "OSX" ]]; then
    cp --verbose "$dropPrefix/${libPrefix}Microsoft.CognitiveServices.Speech"{.core$libSuffix,.java.bindings$jnilibSuffix} "$JAR_DIR/$assetDir"
  fi

  # Copy KWS extension if available
  KWS_LIB=$dropPrefix/${libPrefix}Microsoft.CognitiveServices.Speech.extension.kws$libSuffix
  ! [[ -f $KWS_LIB ]] ||
    cp --verbose "$KWS_LIB" "$JAR_DIR/$assetDir"

  # Copy CODEC extension if available
  CODEC_LIB=$dropPrefix/${libPrefix}Microsoft.CognitiveServices.Speech.extension.codec$libSuffix
  ! [[ -f $CODEC_LIB ]] ||
    cp --verbose "$CODEC_LIB" "$JAR_DIR/$assetDir"

done

cp -v "$SCRIPT_DIR/../../"{REDIST.txt,license.md,ThirdPartyNotices.md} "$JAR_DIR"

# Timestamp
find "$JAR_DIR" -print0 | xargs -0 touch --date=$NOW

