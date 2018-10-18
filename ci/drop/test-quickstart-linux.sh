#!/bin/bash
set -x -e -o pipefail
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SOURCE_ROOT="$SCRIPT_DIR/../.."

USAGE="Usage: $0 [--smoke-test] speech-region image-tag release-drop"
SMOKE_TEST=0
if [[ $1 == --smoke-test ]]; then
  SMOKE_TEST=1
  shift
fi
SPEECH_REGION="${1?$USAGE}"
IMAGE_TAG="${2?$USAGE}"
RELEASE_DROP="${3?$USAGE}"
[[ -f $RELEASE_DROP ]]

[[ -z $SPEECH_SUBSCRIPTION_KEY ]] && {
  echo Environment variable SPEECH_SUBSCRIPTION_KEY not set.
  exit 1
}

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
perl -i -pe 's("YourSubscriptionKey")("'$SPEECH_SUBSCRIPTION_KEY'")' "$TEST_DIR/cpp-linux/"*.cpp
perl -i -pe 's("YourServiceRegion")("'$SPEECH_REGION'")' "$TEST_DIR/cpp-linux/"*.cpp

DOCKER_CMD=(docker run --rm --volume "$(readlink -f "$TEST_DIR"):/test" --workdir /test/cpp-linux)

if [[ $SMOKE_TEST == 1 ]]; then

  "${DOCKER_CMD[@]}" --interactive "$IMAGE_TAG" bash - <<'SCRIPT'
set -e -x -o pipefail
DEBIAN_FRONTEND=noninteractive apt-get install --quiet --no-install-recommends --yes expect
make
export LD_LIBRARY_PATH=/test/speechsdk/lib/x64
PATH_TO_AUDIO=whatstheweatherlike.wav
pulseaudio -D --exit-idle-time=-1
sleep 1.5
pactl load-module module-null-sink sink_name=MicOutput sink_properties=device.description=Virtual_Microphone_Output
pacmd load-module module-virtual-source source_name=VirtualMic
trap 'pulseaudio --kill' EXIT
./quickstart-e2e.expect $PATH_TO_AUDIO "What's the weather like?" ./helloworld
SCRIPT

else
  "${DOCKER_CMD[@]}" "$IMAGE_TAG" make
fi
