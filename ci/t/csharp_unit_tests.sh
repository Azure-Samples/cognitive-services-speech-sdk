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
  DialogRegion "$SPEECHSDK_BOT_REGION" \
  DialogSubscriptionKey "$SPEECHSDK_BOT_SUBSCRIPTION" \
  DialogFunctionalTestBot "$SPEECHSDK_BOT_FUNCTIONALTESTBOT" \
  ConversationTranscriptionEndpoint "$SPEECHSDK_PRINCETON_INROOM_ENDPOINT" \
  ConversationTranscriptionPPEKey "$SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PPE_KEY" \
  ConversationTranscriptionPRODKey "$SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PROD_KEY" \
  SpeechRegionForConversationTranscription "$SPEECHSDK_PRINCETON_REGION" \
  tests/functional/csharp/end2end/csharp.runsettings \
  > "$runSettings" <<"SCRIPT"
  BEGIN {
    %A = splice(@ARGV, 0, $#ARGV);
    $keysRe = "(?:\\b" . (join "\\b|\\b", keys %A) . "\\b)";
    $keysRe = qr/$keysRe/;
  }
  s/(<Parameter name="($keysRe)" value=")([^"]*)/$1.$A{$2}/ge
SCRIPT

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
  ACTUAL_LOG_FILE_NAME=test-$T-$PLATFORM-long-running.trx
  LOG_FILE_NAME=LogFileName=$ACTUAL_LOG_FILE_NAME
else
  TEST_CASE_FILTER="TestCategory!=LongTest&TestCategory!=CompressedStreamTest${OFFLINE_UNIDEC_FILTER}"
  ACTUAL_LOG_FILE_NAME=test-$T-$PLATFORM-$RANDOM.trx
  LOG_FILE_NAME=LogFileName=$ACTUAL_LOG_FILE_NAME
fi

DIAG_FILENAME=diag-${ACTUAL_LOG_FILE_NAME::-4}

"$VSTEST" \
"$(cygpath -aw "$TEST_CODE")" \
--Logger:"trx;$LOG_FILE_NAME" \
--Diag:./vstsconsolelog/$DIAG_FILENAME.txt \
--Blame \
--TestAdapterPath:"$(cygpath -aw "$SOURCE_ROOT")" \
--TestCaseFilter:"$TEST_CASE_FILTER"

exitCode=$?

ACTUAL_LOG_FILE_NAME_RETRY=$ACTUAL_LOG_FILE_NAME

for i in $(seq 1 4); do
  if [[ $exitCode != 0 ]]; then
    echo Parsing the trx file "./TestResults/$ACTUAL_LOG_FILE_NAME_RETRY"
    tests=""
    FAILED="outcome=\"Failed\""
    TESTNAME="testName"
    while read p; do
      if [[ "$p" == *"$FAILED"* ]] ; then
        words=( $p )
        for q in "${words[@]}"; do
          if [[ "$q" == *"$TESTNAME"* ]] ; then
            test=${q:10}
            test="${test::-1}"
            tests="$tests,$test"
          fi
        done
      fi
    done <"./TestResults/$ACTUAL_LOG_FILE_NAME_RETRY"
    tests=${tests:1}

    ACTUAL_LOG_FILE_NAME_RETRY=$ACTUAL_LOG_FILE_NAME-retry$i
    LOG_FILE_NAME_RETRY=LogFileName=$ACTUAL_LOG_FILE_NAME_RETRY
    DIAG_FILENAME_RETRY=$DIAG_FILENAME-retry$i.txt

    echo "Rerunning the following failed test ${tests[*]}"

    "$VSTEST" \
    "$(cygpath -aw "$TEST_CODE")" \
    --Logger:"trx;$LOG_FILE_NAME_RETRY" \
    --Diag:./vstsconsolelog/$DIAG_FILENAME_RETRY \
    --Blame \
    --TestAdapterPath:"$(cygpath -aw "$SOURCE_ROOT")" \
    --Tests:"$tests"
  
    exitCode=$?
  else
    break
  fi
done


set +x
rm -f "$runSettings"
exit $exitCode
