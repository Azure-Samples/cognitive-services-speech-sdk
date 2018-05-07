#!/bin/bash
set -x -e -o pipefail
USAGE="Usage: $0 version repo-dir drop-dir output-dir"
# N.B. VERSION must not contain any magic characters for replacement.
VERSION="${1?$USAGE}"
REPO_DIR="${2?$USAGE}"
# N.B. script it can mess in $DROP_DIR (creates an additional dir)
DROP_DIR="${3?$USAGE}"
OUTPUT_DIR="${4?$USAGE}"

# Normalize all directories
REPO_DIR="$(readlink -f "$REPO_DIR")"
DROP_DIR="$(readlink -f "$DROP_DIR")"
[[ -d $OUTPUT_DIR ]] || mkdir -p "$OUTPUT_DIR"
OUTPUT_DIR="$(readlink -f "$OUTPUT_DIR")"

BASE_NAME="SpeechSDK-Linux-$VERSION"

cd "$DROP_DIR"

mkdir "$BASE_NAME"
cp -p "$REPO_DIR/"{license.md,ThirdPartyNotices.md} "$BASE_NAME"

echo ::: List of files
tar \
  --verbose \
  --mode='uga+rw' \
  --owner root \
  --group root  \
  --mtime=now \
  --transform "s:^Linux-\(x86\|x64\)/Release/public/include/:$BASE_NAME/include/:" \
  --transform "s:^Linux-\(x86\|x64\)/Release/public/lib/:$BASE_NAME/lib/\1/:" \
  -cvzf "$OUTPUT_DIR/$BASE_NAME.tar.gz" \
  "$BASE_NAME" \
  Linux-x86/Release/public/include/* \
  Linux-x*/Release/public/lib/libMicrosoft.CognitiveServices.Speech.core.so

SHA256_SUM="$(sha256sum "$OUTPUT_DIR/$BASE_NAME.tar.gz" | cut -f1 -d' ')"

set +x

echo ::: Archive list of files
tar -tzf "$OUTPUT_DIR/$BASE_NAME.tar.gz"

echo ::: Checksum
# N.B. two spaces in the next line intentional
echo "$SHA256_SUM  $BASE_NAME.tar.gz"

function vsts_setvar {
   echo "##vso[task.setvariable variable=$1;isOutput=true]$2";
}

vsts_setvar RELEASE_SHA256SUM "$SHA256_SUM"
