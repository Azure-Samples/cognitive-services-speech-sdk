#!/usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
set -u -x -e -o pipefail
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SOURCE_ROOT="$SCRIPT_DIR/../.."

USAGE="Usage: $0 image-tag speechsdk-dir arch sample-dir cmd-relative-to-root..."
IMAGE_TAG="${1?$USAGE}"
SPEECHSDK_DIR="${2?$USAGE}"
ARCH="${3?$USAGE}"
SAMPLE_DIR="${4?$USAGE}"
shift 4
[[ -d $SPEECHSDK_DIR ]]
[[ -d $SAMPLE_DIR ]]

SPEECHSDK_DIR_REL=$(realpath --relative-to "$SOURCE_ROOT" "$SPEECHSDK_DIR")
! [[ $SPEECHSDK_DIR_REL = ../* ]]

SAMPLE_DIR_REL=$(realpath --relative-to "$SOURCE_ROOT" "$SAMPLE_DIR")
! [[ $SAMPLE_DIR_REL = ../* ]]

# We're trying to be clean wrt. special characters in file names, but note that
# they will break in the Makefile anyway.

# Remove old builds
git clean -fdx "$SAMPLE_DIR"

DOCKER_CMD=(docker run --rm --volume "$(readlink -f "$SOURCE_ROOT"):/test" --workdir /test)

"${DOCKER_CMD[@]}" "$IMAGE_TAG" \
  make -C "$SAMPLE_DIR_REL" SPEECHSDK_ROOT="/test/$SPEECHSDK_DIR_REL" TARGET_PLATFORM="$ARCH"

"${DOCKER_CMD[@]}" --env "LD_LIBRARY_PATH=/test/$SPEECHSDK_DIR_REL/lib/$ARCH" "$IMAGE_TAG" \
  ./ci/samples/wrap-pulseaudio.sh "$@"
