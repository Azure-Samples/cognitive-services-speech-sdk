# Quickstart: Recognize speech in C++ for macOS

This sample demonstrates how to recognize speech with C++ using the Speech SDK for macOS.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/speech-to-text-from-microphone?tabs=dotnet%2Cx-android%2CmacOS%2Cjava-runtime%2Cwindowsinstall&pivots=programming-language-cpp) on the SDK documentation page for step-by-step instructions.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Mac with a working microphone. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-cpp) for details on system requirements and setup.

## Build the sample

* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Download and extract the Speech SDK
  * **By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
  * Run the following commands after replacing the string `/your/path` with a directory (absolute path) of your choice:

    ```sh
    export SPEECHSDK_ROOT="/your/path"
    mkdir -p "$SPEECHSDK_ROOT"
    wget -O MicrosoftCognitiveServicesSpeech.xcframework.zip https://aka.ms/csspeech/macosbinary
    unzip MicrosoftCognitiveServicesSpeech.xcframework.zip -d "$SPEECHSDK_ROOT"
    ```

* Above commands will copy MicrosoftCognitiveServicesSpeech.xcframework directory under the SPEECHSDK_ROOT, which has the following structure:
* MicrosoftCognitiveServicesSpeech.xcframework
* --> macos-arm64_x86_64
*    --> MicrosoftCognitiveServicesSpeech.framework
* Navigate to the directory of this sample
* Edit the file `Makefile`:
  * In the line `SPEECHSDK_ROOT:=/change/to/point/to/extracted/SpeechSDK` change the right-hand side to point to the location of MicrosoftCognitiveServicesSpeech.framework 
    directory, which can be found under `MicrosoftCognitiveServicesSpeech.xcframework/macos-arm64_x86_64` under the SPEECHSDK_ROOT.
* Edit the `helloworld.cpp` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the endpoint URL `https://YourServiceRegion.api.cognitive.microsoft.com` with the endpoint for your Speech resource. You can find this endpoint in the Azure Portal under your Speech resource's "Keys and Endpoint" section.
    For example, the endpoint might look like `https://westus.api.cognitive.microsoft.com` if your resource is in the West US region. Make sure the endpoint in your code matches the one in your Azure resource, otherwise you'll get a 401 unauthorized access error.
* Run the command `make` to build the sample, the resulting executable will be called `helloworld`.

## Run the sample

To run the sample, you'll need to configure the loader's library path to point to the Speech SDK library.

```sh
export DYLD_FRAMEWORK_PATH="$DYLD_FRAMEWORK_PATH:$SPEECHSDK_ROOT/MicrosoftCognitiveServicesSpeech.xcframework/macos-arm64_x86_64"
```

Run the application:

```sh
./helloworld
```

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-cpp-macos)
* [Speech SDK API reference for C++](https://aka.ms/csspeech/cppref)
