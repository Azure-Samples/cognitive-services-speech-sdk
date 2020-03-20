#!/usr/bin/env bash

USAGE="Usage: $0 platform configuration destination"

set -e -o pipefail

PLATFORM="${1?$USAGE}"
CONFIG="${2?$USAGE}"
DEST="${3?$USAGE}"
ISCROSS="${4?USAGE}"
TARGET="${5-UNKNOWN}"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
. "$SCRIPT_DIR/../functions.sh"

print_vars = PLATFORM CONFIG DEST TARGET =

set -x

# Ensure Unix paths on Windows from here on
if [[ $OS == "Windows_NT" ]]; then
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

if [[ $OS == "Windows_NT" ]]; then
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

  SRCBIN="$BUILD_ROOT/bin"
  SRCLIB="$BUILD_ROOT/lib"
  SRCDYNLIB="$BUILD_ROOT/lib"

  LIBPREFIX=libMicrosoft.CognitiveServices.Speech.
  if [[ $(uname) == Linux ]]; then
    DYNLIBSUFFIX=.so
    SRCJAVABINDINGS="$SRCBIN/libMicrosoft.CognitiveServices.Speech.java.bindings.so"
  else # OSX
    DOTNET_LIBPREFIX=Microsoft.CognitiveServices.Speech.
    DYNLIBSUFFIX=.dylib
    SRCJAVABINDINGS="$SRCBIN/libMicrosoft.CognitiveServices.Speech.java.bindings.jnilib"
  fi

  STATLIBSUFFIX=.a

  if [[ $PLATFORM == Linux-x86 ]]; then
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
SRCPRIVINCJSON="$SOURCE_ROOT/external/json"
SRCPRIVTESTJAR="$BUILD_ROOT/bin/com.microsoft.cognitiveservices.speech.tests.jar"

DESTPUBLIB="$DEST/public/lib"
DESTPUBLIBNET461="$DEST/public/lib/net461"
DESTPUBLIBNETSTANDARD20="$DEST/public/lib/netstandard2.0"
DESTPUBLIBUNIXOSNETSTANDARD20="$DEST/public/lib/UnixOS/netstandard2.0"
DESTPUBLIBIOSNETSTANDARD20="$DEST/public/lib/IOS/netstandard2.0"
DESTPUBINC="$DEST/public/include"
DESTPRIVLIB="$DEST/private/lib"
DESTPRIVLIBUNSTRIPPED="$DEST/private/libunstripped"
DESTPRIVBIN="$DEST/private/bin"
DESTPRIVINC="$DEST/private/include"
DESTPRIVINC2="$DEST/private/include.common"
DESTPRIVINCJSON="$DEST/private/external.json"

printf "\nCopying files to drop location\n"

# N.B. no long option for -p (parents) on OSX.
mkdir -p "$DESTPUBLIB" "$DESTPUBLIBNET461" "$DESTPUBLIBNETSTANDARD20" "$DESTPUBLIBUNIXOSNETSTANDARD20" "$DESTPUBLIBIOSNETSTANDARD20" "$DESTPUBINC" "$DESTPRIVLIB" "$DESTPRIVBIN" "$(dirname "$DESTPRIVINC")" "$(dirname "$DESTPRIVINC2")" "$(dirname "$DESTPRIVINCJSON")"  "$DESTPUBLIB"

# N.B. no long option for -v (verbose) and -p (preserve) on OSX.
CPOPT="-v -p"

# Copy public libraries.
# Note: On Mac, there are two versions of the dynamic libraries:
# - stripped and potentially signed (.dylib): copied here
# - unstripped and unsigned (.unstripped.dylib): copied to private destination below
shopt -s extglob
cp $CPOPT "$SRCDYNLIB"/$LIBPREFIX!(*.unstripped)$DYNLIBSUFFIX "$DESTPUBLIB"

# On Windows and not Android, copy import libraries, XMLDoc, and PDBs.
if [[ $OS == "Windows_NT" ]]; then
  if [[ $TARGET != Android-* ]]; then
    cp $CPOPT "$SRCLIB"/$LIBPREFIX*.lib "$DESTPUBLIB"
    cp $CPOPT "$SRCDYNLIB"/$LIBPREFIX*.pdb "$DESTPUBLIB"

    cp $CPOPT "$SRCDYNLIB"/net461/$LIBPREFIX*.{pdb,xml,dll} "$DESTPUBLIBNET461"
    cp $CPOPT "$SRCDYNLIB"/netstandard2.0/$LIBPREFIX*.{pdb,xml,dll} "$DESTPUBLIBNETSTANDARD20"
    cp $CPOPT "$SRCDYNLIB"UnixOS/netstandard2.0/$LIBPREFIX*.{pdb,xml,dll} "$DESTPUBLIBUNIXOSNETSTANDARD20"
    cp $CPOPT "$SRCDYNLIB"IOS/netstandard2.0/$LIBPREFIX*.{pdb,xml,dll} "$DESTPUBLIBIOSNETSTANDARD20"    
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
echo "uname is set to $uname"
if [[ $(uname) == Linux ]]; then
  echo "ISCROSS IS $ISCROSS, copying unstripped artifacts"
  cp $CPOPT -R "$DESTPUBLIB" "$DESTPRIVLIBUNSTRIPPED"
  if [[ $ISCROSS == true ]]; then
    echo "This is Crosscompiling for $PLATFORM!!!"
    if [[ $PLATFORM == Linux-arm32 ]]; then
      echo "Trying to use arm-linux-gneabihf-strip!"
      find "$DESTPUBLIB" \( -name \*.so -or -name \*.dll \) -print0 | xargs -0 arm-linux-gnueabihf-strip
    elif [[ $PLATFORM == Linux-arm64 ]]; then
      echo "Trying to use aarch64-linux-gnu-strip!"
      find "$DESTPUBLIB" \( -name \*.so -or -name \*.dll \) -print0 | xargs -0 aarch64-linux-gnu-strip
    elif [[ $PLATFORM =~ Windows-(x86|x64) ]]; then
      echo "Platform is $PLATFORM using strip!!!!"
      find "$DESTPUBLIB" \( -name \*.so -or -name \*.dll \) -print0 | xargs -0 strip
    fi
  else
    echo "Trying to use strip!"
    find "$DESTPUBLIB" \( -name \*.so -or -name \*.dll \) -print0 | xargs -0 strip
  fi
fi

# Android: strip shipping binaries, but keep the unstripped version around.
if [[ $TARGET == Android-* ]]; then
  cp $CPOPT -R "$DESTPUBLIB" "$DESTPRIVLIBUNSTRIPPED"
  if [[ $TARGET == Android-arm64 ]]; then
    STRIP=$ANDROID_NDK/toolchains/aarch64-linux-android-4.9/prebuilt/windows-x86_64/aarch64-linux-android/bin/strip.exe
  elif [[ $TARGET == Android-arm32 ]]; then
    STRIP=$ANDROID_NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64/arm-linux-androideabi/bin/strip.exe
  elif [[ $TARGET == Android-x86 ]]; then
    STRIP=$ANDROID_NDK/toolchains/x86-4.9/prebuilt/windows-x86_64/i686-linux-android/bin/strip.exe
  elif [[ $TARGET == Android-x64 ]]; then
    STRIP=$ANDROID_NDK/toolchains/x86_64-4.9/prebuilt/windows-x86_64/x86_64-linux-android/bin/strip.exe
  else
    echo Unsupported architecture $TARGET.
    exit 1
  fi
  find "$DESTPUBLIB" -name \*.so -print0 | xargs -0 $STRIP
fi

if [[ $PLATFORM = OSX-* ]]; then
  # osx: copy the unstripped dylib
  mkdir -p "$DESTPRIVLIBUNSTRIPPED"
  cp $CPOPT "$SRCDYNLIB"/$LIBPREFIX*.unstripped$DYNLIBSUFFIX "$DESTPRIVLIBUNSTRIPPED"
  cp $CPOPT "${SRCJAVABINDINGS%%jnilib}"unstripped.jnilib "$DESTPRIVLIBUNSTRIPPED"

  # copy dotnet dll
  SRCDYNLIB_DOTNETCORE="${SRCBIN}/${CONFIG}MacOS"
  cp $CPOPT "$SRCDYNLIB_DOTNETCORE"/netstandard2.0/$DOTNET_LIBPREFIX*.{pdb,xml,dll} "$DESTPUBLIBNETSTANDARD20"
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
cp $CPOPT "$SRCLIB"/*carbon-tts-mock* "$DESTPRIVLIB"

cp $CPOPT -R "$SRCPRIVINC" "$DESTPRIVINC"
cp $CPOPT -R "$SRCPRIVINC2" "$DESTPRIVINC2"
cp $CPOPT -R "$SRCPRIVINCJSON" "$DESTPRIVINCJSON"

