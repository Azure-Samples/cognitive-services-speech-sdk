#!/bin/bash

action=$1

function check_python_installed() {
    command -v python >/dev/null 2>&1
}

function check_pip_installed() {
    command -v pip >/dev/null 2>&1
}

if [ "$action" == "build" ]; then
    if ! check_python_installed; then
        echo -e "\e[31mPython is not installed. Please install Python to proceed.\e[0m"
        exit 1
    fi

    if ! check_pip_installed; then
        echo -e "\e[31mpip is not installed. Please install pip to proceed.\e[0m"
        exit 1
    fi

    echo "Installing requirements packages..."
    if pip install -r requirements.txt; then
        echo -e "\e[32mRequirements packages installation succeeded.\e[0m"
    else
        echo -e "\e[31mRequirements packages installation failed. Please check your pip installation.\e[0m"
        exit 1
    fi
elif [ "$action" == "run" ]; then
    configFilePath="config.json"
    if [ -f "$configFilePath" ]; then
        subscriptionKey=$(jq -r '.SubscriptionKey' "$configFilePath")
        serviceRegion=$(jq -r '.ServiceRegion' "$configFilePath")
        customSubDomainName=$(jq -r '.CustomSubDomainName' "$configFilePath")
        endpoint="https://${customSubDomainName}.openai.azure.com/"

        if [ -n "$subscriptionKey" ]; then
            export SPEECH_KEY="$subscriptionKey"
            export AZURE_OPENAI_API_KEY="$subscriptionKey"
        fi

        if [ -n "$serviceRegion" ]; then
            export SPEECH_REGION="$serviceRegion"
        fi

        if [ -n "$endpoint" ]; then
            export AZURE_OPENAI_ENDPOINT="$endpoint"
        fi

        echo "Environment variables loaded from $configFilePath"
    else
        echo "File not found: $configFilePath"
    fi

    python -m flask run -h 0.0.0.0 -p 5000
else
    echo -e "\e[31mInvalid action: $action\e[0m"
    echo "Usage: $0 build or $0 run"
    exit 1
fi
