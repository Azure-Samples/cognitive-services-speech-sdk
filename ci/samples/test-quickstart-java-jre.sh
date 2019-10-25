#!/usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
set -x -e -o pipefail
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SOURCE_ROOT="$SCRIPT_DIR/../.."

# Note: this script assumes that the code is already built.

USAGE="Usage: $0 image-tag sdk-jar"
IMAGE_TAG="${1?$USAGE}"
SDK_JAR="${2?$USAGE}"
[[ -f $SDK_JAR ]]
SDK_JAR_REL=$(realpath --relative-to "$SOURCE_ROOT" "$SDK_JAR")
! [[ $SDK_JAR_REL = ../* ]]

docker run --rm \
  --volume "$(readlink -f "$SOURCE_ROOT"):/test" \
  --workdir /test \
  "$IMAGE_TAG" \
  ./ci/samples/wrap-pulseaudio.sh \
    ./ci/samples/quickstart-e2e.expect tests/input/audio/whatstheweatherlike.wav "What's the weather like?" \
      java -cp public_samples/quickstart/java/jre/from-microphone/target/quickstart-eclipse-1.0.0-SNAPSHOT.jar:"$SDK_JAR_REL" speechsdk.quickstart.Main
