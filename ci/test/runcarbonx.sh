#!/bin/bash

[ $# -ne 5 ] && echo -e "Usage: runcarbonx binary_dir keySpeech keyCris keyLuis keySkyman" && exit 1

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

pretty_print "ENTERING runcarbonx.sh"

pretty_print "Running carbonx *speech*"
./tests/e2e_carbonx.sh $BINARY_DIR speech $UserKeySpeech $UserKeyCris $UserKeyLuis $UserKeySkyman

if [ "${AGENT_OS}" == "Darwin" ]; then
   pretty_print "Running carbonx *intent* DISABLED - bug 1203375"
else
  if [ "${AGENT_OS}" == "Windows_NT" ] && [ "${BUILDPLATFORM}" == "Win32" ]; then
    pretty_print "Running carbonx *intent* DISABLED - Fix bug 1203729"
  else
    pretty_print "Running carbonx *speech*"
    ./tests/e2e_carbonx.sh $BINARY_DIR intent $UserKeySpeech $UserKeyCris $UserKeyLuis $UserKeySkyman
  fi
fi
