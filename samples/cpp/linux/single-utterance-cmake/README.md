# Sample: Recognize speech in C++ for Linux (Ubuntu 16.04, Ubuntu 18.04, Debian 9, RHEL 8, CentOS 8) from your microphone

This sample demonstrates how to recognize speech in compressed audio input stream with C++ using the Speech SDK for Linux.
The compressed audio input stream should be either in MP3 or Opus format.


## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* An Ubuntu 16.04, Ubuntu 18.04, Debian 9, RHEL 8, or CentOS 8 PC.
* On Ubuntu, install these packages to build and run this sample:

  ```sh
  sudo apt-get update
  sudo apt-get install build-essential libssl1.0.0 libasound2 wget
  sudo apt-get install libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly
  ```
* On Debian 9, install these packages to build and run this sample:

  ```sh
  sudo apt-get update
  sudo apt-get install build-essential libssl1.0.2 libasound2 wget
  sudo apt-get install libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly
  ```
* On RHEL 8, or CentOS 8, install these packages to build and run this sample:

  ```sh
  sudo yum update
  sudo yum groupinstall "Development tools"
  sudo yum install alsa-lib openssl wget
  sudo yum install gstreamer1 gstreamer1-plugins-base gstreamer1-plugins-good gstreamer1-plugins-ugly-free gstreamer1-plugins-bad-free
  ```

## Build the sample

* [Download the sample code to your development PC.](../../../../README.md#get-the-samples)
* Download and extract the Speech SDK
  * **By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license201809).**
  * Run the following commands after replacing the string `/your/path` with a directory (absolute path) of your choice:

    ```sh
    export SPEECHSDK_ROOT="/your/path"
    mkdir -p "$SPEECHSDK_ROOT"
    wget -O SpeechSDK-Linux.tar.gz https://aka.ms/csspeech/linuxbinary
    tar --strip 1 -xzf SpeechSDK-Linux.tar.gz -C "$SPEECHSDK_ROOT"
    ```
* Navigate to the directory of this sample
* Edit the `single-utterance.cpp` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
* Run 
```sh
mkdir build
cd build
cmake ..
```
to run cmake and generate the makefile.

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

Now you are ready to build the application and run it:

```sh
make
./hello
```
You will be prompted to speak into your microphone.

## References

* [Speech SDK API reference for C++](https://aka.ms/csspeech/cppref)