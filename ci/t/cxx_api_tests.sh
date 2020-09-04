#!/usr/bin/env bash
T="$(basename "$0" .sh)"
BUILD_DIR="$1"
PLATFORM="$2"
BINARY_DIR="$3"
TESTSET="${4:-dev}"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

isOneOf "$PLATFORM" {{Windows,Linux}-{x86,x64},Linux-{arm32,arm64},OSX-x64}-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

TEST_CODE="$BINARY_DIR/cxx_api_tests"
[[ -x $TEST_CODE ]] ||
  exitWithSuccess "Test %s: skip, no test code\n" "$T"

. "$SCRIPT_DIR/../test-harness.sh" || exit 1

PATTERN=
case $TESTSET in
  prod|int)
    # All default (non-hidden) + the specified tag
    PATTERN="~[.],[$TESTSET]"
    ;;
esac

RUN_OFFLINE_UNIDEC_TESTS=false
RUN_HYBRID_TTS_TESTS=false
case $PLATFORM in
  Linux-arm64*)
    UNIDEC_RUNTIME_PATH="$SCRIPT_DIR/../external/unidec/Richland.Speech.UnidecRuntime/linux-aarch64-platforms/lib"
    if [[ -d "$UNIDEC_RUNTIME_PATH" ]]; then
      LD_LIBRARY_PATH="${UNIDEC_RUNTIME_PATH}:$LD_LIBRARY_PATH"
      RUN_OFFLINE_UNIDEC_TESTS=true
    fi
    HYBRID_TTS_RUNTIME_PATH="$SCRIPT_DIR/../external/offline_tts/Linux/arm64"
    if [[ -d "$HYBRID_TTS_RUNTIME_PATH" ]]; then
      LD_LIBRARY_PATH="${HYBRID_TTS_RUNTIME_PATH}:$LD_LIBRARY_PATH"
      RUN_HYBRID_TTS_TESTS=true
    fi
    ;;
  Linux-arm32*)
    HYBRID_TTS_RUNTIME_PATH="$SCRIPT_DIR/../external/offline_tts/Linux/arm32"
    if [[ -d "$HYBRID_TTS_RUNTIME_PATH" ]]; then
      LD_LIBRARY_PATH="${HYBRID_TTS_RUNTIME_PATH}:$LD_LIBRARY_PATH"
      RUN_HYBRID_TTS_TESTS=true
    fi
    ;;
  Linux-x64*)
    UNIDEC_RUNTIME_PATH="$SCRIPT_DIR/../external/unidec/Richland.Speech.UnidecRuntime/linux-amd64-platform/lib"
    if [[ -d "$UNIDEC_RUNTIME_PATH" ]]; then
      LD_LIBRARY_PATH="${UNIDEC_RUNTIME_PATH}:$LD_LIBRARY_PATH"
      RUN_OFFLINE_UNIDEC_TESTS=true
    fi
    HYBRID_TTS_RUNTIME_PATH="$SCRIPT_DIR/../external/offline_tts/Linux/x64"
    if [[ -d "$HYBRID_TTS_RUNTIME_PATH" ]]; then
      LD_LIBRARY_PATH="${HYBRID_TTS_RUNTIME_PATH}:$LD_LIBRARY_PATH"
      RUN_HYBRID_TTS_TESTS=true
    fi
    ;;
  Windows-x64*)
    GSTREAMER_RUNTIME_PATH="$SCRIPT_DIR/../gstreamer/1.0/x86_64/bin"
    if [[ -d "$GSTREAMER_RUNTIME_PATH" ]]; then
      PATH="${GSTREAMER_RUNTIME_PATH}:$PATH"
    fi
    UNIDEC_RUNTIME_PATH="$SCRIPT_DIR/../external/unidec/Richland.Speech.UnidecRuntime/native"
    if [[ -d "$UNIDEC_RUNTIME_PATH" ]]; then
      PATH="${UNIDEC_RUNTIME_PATH}:$PATH"
      RUN_OFFLINE_UNIDEC_TESTS=true
    fi
    HYBRID_TTS_RUNTIME_PATH="$SCRIPT_DIR/../external/offline_tts/Windows/x64"
    if [[ -d "$HYBRID_TTS_RUNTIME_PATH" ]]; then
      PATH="${HYBRID_TTS_RUNTIME_PATH}:$PATH"
      RUN_HYBRID_TTS_TESTS=true
    fi
    ;;
  Windows-x86*)
    GSTREAMER_RUNTIME_PATH="$SCRIPT_DIR/../gstreamer/1.0/x86/bin"
    if [[ -d "$GSTREAMER_RUNTIME_PATH" ]]; then
      PATH="${GSTREAMER_RUNTIME_PATH}:$PATH"
    fi
    ;;
esac

if [[ $RUN_OFFLINE_UNIDEC_TESTS = true ]]; then
  OFFLINE_MODEL_PATH_ROOT="external/unidec/Unidec.Model/model"
  OFFLINE_MODEL_LANGUAGE="en-US"
else
  if [[ $PATTERN ]]; then
    PATTERN="~[unidec]$PATTERN"
  else
    PATTERN="~[unidec]~[.]"
  fi
  OFFLINE_MODEL_PATH_ROOT=""
  OFFLINE_MODEL_LANGUAGE=""
fi

if [[ $RUN_HYBRID_TTS_TESTS = true ]]; then
  OFFLINE_VOICE_PATH="tests/input/synthesis/Mark"
else
  if [[ $PATTERN ]]; then
    PATTERN="~[hybrid_tts]$PATTERN"
  else
    PATTERN="~[hybrid_tts]~[.]"
  fi
  OFFLINE_VOICE_PATH=""
fi

RUN_OFFLINE_RNNT_TESTS=false
BUILD_CONFIGURATION=$(echo $PLATFORM | cut -f3 -d-)
case $PLATFORM in
  Linux-x86*)
    RNNT_RUNTIME_PATH="$SCRIPT_DIR/../external/mas/Linux/x86/Release"
    if [[ -d "$RNNT_RUNTIME_PATH" ]]; then
      LD_LIBRARY_PATH="${RNNT_RUNTIME_PATH}:$LD_LIBRARY_PATH"
      RUN_OFFLINE_RNNT_TESTS=true
    fi
    ;;
  Linux-x64*)
    RNNT_RUNTIME_PATH="$SCRIPT_DIR/../external/mas/Linux/x64/Release"
    if [[ -d "$RNNT_RUNTIME_PATH" ]]; then
      LD_LIBRARY_PATH="${RNNT_RUNTIME_PATH}:$LD_LIBRARY_PATH"
      RUN_OFFLINE_RNNT_TESTS=true
    fi
    ;;
  Windows-x86*)
    RNNT_RUNTIME_PATH="$SCRIPT_DIR/../external/mas/Windows/Win32/Release"
    if [[ -d "$RNNT_RUNTIME_PATH" ]]; then
      PATH="${RNNT_RUNTIME_PATH}:$PATH"
      RUN_OFFLINE_RNNT_TESTS=true
    fi
    ;;
  Windows-x64*)
    RNNT_RUNTIME_PATH="$SCRIPT_DIR/../external/mas/Windows/x64/Release"
    if [[ -d "$RNNT_RUNTIME_PATH" ]]; then
      PATH="${RNNT_RUNTIME_PATH}:$PATH"
      RUN_OFFLINE_RNNT_TESTS=true
    fi
    ;;
  OSX-x64*)
    RNNT_RUNTIME_PATH="$SCRIPT_DIR/../external/mas/OSX/x64/Release"
    if [[ -d "$RNNT_RUNTIME_PATH" ]]; then
      export LD_LIBRARY_PATH="${RNNT_RUNTIME_PATH}:$LD_LIBRARY_PATH"
      RUN_OFFLINE_RNNT_TESTS=true
    fi
    ;;
esac

if [[ $RUN_OFFLINE_RNNT_TESTS = true ]]; then
  declare -A rnnt_spec
  IFS="="
  while read -r key value
  do
    rnnt_spec[$key]=$value
  done < "external/mas/RNNT.Model/model/rnnt_test.config"
  unset IFS
  RNNT_MODEL_PATH="external/mas/RNNT.Model/model/rnnt_test.model"
  RNNT_MODEL_SPEC="${RNNT_MODEL_PATH},${rnnt_spec["matrixKind"]},${rnnt_spec["beamWidth"]}"
  RNNT_MODEL_SPEC+=",${rnnt_spec["insertionBoost"]},${rnnt_spec["recombineKind"]}"
  RNNT_MODEL_SPEC+=",${rnnt_spec["scoreNormKind"]},${rnnt_spec["beamSortKind"]}"
  RNNT_MODEL_SPEC+=",${rnnt_spec["baseFeatDim"]}"
  RNNT_TOKENS="external/mas/RNNT.Model/model/rnnt_test.token.list"
else
  if [[ $PATTERN ]]; then
    PATTERN="~[rnnt]$PATTERN"
  else
    PATTERN="~[rnnt]~[.]"
  fi
  RNNT_MODEL_SPEC=""
  RNNT_TOKENS=""
fi

# Run CTS tests only on specific targets to reduce concurrent sessions
if [[ ! "$PLATFORM" =~ (Windows|Linux|OSX)-(arm64|x64)-Release ]]; then
  if [[ $PATTERN ]]; then
    PATTERN="~[conversation_transcriber]$PATTERN"
  else
    PATTERN="~[conversation_transcriber]~[.]"
  fi
fi

runCatchSuite \
  TESTRUNNER \
  "test-catch-$T-$PLATFORM-$RANDOM" \
  "$PLATFORM" \
  "" \
  "$T" \
  240 \
  "$PATTERN" \
  "$TEST_CODE" \
    --offlineModelPathRoot="$OFFLINE_MODEL_PATH_ROOT" \
    --offlineModelLanguage="$OFFLINE_MODEL_LANGUAGE" \
    --offlineVoicePath="$OFFLINE_VOICE_PATH" \
    --rnntModelSpec="$RNNT_MODEL_SPEC" \
    --rnntTokens="$RNNT_TOKENS"
