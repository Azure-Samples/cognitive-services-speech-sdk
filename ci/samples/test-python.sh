#!/usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
set -u -x -e -o pipefail
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SOURCE_ROOT="$SCRIPT_DIR/../.."

USAGE="Usage: $0 image-tag wheel-dir cmd-relative-to-root..."
IMAGE_TAG="${1?$USAGE}"
WHEEL_DIR="${2?$USAGE}"
shift 2

TESTCMD_ESCAPED="$(printf "%q " "$@")"

[[ -d $WHEEL_DIR ]]
WHEEL_DIR_REL=$(realpath --relative-to "$SOURCE_ROOT" "$WHEEL_DIR")
! [[ $WHEEL_DIR_REL = ../* ]]

WHEEL_DIR_ESCAPED=$(printf "%q" "$WHEEL_DIR_REL")

docker run --rm --interactive \
  --volume "$(readlink -f "$SOURCE_ROOT"):/test" \
  --workdir /test \
  "$IMAGE_TAG" \
  bash - <<SCRIPT
set -u -x -e -o pipefail
MAJORMINOR=\$(python3 -c "import sys; print('%s%s' % (sys.version_info[0:2]))")
pip3 install $WHEEL_DIR_ESCAPED/azure_cognitiveservices_speech-*-cp\$MAJORMINOR-*linux*_x86_64.whl
# Note: as an alternative when running into issues with Ubuntu's default python3/pip3, do
# "pip3 install --user --upgrade pip", then use ~/.local/bin/pip.
./ci/samples/wrap-pulseaudio.sh $TESTCMD_ESCAPED
SCRIPT
