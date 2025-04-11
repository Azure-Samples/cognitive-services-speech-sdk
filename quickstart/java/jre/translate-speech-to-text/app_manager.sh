#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status.

action="$1"

# Check if the correct number of arguments is provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 {build|run|configure}"
    exit 1
fi

check_and_install_openJDK() {
    # Check if JDK is installed
    if ! command -v java &> /dev/null; then
        # Install OpenJDK
        echo "Install OpenJDK..."
        sudo apt-get update
        sudo apt-get install openjdk-11-jdk -y

        # Check installation result
        if command -v java &> /dev/null; then
            echo "OpenJDK was installed successfully, the version is:"
            sudo java -version
        else
            echo "OpenJDK installation failed."
            exit 1
        fi
    fi
}

check_and_install_maven() {
    if ! command -v mvn &> /dev/null; then
        echo "Maven is not installed, installing..."
        sudo apt-get install maven -y

        if [ $? -ne 0 ]; then
            echo "Maven installation failed."
            exit 1
        fi
    fi
}

if [ "$action" = "configure" ]; then
    if ! command -v java &> /dev/null || ! command -v mvn &> /dev/null; then
        read -p "Whether allow us to install java sdk and other dependencies? Please enter Y/N: " isAllow
        if [[ "$isAllow" != "Y" && "$isAllow" != "y" ]]; then
            echo "The operation was canceled."
            exit 1
        fi
    fi

    check_and_install_openJDK
    check_and_install_maven

    echo "Installing Linux dependencies..."
    sudo apt-get update
    sudo apt-get install build-essential ca-certificates libasound2-dev libssl-dev wget

    echo "Installing Java project dependencies..."
    sudo apt-get update
    sudo mvn clean install
    if [ $? -ne 0 ]; then
        echo "Java project dependencies installation failed."
        exit 1
    fi
elif [ "$action" = "build" ]; then
    if ! command -v java &> /dev/null || ! command -v mvn &> /dev/null; then
        echo "Please execute the 'Azure AI Speech Toolkit: Configure and Setup the Sample App' command to install dependencies."
        exit 1
    fi

    echo "Compiling Java files..."
    sudo mvn compile
    if [ $? -ne 0 ]; then
        echo "Java file compilation failed."
        exit 1
    fi

    echo "Compilation succeeded."
elif [ "$action" = "run" ]; then
    if ! command -v java &> /dev/null || ! command -v mvn &> /dev/null; then
        echo "Please execute the 'Azure AI Speech Toolkit: Configure and Setup the Sample App' command to install dependencies."
        exit 1
    fi

    sudo mvn exec:java -Dexec.mainClass="speechsdk.quickstart.Main"
else
    echo "Invalid action: $action"
    echo "Usage: $0 {build|run|configure}"
    exit 1
fi