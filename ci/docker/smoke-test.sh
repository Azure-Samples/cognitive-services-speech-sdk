#!/bin/bash

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

"$SOURCE_DIRECTORY/tests/scripts/download_audio.sh"

./bin/carbonx --speech --input:./whatstheweatherlike.wav --subscription:$SPEECH_SUBSCRIPTION_KEY
