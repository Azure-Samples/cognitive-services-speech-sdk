# Sample: Recognize speech in C++ for Linux from an MP3/Opus file

This sample demonstrates how to recognize speech in compressed audio input stream with C++ using the Speech SDK for Linux.
The compressed audio input stream should be either in MP3 or Opus format.

> **Note:**
> Support for compressed audio input streams was added to the Speech SDK version 1.4.0.
> Check the [compressed audio input article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-use-codec-compressed-audio-input-streams)
> for additional information.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC with a [supported Linux distribution](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=linux).
* On Ubuntu or Debian, install these packages to build and run this sample:

  ```sh
  sudo apt-get update
  sudo apt-get install build-essential libssl-dev libasound2 wget
  sudo apt-get install libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly
  ```

  * On **Ubuntu 22.04 LTS** it is also required to download and install the latest **libssl1.1** package e.g. from http://security.ubuntu.com/ubuntu/pool/main/o/openssl/.

* On RHEL or CentOS, install these packages to build and run this sample:

  ```sh
  sudo yum update
  sudo yum groupinstall "Development tools"
  sudo yum install alsa-lib openssl wget
  sudo yum install gstreamer1 gstreamer1-plugins-base gstreamer1-plugins-good gstreamer1-plugins-ugly-free gstreamer1-plugins-bad-free
  ```

  * See also [how to configure RHEL/CentOS 7 for Speech SDK](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-configure-rhel-centos-7).

## Build the sample

* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Download and extract the Speech SDK
  * **By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
  * Run the following commands after replacing the string `/your/path` with a directory (absolute path) of your choice:

    ```sh
    export SPEECHSDK_ROOT="/your/path"
    mkdir -p "$SPEECHSDK_ROOT"
    wget -O SpeechSDK-Linux.tar.gz https://aka.ms/csspeech/linuxbinary
    tar --strip 1 -xzf SpeechSDK-Linux.tar.gz -C "$SPEECHSDK_ROOT"
    ```
* Navigate to the directory of this sample
* Edit the file `Makefile`:
  * In the line `SPEECHSDK_ROOT:=/change/to/point/to/extracted/SpeechSDK` change the right-hand side to point to the location of your extract Speech SDK for Linux.
  * If you are running on Linux x86 (32-bit), change the line `TARGET_PLATFORM:=x64` to `TARGET_PLATFORM:=x86`.
  * If you are running on Linux ARM64 (64-bit), change the line `TARGET_PLATFORM:=x64` to `TARGET_PLATFORM:=arm64`.
* Edit the `compressed-audio-input.cpp` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
* Run the command `make` to build the sample, the resulting executable will be called `compressed-audio-input`.

## Run the sample

To run the sample, you'll need to configure the loader's library path to point to the Speech SDK library.

* On an x64 machine, run:

  ```sh
  export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$SPEECHSDK_ROOT/lib/x64"
  ```

* On an x86 machine, run:

  ```sh
  export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$SPEECHSDK_ROOT/lib/x86"
  ```

* On an ARM64 machine, run:

  ```sh
  export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$SPEECHSDK_ROOT/lib/arm64"
  ```

Run the application:

```sh
./compressed-audio-input <path to MP3 or Opus file>
```

## References

* [Compressed audio input article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-use-codec-compressed-audio-input-streams)
* [Speech SDK API reference for C++](https://aka.ms/csspeech/cppref)
