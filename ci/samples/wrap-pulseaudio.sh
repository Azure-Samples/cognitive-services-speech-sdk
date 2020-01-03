#!/usr/bin/env bash
set -u -e -o pipefail
on_exit() {
  local exit_code=$?
  pulseaudio --kill || true
  printf "Exiting with exit code %s\n" $exit_code
  exit $exit_code
}
. /etc/os-release
if [[ $ID = debian ]]; then
  # On Debian, patch the PulseAudio config to fix the PulseAudio daemon launch.
  perl -pi.bak -e 's/^(\Q.ifexists module-console-kit.so\E)/\1NOTLOADINGTHIS/' /etc/pulse/default.pa
fi

pulseaudio -D --exit-idle-time=-1
trap on_exit EXIT
sleep 1.5
pactl load-module module-null-sink sink_name=MicOutput sink_properties=device.description=Virtual_Microphone_Output
pacmd load-module module-virtual-source source_name=VirtualMic
"$@"
