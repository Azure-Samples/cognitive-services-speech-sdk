#!/bin/bash

set -x -e -o pipefail

# Launch PA daemon
pulseaudio -D --exit-idle-time=-1
sleep 0.5

# Configure microphone input
pactl load-module module-null-sink sink_name=MicOutput sink_properties=device.description="Virtual_Microphone_Output"
pacmd load-module module-virtual-source source_name=VirtualMic

# Launch paplay in the background
( sleep 0.5; paplay whatstheweatherlike.wav ) &
jobs

# Launch helloworld with pulseaudio-alsa
padsp ./helloworld

# Wait for paplay to complete (in this case it should already be done)
wait

# Stop daemon
pulseaudio --kill
