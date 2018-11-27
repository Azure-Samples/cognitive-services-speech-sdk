#!/usr/bin/env bash
set -x -e
set -o pipefail

T="$(basename "$0" .sh)"
BUILD_DIR="$1"
PLATFORM="$2"
BINARY_DIR="$3"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

isOneOf "$PLATFORM" Windows-x64-Release ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

BINARY_DIR="$BUILD_DIR/Windows/x64/Release/private/bin"

TEST_BINARY="$BINARY_DIR/Microsoft.CognitiveServices.Speech.Tests.ParallelRunner"
[[ -x $TEST_BINARY ]] ||
  exitWithSuccess "Test %s: skip, no test code\n" "$T"

NORMALIZER_DIR="$SPEECHSDK_COMPEVAL_INPUT_DIR/NormAndAlign"
NORMALIZER_BINARY="$NORMALIZER_DIR/NormAndAlign"
[[ -x $NORMALIZER_BINARY ]] ||
  exitWithSuccess "Test %s: skip, no normalizer\n" "$T"

DLL_DIR="$BUILD_DIR/Windows/x64/Release/public/lib"
[[ -d $DLL_DIR ]] || {
  echo Cannot find DLL directory: $BIN_DIR
  exit 1
}

# Copy required dependencies
cp "$DLL_DIR"/*.dll "$BINARY_DIR"

"$TEST_BINARY" $SPEECHSDK_COMPEVAL_NUM_THREADS "$SPEECHSDK_COMPEVAL_INPUT_DIR" "$SPEECHSDK_COMPEVAL_OUTPUT_DIR" "$SPEECHSDK_COMPEVAL_ENDPOINT" $SPEECHSDK_COMPEVAL_KEY $SPEECHSDK_COMPEVAL_MODE

EXPECTED_TRANSCRIPTION="$(cygpath -aw "$SPEECHSDK_COMPEVAL_INPUT_DIR/CompEval/Transcriptions/en-US_complete.txt")"
ACTUAL_TRANSCRIPTION="$(cygpath -aw "$SPEECHSDK_COMPEVAL_OUTPUT_DIR/output.txt")"
SUMMARY="$(cygpath -aw "$SPEECHSDK_COMPEVAL_OUTPUT_DIR/summary.txt")"

# Can run normalizer only from the directory it is in due to hardcoded pinvokes.
cd "$NORMALIZER_DIR"
./NormAndAlign "$EXPECTED_TRANSCRIPTION" "$ACTUAL_TRANSCRIPTION" "$SUMMARY"

ACCEPTABLE_WER=8.82
WER=$(grep -P 'Filtered\tMerged' $SUMMARY | cut -d$'\t' -f9)
echo Calculated WER "$WER"

IS_WER_OK=$(awk 'BEGIN{ print '$WER'<'$ACCEPTABLE_WER' }')

if [[ $IS_WER_OK -eq 1 ]];then
  echo "Test succeeded"
  exit 0
fi

echo "Test failed, WER is too high."
exit 1

