#!/bin/bash

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

# Get some helpers
. "$SCRIPT_DIR/../functions.sh"

set -x -e -o pipefail
USAGE="Usage: $0 version output-dir"
# N.B. VERSION must not contain any magic characters for replacement.
VERSION="${1?$USAGE}"
OUTPUT_DIR="${2?$USAGE}"
[ -d samples ]
git clean -fdx samples

BASE_NAME="SpeechSDK-LinuxSamples-$VERSION"
OUTPUT_TAR="$OUTPUT_DIR/$BASE_NAME.tar.gz"

[[ -d $BASE_NAME ]] && rm -rf "$BASE_NAME"

mkdir "$BASE_NAME"
cp --preserve license.md ThirdPartyNotices.md "$BASE_NAME"
git clean -fdx samples
cp --preserve --recursive samples/cxx/linux/quickstart "$BASE_NAME"

tar \
  --verbose \
  --mode='uga+rw' \
  --owner root \
  --group root  \
  --mtime=now \
  -cvzf "$OUTPUT_TAR" \
  "$BASE_NAME"

SHA256_SUM="$(sha256sum "$OUTPUT_TAR" | cut -f1 -d' ')"

set +x

echo ::: Archive list of files
tar -tzf "$OUTPUT_TAR"

echo ::: Checksum
# N.B. two spaces in the next line intentional
echo "$SHA256_SUM  $BASE_NAME.tar.gz"

vsts_setvar SAMPLES_SHA256SUM "$SHA256_SUM"
