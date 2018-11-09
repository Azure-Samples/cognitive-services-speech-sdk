#!/bin/bash
set -x -e -o pipefail
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SOURCE_ROOT="$SCRIPT_DIR/../.."

USAGE="Usage: $0 image-tag release-drop arch"
IMAGE_TAG="${1?$USAGE}"
RELEASE_DROP="${2?$USAGE}"
ARCH="${3?$USAGE}"
[[ -f $RELEASE_DROP ]]

# Build OOBE image

TEST_DIR="$(mktemp --directory --tmpdir="$SCRIPT_DIR" oobetest-XXXXXX)"
trap '[[ -d $TEST_DIR ]] &&  rm -rf $TEST_DIR' EXIT

mkdir "$TEST_DIR/speechsdk"
tar -xzf "$RELEASE_DROP" --strip-components=1 -C "$TEST_DIR/speechsdk"
cp -av "$SOURCE_ROOT/public_samples/quickstart/cpp-linux" "$TEST_DIR"
cp -p "$SCRIPT_DIR/../run-with-pulseaudio.sh" "$TEST_DIR/cpp-linux"
cp -p "$SOURCE_ROOT/tests/input/audio/whatstheweatherlike.wav" "$TEST_DIR/cpp-linux"
cp -p "$SOURCE_ROOT/ci/quickstart-e2e.expect" "$TEST_DIR/cpp-linux"

perl -i -pe 's(SPEECHSDK_ROOT:=.*)(SPEECHSDK_ROOT:=/test/speechsdk)' "$TEST_DIR/cpp-linux/Makefile"
perl -i -pe 's(TARGET_PLATFORM:=.*)(TARGET_PLATFORM:='"$ARCH"')' "$TEST_DIR/cpp-linux/Makefile"

DOCKER_CMD=(docker run --rm --volume "$(readlink -f "$TEST_DIR"):/test" --workdir /test/cpp-linux)

"${DOCKER_CMD[@]}" --interactive "$IMAGE_TAG" bash - <<SCRIPT
set -e -x -o pipefail
make
export LD_LIBRARY_PATH=/test/speechsdk/lib/$ARCH
PATH_TO_AUDIO=whatstheweatherlike.wav
pulseaudio -D --exit-idle-time=-1
sleep 1.5
pactl load-module module-null-sink sink_name=MicOutput sink_properties=device.description=Virtual_Microphone_Output
pacmd load-module module-virtual-source source_name=VirtualMic
trap 'pulseaudio --kill' EXIT
# Note: this quickstart does not wait for a key press at the end.
./quickstart-e2e.expect \$PATH_TO_AUDIO "What's the weather like?" 1 ./helloworld
SCRIPT
