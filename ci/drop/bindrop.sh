#!/bin/bash

USAGE="Usage: $0 platform configuration destination"

set -e -o pipefail

PLATFORM="${1?$USAGE}"
CONFIG="${2?$USAGE}"
DEST="${3?$USAGE}"
TARGET="${4-UNKNOWN}"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
. "$SCRIPT_DIR/../functions.sh"

print_vars = PLATFORM CONFIG DEST TARGET =

set -x

# Ensure Unix paths on Windows from here on
if [[ $OS = "Windows_NT" ]]; then
  DEST="$(cygpath --unix --absolute "$DEST")"
  SCRIPT_DIR="$(cygpath --unix --absolute "$SCRIPT_DIR")"
fi

SOURCE_ROOT="$SCRIPT_DIR/../.."
BUILD_ROOT="$SOURCE_ROOT/build"

SRCJAR="$BUILD_ROOT/lib/com.microsoft.cognitiveservices.speech.jar"
SRCJARSRC="$BUILD_ROOT/lib/com.microsoft.cognitiveservices.speech-src.zip"

CSHARPSUPPORTED=false
JAVASUPPORTED=true

if [[ $TARGET == UWP ]]; then
  JAVASUPPORTED=false
fi

if [[ $OS = "Windows_NT" ]]; then
  case $TARGET in
    UNKNOWN|UWP) LIBPREFIX=Microsoft.CognitiveServices.Speech.
             DYNLIBSUFFIX=.dll
             STATLIBSUFFIX=.lib

             SRCBIN="$BUILD_ROOT/bin/$CONFIG"
             SRCLIB="$BUILD_ROOT/lib/$CONFIG"
             SRCDYNLIB="$BUILD_ROOT/bin/$CONFIG"

             SRCJAVABINDINGS="$SRCBIN/Microsoft.CognitiveServices.Speech.java.bindings.dll"
             CSHARPSUPPORTED=true
             ;;
    Android-*) LIBPREFIX=libMicrosoft.CognitiveServices.Speech.
             DYNLIBSUFFIX=.so
             STATLIBSUFFIX=.a

             SRCBIN="$BUILD_ROOT/bin"
             SRCLIB="$BUILD_ROOT/lib"
             SRCDYNLIB="$BUILD_ROOT/lib"

             SRCJAVABINDINGS="$SRCBIN/libMicrosoft.CognitiveServices.Speech.java.bindings.so"
             ;;
    *) echo Invalid fourth parameter $TARGET. Should be empty, UWP, or Android-PLATFORM.
       exit 1
       ;;
  esac
else
  LIBPREFIX=libMicrosoft.CognitiveServices.Speech.

  SRCBIN="$BUILD_ROOT/bin"
  SRCLIB="$BUILD_ROOT/lib"
  SRCDYNLIB="$BUILD_ROOT/lib"

  if [[ $(uname) = Linux ]]; then
    DYNLIBSUFFIX=.so
    SRCJAVABINDINGS="$SRCBIN/libMicrosoft.CognitiveServices.Speech.java.bindings.so"
  else
    DYNLIBSUFFIX=.dylib
    SRCJAVABINDINGS="$SRCBIN/libMicrosoft.CognitiveServices.Speech.java.bindings.jnilib"
  fi

  STATLIBSUFFIX=.a

  if [[ $PLATFORM == Linux-arm* || $PLATFORM == Linux-x86 || $TARGET == OSX ]]; then
    CSHARPSUPPORTED=false
  else
    CSHARPSUPPORTED=true
  fi

  if [[ $TARGET == IOS ]]; then
     CSHARPSUPPORTED=false
     JAVASUPPORTED=false
  fi
fi

SRCINC="$SOURCE_ROOT/source/public"
SRCPRIVINC="$SOURCE_ROOT/source/core/include"
SRCPRIVINC2="$SOURCE_ROOT/source/core/common/include"
SRCPRIVTESTJAR="$BUILD_ROOT/bin/com.microsoft.cognitiveservices.speech.tests.jar"

DESTPUBLIB="$DEST/public/lib"
DESTPUBLIBNET461="$DEST/public/lib/net461"
DESTPUBLIBNETSTANDARD20="$DEST/public/lib/netstandard2.0"
DESTPUBLIBUNIXOSNETSTANDARD20="$DEST/public/lib/UnixOS/netstandard2.0"
DESTPUBINC="$DEST/public/include"
DESTPRIVLIB="$DEST/private/lib"
DESTPRIVLIBUNSTRIPPED="$DEST/private/libunstripped"
DESTPRIVBIN="$DEST/private/bin"
DESTPRIVINC="$DEST/private/include"
DESTPRIVINC2="$DEST/private/include.common"


printf "\nCopying files to drop location\n"

# N.B. no long option for -p (parents) on OSX.
mkdir -p "$DESTPUBLIB" "$DESTPUBLIBNET461" "$DESTPUBLIBNETSTANDARD20" "$DESTPUBLIBUNIXOSNETSTANDARD20" "$DESTPUBINC" "$DESTPRIVLIB" "$DESTPRIVBIN" "$(dirname "$DESTPRIVINC")" "$(dirname "$DESTPRIVINC2")"  "$DESTPUBLIB"

# N.B. no long option for -v (verbose) and -p (preserve) on OSX.
CPOPT="-v -p"

cp $CPOPT "$SRCDYNLIB"/$LIBPREFIX*$DYNLIBSUFFIX "$DESTPUBLIB"
# On Windows and not Android, copy import libraries, XMLDoc, and PDBs.
if [[ $OS = "Windows_NT" ]]; then
  if [[ $TARGET != Android-* ]]; then
    cp $CPOPT "$SRCLIB"/$LIBPREFIX*.lib "$DESTPUBLIB"
    cp $CPOPT "$SRCDYNLIB"/$LIBPREFIX*.pdb "$DESTPUBLIB"

    cp $CPOPT "$SRCDYNLIB"/net461/$LIBPREFIX*.{pdb,xml,dll} "$DESTPUBLIBNET461"
    cp $CPOPT "$SRCDYNLIB"/netstandard2.0/$LIBPREFIX*.{pdb,xml,dll} "$DESTPUBLIBNETSTANDARD20"
    cp $CPOPT "$SRCDYNLIB"UnixOS/netstandard2.0/$LIBPREFIX*.{pdb,xml,dll} "$DESTPUBLIBUNIXOSNETSTANDARD20"
  fi
fi

if [[ $JAVASUPPORTED == true ]]; then
  # Copy .jar
  cp $CPOPT "$SRCJAR" "$DESTPUBLIB"
  cp $CPOPT "$SRCJARSRC" "$DESTPUBLIB"
  cp $CPOPT "$SRCJAVABINDINGS" "$DESTPUBLIB"

  # Copy (private) test .jar
  cp $CPOPT "$SRCPRIVTESTJAR" "$DESTPRIVBIN"
fi


# Linux: strip shipping binaries, but keep the unstripped version around.
# N.B. there's an .so in disguise with the .dll extension
if [[ $(uname) = Linux ]]; then
  cp $CPOPT -R "$DESTPUBLIB" "$DESTPRIVLIBUNSTRIPPED"
  find "$DESTPUBLIB" \( -name \*.so -or -name \*.dll \) -print0 | xargs -0 strip
fi

# Android: strip shipping binaries, but keep the unstripped version around.
if [[ $TARGET = Android-* ]]; then
  cp $CPOPT -R "$DESTPUBLIB" "$DESTPRIVLIBUNSTRIPPED"
  if [[ $TARGET = Android-arm64 ]]; then
    STRIP=$ANDROID_NDK/toolchains/aarch64-linux-android-4.9/prebuilt/windows-x86_64/aarch64-linux-android/bin/strip.exe
  elif [[ $TARGET = Android-arm32 ]]; then
    STRIP=$ANDROID_NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64/arm-linux-androideabi/bin/strip.exe
  elif [[ $TARGET = Android-x86 ]]; then
    STRIP=$ANDROID_NDK/toolchains/x86-4.9/prebuilt/windows-x86_64/i686-linux-android/bin/strip.exe
  elif [[ $TARGET = Android-x64 ]]; then
    STRIP=$ANDROID_NDK/toolchains/x86_64-4.9/prebuilt/windows-x86_64/x86_64-linux-android/bin/strip.exe
  else
    echo Unsupported architecture $TARGET.
    exit 1
  fi
  find "$DESTPUBLIB" -name \*.so -print0 | xargs -0 $STRIP
fi

# copy additional private binaries (non-shipping)
for var in carbonx Microsoft.CognitiveServices.Speech.Tests.ParallelRunner core_tests cxx_api_tests; do
  [[ -e "$SRCBIN/$var" ]] && mkdir -p "$DESTPRIVBIN" && cp $CPOPT "$SRCBIN/$var" "$DESTPRIVBIN"
done

# N.B. no long option for -R (recursive) on OSX.
cp $CPOPT -R "$SRCINC"/* "$DESTPUBINC"

# N.B. Using '-I -n 1' and replacement instead of "cp --target" since --target
# is not available on OSX.
find "$SRCLIB" -type f -name \*$STATLIBSUFFIX -not -name $LIBPREFIX\* -print0 |
  xargs -n 1 -0 -I % cp % "$DESTPRIVLIB"
cp $CPOPT "$SRCLIB"/*carbon-mock* "$DESTPRIVLIB"

cp $CPOPT -R "$SRCPRIVINC" "$DESTPRIVINC"
cp $CPOPT -R "$SRCPRIVINC2" "$DESTPRIVINC2"
