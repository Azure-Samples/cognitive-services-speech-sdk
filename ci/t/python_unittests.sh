#!/usr/bin/env bash

set -e -u -o pipefail

T="$(basename "$0" .sh)"
BUILD_DIR=`realpath "$1"`
PLATFORM="$2"
BINARY_DIR="$3"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

## assumes that build_dir is one level deeper than source

isOneOf "$PLATFORM" {{Windows,Linux,OSX}-x64,Windows-x86}-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

VIRTUALENV_NAME=speechsdktest$$

if [[ $PLATFORM == Windows* ]]; then
    PYTHON=python
else
    PYTHON=python3
fi

virtualenv -p ${PYTHON} ${VIRTUALENV_NAME}

if [[ $PLATFORM == Windows-* ]]; then
    VIRTUALENV_PYTHON=${PWD}/${VIRTUALENV_NAME}/Scripts/python.exe
else
    VIRTUALENV_PYTHON=${PWD}/${VIRTUALENV_NAME}/bin/python
fi

# install dependencies inside the virtualenv
${VIRTUALENV_PYTHON} -m pip install pytest==4.2.0

if ! existsExactlyOneFile ${BUILD_DIR}/*.whl; then
    exitWithError "there is more than one wheel built, don't know which one to choose\n"
fi

# try installing the azure-cognitiveservices-speech wheel
${VIRTUALENV_PYTHON} -m pip install ${BUILD_DIR}/*.whl

# run pytest on test files in the source tree
if [[ $PLATFORM == Windows-* ]] || [[ $PLATFORM == Linux* ]]; then
    extra_args=--no-use-default-microphone
else
    extra_args=
fi

UNITTEST_ERROR=false
${VIRTUALENV_PYTHON} -m pytest -v ${SCRIPT_DIR}/../../source/bindings/python/test \
    --inputdir $SPEECHSDK_INPUTDIR/audio \
    --subscription $SPEECHSDK_SPEECH_KEY \
    --speech-region $SPEECHSDK_SPEECH_REGION \
    --luis-subscription $SPEECHSDK_LUIS_KEY \
    --luis-region $SPEECHSDK_LUIS_REGION \
    --language-understanding-app-id $SPEECHSDK_LUIS_HOMEAUTOMATION_APPID \
    --junitxml=test-$T-$PLATFORM.xml \
    $extra_args || UNITTEST_ERROR=true

# run samples as part of unit test
source $SCRIPT_DIR/../test-harness.sh
cd ${SCRIPT_DIR}/../public_samples/samples/python/console

function runPythonSampleSuite {
  local usage testStateVarPrefix output platform redactStrings testsuiteName timeoutSeconds testCases
  usage="Usage: ${FUNCNAME[0]} <testStateVarPrefix> <output> <platform> <redactStrings> <testsuiteName> <timeoutSeconds> <command...>"
  testStateVarPrefix="${1?$usage}"
  output="${2?$usage}"
  platform="${3?$usage}"
  redactStrings="${4?$usage}"
  testsuiteName="${5?$usage}"
  timeoutSeconds="${6?$usage}"

  # monkeypatch session callbacks into samples without callbacks to get session ids
  cat > monkey.py <<SCRIPT
def newinit(obj, *args, **kwargs):
    obj.__old_init(*args, **kwargs)
    obj.session_started.connect(lambda evt: print('SESSION_STARTED: {}'.format(evt)))
    obj.session_stopped.connect(lambda evt: print('SESSION_STOPPED: {}'.format(evt)))
def patch(recotype):
    recotype.__old_init = recotype.__init__
    recotype.__init__ = newinit
SCRIPT

  testCases=(
    "import speech_sample; import monkey; monkey.patch(speech_sample.speechsdk.SpeechRecognizer); speech_sample.speech_recognize_once_from_file()"
    "import speech_sample; import monkey; monkey.patch(speech_sample.speechsdk.SpeechRecognizer); speech_sample.speech_recognize_once_from_file_with_customized_model()"
    "import speech_sample; import monkey; monkey.patch(speech_sample.speechsdk.SpeechRecognizer); speech_sample.speech_recognize_once_from_file_with_custom_endpoint_parameters()"
    "import speech_sample; import monkey; monkey.patch(speech_sample.speechsdk.SpeechRecognizer); speech_sample.speech_recognize_async_from_file()"
    "import speech_sample; speech_sample.speech_recognize_continuous_from_file()"
    "import speech_sample; speech_sample.speech_recognition_with_pull_stream()"
    "import speech_sample; speech_sample.speech_recognition_with_push_stream()"
    "import translation_sample; import monkey; monkey.patch(translation_sample.speechsdk.translation.TranslationRecognizer); translation_sample.translation_once_from_file()"
    "import translation_sample; translation_sample.translation_continuous()"
    "import intent_sample; import monkey; monkey.patch(intent_sample.speechsdk.intent.IntentRecognizer); intent_sample.recognize_intent_once_from_file()"
    "import intent_sample; intent_sample.recognize_intent_continuous()"
  )

  # these samples use microphone input
  # "import intent_sample; intent_sample.recognize_intent_once_from_mic()"
  # "import translation_sample; translation_sample.translation_once_from_mic()"
  # "import speech_sample; speech_sample.speech_recognize_once_from_mic()"
  # "import speech_sample; speech_recognize_keyword_from_microphone()"

  startTests "$testStateVarPrefix" "$output" "$platform" "$redactStrings"
  startSuite "$testStateVarPrefix" "$testsuiteName"

  for testCase in "${testCases[@]}"; do
    runTest "$testStateVarPrefix" "$testCase" "$timeoutSeconds" \
      ${VIRTUALENV_PYTHON} -c "$testCase" || true
  done

  endSuite "$testStateVarPrefix"
  endTests "$testStateVarPrefix"
}

runPythonSampleSuite \
  TESTRUNNER \
  "test-pysamples-$T-$PLATFORM" \
  "$PLATFORM" \
  "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_LUIS_KEY" \
  "pysamples-$T" \
  240
# If samples fail, script will stop here.
# Otherwise, we'll fail the script if the unit tests failed above:
[[ $UNITTEST_ERROR == false ]] || exitWithError "Python unit tests failed.\n"

