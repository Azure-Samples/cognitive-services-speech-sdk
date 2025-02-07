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
    sudo ./dotnet-install.sh --version 8.0.405 --install-dir $dotnetInstallPath

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
    dotnet build batchclient/batchclient.csproj
    if [ $? -ne 0 ]; then
        echo "Build failed, exiting..."
        exit 1
    fi
elif [ "$action" = "run" ]; then
    ENV_FILE_PATH=".env/.env.dev"
    if [[ -f "$ENV_FILE_PATH" ]]; then
        while IFS= read -r line; do
            if [[ -n "$line" && ! "$line" =~ ^\s*# ]]; then
                IFS='=' read -r key value <<< "$line"
                export "$key"="$value"
            fi
        done < "$ENV_FILE_PATH"

        export SPEECH_KEY=$SPEECH_RESOURCE_KEY
        export SPEECH_REGION=$SERVICE_REGION
    else
        echo "File not found: $ENV_FILE_PATH. You can create one to set environment variables or manually set secrets in environment variables."
    fi

    read -p "Please enter SAS URI pointing to audio files stored in Azure Blob Storage. If input multiple, please separate them with commas: " recordingsBlobUri
    if [ -z "$recordingsBlobUri" ]; then
        echo "Not enter the Azure Blob SAS URL of the input audio file. Exiting..."
        exit 1
    fi

    read -p "Please enter the locale of the input audio file (e.g. en-US, zh-CN, etc.): " recordingsLocale
    if [ -z "$recordingsLocale" ]; then
        echo "Not enter the locale. Exiting..."
        exit 1
    fi

    dotnet batchclient/bin/Debug/net8.0/BatchTranscriptions.dll --recordingsBlobUri $recordingsBlobUri --locale  $recordingsLocale
else
    echo "Invalid action: $action"
    echo "Usage: $0 {build|run|configure}"
    exit 1
fi