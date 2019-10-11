#!/usr/bin/env bash

set -x -e -o pipefail

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
UNIDEC_SOURCE_ROOT="$SCRIPT_DIR/../source/extensions/unidec"
UNIDEC_EXTERNAL_ROOT="$SCRIPT_DIR/../external/unidec"
UNIDEC_RUNTIME_NUGET_SOURCE="https://msasg.pkgs.visualstudio.com/_packaging/Bing/nuget/v3/index.json"
UNIDEC_MODEL_NUGET_SOURCE="https://msasg.pkgs.visualstudio.com/_packaging/CarbonOfflineModel/nuget/v3/index.json"

# TODO also look at target platform

if [[ $SPEECHSDK_ENABLE_UNIDEC == true ]]; then
  cp $UNIDEC_SOURCE_ROOT/nuget.config $UNIDEC_EXTERNAL_ROOT/nuget.config
fi

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

    if [[ $SPEECHSDK_ENABLE_UNIDEC == true ]]; then
      # For Unidec runtime and offline model packages installation
      sudo apt-get install --yes nuget
      
      # Install UnidecRuntime libSpeechToText.so dependencies (Linux-x64)
      sudo apt-get install --yes libboost-program-options1.58.0
      # libstdc++.so.6 with GLIBCXX_3.4.22 is not available by default in
      # Ubuntu 16.04 LTS
      sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
      sudo apt-get update
      sudo apt-get install --yes libstdc++6
      
      # Install Unidec runtime and offline model packages
      # Note: DownloadBuildArtifacts task is used to get libraries for target platforms other than Windows-x64
      NUGET_CONFIG="$UNIDEC_EXTERNAL_ROOT/nuget.config"
      nuget sources add -name Richland    -source $UNIDEC_RUNTIME_NUGET_SOURCE -username carbon -password $VSTS_TOKEN -ConfigFile $NUGET_CONFIG
      nuget sources add -name UnidecModel -source $UNIDEC_MODEL_NUGET_SOURCE   -username carbon -password $VSTS_TOKEN -ConfigFile $NUGET_CONFIG
      nuget install $UNIDEC_SOURCE_ROOT/packages.config -OutputDirectory $UNIDEC_EXTERNAL_ROOT -ExcludeVersion -ConfigFile $NUGET_CONFIG
    fi
    ;;

  Windows-x64)
    # Install Unidec runtime and offline model packages
    # Note: DownloadBuildArtifacts task is used to get libraries for target platforms other than Windows-x64
    if [[ $SPEECHSDK_ENABLE_UNIDEC == true ]]; then
      NUGET_CONFIG=$(cygpath -aw "$UNIDEC_EXTERNAL_ROOT/nuget.config")
      nuget.exe sources add -name Richland    -source $UNIDEC_RUNTIME_NUGET_SOURCE -username carbon -password $VSTS_TOKEN -ConfigFile $NUGET_CONFIG
      nuget.exe sources add -name UnidecModel -source $UNIDEC_MODEL_NUGET_SOURCE   -username carbon -password $VSTS_TOKEN -ConfigFile $NUGET_CONFIG
      nuget.exe install $(cygpath -aw "$UNIDEC_SOURCE_ROOT/packages.config") -OutputDirectory $(cygpath -aw "$UNIDEC_EXTERNAL_ROOT") -ExcludeVersion -ConfigFile $NUGET_CONFIG
    fi
    ;;

  OSX-x64)
    # log brew-related version information
    brew config
    brew install pkg-config coreutils bash ninja
    # custom-install swig 3.0.12
    brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/f3544543a3115023fc7ca962c21d14b443f419d0/Formula/swig.rb
    ;;
esac

if [[ $SPEECHSDK_ENABLE_UNIDEC == true ]]; then
  # Prevent hits from CredentialScanner
  rm $UNIDEC_EXTERNAL_ROOT/nuget.config
fi
