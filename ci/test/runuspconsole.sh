#!/bin/bash

[ $# -ne 5 ] && echo -e "Usage: runuspconsole binary_dir keySpeech keyCris keyLuis keySkyman" && exit 1

BINARY_DIR=$1
UserKeySpeech=$2
UserKeyCris=$3
UserKeyLuis=$4
UserKeySkyman=$5

set -e

pretty_print() {
  printf '%.0s=' {1..78}; echo
  echo $1
  printf '%.0s=' {1..78}; echo
}

pretty_print "ENTERING runuspconsole.sh"

./tests/e2e_uspconsole.sh $BINARY_DIR $UserKeySpeech $UserKeyCris $UserKeyLuis $UserKeySkyman
