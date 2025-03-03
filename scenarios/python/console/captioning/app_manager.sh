#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status.

action=$1

# Check if the correct number of arguments is provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 {build|run|configure}"
    exit 1
fi

function test_python_installed {
    command -v python3 &>/dev/null
}

function test_pip_installed {
    command -v pip3 &>/dev/null
}

if [ "$action" = "configure" ]; then
    if ! test_python_installed; then
        echo "Python is not installed. Please install Python to proceed." >&2
        exit 1
    fi

    if ! test_pip_installed; then
        echo "pip is not installed. Please install pip to proceed." >&2
        exit 1
    fi
elif [ "$action" = "build" ]; then
    echo "Installing azure-cognitiveservices-speech package..."
    if pip3 install azure-cognitiveservices-speech; then
        echo "Package installation succeeded."
    else
        echo "Package installation failed. Please check your pip installation." >&2
        exit 1
    fi
elif [ "$action" = "run" ]; then
    # Load environment variables from .env file
    ENV_FILE=".env/.env.dev"
    if [ -f "$ENV_FILE" ]; then
        source "$ENV_FILE"

    else
        echo "Environment file $ENV_FILE not found. You can create one to set environment variables or manually set secrets in environment variables."
    fi

    export SPEECH_KEY=$SPEECH_RESOURCE_KEY
    export SPEECH_REGION=$SERVICE_REGION

    read -p "Please enter the path to the input .wav file (press Enter to use the default microphone): " inputFile
    if [ -z "$inputFile" ]; then
        python3 ./captioning.py
    else
        python3 ./captioning.py --input "$inputFile"
    fi

else
    echo "Invalid action: $action"
    echo "Usage: $0 {build|run|configure}"
    exit 1
fi
