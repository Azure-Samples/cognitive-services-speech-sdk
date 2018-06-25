#!/bin/bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#

set -e -o pipefail

SCRIPT_NAME="$(basename "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(readlink -f "$(dirname "${BASH_SOURCE[0]}")")"

. "$SCRIPT_DIR/functions.sh"

function docker-build {
  local usage="Usage: $0 <tag> <directory>"
  local tag=${1?$usage}
  local directory=${2?$usage}
  shift 2

  local old_image=$(docker images -q "$tag")

  docker build --force-rm --tag "$tag" "$@" "$directory"

  local new_image=$(docker images -q "$tag")

  # Image was updated, try to delete the old version (ignoring errors)
  if [[ -n $old_image && $old_image != $new_image ]]; then
    docker rmi "$old_image" || true
  fi
}

USAGE="Usage: $0 <tag> <directory>"
TAG=${1?$USAGE}
DIRECTORY=${2?$USAGE}

[[ -d $DIRECTORY ]] || exitWithError "Directory '%s' does not exist\n" "$d"

"$SCRIPT_DIR/build-dockerfile.sh" "$DIRECTORY"

shift 2

docker-build "$TAG" "$DIRECTORY" "$@"
