#!/bin/bash

action=$1

function check_python_installed() {
    command -v python >/dev/null 2>&1
}

function check_pip_installed() {
    command -v pip >/dev/null 2>&1
}

if [ "$action" == "configure" ]; then
    echo "Installing Linux platform required dependencies..."
    sudo apt-get update
    sudo apt-get install -y build-essential libssl-dev libasound2 wget

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
elif [ "$action" == "run" ]; then

    # Load environment variables from .env file
    ENV_FILE=".env/.env.dev" 
    if [ -f "$ENV_FILE" ]; then
        source "$ENV_FILE"

        echo "SPEECH_RESOURCE_KEY:" $SPEECH_RESOURCE_KEY
        echo "SERVICE_REGION:" $SERVICE_REGION
        echo "CUSTOM_SUBDOMAIN_NAME:" $CUSTOM_SUBDOMAIN_NAME

        # Ensure environment variables are available to the C++ binary
        export SPEECH_KEY=$SPEECH_RESOURCE_KEY
        export AZURE_OPENAI_API_KEY=$SPEECH_RESOURCE_KEY
        export SPEECH_REGION=$SERVICE_REGION
        export AZURE_OPENAI_ENDPOINT="https://${CUSTOM_SUBDOMAIN_NAME}.openai.azure.com/"
        echo "Environment variables loaded from $ENV_FILE"

    else
        echo "Environment file $ENV_FILE not found. You can create one to set environment variables or manually set secrets in environment variables."
    fi
    # configFilePath="config.json"
    # if [ -f "$configFilePath" ]; then
    #     subscriptionKey=$(jq -r '.SubscriptionKey' "$configFilePath")
    #     serviceRegion=$(jq -r '.ServiceRegion' "$configFilePath")
    #     customSubDomainName=$(jq -r '.CustomSubDomainName' "$configFilePath")
    #     endpoint="https://${customSubDomainName}.openai.azure.com/"

    #     if [ -n "$subscriptionKey" ]; then
    #         export SPEECH_KEY="$subscriptionKey"
    #         export AZURE_OPENAI_API_KEY="$subscriptionKey"
    #     fi

    #     if [ -n "$serviceRegion" ]; then
    #         export SPEECH_REGION="$serviceRegion"
    #     fi

    #     if [ -n "$endpoint" ]; then
    #         export AZURE_OPENAI_ENDPOINT="$endpoint"
    #     fi

    #     echo "Environment variables loaded from $configFilePath"
    # else
    #     echo "File not found: $configFilePath"
    # fi

    # Start the Flask server in the background
    python -m flask run -h 0.0.0.0 -p 5000 &

    # Capture the PID of the Flask process
    # FLASK_PID=$!

    # # Add a small delay to give the server time to start
    # sleep 5

    # # Open the URL in the default browser
    # xdg-open "http://127.0.0.1:5000" &

    # # Keep the terminal session alive to prevent it from closing
    # echo -e "\e[32mServer is running. Press any key to exit.\e[0m"
    # read -n 1 -s
else
    echo -e "\e[31mInvalid action: $action\e[0m"
    echo "Usage: $0 configure or $0 run"
    exit 1
fi
