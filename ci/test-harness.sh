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

function runTest {
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

  local REDACT

  if [[ -n ${!redactStringsRef} ]]; then
    REDACT=(perl -lpe 'BEGIN { $re = sprintf "(?:%s)", (join "|", map { quotemeta $_ } splice @ARGV); $re = qr/$re/ } s/$re/***/gi' ${!redactStringsRef})
  else
    REDACT=(cat)
  fi

  print_vars = TEST_NAME TIMEOUT_SECONDS COMMAND - |
    "${REDACT[@]}" |
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
    "${REDACT[@]}" 1>> "${!outputRef}.out"
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
}
