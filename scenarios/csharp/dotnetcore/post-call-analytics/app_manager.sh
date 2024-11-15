#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status.

action="$1"

# Check if the correct number of arguments is provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 {build|run|configure}"
    exit 1
fi

dotnetInstallPath="/usr/share/dotnet"
export PATH=$PATH:$dotnetInstallPath

install_dotnet8() {
    echo "Installing .NET SDK 8.0..."
    wget https://dot.net/v1/dotnet-install.sh
    chmod +x dotnet-install.sh
    sudo ./dotnet-install.sh --version 8.0.403 --install-dir $dotnetInstallPath

    if [ $? -eq 0 ]; then
        echo 'export PATH=$PATH:/usr/share/dotnet' >> ~/.bashrc
        source ~/.bashrc
        sudo rm -f ./dotnet-install.sh
    else
        echo "Installation of .NET SDK 8.0 failed, exiting..."
        sudo rm -f ./dotnet-install.sh
        exit 1
    fi
}

if [ "$action" = "configure" ]; then
    echo "Installing Linux dependencies..."
    sudo apt-get update
    sudo apt-get install -y libssl-dev libasound2

    # Install .NET SDK if it's not already installed or version is less than 8.0
    if ! command -v dotnet &> /dev/null || [[ "$(dotnet --version)" < "8.0" ]]; then
        install_dotnet8
    else
        echo ".NET SDK 8.0 is already installed"
    fi
elif [ "$action" = "build" ]; then

    dotnet build ./post-call-analytics.sln
    if [ $? -ne 0 ]; then
        echo "Build failed, exiting..."
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

    echo "Please enter the name of your Azure OpenAI deployment name (hit Enter to skip):"
    read aoaiDeploymentName
    if [ ! -z "$aoaiDeploymentName" ]; then
        export AOAI_DEPLOYMENT_NAME=$aoaiDeploymentName
        export AOAI_KEY=$SPEECH_RESOURCE_KEY
        export AOAI_ENDPOINT="https://${CUSTOM_SUBDOMAIN_NAME}.openai.azure.com/"
    else
        # unset APAI parameters if deployment name is not provided
        export AOAI_DEPLOYMENT_NAME=""
        export AOAI_KEY=""
        export AOAI_ENDPOINT=""
    fi

    echo "Please enter the path to the input audio file:"
    read inputFile
    if [ -z "$inputFile" ]; then
        echo "No input file specified. Exiting..."
        exit 1
    fi

    echo "Environment variables set:\n SPEECH_KEY=$SPEECH_KEY\n SPEECH_REGION=$SPEECH_REGION\n AOAI_KEY=$AOAI_KEY\n AOAI_ENDPOINT=$AOAI_ENDPOINT\n AOAI_DEPLOYMENT_NAME=$AOAI_DEPLOYMENT_NAME"

    dotnet run --project ./post-call-analytics.csproj $inputFile
else
    echo "Invalid action: $action"
    echo "Usage: $0 {build|run|configure}"
    exit 1
fi