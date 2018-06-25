#!/bin/bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#

set -e -o pipefail

SCRIPT_NAME="$(basename "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(readlink -f "$(dirname "${BASH_SOURCE[0]}")")"

. "$SCRIPT_DIR/functions.sh"

# Pre-requisites (TODO avoid hardcoding)

# TODO cp /usr/bin/qemu-arm-static "${SCRIPT_DIR}/linux/arm32v7"

USAGE="Usage: $0 <directory> [<build-arguments>...]"
DIR=${1?$USAGE}
TAG="$(getTagFromDir "$DIR")"
shift 1

"$SCRIPT_DIR/build-image.sh" "$TAG" "$DIR" "$@"
