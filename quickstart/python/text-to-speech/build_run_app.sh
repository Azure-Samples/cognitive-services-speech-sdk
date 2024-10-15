#!/bin/bash

action=$1

temp_python_installation_directory="$TEMP/Python310"
temp_python_path="$temp_python_installation_directory/python"

function test_python3_installed {
    if command -v python3 &> /dev/null; then
        python_version=$(python3 --version 2>&1)
        if [[ $python_version == Python 3* ]]; then
            return 0
        fi
    fi
    return 1
}

function install_packages {
    local python_directory=$1
    local real_python_path="$python_directory/python"

    if ! $real_python_path -m pip install azure-cognitiveservices-speech; then
        echo "The pip is not installed. Installing pip..."

        curl -o "$python_directory/get-pip.py" https://bootstrap.pypa.io/get-pip.py
        if [[ $? -ne 0 ]]; then
            echo "Failed to download pip, exiting..." >&2
            exit 1
        fi

        $real_python_path "$python_directory/get-pip.py" --no-warn-script-location --prefix "$python_directory"
        if [[ $? -ne 0 ]]; then
            echo "The pip installation failed, exiting..." >&2
            exit 1
        fi

        rm -f "$python_directory/get-pip.py"

        $real_python_path -m pip install azure-cognitiveservices-speech
        if [[ $? -ne 0 ]]; then
            echo "The azure-cognitiveservices-speech package installation failed, exiting..." >&2
            exit 1
        fi
    fi
}

if [[ $action == "build" ]]; then
    if test_python3_installed; then
        python_directory=$(dirname "$(command -v python3)")
        install_packages "$python_directory"
    elif [[ ! -x "$temp_python_path" ]]; then
        echo "Python 3 is not installed. Installing Python 3 to $temp_python_installation_directory..."

        mkdir -p "$temp_python_installation_directory"

        if [[ $(uname -m) == "x86_64" ]]; then
            python_download_url="https://www.python.org/ftp/python/3.10.0/python-3.10.0-amd64.exe"
        else
            python_download_url="https://www.python.org/ftp/python/3.10.0/python-3.10.0-win32.exe"
        fi

        python_installer_path="./python-installer.exe"
        curl -o "$python_installer_path" "$python_download_url"
        if [[ $? -ne 0 ]]; then
            echo "Failed to download python3, exiting..." >&2
            exit 1
        fi

        start-process "$python_installer_path" "/quiet InstallAllUsers=0 PrependPath=0 TargetDir=$temp_python_installation_directory"
        wait

        if [[ ! -x "$temp_python_path" ]]; then
            echo "Python3 installation failed, exiting..." >&2
            exit 1
        fi

        rm -f "$python_installer_path"

        install_packages "$temp_python_installation_directory"
    else
        install_packages "$temp_python_installation_directory"
    fi
elif [[ $action == "run" ]]; then
    if command -v python3 &> /dev/null; then
        python3 quickstart.py
    elif [[ -x "$temp_python_path" ]]; then
        "$temp_python_path" quickstart.py
    else
        echo "Python is not found. Please first run the script with build action to install Python." >&2
        exit 1
    fi
else
    echo "Invalid action: $action" >&2
    echo "Usage: build or run" >&2
    exit 1
fi
