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
    sudo ./dotnet-install.sh --version 8.0.408 --install-dir $dotnetInstallPath

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
    dotnet build helloworld/helloworld.csproj
    if [ $? -ne 0 ]; then
        echo "Build failed, exiting..."
        exit 1
    fi
elif [ "$action" = "run" ]; then
    dotnet helloworld/bin/Debug/net8.0/helloworld.dll
else
    echo "Invalid action: $action"
    echo "Usage: $0 {build|run|configure}"
    exit 1
fi