#!/bin/bash

set -x -e -o pipefail

# TODO pip: chose specific version

# TODO also look at target platform

case $SPEECHSDK_BUILD_AGENT_PLATFORM in
  Linux-x64)
    sudo apt-get update
    sudo apt-get install --yes pkg-config zlib1g-dev libssl-dev libcurl4-openssl-dev libasound2-dev swig uuid-dev python-dev python-setuptools
    pip install -U pytest
    ;;
  Windows-x64)
    pip install -U pytest
    ;;
  OSX-x64)
    brew info openssl
    brew install swig ossp-uuid pkg-config
    sudo easy_install -U pytest
    ;;
esac
