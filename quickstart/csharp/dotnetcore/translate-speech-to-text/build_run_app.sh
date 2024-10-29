#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status.

action="$1"

# Check if the correct number of arguments is provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 {build|run}"
    exit 1
fi

dotnetInstallPath="/usr/share/dotnet"
export PATH=$PATH:$dotnetInstallPath

install_dotnet6() {
    wget https://dot.net/v1/dotnet-install.sh
    chmod +x dotnet-install.sh
    sudo ./dotnet-install.sh --version 6.0.427 --install-dir $dotnetInstallPath

    if [ $? -eq 0 ]; then
        echo 'export PATH=$PATH:/usr/share/dotnet' >> ~/.bashrc
        source ~/.bashrc
        sudo rm -f ./dotnet-install.sh
    else
        echo "Installation of .NET SDK 6.0 failed, exiting..."
        sudo rm -f ./dotnet-install.sh
        exit 1
    fi
}

if [ "$action" = "build" ]; then
    sudo apt-get update
    sudo apt-get install libssl-dev libasound2

    if ! command -v dotnet &> /dev/null || [[ "$(dotnet --version)" < "6.0" ]]; then
        echo "Installing .NET SDK 6.0..."
        install_dotnet6
    fi

    dotnet build helloworld/helloworld.csproj
    if [ $? -ne 0 ]; then
        echo "Build failed, exiting..."
        exit 1
    fi
elif [ "$action" = "run" ]; then
    dotnet helloworld/bin/Debug/net6.0/helloworld.dll
else
    echo "Invalid action: $action"
    echo "Usage: $0 {build|run}"
    exit 1
fi