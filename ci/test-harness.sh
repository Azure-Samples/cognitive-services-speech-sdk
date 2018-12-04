#!/bin/bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
# N.B. meant to be sourced
#

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

# Pull in common functions
. "$SCRIPT_DIR/functions.sh"

function startTests {
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

function startSuite {
  local _testStateVarPrefix=$1
  local outputRef=${_testStateVarPrefix}_output
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

function endSuite {
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

function endTests {
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
  $([[ $(uname) = Darwin ]] && printf 'g' ]])stdbuf -o0 -e0 perl -lpe 'BEGIN { if (@ARGV) { $re = sprintf "(?:%s)", (join "|", map { quotemeta $_ } splice @ARGV); $re = qr/$re/ } } $re and s/$re/***/gi' $@
}

function runTest {
  local testOutput
  if [[ $1 = --output ]]; then
    testOutput="$2"
    shift 2
  fi
  local _testStateVarPrefix="$1"
  local TEST_NAME="$2"
  local TIMEOUT_SECONDS="$3"
  shift 3
  local COMMAND="$@"

  local testsRef=${_testStateVarPrefix}_tests
  local failuresRef=${_testStateVarPrefix}_failures
  local timeRef=${_testStateVarPrefix}_time
  local outputRef=${_testStateVarPrefix}_output
  local testsuiteNameRef=${_testStateVarPrefix}_testsuiteName
  local classnameRef=${_testStateVarPrefix}_classname
  local redactStringsRef=${_testStateVarPrefix}_redactStrings

  print_vars = TEST_NAME TIMEOUT_SECONDS COMMAND - |
    redact ${!redactStringsRef} |
    tee -a "${!outputRef}.out"

  local START_SECONDS EXIT_CODE END_SECONDS TIME_SECONDS TAIL

  local COREUTILS_PREFIX
  if [[ $(uname) = Darwin ]]; then
    COREUTILS_PREFIX=g
  else
    COREUTILS_PREFIX=
  fi

  START_SECONDS=$(perl -MTime::HiRes=clock_gettime -le 'print clock_gettime()')
  ${COREUTILS_PREFIX}timeout -k 5s $TIMEOUT_SECONDS ${COREUTILS_PREFIX}stdbuf -o0 -e0 "$@" 2>&1 |
    redact ${!redactStringsRef} |
    if [[ -n $testOutput ]]; then tee "$testOutput"; else cat; fi \
      1>> "${!outputRef}.out"

  EXIT_CODE=${PIPESTATUS[0]}
  END_SECONDS=$(perl -MTime::HiRes=clock_gettime -le 'print clock_gettime()')
  TIME_SECONDS=$(perl -e "printf '%0.3f', $END_SECONDS - $START_SECONDS")
  TAIL="$(tail -20 "${!outputRef}.out")"

  print_vars EXIT_CODE TIME_SECONDS = |
    tee -a "${!outputRef}.out"
  echo

  case $EXIT_CODE in
    0)
      printf '<testcase classname="%s" name="%s" time="%s"/>\n' \
        "${!classnameRef}" "$TEST_NAME" "$TIME_SECONDS" >> "${!outputRef}.xml.parts"
      ;;
      # TODO need to distinguish error (e.g., 124 - timeout) from failure
    *)
      printf '<testcase classname="%s" name="%s" time="%s">\n  <failure message="%s"><![CDATA[%s]]></failure>\n</testcase>\n' \
        "${!classnameRef}" \
        "$TEST_NAME" \
        "$TIME_SECONDS" \
        "EXITCODE-$EXIT_CODE" \
        "$TAIL" \
        >> "${!outputRef}.xml.parts"
      eval "(( $failuresRef++ ))"
      ;;
  esac

  eval $timeRef=\"$(perl -e "printf '%0.3f', ${!timeRef} + $TIME_SECONDS")\"

  eval "(( $testsRef++ ))"

  [[ $EXIT_CODE == 0 ]]
}

function addToTestOutput {
  local _testStateVarPrefix="$1"
  shift

  local outputRef=${_testStateVarPrefix}_output
  local redactStringsRef=${_testStateVarPrefix}_redactStrings

  "$@" |
    redact ${!redactStringsRef} |
    tee -a "${!outputRef}.out"
}

# Run Catch2 test suite, each test-case in a separate process, for robustness.
# Use the Catch2 XML reporter is streaming and will have more details than the
# JUnit one in case of crashes.
function runCatchSuite {
  local usage testStateVarPrefix output platform redactStrings testsuiteName timeoutSeconds testCases testCaseIndex catchOut exitCode
  usage="Usage: ${FUNCNAME[0]} <testStateVarPrefix> <output> <platform> <redactStrings> <testsuiteName> <timeoutSeconds> <command...>"
  testStateVarPrefix="${1?$usage}"
  output="${2?$usage}"
  platform="${3?$usage}"
  redactStrings="${4?$usage}"
  testsuiteName="${5?$usage}"
  timeoutSeconds="${6?$usage}"
  shift 6

  testCases=()
  readarray -t testCases < <("$1" --list-test-names-only | tr -d \\r)
  [[ ${#testCases[@]} != 0 ]] || {
    echo Failed to discover any test cases.
    return 1
  }

  startTests "$testStateVarPrefix" "$output" "$platform" "$redactStrings"
  startSuite "$testStateVarPrefix" "$testsuiteName"

  # Remove individual catch output files, ignoring errors
  rm -f "catch$output-"*.{xml,txt}

  testCaseIndex=0
  for testCase in "${testCases[@]}"; do
    ((testCaseIndex++))
    catchOut="catch$output-$testCaseIndex"
    runTest --output "$catchOut.txt" "$testStateVarPrefix" "$testCase" "$timeoutSeconds" \
      "$@" --reporter xml --durations yes --out "$catchOut.xml" \
        "$testCase" || if [[ -s $catchOut.xml ]]; then
            echo 'Test failed. Potential details here (cf. success="false"), or in the logs included in the test results file:'
            addToTestOutput "$testStateVarPrefix" cat "$catchOut.xml"
            if [[ $(uname) = Linux ]] && grep -q -F $'<FatalErrorCondition\n<Exception' "$catchOut.xml"; then
              addToTestOutput "$testStateVarPrefix" perl "$SCRIPT_DIR/decode-stack.pl" "$catchOut.txt"
            fi
          fi
  done

  endSuite "$testStateVarPrefix"
  endTests "$testStateVarPrefix"
}
