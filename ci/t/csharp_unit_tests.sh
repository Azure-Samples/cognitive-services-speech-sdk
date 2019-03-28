#!/usr/bin/env bash
T="$(basename "$0" .sh)"
BUILD_DIR="$1"
PLATFORM="$2"
BINARY_DIR="$3"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

isOneOf "$PLATFORM" Windows-{x86,x64}-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

TEST_CODE="$BINARY_DIR/Microsoft.CognitiveServices.Speech.Tests.EndToEnd.dll"
[[ -f $TEST_CODE ]] ||
  exitWithSuccess "Test %s: skip, no test code\n" "$T"

SOURCE_ROOT="$SCRIPT_DIR/../.."

VSWHERE="/c/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
VSDIR_WINDOWS="$("$VSWHERE" -version '[15.0,16.0)' -latest -products '*' -requires Microsoft.VisualStudio.PackageGroup.TestTools.Core -property installationPath)"
[[ $? == 0 ]] || {
  echo Error: cannot run \"$VSWHERE\"
  exit 1
}
VSDIR="$(cygpath -au "$VSDIR_WINDOWS")"
VSTEST="$VSDIR/Common7/IDE/CommonExtensions/Microsoft/TestWindow/vstest.console.exe"

# TODO tempname
runSettings=csharp.runsettings

SPEECHSDK_INPUTDIR_WINDOWS="$(cygpath -aw "$SPEECHSDK_INPUTDIR")"

perl -p - \
  UnifiedSpeechSubscriptionKey "$SPEECHSDK_SPEECH_KEY" \
  Region "$SPEECHSDK_SPEECH_REGION" \
  DeploymentId "$SPEECHSDK_SPEECH_ENDPOINTID_ENUS" \
  InputDir "$SPEECHSDK_INPUTDIR_WINDOWS" \
  LanguageUnderstandingSubscriptionKey "$SPEECHSDK_LUIS_KEY" \
  LanguageUnderstandingServiceRegion "$SPEECHSDK_LUIS_REGION" \
  LanguageUnderstandingHomeAutomationAppId "$SPEECHSDK_LUIS_HOMEAUTOMATION_APPID" \
  tests/functional/csharp/end2end/csharp.runsettings \
  > "$runSettings" <<"SCRIPT"
  BEGIN {
    %A = splice(@ARGV, 0, $#ARGV);
    $keysRe = "(?:\\b" . (join "\\b|\\b", keys %A) . "\\b)";
    $keysRe = qr/$keysRe/;
  }
  s/(<Parameter name="($keysRe)" value=")([^"]*)/$1.$A{$2}/ge
SCRIPT

set -x
if [[ $SPEECHSDK_LONG_RUNNING = true ]]; then
  TEST_CASE_FILTER="TestCategory=LongTest&TestCategory!=CompressedStreamTest"
  LOG_FILE_NAME=LogFileName=test-$T-$PLATFORM-long-running.trx
else
  TEST_CASE_FILTER="TestCategory!=LongTest&TestCategory!=CompressedStreamTest"
  LOG_FILE_NAME=LogFileName=test-$T-$PLATFORM.trx
fi

"$VSTEST" \
"$(cygpath -aw "$TEST_CODE")" \
--Settings:"$runSettings" \
--Logger:"trx;$LOG_FILE_NAME" \
--Blame \
--TestAdapterPath:"$(cygpath -aw "$SOURCE_ROOT")" \
--TestCaseFilter:"$TEST_CASE_FILTER"

exitCode=$?
set +x
rm -f "$runSettings"
exit $exitCode
