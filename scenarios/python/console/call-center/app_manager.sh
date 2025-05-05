#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status.

action="$1"

# Check if the correct number of arguments is provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 {build|run}"
    exit 1
fi

if [ "$action" == "build" ]; then
    command -v python3 >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "Python 3 is not found. Please install python3 first. Exiting..."
        exit 1
    fi
elif [ "$action" == "run" ]; then
    configFilePath="config.json"
    if [ -f "$configFilePath" ]; then
        aiServiceKey=$(grep '"SubscriptionKey"' "$configFilePath" | sed 's/.*: *"\(.*\)".*/\1/')
        aiServiceRegion=$(grep '"ServiceRegion"' "$configFilePath" | sed 's/.*: *"\(.*\)".*/\1/')
    else
        echo -e "The config.json file is not found."
        exit 1
    fi

    if command -v python3 &> /dev/null; then
        python3 ./call_center.py --speechKey "$aiServiceKey" --speechRegion "$aiServiceRegion" --languageKey "$aiServiceKey" --languageEndpoint "https://$aiServiceRegion.stt.speech.microsoft.com" --input "https://github.com/Azure-Samples/cognitive-services-speech-sdk/raw/master/scenarios/call-center/sampledata/Call1_separated_16k_health_insurance.wav" --output summary.json
    else
        echo -e "Python 3 is not found. Please install python3 first. Exiting..."
        exit 1
    fi
else
    echo "Invalid action: $action"
    echo "Usage: $0 {build|run}"
    exit 1
fi