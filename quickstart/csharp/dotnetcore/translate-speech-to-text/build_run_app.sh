#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 {build|run}"
    exit 1
fi

action=$1

install_dotnet6() {
    wget https://dot.net/v1/dotnet-install.sh
    chmod +x dotnet-install.sh
    ./dotnet-install.sh --version 6.0.427

    if [ $? -ne 0 ]; then
        echo "Installation of .NET SDK 6.0 failed, exiting..."
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

    dotnet build ./helloworld --configuration release
    if [ $? -eq 0 ]; then
        echo "Build succeeded."
    else
        echo "Build failed, exiting..."
        exit 1
    fi
elif [ "$action" = "run" ]; then
    dotnet run --project ./helloworld/helloworld.csproj --configuration release
else
    echo "Invalid action: $action"
    echo "Usage: $0 {build|run}"
    exit 1
fi
