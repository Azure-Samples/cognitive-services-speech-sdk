#!/bin/bash

set -e -u -o pipefail

function retry() {
  local maxRetries retry usage
  usage="Usage: ${FUNCNAME[0]} <num-retries> <cmd...>"
  numRetries="${1?$usage}"
  shift 1
  retry=0
  while ((retry++ <= numRetries)); do
    "$@" && return 0 \
      || true # in case we run with "set -e"
  done
  printf "Command didn't succeed with $numRetries retries: %s\n" "$*" 1>&2
  return 1
}

function on_exit() {
  local exit_code=$?
  ! [[ -d $TEMP_DIR ]] || rm -rf "$TEMP_DIR"
  exit $exit_code
}

USAGE="Usage: downloader.sh <url> <sha256> <extract> <dest-dir> <store-facl>"
# N.B. we assume $URL has no query parameters
URL=${1?$USAGE}
SHA256=${2?$USAGE}
EXTRACT=${3?$USAGE}
DEST_DIR=${4?$USAGE}
STORE_FACL=${5?$USAGE}

! [[ -e $DEST_DIR ]] || {
  echo A file/directory \'$DEST_DIR\' already exists, remove first.
  exit 1
}

TEMP_DIR=$(mktemp --directory dl-XXXXXX)

cd "$TEMP_DIR"

trap on_exit EXIT

TEMP_FILE=$(mktemp dl-XXXXXX)

echo Downloading \'$URL\'...
retry 2 curl --continue-at - "$URL" --output "$TEMP_FILE"

echo Checking SHA256...
echo "$SHA256  $TEMP_FILE" | sha256sum --check --strict -

if [[ $EXTRACT = true ]]; then
  echo Extracting...
  TEMP_EXTRACT_DIR=$(mktemp --directory dl-XXXXXX.dir)
  case $URL in
    *.tar.bz2)
      tar -xj -C "$TEMP_EXTRACT_DIR" -f "$TEMP_FILE"
      ;;
    *.tar.gz)
      tar -xz -C "$TEMP_EXTRACT_DIR" -f "$TEMP_FILE"
      ;;
    *)
      echo Unsupported file format for extraction.
      exit 1
  esac

  # Rename to desired artifact name.
  # If there's just a single directory at the top-level, remove it.

  shopt -s nullglob dotglob
  extractedTopLevelFiles=("$TEMP_EXTRACT_DIR/"*)
  extractedTopLevelDirectories=("$TEMP_EXTRACT_DIR/"*/)

  numberToLevelFiles=${#extractedTopLevelFiles[@]}
  numberToLevelDirectories=${#extractedTopLevelDirectories[@]}

  # TODO do we need this for multiple levels down?

  if ((numberToLevelFiles == 1 && numberToLevelDirectories == 1)); then
    echo Exactly one directory in the root. Moving up.
    mv "$TEMP_EXTRACT_DIR/"* "../$DEST_DIR"
  else
    mv "$TEMP_EXTRACT_DIR" "../$DEST_DIR"
  fi
else
  # No extract. Just move into destination directory.
  mkdir "../$DEST_DIR"
  mv "$TEMP_FILE" "../$DEST_DIR/"$(basename $URL)
fi

cd ..

# Archive permissions
# (doesn't make much sense without extraction, but it's possible)
if [[ $STORE_FACL == true ]]; then
  cd "$DEST_DIR"
  ! [[ -e .facl ]] || {
    echo Cannot store access permission in .facl, file is already present.
    exit 1
  }
  echo Storing access permission in the .facl file
  getfacl -R . > .facl
  cd ..
fi

echo Success. Downloaded to \'$DEST_DIR\' directory.

find "$DEST_DIR" -ls
