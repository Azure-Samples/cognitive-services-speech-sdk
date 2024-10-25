#!/bin/bash

action=$1

function test_python_installed {
    command -v python3 &>/dev/null
}

function test_pip_installed {
    command -v pip3 &>/dev/null
}

if [[ $action == "build" ]]; then
    if ! test_python_installed; then
        echo "Python is not installed. Please install Python to proceed." >&2
        exit 1
    fi

    if ! test_pip_installed; then
        echo "pip is not installed. Please install pip to proceed." >&2
        exit 1
    fi

    echo "Installing azure-cognitiveservices-speech package..."
    if pip3 install azure-cognitiveservices-speech; then
        echo "Package installation succeeded."
    else
        echo "Package installation failed. Please check your pip installation." >&2
        exit 1
    fi
elif [[ $action == "run" ]]; then
    envFilePath=".env/.env.dev"
    if [[ -f $envFilePath ]]; then
        while IFS='=' read -r key value; do
            if [[ -n $key && $key != \#* ]]; then
                key=$(echo $key | xargs)
                value=$(echo $value | xargs)

                if [[ $key == "SPEECH_RESOURCE_KEY" ]]; then
                    export SPEECH_KEY=$value
                elif [[ $key == "SERVICE_REGION" ]]; then
                    export SPEECH_REGION=$value
                fi
            fi
        done < "$envFilePath"
        echo "Environment variables loaded from $envFilePath"
    else
        echo "File not found: $envFilePath" >&2
    fi

    read -p "Do you want to specify an input file? (y/n) " useInputFile
    if [[ $useInputFile == 'y' ]]; then
        read -p "Please enter the path to the input .wav file: " inputFile
        python3 ./captioning.py --input "$inputFile"
    else
        python3 ./captioning.py
    fi
else
    echo "Invalid action: $action" >&2
    echo "Usage: $0 build or $0 run" >&2
    exit 1
fi
