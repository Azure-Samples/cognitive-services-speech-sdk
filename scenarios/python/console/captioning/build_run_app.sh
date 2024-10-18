#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status.

action="$1"

# Define Python installation directory
tempPythonInstallationDirectory="$TMP/Python310"
tempPythonPath="$tempPythonInstallationDirectory/python"

# Function to test if Python 3 is installed
test_python3_installed() {
    if command -v python &> /dev/null; then
        python_version=$(python --version 2>&1)
        if [[ "$python_version" == Python\ 3* ]]; then
            return 0
        fi
    fi
    return 1
}

# Function to install packages
install_packages() {
    python_directory="$1"
    real_python_path="$python_directory/python"
    
    if ! "$real_python_path" -m pip --version &> /dev/null; then
        echo "pip is not installed. Installing pip..."
        curl -s -o "$python_directory/get-pip.py" "https://bootstrap.pypa.io/get-pip.py"
        "$real_python_path" "$python_directory/get-pip.py" --no-warn-script-location --prefix "$python_directory"
        rm -f "$python_directory/get-pip.py"
    fi

    "$real_python_path" -m pip install azure-cognitiveservices-speech
}

if [ "$action" == "build" ]; then
    if test_python3_installed; then
        python_directory=$(dirname "$(command -v python)")
        install_packages "$python_directory"
    elif [ ! -f "$tempPythonPath" ]; then
        echo "Python 3 is not installed. Installing Python 3 to $tempPythonInstallationDirectory..."
        mkdir -p "$tempPythonInstallationDirectory"

        if [ "$(uname -m)" == "x86_64" ]; then
            python_download_url="https://www.python.org/ftp/python/3.10.0/python-3.10.0-amd64.exe"
        else
            python_download_url="https://www.python.org/ftp/python/3.10.0/python-3.10.0-win32.exe"
        fi

        python_installer_path="./python-installer.exe"
        curl -s -o "$python_installer_path" "$python_download_url"

        # Assuming 'wine' is installed for running Windows executables
        wine "$python_installer_path" /quiet InstallAllUsers=0 PrependPath=0 TargetDir="$tempPythonInstallationDirectory"

        if [ ! -f "$tempPythonPath" ]; then
            echo "Python installation failed, exiting..." >&2
            exit 1
        fi

        rm -f "$python_installer_path"

        install_packages "$tempPythonInstallationDirectory"
    else
        install_packages "$tempPythonInstallationDirectory"
    fi

    env_file_path=".env/.env.dev"
    if [ -f "$env_file_path" ]; then
        while IFS= read -r line; do
            if [ "$line" && ! "$line" =~ ^\s*# ]; then
                key=$(echo "$line" | cut -d '=' -f 1 | xargs)
                value=$(echo "$line" | cut -d '=' -f 2- | xargs)

                if [ "$key" == "SPEECH_RESOURCE_KEY" ]; then
                    export SPEECH_KEY="$value"
                elif [ "$key" == "SERVICE_REGION" ]; then
                    export SPEECH_REGION="$value"
                fi
            fi
        done < "$env_file_path"
        echo "Environment variables loaded from $env_file_path"
    else
        echo "File not found: $env_file_path"
    fi
elif [ "$action" == "run" ]; then
    if command -v python &> /dev/null; then
        python ./captioning.py
    elif [ -f "$tempPythonPath" ]; then
        "$tempPythonPath" ./captioning.py
    else
        echo "Python is not found. Please first run the script with build action to install Python." >&2
        exit 1
    fi
else
    echo "Invalid action: $action" >&2
    echo "Usage: $0 build or run" >&2
    exit 1
fi
