#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status.

action="$1"

# Check if the correct number of arguments is provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 {build|run}"
    exit 1
fi

if [[ $action == "build" ]]; then
    if ! command -v python3 &> /dev/null; then
        echo "Python3 is not installed. Please install Python3 first. Exiting..."
        exit 1
    fi

    if ! command -v pip3 &> /dev/null; then
        echo "pip3 is not installed. Please install pip3 first. Exiting..."
        exit 1
    fi

    pip3 install azure-cognitiveservices-speech
    if [ $? -eq 0 ]; then
        echo "Installation Microsoft.CognitiveServices.Speech package is succeeded."
    else
        echo "Installation Microsoft.CognitiveServices.Speech package is failed, exiting..."
        exit 1
    fi
elif [[ $action == "run" ]]; then
    if command -v python3 &> /dev/null; then
        python3 quickstart.py
    else
        echo "Python3 is not installed. Please install Python3 first. Exiting..." >&2
        exit 1
    fi
else
    echo "Invalid action: $action"
    echo "Usage: $0 {build|run}"
    exit 1
fi
