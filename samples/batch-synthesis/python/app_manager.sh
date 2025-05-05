#!/bin/bash

action=$1

function check_python_installed() {
    command -v python >/dev/null 2>&1
}

function check_pip_installed() {
    command -v pip >/dev/null 2>&1
}

if [[ "$ACTION" == "configure" ]]; then
    echo "Installing Linux dependencies..."
    sudo apt-get update
    sudo apt-get install -y libssl-dev libasound2

    if ! check_python_installed; then
        echo -e "\e[31mPython is not installed. Please install Python to proceed.\e[0m"
        exit 1
    fi

    if ! check_pip_installed; then
        echo -e "\e[31mpip is not installed. Please install pip to proceed.\e[0m"
        exit 1
    fi

    echo "Installing requirements packages..."
    if ! pip install -r requirements.txt; then
        exit 1
    fi
elif [[ "$ACTION" == "run" ]]; then
    ENV_FILE_PATH=".env/.env.dev"
    if [[ -f "$ENV_FILE_PATH" ]]; then
        while IFS= read -r line; do
            if [[ -n "$line" && ! "$line" =~ ^\s*# ]]; then
                IFS='=' read -r key value <<< "$line"
                export "$key"="$value"
            fi
        done < "$ENV_FILE_PATH"

        export CUSTOM_DOMAIN=$CUSTOM_SUBDOMAIN_NAME
        export SPEECH_KEY=$SPEECH_RESOURCE_KEY
        export SPEECH_REGION=$SERVICE_REGION
    else
        echo "File not found: $ENV_FILE_PATH. You can create one to set environment variables or manually set secrets in environment variables."
    fi

    echo "Running synthesis.py"
    python synthesis.py
else
    echo -e "\e[31mInvalid action: $action\e[0m"
    echo "Usage: $0 configure or $0 run"
    exit 1
fi