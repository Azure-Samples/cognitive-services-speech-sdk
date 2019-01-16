#!/bin/bash
# Copyright (c) <2018> <Maksym Rusynyk>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

SIMULATOR_NAME="$1"
COMMAND="$2"

print_params () {
  echo "$1. Example:
    ./scripts/ios_simulator_cli.sh SIMULATOR_NAME COMMAND
  Where COMMAND can be:
    create - create new simulator
    shutdown - shutdown simulator
    delete - delete simulator
    boot - boot simulator
    open - open simulator
    locale - set locale
    getuid - get simulator uid
    status - print the status of the simulator
    wait - block until simulator has booted"
}

exit_if_no_simulator() {
  SIMULATOR_ID=$( get_uid )
  if [ -z $SIMULATOR_ID ]
  then
    echo "No simulator with name [$SIMULATOR_NAME]"
    exit -1
  fi
}

get_uid() {
  echo $( xcrun simctl list | grep -w $SIMULATOR_NAME | awk 'match($0, /\(([-0-9A-F]+)\)/) { print substr( $0, RSTART + 1, RLENGTH - 2 )}' )
}

create () {
  TYPE=$1
  RUNTIME=$2

  if [ -z $TYPE ] || [ -z $RUNTIME ]
  then
  echo "Device type and ios runtime has to be specified. Run:
  - \"xcrun simctl list devicetypes\" to get the full list of the models of simulators
  - \"xcrun simctl list runtimes\" to get OS version
  Example:
  ./ios_simulator_cli.sh iphone7_snapshots create iPhone-7-Plus iOS-11-4"
  exit 1
  fi

  if [ $( get_uid ) ]
  then
  delete
  fi

  SIMULATOR_ID=$( xcrun simctl create $SIMULATOR_NAME com.apple.CoreSimulator.SimDeviceType.$TYPE com.apple.CoreSimulator.SimRuntime.$RUNTIME )
  echo $SIMULATOR_ID
}

delete () {
  SIMULATOR_ID=$( get_uid )

  if [ -z $SIMULATOR_ID ]
  then
    return
  fi

  shutdown
  xcrun simctl delete $SIMULATOR_ID
}

boot () {
  SIMULATOR_ID=$( get_uid )
  exit_if_no_simulator
  xcrun simctl boot $SIMULATOR_ID
}

open_simulator () {
  SIMULATOR_ID=$( get_uid )
  echo $SIMULATOR_ID
  exit_if_no_simulator
  open -a $(xcode-select -p)/Applications/Simulator.app --args -CurrentDeviceUDID $SIMULATOR_ID

  wait_for_simulator_boot
}

wait_for_simulator_boot() {
  RETRY_DELAY=1
  retry_cnt=10
  for (( ; ; ))
  do
    SIMULATOR_STATUS=$(get_status)

    if [ $SIMULATOR_STATUS = "Booted" ]
    then
      break
    else
      echo "Retrying in $RETRY_DELAY seconds, status is [$SIMULATOR_STATUS]"
    fi

    if [ $retry_cnt = 0 ]
    then
      echo "Cannot boot simulator $SIMULATOR_ID"
      exit -1
    fi
    retry_cnt=$((retry_cnt - 1))
    sleep $RETRY_DELAY
  done
}

get_status () {
  echo $(xcrun simctl list | grep -w $SIMULATOR_NAME | awk 'match($0, /\(([a-zA-Z]+)\)/) { print substr( $0, RSTART + 1, RLENGTH - 2 )}')
}

shutdown () {
  SIMULATOR_ID=$( get_uid )
  RETRY_DELAY=1
  retry_cnt=10

  if [ -z $SIMULATOR_ID ]
  then
    return
  fi

  for (( ; ; ))
  do
    SIMULATOR_STATUS=$(get_status)

    if [ "$SIMULATOR_STATUS" = "Shutdown" ]
    then
      break
    elif [ "$SIMULATOR_STATUS" = "Booted" ]
    then
      xcrun simctl shutdown $SIMULATOR_ID
    else
      echo "Retrying in $RETRY_DELAY seconds, status is [$SIMULATOR_STATUS]"
    fi

    if [ $retry_cnt = 0 ]
    then
      echo "Cannot shutdown simulator $SIMULATOR_ID"
      exit -1
    fi
    retry_cnt=$((retry_cnt - 1))
    sleep $RETRY_DELAY
  done
}

set_locale () {
  LOCALE=$1

  if [ -z $LOCALE ]
  then
    echo "Locale has to be provided. Example:
  ./scripts/simulator.sh boot iphone7_snapshots uk_UA"
    exit -1
  fi

  SIMULATOR_ID=$( get_uid )
  PLIST_FILE="$HOME/Library/Developer/CoreSimulator/Devices/$SIMULATOR_ID/data/Library/Preferences/.GlobalPreferences.plist"
  exit_if_no_simulator
  plutil -replace AppleLocale -string $LOCALE $PLIST_FILE
  plutil -replace AppleLanguages -json "[ \"$LOCALE\" ]" $PLIST_FILE
  plutil -replace AppleKeyboards -json '[ "uk_UA@sw=Ukrainian;hw=Automatic" ]' $PLIST_FILE
}

if [ -z $SIMULATOR_NAME ] || [ -z $COMMAND ]
then
  print_params "Simulator name and COMMAND has to be specified"
  exit 1
fi

case $COMMAND in
  ("getuid") get_uid ;;
  ("create") create $3 $4 ;;
  ("delete") delete ;;
  ("boot") boot ;;
  ("wait") wait_for_simulator_boot ;;
  ("open") open_simulator ;;
  ("status") get_status ;;

  ("shutdown") shutdown ;;

  ("locale") set_locale $3 ;;
  (*)
    print_params "Unknown COMMAND [$COMMAND]"
    exit 1
    ;;
esac
