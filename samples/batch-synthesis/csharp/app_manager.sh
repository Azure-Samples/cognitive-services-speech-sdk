#!/bin/bash

ACTION=$1
DOTNET_PATH="dotnet"
DOTNET_VERSION="8.0"
DOTNET_TEMP_DIR="${TEMP:-/tmp}/dotnet"
DOTNET_TEMP_PATH="$DOTNET_TEMP_DIR/dotnet"

function is_dotnet_meet_requirement {
    if ! command -v $DOTNET_PATH &> /dev/null; then
        return 1
    fi

    DOTNET_VERSION_INSTALLED=$($DOTNET_PATH --version)

    if [[ $(echo -e "$DOTNET_VERSION_INSTALLED\n$DOTNET_VERSION" | sort -V | head -n 1) != "$DOTNET_VERSION" ]]; then
        echo "Dotnet version is below the required version ($DOTNET_VERSION). Found version: $DOTNET_VERSION_INSTALLED" >&2
        return 1
    fi
    return 0
}

function install_dotnet {
    echo "Installing .NET SDK 8.0..."

    curl -sSL https://dot.net/v1/dotnet-install.sh -o dotnet-install.sh
    if [[ $? -ne 0 ]]; then
        echo "Failed to download dotnet-install.sh, exiting..." >&2
        exit 1
    fi

    bash dotnet-install.sh --install-dir $DOTNET_TEMP_DIR --version 8.0.405
    if [[ $? -ne 0 ]]; then
        echo "Failed to install .NET SDK, exiting..." >&2
        exit 1
    fi

    export PATH="$PATH:$DOTNET_TEMP_DIR"

    echo ".NET 8.0 installed successfully."
    rm -f dotnet-install.sh
}

if [[ "$ACTION" == "configure" ]]; then
    echo "Installing Linux dependencies..."
    sudo apt-get update
    sudo apt-get install -y libssl-dev libasound2
    
    # Install .NET SDK if it's not already installed or version is less than 8.0
    if ! command -v dotnet &> /dev/null || [[ "$(dotnet --version)" < "8.0" ]]; then
        install_dotnet
    else
        echo ".NET SDK 8.0 is already installed"
    fi
elif [[ "$ACTION" == "build" ]]; then
    dotnet build ./BatchSynthesisSample.sln
    if [ $? -ne 0 ]; then
        echo "Build failed, exiting..."
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

        export SPEECH_ENDPOINT="https://$CUSTOM_SUBDOMAIN_NAME.cognitiveservices.azure.com"
        export SPEECH_KEY=$SPEECH_RESOURCE_KEY
        export SPEECH_REGION=$SERVICE_REGION
    else
        echo "File not found: $ENV_FILE_PATH. You can create one to set environment variables or manually set secrets in environment variables."
    fi

    dotnet run --project ./BatchSynthesisSample/BatchSynthesisSample.csproj

    if [[ $? -ne 0 ]]; then
        echo "Run failed, exiting with status 1." >&2
        exit 1
    fi
else
    echo "Invalid action: $ACTION" >&2
    echo "Usage: {configure | build | run}" >&2
    exit 1
fi