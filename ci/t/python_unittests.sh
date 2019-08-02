#!/usr/bin/env bash

set -e -u -o pipefail

T="$(basename "$0" .sh)"
BUILD_DIR=`realpath "$1"`
PLATFORM="$2"
BINARY_DIR="$3" # unused
TESTSET="${4:-dev}"

# needs to point to <repo root>/ci, will be overwritten by scripts that are sourced below
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"/..

function runUnitTests {
  local extra_args
  # run pytest on test files in the source tree
  if [[ $PLATFORM == Windows-* ]] || [[ $PLATFORM == Linux* ]]; then
    extra_args=(--no-use-default-microphone)
  fi

  ${VIRTUALENV_PYTHON} -m pytest -v ${SCRIPT_DIR}/../source/bindings/python/test \
      --inputdir $SPEECHSDK_INPUTDIR/audio \
      --subscription $SPEECHSDK_SPEECH_KEY \
      --speech-region $SPEECHSDK_SPEECH_REGION \
      --luis-subscription $SPEECHSDK_LUIS_KEY \
      --luis-region $SPEECHSDK_LUIS_REGION \
      --language-understanding-app-id $SPEECHSDK_LUIS_HOMEAUTOMATION_APPID \
      --junitxml=test-$T-$PLATFORM-py${MAJORMINOR}.xml \
      "${extra_args[@]}"
}


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
def patch(recotype):
    recotype.__old_init = recotype.__init__
    recotype.__init__ = newinit
SCRIPT

  # mock the user input for sample code which acquires it by input()
  cat > mock_input.py <<SCRIPT
import unittest.mock
class Mock:
    def __init__(self, mock_input_text = 'Hi there'):
        self._mock_input_text = mock_input_text
        self._invoke_count = 0
    def mock_input(self):
        if self._invoke_count == 0:
            self._invoke_count = self._invoke_count + 1
            return self._mock_input_text
        else:
            raise EOFError
    def run_with_mock_input(self, case):
        with unittest.mock.patch('builtins.input', self.mock_input):
            case()
SCRIPT

  sleepCmd="; import time; time.sleep(1.)"
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
    "import speech_synthesis_sample; import mock_input; mock_input.Mock().run_with_mock_input(speech_synthesis_sample.speech_synthesis_to_speaker)"
    "import speech_synthesis_sample; import mock_input; mock_input.Mock().run_with_mock_input(speech_synthesis_sample.speech_synthesis_with_language)"
    "import speech_synthesis_sample; import mock_input; mock_input.Mock().run_with_mock_input(speech_synthesis_sample.speech_synthesis_with_voice)"
    "import speech_synthesis_sample; import mock_input; mock_input.Mock().run_with_mock_input(speech_synthesis_sample.speech_synthesis_to_wave_file)"
    "import speech_synthesis_sample; import mock_input; mock_input.Mock().run_with_mock_input(speech_synthesis_sample.speech_synthesis_to_mp3_file)"
    "import speech_synthesis_sample; import mock_input; mock_input.Mock().run_with_mock_input(speech_synthesis_sample.speech_synthesis_to_pull_audio_output_stream)"
    "import speech_synthesis_sample; import mock_input; mock_input.Mock().run_with_mock_input(speech_synthesis_sample.speech_synthesis_to_push_audio_output_stream)"
    "import speech_synthesis_sample; import mock_input; mock_input.Mock().run_with_mock_input(speech_synthesis_sample.speech_synthesis_to_result)"
    "import speech_synthesis_sample; import mock_input; mock_input.Mock().run_with_mock_input(speech_synthesis_sample.speech_synthesis_to_audio_data_stream)"
    "import speech_synthesis_sample; import mock_input; mock_input.Mock().run_with_mock_input(speech_synthesis_sample.speech_synthesis_events)"
    "import speech_synthesis_sample; import mock_input; mock_input.Mock().run_with_mock_input(speech_synthesis_sample.speech_synthesis_word_boundary_event)"
  )
  # sleep a bit at the end of each sample to make sure the interpreter is still alive during cleanup
  testCases=( "${testCases[@]/%/"${sleepCmd}"}" )

  # these samples use microphone input
  # "import intent_sample; intent_sample.recognize_intent_once_from_mic()"
  # "import intent_sample; intent_sample.recognize_intent_once_async_from_mic()"
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

source "$SCRIPT_DIR/functions.sh" || exit 1
source $SCRIPT_DIR/test-harness.sh

platformParts=(${PLATFORM//-/ })
os="${platformParts[0]}"
arch="${platformParts[1]}"
flavor="${platformParts[2]}"

isOneOf "$PLATFORM" {{Windows,Linux,OSX}-x64,Windows-x86}-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"
[[ ${flavor} == Release ]] || exitWithSuccess "Test %s: run only on Release flavor\n" "$T"

IN_VSTS=$([[ -n ${SYSTEM_DEFINITIONID:-} && -n ${SYSTEM_COLLECTIONID:-} ]] && echo true || echo false)

if [[ $PLATFORM == Windows* ]]; then
  GLOBALPYTHON=python
else
  GLOBALPYTHON=python3
fi

if [[ ! $IN_VSTS ]]; then
  echo "Running outside VSTS; using system Python interpreter."
  PYTHONS=($GLOBALPYTHON)
  [[ $TESTSET = dev ]] || echo Testset $TESTSET not supported outside of VSTS.
else
  echo "Running inside VSTS"

  case $os in
    Linux)
      hostedtoolsPath=/opt/hostedtoolcache
      ;;
    Windows)
      hostedtoolsPath=C:/hostedtoolcache/windows
      ;;
    OSX)
      hostedtoolsPath=/Users/vsts/hostedtoolcache
      ;;
    *)
      exitWithError "Unexpected platform: $PLATFORM.\n"
      ;;
  esac

  pythonBasePath=${hostedtoolsPath}/Python

  case $TESTSET in
    prod|int)
      # Run tests for all Python versions
      PYTHONS=($(getLatest ${pythonBasePath}/3.5.*)/$arch/python
        $(getLatest ${pythonBasePath}/3.6.*)/$arch/python
        $(getLatest ${pythonBasePath}/3.7.*)/$arch/python
      )
      ;;
    dev)
      # Run tests for only a single Python version

      # defines which python version is used for tests during dev builds on which platform
      declare -A pythonVersionsByOs=([OSX]=3.5 [Linux]=3.6 [Windows]=3.7)

      PYTHONS=($(getLatest ${pythonBasePath}/${pythonVersionsByOs[$os]}.*)/$arch/python)
      ;;
    *)
      exitWithError "Bad test set definition: $TESTSET.\n"
  esac
fi

UNITTEST_ERROR=false
SAMPLE_ERROR=false

${GLOBALPYTHON} -m pip install virtualenv==16.3.0

for PYTHON in ${PYTHONS[@]}; do
  MAJORMINOR=$("${PYTHON}" -c "import sys; print('%s%s' % (sys.version_info[0:2]))")
  VIRTUALENV_NAME=speechsdktest-${MAJORMINOR}-$$

  ${GLOBALPYTHON} -m virtualenv -p ${PYTHON} ${VIRTUALENV_NAME}

  if [[ $PLATFORM == Windows-* ]]; then
      VIRTUALENV_PYTHON=${PWD}/${VIRTUALENV_NAME}/Scripts/python.exe
  else
      VIRTUALENV_PYTHON=${PWD}/${VIRTUALENV_NAME}/bin/python
  fi

  # install dependencies inside the virtualenv
  ${VIRTUALENV_PYTHON} -m pip install pytest==4.2.0 requests==2.21.0

  # try installing the azure-cognitiveservices-speech wheel
  wheel=(${BUILD_DIR}/*cp$MAJORMINOR*.whl)
  echo "Installing ${wheel}"
  ${VIRTUALENV_PYTHON} -m pip install ${wheel}

  runUnitTests || UNITTEST_ERROR=true

  # run samples as part of unit test
  pushd ${SCRIPT_DIR}/../public_samples/samples/python/console
  runPythonSampleSuite \
    TESTRUNNER \
    "test-py${MAJORMINOR}samples-$T-$PLATFORM" \
    "$PLATFORM" \
    "$SPEECHSDK_SPEECH_KEY $SPEECHSDK_LUIS_KEY $SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PPE_KEY $SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PROD_KEY" \
    "py${MAJORMINOR}samples-$T" \
    240 || SAMPLE_ERROR=true
  popd
done

[[ $UNITTEST_ERROR == false ]] && [[ $SAMPLE_ERROR == false ]] || exitWithError "Python unit tests failed.\n"

