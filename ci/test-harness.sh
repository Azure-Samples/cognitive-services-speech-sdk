#!/usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
# N.B. meant to be sourced
#

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

# Pull in common functions
. "$SCRIPT_DIR/functions.sh"

# Create an artificial result .xml file for an arbitrary test and populate its root element
function startConstructedTestRunOutput {
  # TODO error on non-alpha:
  local _testStateVarPrefix=$1
  local outputRef=${_testStateVarPrefix}_output

  eval ${_testStateVarPrefix}_output=\"$2\"
  eval ${_testStateVarPrefix}_platform=\"$3\"
  eval ${_testStateVarPrefix}_redactStrings=\"$4\"
  eval ${_testStateVarPrefix}_allpass=true

  cat > ${!outputRef}.xml <<XML
<?xml version="1.0" encoding="UTF-8"?>
<testsuites>
XML
}

function startConstructedSuiteOutput {
  local _testStateVarPrefix=$1
  local platformRef=${_testStateVarPrefix}_platform

  # Reset suite state
  eval ${_testStateVarPrefix}_errors=0
  eval ${_testStateVarPrefix}_failures=0
  eval ${_testStateVarPrefix}_tests=0
  eval ${_testStateVarPrefix}_skipped=0
  eval ${_testStateVarPrefix}_time=0
  eval ${_testStateVarPrefix}_timestamp=\"$(date -u +"%Y-%m-%dT%H:%M:%SZ")\"
  eval ${_testStateVarPrefix}_testsuiteName=\"$2 - ${!platformRef}\"
  eval ${_testStateVarPrefix}_classname=\"$2.global\"
}

function endConstructedSuiteOutput {
  local _testStateVarPrefix="$1"

  local errorsRef=${_testStateVarPrefix}_errors
  local failuresRef=${_testStateVarPrefix}_failures
  local testsRef=${_testStateVarPrefix}_tests
  local skippedRef=${_testStateVarPrefix}_skipped
  local timeRef=${_testStateVarPrefix}_time
  local timestampRef=${_testStateVarPrefix}_timestamp
  local outputRef=${_testStateVarPrefix}_output
  local testsuiteNameRef=${_testStateVarPrefix}_testsuiteName
  local allpassRef=${_testStateVarPrefix}_allpass

  [[ ${!failuresRef} == 0 ]] || eval $allpassRef=false

  (
  cat <<XML
  <testsuite name="${!testsuiteNameRef}" errors="${!errorsRef}" failures="${!failuresRef}" skipped="${!skippedRef}" tests="${!testsRef}" hostname="$(hostname)" time="${!timeRef}" timestamp="${!timestampRef}">
XML
  [[ -f ${!outputRef}.xml.parts ]] && cat ${!outputRef}.xml.parts
  echo '<system-out><![CDATA['
  [[ -f ${!outputRef}.out ]] && cat ${!outputRef}.out
  cat <<'XML'
]]></system-out>
  </testsuite>
XML
  ) >> ${!outputRef}.xml

  # Remove temporary suite files
  rm -f ${!outputRef}.{out,xml.parts}
}

function endConstructedTestRunOutput {
  local _testStateVarPrefix="$1"
  local outputRef=${_testStateVarPrefix}_output
  local allpassRef=${_testStateVarPrefix}_allpass
  echo '</testsuites>' >> ${!outputRef}.xml
  # TODO perhaps more clean-up

  # Return exit code 0 or 1
  ${!allpassRef}
}

function redact {
  # N.B. receiving stdin as first command in function.
  perl -MIO::Handle -lpe 'BEGIN { STDOUT->autoflush(1); STDERR->autoflush(1); if (@ARGV) { $re = sprintf "(?:%s)", (join "|", map { quotemeta $_ } splice @ARGV); $re = qr/$re/ } } $re and s/$re/***/gi' $@
}

function runTest {
  # When given a specific output destination, emit to that; otherwise create temporary files
  # to be combined later (should be used in conjunction with endConstructedSuiteOutput)
  local testOutput tempOutputRef
  testOutput=
  if [[ $1 = --output ]]; then
    testOutput="$2"
    shift 2
  else
    tempOutputRef=${1}_output
    testOutput=${!tempOutputRef}.out
  fi

  local _testStateVarPrefix="$1"
  local TEST_NAME="$2"
  local TIMEOUT_SECONDS="$3"
  shift 3
  local COMMAND="$@"

  local testsRef=${_testStateVarPrefix}_tests
  local failuresRef=${_testStateVarPrefix}_failures
  local timeRef=${_testStateVarPrefix}_time
  local testsuiteNameRef=${_testStateVarPrefix}_testsuiteName
  local classnameRef=${_testStateVarPrefix}_classname
  local redactStringsRef=${_testStateVarPrefix}_redactStrings

  local callStdbuf=()
  [[ $(type -t stdbuf) != file ]] || callStdbuf=(stdbuf)
  [[ $(type -t gstdbuf) != file ]] || callStdbuf=(gstdbuf)
  [[ -z ${callStdbuf:-} ]] || callStdbuf+=(-o0 -e0)

  local cmdTimeout=false
  [[ $(type -t timeout) != file ]] || cmdTimeout=timeout
  [[ $(type -t gtimeout) != file ]] || cmdTimeout=gtimeout

  print_vars = TEST_NAME TIMEOUT_SECONDS COMMAND - |
    redact ${!redactStringsRef} |
    tee -a "$testOutput"

  local START_SECONDS EXIT_CODE TIME_SECONDS

  START_SECONDS=$(get_time)
  (
  set +o pipefail
  $cmdTimeout -k 5s $TIMEOUT_SECONDS ${callStdbuf[@]} "$@" 2>&1 |
    redact ${!redactStringsRef} >> "$testOutput"
  exit ${PIPESTATUS[0]}
  )
  EXIT_CODE=$?

  TIME_SECONDS=$(get_seconds_elapsed "$START_SECONDS")

  print_vars EXIT_CODE TIME_SECONDS = |
    tee -a "$testOutput"
  echo

  if [[ -n $tempOutputRef ]];
  then
    # This test is being run with constructed output via temporary files. Let's fake some readable
    # results.
    case $EXIT_CODE in
      0)
        printf '<testcase classname="%s" name="%s" time="%s"/>\n' \
          "${!classnameRef}" "$TEST_NAME" "$TIME_SECONDS" >> "${!tempOutputRef}.xml.parts"
        ;;
        # TODO need to distinguish error (e.g., 124 - timeout) from failure
      *)
        printf '<testcase classname="%s" name="%s" time="%s">\n  <failure message="%s"><![CDATA[%s]]></failure>\n</testcase>\n' \
          "${!classnameRef}" \
          "$TEST_NAME" \
          "$TIME_SECONDS" \
          "EXITCODE-$EXIT_CODE" \
          `tail -n 10 $testOutput` \
          >> "${!tempOutputRef}.xml.parts"
      eval "(( ++$failuresRef ))"
      ;;
  esac
  fi

  eval $timeRef=\"$(perl -e "printf '%0.3f', ${!timeRef} + $TIME_SECONDS")\"

  eval "(( ++$testsRef ))"

  return $EXIT_CODE
}

function sanitize() {
   local s="${1?need a string}" # receive input in first argument
   s="${s//[^[:alnum:]]/-}"     # replace all non-alnum characters to -
   s="${s//+(-)/-/g}"           # convert multiple - to single -
   s="${s/#-}"                  # remove - from start
   s="${s/%-}"                  # remove - from end
   _sanitized="${s,,}"
}

# Run Catch2 test suite, each test-case in a separate process, for robustness.
function runCatchSuite {
  local usage testStateVarPrefix output platform redactStrings testsuiteName timeoutSeconds testCases testCaseIndex catchOut logPath exitCode pattern
  usage="Usage: ${FUNCNAME[0]} <testStateVarPrefix> <output> <platform> <redactStrings> <testsuiteName> <timeoutSeconds> <pattern> <command...>"
  testStateVarPrefix="${1?$usage}"
  output="${2?$usage}"
  platform="${3?$usage}"
  redactStrings="${4?$usage}"
  testsuiteName="${5?$usage}"
  timeoutSeconds="${6?$usage}"
  pattern="${7?$usage}"
  shift 7

  testCases=()

# readarray -t testCases < <("$1" --list-test-names-only "$pattern" | tr -d \\r)
# We were using process substitution before for find/readarray, which started failing.
# Possibly related to https://github.com/git-for-windows/git/issues/2291, although
# we could not correlate with the software running on the hosted agents.
TESTCASE_FILE=$(mktemp proj-XXXXXX)
on_exit() {
  local exit_code=$?
  rm -f "$TESTCASE_FILE"
  printf "Exiting with exit code %s\n" $exit_code
  exit $exit_code
}
trap on_exit EXIT

"$1" --list-test-names-only "$pattern" | tr -d \\r > "$TESTCASE_FILE"

readarray -t testCases < "$TESTCASE_FILE"

  [[ ${#testCases[@]} != 0 ]] || {
    echo Failed to discover any test cases.
    return 1
  }

  testCaseIndex=0
  for i in $(seq 1 3); do
    failedTestCases=()
     for testCase in "${testCases[@]}"; do
       ((testCaseIndex++))
       sanitize "$testCase"
       catchOut=`printf "%s-%s-%03d" $output $_sanitized $testCaseIndex`
       logPath="$catchOut.log"
       echo `printf "Emitting full result details to: %s" $logPath`
       runTest --output "$logPath" "$testStateVarPrefix" "$testCase" "$timeoutSeconds" \
       "$@" --reporter vstest --durations yes --out "$catchOut.trx" --attachment "../../$logPath" "$testCase" 
       exitCode=$?
       if [ $exitCode -ne 0 ] ; then
         failedTestCases=( "${failedTestCases[@]}" "$testCase" )
         if [ $i == 3 ]; then
           echo 'Test failed. Potential details in the logs included in the test results files.'
         fi
       fi
      done
    testCases=("${failedTestCases[@]}")
  done

  totalFailedTest=${#testCases[@]}
  exit $totalFailedTest
}
