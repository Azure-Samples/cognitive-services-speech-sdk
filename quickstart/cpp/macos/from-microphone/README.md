# Quickstart: Recognize speech in C++ for macOS

This sample demonstrates how to recognize speech with C++ using the Speech SDK for macOS.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-cpp-macos) on the SDK documentation page for step-by-step instructions.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Mac with a working microphone.

## Build the sample

* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Download and extract the Speech SDK
  * **By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license201809).**
  * Run the following commands after replacing the string `/your/path` with a directory (absolute path) of your choice:

    ```sh
    export SPEECHSDK_ROOT="/your/path"
    mkdir -p "$SPEECHSDK_ROOT"
    wget -O MicrosoftCognitiveServicesSpeech.framework.zip https://aka.ms/csspeech/macosbinary
    tar --strip 1 -xzf MicrosoftCognitiveServicesSpeech.framework.zip -C "$SPEECHSDK_ROOT"
    ```
* Navigate to the directory of this sample
* Edit the file `Makefile`:
  * In the line `SPEECHSDK_ROOT:=/change/to/point/to/extracted/SpeechSDK` change the right-hand side to point to the location of your extracted Speech SDK framework.
* Edit the `helloworld.cpp` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
* Run the command `make` to build the sample, the resulting executable will be called `helloworld`.

## Run the sample

To run the sample, you'll need to configure the loader's library path to point to the Speech SDK library.

```sh
export DYLD_FRAMEWORK_PATH="DYLD_FRAMEWORK_PATH:$SPEECHSDK_ROOT"
```

Run the application:

```sh
./helloworld
```

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-cpp-macos)
* [Speech SDK API reference for C++](https://aka.ms/csspeech/cppref)
