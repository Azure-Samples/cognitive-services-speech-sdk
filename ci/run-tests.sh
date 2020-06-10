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

LONG_RUNNING_FLAG="long-running"
for define_key in "${defines[@]}"; do
  if [[ "$define_key" == *"$LONG_RUNNING_FLAG"* ]] ; then
    SPEECHSDK_LONG_RUNNING=${define_key:13}
  fi
done

if [[ "$SPEECHSDK_LONG_RUNNING" != "true" ]] ; then
  LONG_RUNNING_FLAG=""
fi

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

# Binary directory, with flavor appended for multi-config generators.
binaryDir="${options[build-dir]}/bin"
[[ $os != Windows* ]] || binaryDir+=/$flavor

# For Python, these entries need to be in the root
[[ ! -d ./input ]] && cp -rf tests/input .
[[ ! -e ./test.defaults.json ]] && cp ./tests/test.defaults.json .
[[ ! -e ./test.audio.utterances.json ]] && cp ./tests/test.audio.utterances.json .
[[ ! -e ./test.subscriptions.regions.json ]] && cp ./tests/test.subscriptions.regions.json .

[[ ! -d $binaryDir/input ]] && cp -rf tests/input $binaryDir
[[ ! -e $binaryDir/test.defaults.json ]] && cp ./tests/test.defaults.json $binaryDir
[[ ! -e $binaryDir/test.audio.utterances.json ]] && cp ./tests/test.audio.utterances.json $binaryDir
[[ ! -e $binaryDir/test.subscriptions.regions.json ]] && cp ./tests/test.subscriptions.regions.json $binaryDir

if [ -d buildfromdropx64 ]; then
    [[ ! -d buildfromdropx64/bin/input ]] && cp -rf tests/input buildfromdropx64/bin
    [[ ! -e buildfromdropx64/bin/test.defaults.json ]] && cp ./tests/test.defaults.json buildfromdropx64/bin
    [[ ! -e buildfromdropx64/bin/test.audio.utterances.json ]] && cp ./tests/test.audio.utterances.json buildfromdropx64/bin
    [[ ! -e buildfromdropx64/bin/test.subscriptions.regions.json ]] && cp ./tests/test.subscriptions.regions.json buildfromdropx64/bin
fi

if [ -d buildfromdropx86 ]; then
    [[ ! -d buildfromdropx86/bin/input ]] && cp -rf tests/input buildfromdropx86/bin
    [[ ! -e buildfromdropx86/bin/test.defaults.json ]] && cp ./tests/test.defaults.json buildfromdropx86/bin
    [[ ! -e buildfromdropx86/bin/test.audio.utterances.json ]] && cp ./tests/test.audio.utterances.json buildfromdropx86/bin
    [[ ! -e buildfromdropx86/bin/test.subscriptions.regions.json ]] && cp ./tests/test.subscriptions.regions.json buildfromdropx86/bin
fi

cmdTimeout=
[[ $(type -t timeout) != file ]] || cmdTimeout=timeout
[[ $(type -t gtimeout) != file ]] || cmdTimeout=gtimeout
[[ -n cmdTimeout ]] || exitWithFailure "coreutils timeout command not found\n"

callStdbuf=()
[[ $(type -t gstdbuf) != file ]] || callStdbuf=(gstdbuf)
[[ $(type -t stdbuf) != file ]] || callStdbuf=(stdbuf)
[[ -z $callStdbuf ]] || callStdbuf+=(-o0 -e0)

# create console log folder
rm -rf vstsconsolelog
mkdir -p vstsconsolelog

pass=0
total=0
pids=()
declare -A pid_test_map
declare -A pid_log_map
for testfile in "${testsToRun[@]}"; do
  T="$(basename "$testfile" .sh)"
  echo Starting $T with timeout ${options[timeout]}
  START_SECONDS=$(get_time)
  consolelogfilename=consolelog-$LONG_RUNNING_FLAG-$T-${options[platform]}.txt
  $cmdTimeout -k 5s "${options[timeout]}" ${callStdbuf[@]} \
  "$testfile" "${options[build-dir]}" "${options[platform]}" "$binaryDir" "${options[test-set]}" $SPEECHSDK_LONG_RUNNING &> ./vstsconsolelog/$consolelogfilename &
  pid=$!
  pid_test_map[$pid]=$T
  pid_log_map[$pid]=$consolelogfilename
  pids+=("$pid")
  TIME_SECONDS=$(get_seconds_elapsed "$START_SECONDS")
  ((total++))
done

echo Total number of test are $total

for pid in "${pids[@]}"; do
  wait $pid
  exitCode=$?
  if [[ $exitCode == 0 ]]; then
    ((pass++))
    echo Passed: ${pid_test_map[$pid]} 
  else
    echo Failed: ${pid_test_map[$pid]}. The console log file is: ${pid_log_map[$pid]}
  fi
done

echo Pass '(including skip)' $pass / $total.
((pass == total))
