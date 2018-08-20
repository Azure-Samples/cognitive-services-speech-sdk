#!/bin/bash

set -x -e -o pipefail

USAGE="Usage: $0 path-to-audio cmd ... "

PATH_TO_AUDIO="${1?$USAGE}"
shift 1

# Launch PA daemon
pulseaudio -D --exit-idle-time=-1
trap 'pulseaudio --kill' EXIT
sleep 1.5

# Configure microphone input
pactl load-module module-null-sink sink_name=MicOutput sink_properties=device.description=Virtual_Microphone_Output
pacmd load-module module-virtual-source source_name=VirtualMic

# Launch paplay in the background
( sleep 1.5; paplay "$PATH_TO_AUDIO" ) &
jobs

# Launch helloworld with pulseaudio-alsa
padsp "$@"

# Wait for paplay to complete (in this case it should already be done)
wait

# Stop daemon
trap '' EXIT
pulseaudio --kill
