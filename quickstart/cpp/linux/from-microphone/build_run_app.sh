#!/bin/bash

# Function to determine the platform based on architecture
get_platform() {
    local arch=$(uname -m)
    case $arch in
        x86_64)
            echo "x64"
            ;;
        i386 | i686)
            echo "x86"
            ;;
        aarch64)
            echo "arm64"
            ;;
        *)
            echo "Unsupported architecture: $arch" >&2
            exit 1
            ;;
    esac
}


# Check if the correct number of arguments is provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 {build|run}"
    # echo "Usage: $0 {build|run} [subscription_key] [region]"
    exit 1
fi

# Get the parameter
action=$1

SPEECHSDK_ROOT="$(pwd)/SpeechSDKLib"
mkdir -p "$SPEECHSDK_ROOT"

if [ "$action" == "build" ]; then
    # if [ "$#" -ne 3 ]; then
    #     echo "Error: Invalid number of arguments for build."
    #     echo "Usage: $0 build <subscription_key> <region>"
    #     exit 1
    # fi
    
    # subscription_key=$2
    # region=$3

    # Install SDKs and libraries
    echo "Installing SDKs and libraries..."
    sudo apt-get update
    sudo apt-get install build-essential libssl-dev libasound2 wget
    
    # if SPEECHSDK_ROOT is empty, download the Speech SDK
    if [ -z "$(ls -A $SPEECHSDK_ROOT)" ]; then
        echo "Downloading the Speech SDK..."

        wget -O SpeechSDK-Linux.tar.gz https://aka.ms/csspeech/linuxbinary
        tar --strip 1 -xzf SpeechSDK-Linux.tar.gz -C "$SPEECHSDK_ROOT"
        rm SpeechSDK-Linux.tar.gz
    else
        echo "Speech SDK already exists in $SPEECHSDK_ROOT. Skip downloading."
    fi

    # Update Makefile
    makefile_path="$(pwd)/Makefile"
    echo "Updating Makefile at $makefile_path"

    # Replace the SPEECHSDK_ROOT line in the Makefile
    sed -i "s|^SPEECHSDK_ROOT:=.*|SPEECHSDK_ROOT:=$SPEECHSDK_ROOT|" "$makefile_path"

    # Replace the TARGET_PLATFORM line in the Makefile
    platform=$(get_platform)
    sed -i "s/^TARGET_PLATFORM:=.*/TARGET_PLATFORM:=$platform/" "$makefile_path"

    # # Update Subscription Key and Region in project source code
    # source_path="$(pwd)/helloworld.cpp"

    # # update the subscription key and region in the source code
    # sed -i "s|YourSubscriptionKey|$subscription_key|" "$source_path"
    # sed -i "s|YourServiceRegion|$region|" "$source_path"
    # echo "Updated the subscription key and region in $source_path"

    # Build the project
    make

elif [ "$action" == "run" ]; then
    # Export the LD_LIBRARY_PATH
    platform=$(get_platform)
    export LD_LIBRARY_PATH="$SPEECHSDK_ROOT/lib/$platform"
    
    # Run the project
    ./helloworld

else
    echo "Invalid action: $action"
    echo "Usage: $0 {build|run}"
    exit 1
fi