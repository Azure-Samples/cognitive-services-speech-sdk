#!/bin/bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#

set -e -o pipefail

SCRIPT_NAME="$(basename "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(readlink -f "$(dirname "${BASH_SOURCE[0]}")")"

. "$SCRIPT_DIR/functions.sh"

# TODO ubuntu arm32v7 is currently disabled. Hosted agents
#      do not have necessary kernel support.
# TODO cp /usr/bin/qemu-arm-static "${SCRIPT_DIR}/linux/arm32v7"

readarray -t TARGETS < <(find "$SCRIPT_DIR" -name buildspec.txt -printf '%h\n')

for dir in "${TARGETS[@]}"; do
     TAG="$(getTagFromDir "$dir")"
     "$SCRIPT_DIR/build-image.sh" "$TAG" "$dir" "$@"
done
