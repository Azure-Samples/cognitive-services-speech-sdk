#!/bin/bash
set -x -e -o pipefail
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SOURCE_ROOT="$SCRIPT_DIR/../.."

USAGE="Usage: $0 [--smoke-test] release-drop"
SMOKE_TEST=0
if [[ $1 == --smoke-test ]]; then
  SMOKE_TEST=1
  shift
fi

RELEASE_DROP="${1?$USAGE}"
[[ -f $RELEASE_DROP ]]

# Build OOBE image
DOCKER_TAG=csspeechdev_linuxoobe_x64
DOCKER_DIR="$SOURCE_ROOT/ci/docker/linuxoobe/x64"
"$SOURCE_ROOT/ci/docker/build-image.sh" "$DOCKER_TAG" "$DOCKER_DIR"

TEST_DIR="$(mktemp --directory --tmpdir="$SCRIPT_DIR" oobetest-XXXXXX)"
trap '[[ -d $TEST_DIR ]] &&  rm -rf $TEST_DIR' EXIT

mkdir "$TEST_DIR/speechsdk"
tar -xzf "$RELEASE_DROP" --strip-components=1 -C "$TEST_DIR/speechsdk"
cp -p "$SOURCE_ROOT/tests/input/audio/whatstheweatherlike.wav" "$TEST_DIR"
cp -av "$SOURCE_ROOT/samples/cxx/linux/quickstart" "$TEST_DIR"

perl -i -pe 's(SPEECHSDK_ROOT:=.*)(SPEECHSDK_ROOT:=/test/speechsdk)' "$TEST_DIR/quickstart/Makefile"
perl -i -pe 's(L"YourSubscriptionKey")(L"'$SPEECH_SUBSCRIPTION_KEY'")' "$TEST_DIR/quickstart/"*.cpp
perl -i -pe 's(L"YourServiceRegion")(L"westus")' "$TEST_DIR/quickstart/"*.cpp
perl -i -pe 's(L"YourAudioFile.wav")(L"/test/whatstheweatherlike.wav")' "$TEST_DIR/quickstart/"*.cpp
perl -i -pe 's(//#define FROM_FILE)(#define FROM_FILE)' "$TEST_DIR/quickstart/"*.cpp

if [[ $SMOKE_TEST == 1 ]]; then
  docker run --rm --volume "$(readlink -f "$TEST_DIR"):/test" --workdir /test/quickstart "$DOCKER_TAG" bash -c 'make && echo | LD_LIBRARY_PATH=/test/speechsdk/lib/x64 ./quickstart-linux'
else
  docker run --rm --volume "$(readlink -f "$TEST_DIR"):/test" --workdir /test/quickstart "$DOCKER_TAG" make
fi
