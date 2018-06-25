#!/bin/bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
# This script is run through build-and-smoke-test.sh. The source root repository
# is mapped as a volume (/csspeech), the subscription key is provided as an environment
# variable ($SPEECH_SUBSCRIPTION_KEY).

set -e -o pipefail -x

SCRIPT_NAME="$(basename "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(readlink -f "$(dirname "${BASH_SOURCE[0]}")")"

. "$SCRIPT_DIR/functions.sh"

SOURCE_DIRECTORY="$(readlink -f "$SCRIPT_DIR/../..")"
BUILD_CONFIGURATION="${1:-Debug}"
BUILD_DIRECTORY="${2:-HOME/build-tmp}"

[[ -d $BUILD_DIRECTORY ]] && exitWithError "Directory '%s' DOES exist\n" "$BUILD_DIRECTORY"

mkdir -p "$BUILD_DIRECTORY"

cd "$BUILD_DIRECTORY"

cmake -DCMAKE_BUILD_TYPE="$BUILD_CONFIGURATION" "$SOURCE_DIRECTORY"

cmake --build . -- -j$(nproc)

./bin/carbonx --speech --input:/csspeech/tests/input/audio/whatstheweatherlike.wav --subscription:$SPEECH_SUBSCRIPTION_KEY
