#!/bin/bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#

set -e -o pipefail -x

SCRIPT_NAME="$(basename "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(readlink -f "$(dirname "${BASH_SOURCE[0]}")")"

. "$SCRIPT_DIR/functions.sh"

SOURCE_DIRECTORY="$SCRIPT_DIR/../.."

[[ -n $SPEECH_SUBSCRIPTION_KEY ]] || exitWithError '$SPEECH_SUBSCRIPTION_KEY not set, please specify\n'

USAGE="Usage: $0 <dir> <build-configuration> <build-directory-relative-to-repo-or-absolute-in-container>"
# TODO should be allow relative to this dir for DIR?
DIR=${1?$USAGE}
BUILD_CONFIGURATION=${2?$USAGE}
BUILD_DIRECTORY=${3?$USAGE}

TAG="$(getTagFromDir "$DIR")"

SPEECH_SUBSCRIPTION_KEY=$SPEECH_SUBSCRIPTION_KEY \
docker run \
  --rm \
  --volume "$SOURCE_DIRECTORY":/csspeech \
  --env SPEECH_SUBSCRIPTION_KEY \
  --workdir /csspeech \
  "$TAG" \
  /csspeech/ci/docker/smoke-test.sh "$BUILD_CONFIGURATION" "$BUILD_DIRECTORY"
