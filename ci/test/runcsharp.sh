#!/bin/bash

[ $# -ne 5 ] && echo -e "Usage: runcsharp binary_dir keySpeech keyCris keyLuis keySkyman" && exit 1

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

pretty_print "ENTERING runcsharp.sh"

pretty_print "Running carbon_csharp *speech*"
./tests/e2e_carbon_csharp.sh $BINARY_DIR speech $UserKeySpeech $UserKeyCris $UserKeyLuis $UserKeySkyman

# Disabling due to synthesis service being down
#pretty_print "Running carbon_csharp *translation*"
#./tests/e2e_carbon_csharp.sh $BINARY_DIR translation $UserKeySpeech $UserKeyCris $UserKeyLuis $UserKeySkyman

#pretty_print "Running carbon_csharp *intent*"
#./tests/e2e_carbon_csharp.sh $BINARY_DIR intent $UserKeySpeech $UserKeyCris $UserKeyLuis $UserKeySkyman
