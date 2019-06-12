#!/usr/bin/env bash

set -x -e -o pipefail

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

# TODO also look at target platform

case $SPEECHSDK_BUILD_AGENT_PLATFORM in
  Linux-x64)

    sudo apt-get update
    # https://github.com/Microsoft/vsts-agent-docker/blob/master/ubuntu/16.04/standard/Dockerfile pulls in libssl-dev 1.1 via ppa:ondrej/php.
    # Remove it first, and install the Xenial default one (1.0*)
    sudo apt-get remove --yes libssl-dev
    sudo apt-get install --yes --target-release xenial-updates libssl-dev
    sudo apt-get install --yes pkg-config zlib1g-dev libasound2-dev uuid-dev default-jdk libpcre++-dev bison automake
    sudo apt-get install --yes --no-install-recommends libgstreamer1.0-dev \
                               gstreamer1.0-plugins-base \
                               gstreamer1.0-plugins-good \
                               gstreamer1.0-plugins-ugly \
                               gstreamer1.0-plugins-bad \

    sudo "$SCRIPT_DIR/install-swig.sh"

    # For Unidec runtime and model packages installation
    sudo apt-get install --yes nuget
    
    # Richland.Speech.UnidecRuntime.linux/native/libSpeechToText.so requires
    # libstdc++.so.6 with GLIBCXX_3.4.22 which is not available by default in
    # Ubuntu 16.04 LTS
    sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get install --yes libstdc++6
    ;;
  Windows-x64)
    ;;
  OSX-x64)
    # log brew-related version information
    brew config
    brew install pkg-config coreutils bash ninja
    # custom-install swig 3.0.12
    brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/f3544543a3115023fc7ca962c21d14b443f419d0/Formula/swig.rb
    ;;
esac
