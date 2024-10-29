#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status.

action="$1"

# Check if the correct number of arguments is provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 {build|run}"
    exit 1
fi

if [[ $action == "build" ]]; then
    if ! command -v python &> /dev/null; then
        echo "Python is not installed. Please install Python first. Exiting..."
        exit 1
    fi

    if ! command -v pip &> /dev/null; then
        echo "pip is not installed. Please install pip first. Exiting..."
        exit 1
    fi

    pip install azure-cognitiveservices-speech
    if [ $? -ne 0 ]; then
        echo "Installation Microsoft.CognitiveServices.Speech package is failed, exiting..."
        exit 1
    fi
elif [[ $action == "run" ]]; then
    if command -v python &> /dev/null; then
        python quickstart.py
    else
        echo "Python is not installed. Please install Python first. Exiting..." >&2
        exit 1
    fi
else
    echo "Invalid action: $action"
    echo "Usage: $0 {build|run}"
    exit 1
fi
