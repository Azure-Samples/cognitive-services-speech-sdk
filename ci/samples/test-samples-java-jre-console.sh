#!/bin/bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
set -u -x -e -o pipefail
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SOURCE_ROOT="$SCRIPT_DIR/../.."

# Note: this script assumes that the code is already built.

USAGE="Usage: $0 image-tag sdk-jar"
IMAGE_TAG="${1?$USAGE}"
SDK_JAR="${2?$USAGE}"
[[ -f $SDK_JAR ]]
SDK_JAR_REL=$(realpath --relative-to "$SOURCE_ROOT" "$SDK_JAR")
! [[ $SDK_JAR_REL = ../* ]]

# Note: don't wait for keypress. Ignores crashes after successful reco.
docker run --rm \
  --volume "$(readlink -f "$SOURCE_ROOT"):/test" \
  --workdir /test \
  "$IMAGE_TAG" \
  ./ci/samples/wrap-pulseaudio.sh \
    ./ci/samples/test-samples-java-jre-console.expect tests/input \
      java -cp public_samples/samples/java/jre/console/target/SpeechSDKDemo-0.0.1-SNAPSHOT.jar:"$SDK_JAR_REL" com.microsoft.cognitiveservices.speech.samples.console.Main
