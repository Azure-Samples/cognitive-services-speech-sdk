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

function run_tests {

    if [ ! -z $endpoint ]; then
        pretty_print "Running unit tests against local test server ($endpoint)"
    else
        pretty_print "Running unit tests against prod"
    fi
    
    pretty_print "Running usp_tests"
    ./tests/unit_tests.sh $BINARY_DIR usp_tests $UserKeySpeech $UserKeyCris $UserKeyLuis $UserKeySkyman $endpoint

    pretty_print "Running cxx_api_tests"
    ./tests/unit_tests.sh $BINARY_DIR cxx_api_tests $UserKeySpeech $UserKeyCris $UserKeyLuis $UserKeySkyman $endpoint

    if [ "${AGENT_OS}" == "Windows_NT" ] && [ "${BUILDPLATFORM}" == "Win32" ]; then
        : # we don't build python bindings on Win32
    else
        pretty_print "Running Python tests"
        py_args="--filename ./tests/input/whatstheweatherlike.wav --subscription $UserKeySpeech"
        [ ! -z $endpoint ] && py_args+=" --endpoint $endpoint"
        py.test -s ./bindings/python/test.py $py_args
    fi
}

pretty_print "ENTERING rununittest.sh"

#pushd ./tests 
#npm install
#nohup node test-server.js > /dev/null 2>&1 &
#SERVER_PID=$!
#popd

#trap 'kill $SERVER_PID' EXIT

#sleep 1s

#if ps -p $SERVER_PID > /dev/null
#then
#    pretty_print "local test server is up and running (pid: $SERVER_PID)"
#    endpoint="ws://localhost:8080/"
#else 
#    echo "Couldn't start the local test server"
#    endpoint=""
#fi

run_tests

#kill -INT $SERVER_PID
#trap '' EXIT

#if [ ! -z $endpoint ]; then
#     endpoint=""
#     run_tests
#fi

