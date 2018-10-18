#!/bin/bash

set -x -e -o pipefail

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

# TODO pip: chose specific version

# TODO also look at target platform

case $SPEECHSDK_BUILD_AGENT_PLATFORM in
  Linux-x64)

    # Attempting to diagnose and mitigate the case when we arrive here and
    # somebody already has taken the lock. ICM 84595724
    PIDS="$(sudo lsof -t /var/lib/dpkg/lock || true)"
    if [[ -n $PIDS ]]; then
      (
        set +e
        echo PID for /var/lib/dpkg/lock: $PIDS
        echo Running processes:
        sudo ps axjfw
        echo LSOF
        sudo lsof /var/lib/dpkg/lock
        echo Sleeping 30 seconds, then trying to kill.
        sleep 30
        sudo lsof -t /var/lib/dpkg/lock | sudo xargs --no-run-if-empty --verbose kill -9
        echo LSOF after kill
        sudo lsof /var/lib/dpkg/lock
        sudo dpkg --configure -a
        echo DONE
      )
    fi

    sudo apt-get update
    # https://github.com/Microsoft/vsts-agent-docker/blob/master/ubuntu/16.04/standard/Dockerfile pulls in libssl-dev 1.1 via ppa:ondrej/php.
    # Remove it first, and install the Xenial default one (1.0*)
    sudo apt-get remove --yes libssl-dev
    sudo apt-get install --yes --target-release xenial-updates libssl-dev
    sudo apt-get install --yes pkg-config zlib1g-dev libcurl4-openssl-dev libasound2-dev uuid-dev python-dev python-setuptools default-jdk libpcre++-dev bison automake
    sudo "$SCRIPT_DIR/install-swig.sh"
    pip install -U pytest
    ;;
  Windows-x64)
    pip install -U pytest
    ;;
  OSX-x64)
    brew install swig pkg-config coreutils bash

    sudo easy_install pip

    # ignore install six, otherwise pip install pytest will complain about six installed by distutils
    sudo -H pip install --ignore-installed six
    sudo -H pip install pytest
    ;;
esac
