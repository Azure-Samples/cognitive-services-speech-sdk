#!/bin/bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#

set -e -o pipefail

SCRIPT_NAME="$(basename "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(readlink -f "$(dirname "${BASH_SOURCE[0]}")")"
SNIP_DIR="$SCRIPT_DIR/snippets"

. "$SCRIPT_DIR/functions.sh"

for d in "$@"; do

  [[ -d $d ]] || exitWithError "Directory '%s' does not exist\n" "$d"

  BUILD_SPEC_PATH="$d/buildspec.txt"
  DOCKERFILE_PATH="$d/Dockerfile"

  [[ -f $BUILD_SPEC_PATH ]] ||
    exitWithError "Could not find build spec file '%s'\n" "$BUILD_SPEC_PATH"

  readarray -t buildSpec < "$BUILD_SPEC_PATH"

  FIRST=1
  for b in "${buildSpec[@]}"; do
     [[ -z $b ]] && continue

     SNIP_PATH="$SNIP_DIR/$b.txt"
     [[ -f $SNIP_PATH ]] ||
       exitWithError "Could not find snippet '%s' at path '%s'\n" "$b" "$SNIP_DIR"
     (( FIRST = 1 )) && {
       printf "\n"
       FIRST=0
     }
     cat "$SNIP_PATH"
  done > "$DOCKERFILE_PATH"

  printf "buildspec.txt\n" > "$d/.dockerignore"

done
