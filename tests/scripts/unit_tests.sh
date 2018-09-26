#!/bin/bash

function die {
    set +x
    echo -e $1
    exit 1
}

function run_usp_tests {
    echo "Run usp tests."
    (set -x; $USP_TESTS_EXE $args --regionId westus --reporter junit --out test-usp_tests-$AGENT_OS.xml --success)
    echo -e "\n"
}

function run_cxx_api_tests {
    echo "Run cxx_api tests"
    (set -x; $CXX_API_TESTS_EXE $args --keyLUIS $UserKeyLuis --luisAppId $TEST_INTENT_HOMEAUTOMATION_APPID --regionId westus --reporter junit --out test-cxx_api-$AGENT_OS.xml --success)
    echo -e "\n"
}

set -e

[ $# -lt 4 ] && die "Usage: unit_tests binary_dir action(all|usp_tests|cxx_api_tests) keyLuis keySkyman endpoint"

BINARY_DIR="$1"
USP_TESTS_EXE="$BINARY_DIR/usp_tests"
CXX_API_TESTS_EXE="$BINARY_DIR/cxx_api_tests"

Action=$2
UserKeyLuis=$3
UserKeySkyman=$4
endpoint=$5

args="--keySpeech $UserKeySkyman"
[ ! -z $endpoint ] && args+=" --endpoint $endpoint"

if [ "$Action" = "all" ]; then 
    run_usp_tests
    run_cxx_api_tests
elif [ "$Action" = "usp_tests" ]; then
    run_usp_tests
elif [ "$Action" = "cxx_api_tests" ]; then
    run_cxx_api_tests
else 
    die "Unknown action: $Action"
fi

echo -e "\nDone\n"
