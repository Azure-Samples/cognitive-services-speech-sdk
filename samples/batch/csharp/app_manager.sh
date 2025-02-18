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
    # Load environment variables from .env file
    ENV_FILE=".env/.env.dev"
    if [ -f "$envFilePath" ]; then
        # Read each line of the file and process it
        while IFS= read -r line; do
            # Ignore empty lines and lines that start with `#` (comments)
            if [[ -n "$line" && ! "$line" =~ ^\s*# ]]; then
                # Split each line into key and value
                IFS='=' read -r key value <<< "$line"
                key=$(echo "$key" | xargs)   # Trim whitespace from key
                value=$(echo "$value" | xargs) # Trim whitespace from value

                # Set the environment variable in the current session
                export "$key=$value"
            fi
        done < "$envFilePath"
    else
        echo "File not found: $envFilePath. You can create one to set environment variables or manually set secrets."
    fi

    read -p "Do you want to use RecordingsBlobUris [y] or RecordingsContainerUri [n]? Please enter y/n: " useBlobUrisOrContainerUri
    if [[ "$useBlobUrisOrContainerUri" =~ ^[yY]$ ]]; then
        choice=0
    elif [[ "$useBlobUrisOrContainerUri" =~ ^[nN]$ ]]; then
        choice=1
    else
        echo -e "Invalid input. Exiting..."
        exit 1
    fi

    if [ "$choice" -eq 0 ]; then
        read -p "Please enter SAS URI pointing to audio files stored in Azure Blob Storage. If input multiple, please separate them with commas: " recordingsBlobUris
        if [ -z "$recordingsBlobUris" ]; then
            echo -e "Not enter the Azure Blob SAS URL of the input audio file."
            exit 1
        fi
    else
        read -p "Please enter the SAS URI of the container in Azure Blob Storage where the audio files are stored: " recordingsContainerUri
        if [ -z "$recordingsContainerUri" ]; then
            echo -e "Not enter the Azure Blob Container SAS URI of the input audio file."
            exit 1
        fi
    fi

    read -p "Please enter the locale of the input audio file (e.g. en-US, zh-CN, etc.): " recordingsLocale
    if [ -z "$recordingsLocale" ]; then
        echo -e "Not enter the locale."
        exit 1
    fi

    args=("--key" "$SPEECH_RESOURCE_KEY"
        "--region" "$SERVICE_REGION"
        "--locale" "$recordingsLocale")

    if [ "$choice" -eq 0 ]; then
        args+=("--recordingsBlobUris" "$recordingsBlobUris")
    else
        args+=("--recordingsContainerUri" "$recordingsContainerUri")
    fi

    dotnet batchclient/bin/Debug/net8.0/BatchTranscriptions.dll "${args[@]}"
else
    echo "Invalid action: $action"
    echo "Usage: $0 {build|run|configure}"
    exit 1
fi