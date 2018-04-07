#!/bin/bash

[ $# -ne 5 ] && echo -e "Usage: rununittest binary_dir keySpeech keyCris keyLuis keySkyman" && exit 1

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

pretty_print "ENTERING rununittest.sh"

pretty_print "Running usp_tests"
./tests/unit_tests.sh $BINARY_DIR usp_tests $UserKeySpeech $UserKeyCris $UserKeyLuis $UserKeySkyman

pretty_print "Running cxx_api_tests"
./tests/unit_tests.sh $BINARY_DIR cxx_api_tests $UserKeySpeech $UserKeyCris $UserKeyLuis $UserKeySkyman

pretty_print "Python test currently disabled"
#pretty_print "Running Python tests"
#py.test -s ./bindings/python/test.py --filename whatstheweatherlike.wav
