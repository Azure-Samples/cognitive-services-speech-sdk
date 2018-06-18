#!/bin/bash

set -x -e -o pipefail

# TODO pip: chose specific version

# TODO also look at target platform

case $SPEECHSDK_BUILD_AGENT_PLATFORM in
  Linux-x64)
    sudo apt-get update
    # https://github.com/Microsoft/vsts-agent-docker/blob/master/ubuntu/16.04/standard/Dockerfile pulls in libssl-dev 1.1 via ppa:ondrej/php.
    # Remove it first, and install the Xenial default one (1.0*)
    sudo apt-get remove --yes libssl-dev
    sudo apt-get install --yes --target-release xenial-updates libssl-dev
    sudo apt-get install --yes pkg-config zlib1g-dev libcurl4-openssl-dev libasound2-dev swig uuid-dev python-dev python-setuptools default-jdk
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
