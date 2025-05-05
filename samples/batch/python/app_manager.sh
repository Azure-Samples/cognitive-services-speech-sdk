#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status.

action="$1"

# Check if the correct number of arguments is provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 {configure|run}"
    exit 1
fi

if [ "$action" == "configure" ]; then
    command -v python3 >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "Python 3 is not found. Please install python3 first. Exiting..."
        exit 1
    fi

    packageName="swagger_client"
    python -c "import $packageName" 2>/dev/null
    if [ $? -ne 0 ]; then
        echo -e "$packageName is not installed. Please follow the guide in README.md to install. Exiting..."
        exit 1
    fi

    pip install requests
elif [ "$action" == "run" ]; then
    # Load environment variables from .env file
    ENV_FILE=".env/.env.dev" 
    if [ -f "$ENV_FILE" ]; then
        source "$ENV_FILE"

        # Ensure environment variables are available to the C++ binary
        export SPEECH_KEY=$SPEECH_RESOURCE_KEY
        export SPEECH_REGION=$SERVICE_REGION
    else
        echo "Environment file $ENV_FILE not found. You can create one to set environment variables or manually set secrets in environment variables."
    fi

    read -p "Do you want to use RecordingsBlobUris [1] or RecordingsContainerUri [2]? Please enter 1/2: " useBlobUrisOrContainerUri
    if [[ "$useBlobUrisOrContainerUri" == "1" ]]; then
        choice=0
    elif [[ "$useBlobUrisOrContainerUri" == "2" ]]; then
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

    if command -v python3 &> /dev/null; then
        args=( "--service_key" "$SPEECH_KEY"
               "--service_region" "$SPEECH_REGION"
               "--locale" "$recordingsLocale" )

        if [ "$choice" -eq 0 ]; then
            args+=( "--recordings_blob_uris" "$recordingsBlobUris" )
        else
            args+=( "--recordings_container_uri" "$recordingsContainerUri" )
        fi

        python3 ./python-client/main.py "${args[@]}"
    else
        echo -e "Python 3 is not found. Please install python3 first. Exiting..."
        exit 1
    fi
else
    echo "Invalid action: $action"
    echo "Usage: $0 {configure|run}"
    exit 1
fi