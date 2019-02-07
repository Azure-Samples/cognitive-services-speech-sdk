#!/bin/bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
set -u -x -e -o pipefail
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SOURCE_ROOT="$SCRIPT_DIR/../.."

USAGE="Usage: $0 image-tag speechsdk-dir arch"
IMAGE_TAG="${1?$USAGE}"
SPEECHSDK_DIR="${2?$USAGE}"
ARCH="${3?$USAGE}"
[[ -d $SPEECHSDK_DIR ]]

exec "$SCRIPT_DIR/test-cpp.sh" "$IMAGE_TAG" "$SPEECHSDK_DIR" "$ARCH" "$SOURCE_ROOT/public_samples/samples/cpp/windows/console/samples" \
  ./ci/samples/test-samples-cpp-console.expect \
    tests/input \
    ./public_samples/samples/cpp/windows/console/samples/sample
