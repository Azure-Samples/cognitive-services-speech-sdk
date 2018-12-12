#!/usr/bin/env bash

set -e
set -x

T="$(basename "$0" .sh)"
BUILD_DIR=`realpath "$1"`
PLATFORM="$2"
BINARY_DIR="$3"

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/../functions.sh" || exit 1

## assumes that build_dir is one level deeper than source

isOneOf "$PLATFORM" {{Windows,Linux,OSX}-x64,Windows-x86}-{Debug,Release} ||
  exitWithSuccess "Test %s: skip on this platform\n" "$T"

VIRTUALENV_NAME=carbontest$$

if [[ $PLATFORM == Windows* ]]; then
    PYTHON=python
else
    PYTHON=python3
fi

virtualenv -p ${PYTHON} ${VIRTUALENV_NAME}

if [[ $SPEECHSDK_BUILD_AGENT_PLATFORM == Windows* ]]; then
    VIRTUALENV_PYTHON=${VIRTUALENV_NAME}/Scripts/python.exe
else
    VIRTUALENV_PYTHON=${VIRTUALENV_NAME}/bin/python
fi

# install dependencies inside the virtualenv
${VIRTUALENV_PYTHON} -m pip install pytest==4.0.0

if !existsExactlyOneFile ${BUILD_DIR}/dist/*.whl; then
    exitWithError "there is more than one wheel built, don't know which one to choose"
fi

# try installing the azure-cognitiveservices-speech wheel
${VIRTUALENV_PYTHON} -m pip install ${BUILD_DIR}/dist/*.whl

# run pytest on test files in the source tree
if [[ $SPEECHSDK_BUILD_AGENT_PLATFORM == Windows* ]]; then
    extra_args=--no-use-default-microphone
else
    extra_args=
fi

${VIRTUALENV_PYTHON} -m pytest -v ${SCRIPT_DIR}/../../source/bindings/python/test \
    --inputdir $SPEECHSDK_INPUTDIR/audio \
    --subscription $SPEECHSDK_SPEECH_KEY \
    --speech-region $SPEECHSDK_SPEECH_REGION \
    --luis-subscription $SPEECHSDK_LUIS_KEY \
    --luis-region $SPEECHSDK_LUIS_REGION \
    --language-understanding-app-id $SPEECHSDK_LUIS_HOMEAUTOMATION_APPID \
    $extra_args

# TODO: run samples as part of unit test
