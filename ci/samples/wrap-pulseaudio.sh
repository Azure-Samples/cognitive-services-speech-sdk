#!/bin/bash
set -u -e -x -o pipefail
on_exit() {
  local exit_code=$?
  pulseaudio --kill || true
  printf "Exiting with exit code %s\n" $exit_code
  exit $exit_code
}
pulseaudio -D --exit-idle-time=-1
trap on_exit EXIT
sleep 1.5
pactl load-module module-null-sink sink_name=MicOutput sink_properties=device.description=Virtual_Microphone_Output
pacmd load-module module-virtual-source source_name=VirtualMic
"$@"
