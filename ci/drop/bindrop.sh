#!/bin/bash

USAGE="Usage: $0 platform configuration destination"

PLATFORM="${1?$USAGE}"
CONFIG="${2?$USAGE}"
DEST="${3?$USAGE}"

set -e -x

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

# Ensure Unix paths on Windows from here on
if [[ $OS = "Windows_NT" ]]; then
  DEST="$(cygpath --unix --absolute "$DEST")"
  SCRIPT_DIR="$(cygpath --unix --absolute "$SCRIPT_DIR")"
fi

SOURCE_ROOT="$SCRIPT_DIR/../.."
BUILD_ROOT="$SOURCE_ROOT/build/$PLATFORM"

SRCJAR="$BUILD_ROOT/bindings/java/carbon_java_lib.jar"

if [[ $OS = "Windows_NT" ]]; then
  LIBPREFIX=carbon
  DYNLIBSUFFIX=.dll
  STATLIBSUFFIX=.lib

  SRCBIN="$BUILD_ROOT/bin/$CONFIG"
  SRCLIB="$BUILD_ROOT/lib/$CONFIG"
  SRCDYNLIB="$BUILD_ROOT/bin/$CONFIG"
else
  LIBPREFIX=libcarbon

  if [[ $(uname) = Linux ]]; then
    DYNLIBSUFFIX=.so
  else
    DYNLIBSUFFIX=.dylib
  fi

  STATLIBSUFFIX=.a

  SRCBIN="$BUILD_ROOT/bin"
  SRCLIB="$BUILD_ROOT/lib"
  SRCDYNLIB="$BUILD_ROOT/lib"
fi

SRCINC="$SOURCE_ROOT/source/public"
SRCPRIVINC="$SOURCE_ROOT/source/core/include"

DESTPUBLIB="$DEST/public/lib"
DESTPUBINC="$DEST/public/include"
DESTPRIVLIB="$DEST/private/lib"
DESTPRIVINC="$DEST/private/include"

printf "\nCopying files to drop location\n"

# N.B. no long option for -p (parents) on OSX.
mkdir -p "$DESTPUBLIB" "$(dirname "$DESTPUBINC")" "$DESTPRIVLIB" "$(dirname "$DESTPRIVINC")"

# N.B. no long option for -v (verbose) and -p (preserve) on OSX.
CPOPT="-v -p"

cp $CPOPT "$SRCDYNLIB"/$LIBPREFIX*$DYNLIBSUFFIX "$DESTPUBLIB"
# On Windows, copy import libraries
[[ $OS = "Windows_NT" ]] && cp $CPOPT "$SRCLIB"/$LIBPREFIX*.lib "$DESTPUBLIB"

# Copy .jar if available
[[ -e $SRCJAR ]] && cp $CPOPT "$SRCJAR" "$DESTPUBLIB"

# N.B. no long option for -R (recursive) on OSX.
cp $CPOPT -R "$SRCINC"* "$DESTPUBINC"

# N.B. Using '-I -n 1' and replacement instead of "cp --target" since --target
# is not available on OSX.
find "$SRCLIB" -type f -name \*$STATLIBSUFFIX -not -name $LIBPREFIX\* -print0 |
  xargs -n 1 -0 -I % cp % "$DESTPRIVLIB"

cp $CPOPT -R "$SRCPRIVINC" "$DESTPRIVINC"
