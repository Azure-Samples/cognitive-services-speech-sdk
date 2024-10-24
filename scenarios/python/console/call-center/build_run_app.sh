#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status.

action="$1"

# Check if the correct number of arguments is provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 {build|run}"
    exit 1
fi

# Function to install packages
install_packages() {
    if ! command -v pip &> /dev/null; then
        echo "The pip is not installed. Installing..."
        sudo apt update
        sudo apt install python3-pip -y

        if [ $? -eq 0 ]; then
            echo "The pip was successfully installed!"
        else
            echo "The pip installation failed."
            exit 1
        fi
    fi

    pip install azure-cognitiveservices-speech
    if [ $? -eq 0 ]; then
        echo "The azure-cognitiveservices-speech package was successfully installed!"
    else
        echo "The azure-cognitiveservices-speech package installation failed."
        exit 1
    fi
}

if [ "$action" == "build" ]; then
    if ! command -v python3 &>/dev/null; then
        echo "Python 3 is not installed. Installing..."
        sudo apt update
        sudo apt install -y python3.10

        PYTHON_PATH=$(command -v python3.10)
        if [ -z "$PYTHON_PATH" ]; then
            echo "Python 3.10 installation failed. Exiting..."
            exit 1
        else
            echo "Python 3.10 is installed."
            sudo update-alternatives --install /usr/bin/python3 python3 "$PYTHON_PATH" 1
        fi
    fi

    install_packages
elif [ "$action" == "run" ]; then
    python ./call_center.py --speechKey *** --speechRegion *** --languageKey *** --languageEndpoint ***

    if [ $? -ne 0 ]; then
        echo "Python is not found. Please first run the script with build action to install Python." >&2
        exit 1
    fi
else
    echo "Invalid action: $action"
    echo "Usage: $0 {build|run}"
    exit 1
fi