#!/bin/bash
# To be sourced from depending scripts.

SCRIPT_NAME="$(basename "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(readlink -f "$(dirname "${BASH_SOURCE[0]}")")"

function getTagFromDir {
  local DIR TAG
  DIR="$(readlink -f "$1")"

  if [[ -d $DIR && -r $DIR/buildspec.txt ]]; then
    TAG="${DIR//\//_}"
    TAG="csspeechdev_${TAG:$((${#SCRIPT_DIR} + 1))}"
    printf "%s\n" $TAG
  else
    false
  fi
}

function exitWithError {
  printf "$@" 1>&2
  exit 1
}
