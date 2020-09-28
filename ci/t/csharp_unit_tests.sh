#!/usr/bin/env bash
T="$(basename "$0" .sh)"
BUILD_DIR="$1"
PLATFORM="$2"
BINARY_DIR="$3"
SPEECHSDK_LONG_RUNNING="$5"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

isOneOf "$PLATFORM" Windows-{x86,x64}-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

TEST_CODE="$BINARY_DIR/Microsoft.CognitiveServices.Speech.Tests.EndToEnd.dll"
[[ -f $TEST_CODE ]] ||
  exitWithSuccess "Test %s: skip, no test code\n" "$T"

SOURCE_ROOT="$SCRIPT_DIR/../.."

VSWHERE="/c/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
VSDIR_WINDOWS="$("$VSWHERE" -version '[15.0,19.0)' -latest -products '*' -requires Microsoft.VisualStudio.PackageGroup.TestTools.Core -property installationPath)"
[[ $? == 0 ]] || {
  echo Error: cannot run \"$VSWHERE\"
  exit 1
}
VSDIR="$(cygpath -au "$VSDIR_WINDOWS")"
VSTEST="$VSDIR/Common7/IDE/CommonExtensions/Microsoft/TestWindow/vstest.console.exe"

# TODO tempname
runSettings=csharp.runsettings

SPEECHSDK_INPUTDIR_WINDOWS="$(cygpath -aw "$SPEECHSDK_INPUTDIR")"

RUN_OFFLINE_UNIDEC_TESTS=false
case $PLATFORM in
  Windows-x64*)
    UNIDEC_RUNTIME_PATH="$SOURCE_ROOT/external/unidec/Richland.Speech.UnidecRuntime/native"
    if [[ -d "$UNIDEC_RUNTIME_PATH" ]]; then
      PATH="$(cygpath -aw "$UNIDEC_RUNTIME_PATH"):$PATH"
      RUN_OFFLINE_UNIDEC_TESTS=true
    fi
    ;;
  # Skip on other platforms for now
esac

if [[ $RUN_OFFLINE_UNIDEC_TESTS = true ]]; then
  OFFLINE_UNIDEC_FILTER=""
else
  OFFLINE_UNIDEC_FILTER="&TestCategory!=OfflineUnidec"
fi

if [[ $SPEECHSDK_LONG_RUNNING = true ]]; then
  TEST_CASE_FILTER="TestCategory=LongTest&TestCategory!=CompressedStreamTest${OFFLINE_UNIDEC_FILTER}"
  ACTUAL_LOG_FILE_NAME=test-csharp-$T-$PLATFORM-$RANDOM-long-running.trx
  LOG_FILE_NAME=LogFileName=$ACTUAL_LOG_FILE_NAME
else
  TEST_CASE_FILTER="TestCategory!=LongTest&TestCategory!=CompressedStreamTest${OFFLINE_UNIDEC_FILTER}"
  ACTUAL_LOG_FILE_NAME=test-csharp-$T-$PLATFORM-$RANDOM.trx
  LOG_FILE_NAME=LogFileName=$ACTUAL_LOG_FILE_NAME
fi

DIAG_FILENAME=diag-${ACTUAL_LOG_FILE_NAME::-4}

"$VSTEST" \
"$(cygpath -aw "$TEST_CODE")" \
--Logger:"trx;$LOG_FILE_NAME" \
--Diag:./vstsconsolelog/$DIAG_FILENAME.txt \
--Blame \
--TestAdapterPath:"$(cygpath -aw "$SOURCE_ROOT")" \
--TestCaseFilter:"$TEST_CASE_FILTER" \
--InIsolation

exitCode=$?

set +x
rm -f "$runSettings"
exit $exitCode
