#!/usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
set -u -x -e -o pipefail
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SOURCE_ROOT="$SCRIPT_DIR/../.."

USAGE="Usage: $0 image-tag wheel-dir"
IMAGE_TAG="${1?$USAGE}"
WHEEL_DIR="${2?$USAGE}"

[[ -d $WHEEL_DIR ]]

exec "$SCRIPT_DIR/test-python.sh" "$IMAGE_TAG" "$WHEEL_DIR" \
  ./ci/samples/test-samples-python-console.expect tests/input \
    python3 public_samples/samples/python/console/main.py
