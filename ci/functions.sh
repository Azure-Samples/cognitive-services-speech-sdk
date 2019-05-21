#!/usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
# To be sourced from depending scripts.

# https://github.com/Microsoft/vsts-tasks/blob/master/docs/authoring/commands.md

# Ensure now logging of commands to not confuse the agent...

vsts_setvar() (
  set +x
  echo Setting Build Variable $1=$2
  echo "##vso[task.setvariable variable=$1]$2"
)

vsts_setvars_by_ref() (
  set +x
  for i in "$@"; do
    vsts_setvar "$i" "${!i}"
  done
)

vsts_setoutvar() (
  set +x
  echo Setting Build Output Variable $1=$2
  echo "##vso[task.setvariable variable=$1;isOutput=true]$2"
)

vsts_updatebuildnumber() (
  set +x
  echo Updating build number to $1
  echo "##vso[build.updatebuildnumber]$1"
)

vsts_addbuildtag() (
  set +x
  echo Adding build tag $1
  echo "##vso[build.addbuildtag]$1"
)

vsts_logissue() (
  set +x
  local type="$1"
  local message="$2"
  shift 2
  local out="##vso[task.logissue type=$type"

  # Append additional properties (sourcepath, linenumber, columnnumber, code)
  [[ $# == 0 ]] || out+="$(printf ';%s' "$@")"

  out+="]$message"

  echo "$out"
)

function existsExactlyOneDir {
  [[ $# -eq 1 && -d $1 ]]
}

function existsExactlyOneFile {
  [[ $# -eq 1 && -f $1 ]]
}

# Print arguments, surrounded by lines of equal characters.
function pretty_print {
  printf '%.0s=' {1..78}
  echo
  printf '%s\n' "$@"
  printf '%.0s=' {1..78}
  echo
}

# Print variables and their values, and separators (- or =)
function print_vars {
  for i in "$@"; do
    case $i in
      [=-])
         printf "%.0s$i" {1..78}
         echo
         ;;
      *) printf "%-30s  %-30s\n" "$i" "${!i}" ;;
    esac
  done
}

function joinArgs {
  SEP=$1
  shift 1

  [[ $# -ge 1 ]] && {
    printf '%s' $1
    shift 1
  }
  [[ $# -ge 1 ]] && printf "$SEP%s" "$@"
}

function die {
  set +x
  printf "$1"
  exit 1
}

function getAuthorizationToken {
  local usage key region token_endpoint output exit_code
  usage="Usage: ${FUNCNAME[0]} key region"
  key="${1?$usage}"
  region="${2?$usage}"
  token_endpoint="https://$region.api.cognitive.microsoft.com/sts/v1.0/issueToken"

  output="$(curl -s -S -X POST "$token_endpoint" -H "Content-type: application/x-www-form-urlencoded" -H "Content-Length: 0" -H "Ocp-Apim-Subscription-Key: $key")"
  exit_code=$?
  if [[ $exit_code == 0 ]]; then
    # Check for Base64 URL encoding
    if [[ $output =~ ^[a-zA-Z0-9_.-]*$ ]]; then
      printf "%s" "$output"
    else
      printf "%s" "$output" 1>&2
      return 1
    fi
  else
    return $exit_code
  fi

# The following variant requires curl 7.55.0 or up; no key as parameter.
#  # Use built-in + stdin to pass the key
#  output="$(printf "Content-type: application/x-www-form-urlencoded\nContent-Length: 0\nOcp-Apim-Subscription-Key: %s\n" "$key" |
#    curl -s -S -X POST "$token_endpoint" -H '@-')"
#  exit_code=$?
#  if [[ $exit_code == 0 ]]; then
#    # Check for Base64 URL encoding
#    if [[ $output =~ ^[a-zA-Z0-9_.-]*$ ]]; then
#      printf "%s" "$output"
#    else
#      printf "%s" "$output" 1>&2
#      return 1
#    fi
#  else
#    return $exit_code
#  fi
}

function exitWithError {
  printf "$@" 1>&2
  exit 1
}

function exitWithSuccess {
  printf "$@" 1>&2
  exit 0
}

function isOneOf {
  local i="$1"
  local j
  shift
  for j in "$@"; do
    [[ $i != $j ]] || return 0
  done
  return 1
}

function speechWebSocketsEndpoint {
  local usage region mode format language
  usage="Usage: ${FUNCNAME[0]} <region> <mode> <format> <language>"
  region="${1?$USAGE}" # e.g., westus
  mode="${2?$USAGE}" # conversation dictation interactive
  format="${3?$USAGE}" # simple detailed
  language="${4?$USAGE}" # e.g., en-us
  echo "wss://$region.stt.speech.microsoft.com/speech/recognition/$mode/cognitiveservices/v1?format=$format&language=$language"
}

function crisWebSocketsEndpoint {
  local usage region mode endpointId
  usage="Usage: ${FUNCNAME[0]} <region> <mode> <endpoint-id>"
  region="${1?$USAGE}" # e.g., westus
  mode="${2?$USAGE}" # conversation dictation interactive
  endpointId="${3?$USAGE}" # CRIS endpoint ID
  echo "wss://$region.stt.speech.microsoft.com/speech/recognition/$mode/cognitiveservices/v1?cid=$endpointId"
}

function patchSamplesFromTestConfig() (
  # N.B. subshell.
  set -u -e -o pipefail
  local usage samplesDir testConfig vars
  usage="Usage: ${FUNCNAME[0]} <samples-dir> <test-config> [<additional evaluate-test-config.pl parameters, e.g., -D>]"
  samplesDir="${1?$usage}"
  testConfig="${2?$usage}"
  shift 2
  scriptDir="$(dirname "${BASH_SOURCE[0]}")"
  vars="$(perl "$scriptDir/evaluate-test-config.pl" --input "$testConfig" --format bash-variable "$@")" ||
    exitWithError "Error: could not evaluate test config '%s'.\n" "$testConfig"
  eval -- "$vars"
  perl "$scriptDir/patch-samples-config.pl" "$samplesDir" "$SPEECHSDK_INPUTDIR/audio/whatstheweatherlike.wav" \
    $SPEECHSDK_SPEECH_KEY \
    $SPEECHSDK_SPEECH_REGION \
    $SPEECHSDK_SPEECH_ENDPOINTID_ENUS \
    $SPEECHSDK_LUIS_KEY \
    $SPEECHSDK_LUIS_REGION \
    $SPEECHSDK_LUIS_HOMEAUTOMATION_APPID \
    HomeAutomation.TurnOn \
    another-intent \
    yet-another-intent \
    "$SPEECHSDK_INPUTDIR/kws/Computer/kws.table" \
    Computer
)

function retry() {
  local maxRetries retry usage
  usage="Usage: ${FUNCNAME[0]} <num-retries> <cmd...>"
  numRetries="${1?$usage}"
  shift 1
  retry=0
  while ((retry++ <= numRetries)); do
    "$@" && return 0 \
      || true # in case we run with "set -e"
  done
  printf "Command didn't succeed in $retry attempt(s): %s\n" "$*" 1>&2
  return 1
}

function unity_batch() {
  local log_dir editor_log old_log_dir unity_exe tail_pid exit_code

  log_dir=$HOME/AppData/Local/Unity/Editor
  editor_log=$log_dir/Editor.log
  old_log_dir=${log_dir}/old
  unity_exe="/c/Program Files/Unity2018.3.0f2/Editor/Unity.exe"

  [[ -x $unity_exe ]] || {
    printf "Error: cannot find Unity here: %s\n" "$unity_exe"
    return 1
  }

  # Move an old log file out of the way
  [[ -d $old_log_dir ]] || mkdir -p "$old_log_dir"
  ! [[ -f $editor_log ]] || mv --backup=numbered "$editor_log" "$old_log_dir"

  # Tail future logs into the console
  tail -F "$editor_log" &
  tail_pid=$!

  "$unity_exe" -batchmode -quit -nographics "$@"
  exit_code=$?

  # Allow some extra time for logs being fully flushed and output.
  sleep 7

  kill "$tail_pid" || true

  # Unity doesn't (always?) end logs with a newline.
  printf "\n"

  return $exit_code
}

function get_time() {
  perl -MTime::HiRes=clock_gettime -e 'print clock_gettime()'
}

function get_seconds_elapsed() {
  perl -MTime::HiRes=clock_gettime -e 'printf "%0.3f", clock_gettime() - shift' "$1"
}

function getLatest() {
  local sorted
  sorted=($(printf '%s\n' "$@" | sort --version-sort))
  echo ${sorted[-1]}
}
