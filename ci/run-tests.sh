#!/usr/bin/env bash

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/functions.sh" || exit 1

# Associative array for most options
declare -A options
options[timeout]="3600s"
options[test-set]="dev"

# Defines we collect separately in an array
defines=()

# tests files (full path) that are to be skipped
skips=()

while [[ $# > 0 ]]
do
  key="$1"

  case $key in
    --test-configuration)
      [[ -r $2 ]] ||
        exitWithError "Error: test configuration '%s' non-existing or not readable.\n" "$2"
      options[${key:2}]="$2"
      shift
      ;;
    --build-dir)
      [[ -d $2 ]] ||
        exitWithError "Error: argument '%s' to %s is not a directory\n" "$2" "$key"

      options[${key:2}]="$2"
      shift
      ;;
    --platform)
      # TODO fix OSX vs. macOS (everywhere)
      platformRe="(Windows|WindowsUwp|Linux|macOS|OSX|iOS|Android)-(arm32|arm64|x86|x64)-(Debug|Release)"
      [[ $2 =~ ^$platformRe$ ]] ||
        exitWithError "Error: invalid platform specification '%s', expected %s.\n" "$2" "$platformRe"
      options[${key:2}]="$2"
      os="${BASH_REMATCH[1]}"
      arch="${BASH_REMATCH[2]}"
      flavor="${BASH_REMATCH[3]}"
      bitness="$([[ $arch == x86 || $arch == arm32 ]] && echo 32 || echo 64)"
      shift
      ;;
    --define|-D)
      [[ $2 =~ [^=]*=.* ]] ||
        exitWithError "Error: expected key=value arguments for %s option, got '%s'\n" "$key" "$2"
      defines+=(--define "$2")
      shift
      ;;
    --timeout)
      [[ -n $2 ]] ||
        exitWithError "Error: expected argument for %s option\n" "$key"
      options[${key:2}]="$2"
      shift
      ;;
    --verbose|-v)
      options[verbose]=1
      ;;
    --skip|-s)
      # test names (basename without extension) that are to be skipped
      [[ -n $2 ]] ||
        exitWithError "Error: expected argument for %s option\n" "$key"
      skips+=("$SCRIPT_DIR/t/$2.sh")
      shift
      ;;
    --test-set)
      testSetRe="(dev|int|prod)"
      [[ $2 =~ ^$testSetRe$ ]] ||
        exitWithError "Error: invalid test set '%s', expected %s.\n" "$2" "$testSetRe"
      options[${key:2}]="$2"
      shift
      ;;
    --)
      # Stop processing options, remaining arguments are test files.
      shift
      break
      ;;
    *)
      exitWithError "Error: unrecognized option '%s'\n" "$key"
      ;;
  esac
  shift # past argument or value
done

verbose_switch=
[[ -z ${options[verbose]} ]] ||
  verbose_switch=--verbose

if [[ $# == 0 ]]; then
  # Note: exclude scripts starting with an underscore
  tests=("$SCRIPT_DIR/t/"[^_]*.sh)
else
  for t in "$@"; do
    t="$SCRIPT_DIR/t/$t.sh"
    [[ -x $t ]] || {
      echo Error: cannot find test file $t.
      exit 1
    }
    tests+=("$t")
  done
fi

# skip tests as specified
testsToRun=()
for t in "${tests[@]}"; do
  isOneOf "$t" "${skips[@]}" || testsToRun+=("$t")
done

# Check for required options
for k in test-configuration build-dir platform; do
  [[ -n ${options[$k]} ]] || {
    echo Error: missing --$k option.
    exit 1
  }
done

# Binary directory, with flavor appended for multi-config generators.
binaryDir="${options[build-dir]}/bin"
[[ $os != Windows* ]] || binaryDir+=/$flavor

VARS="$(perl "$SCRIPT_DIR/"evaluate-test-config.pl $verbose_switch --format bash-environment --input "${options[test-configuration]}" "${defines[@]}")" || {
  echo Error: could not evaluate test config. 1>&2
  exit 1
}

eval -- "$VARS"

cmdTimeout=
[[ $(type -t timeout) != file ]] || cmdTimeout=timeout
[[ $(type -t gtimeout) != file ]] || cmdTimeout=gtimeout
[[ -n cmdTimeout ]] || exitWithFailure "coreutils timeout command not found\n"

callStdbuf=()
[[ $(type -t gstdbuf) != file ]] || callStdbuf=(gstdbuf)
[[ $(type -t stdbuf) != file ]] || callStdbuf=(stdbuf)
[[ -z $callStdbuf ]] || callStdbuf+=(-o0 -e0)

pass=0
total=0
for testfile in "${testsToRun[@]}"; do
  T="$(basename "$testfile" .sh)"
  echo Starting $T with timeout ${options[timeout]}
  START_SECONDS=$(get_time)
  $cmdTimeout -k 5s "${options[timeout]}" ${callStdbuf[@]} \
  "$testfile" "${options[build-dir]}" "${options[platform]}" "$binaryDir" "${options[test-set]}"
  exitCode=$?
  TIME_SECONDS=$(get_seconds_elapsed "$START_SECONDS")
  if [[ $exitCode == 0 ]]; then
    ((pass++))
    echo test suite $T: passed, \($TIME_SECONDS seconds\)
  else
      # if the test suite failed, wait two seconds and re-run it up to three times.
      for retry in {1..3}; do
        echo "Retry($retry) failed test suite $T with timeout ${options[timeout]}"
        sleep 2s
        START_SECONDS=$(get_time)
        $cmdTimeout -k 5s "${options[timeout]}" ${callStdbuf[@]} \
        "$testfile" "${options[build-dir]}" "${options[platform]}" "$binaryDir" "${options[test-set]}"
        exitCode=$?
        TIME_SECONDS=$(get_seconds_elapsed "$START_SECONDS")
        if [[ $exitCode == 0 ]]; then
          ((pass++))
          echo "Test suite $T: passed after ($retry) retries, \($TIME_SECONDS seconds\)"
          vsts_logissue warning "${options[platform]}: test suite $T passed after ($retry) retries, source $testfile."
          break
        else
          echo Test suite $T: failed with error code $exitCode, \($TIME_SECONDS seconds\)
          vsts_logissue warning "${options[platform]}: test suite $T failed, exit code $exitCode, source $testfile."
        fi
      done

      if [[ $exitCode != 0 ]]; then
        echo Test suite $T: failed after multiple reties.
        vsts_logissue error "${options[platform]}: test suite $T failed, failed after multiple retries with exit code $exitCode, source $testfile."
      fi
  fi
  ((total++))
done
echo Pass '(including skip)' $pass / $total.
((pass == total))

