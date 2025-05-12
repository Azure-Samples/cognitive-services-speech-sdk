# Sample: Recognize speech in C++ for Linux from an MP3/Opus file

This sample demonstrates how to recognize speech in compressed audio input stream with C++ using the Speech SDK for Linux.
The compressed audio input stream should be either in MP3 or Opus format.

> **Note:**
> Support for compressed audio input streams was added to the Speech SDK version 1.4.0.
> Check the [compressed audio input article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-use-codec-compressed-audio-input-streams)
> for additional information.

## Option 1: Run the Sample Easily in Visual Studio Code (Recommended)
This is the easiest way to get started. The Azure AI Speech Toolkit extension automates setup, environment configuration, build, and run.

- Install [Azure AI Speech Toolkit](https://marketplace.visualstudio.com/items?itemName=ms-azureaispeech.azure-ai-speech-toolkit) extension in VS Code.
- In extension's panel, click `View Samples` and download this sample from sample gallery.
- From the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`), run:
  - `Azure AI Speech Toolkit: Configure Azure Speech Resources` to select a speech resource (one-time setup).
  - `Azure AI Speech Toolkit: Configure and Setup the Sample App` to prepare the project (one-time setup).
  - `Azure AI Speech Toolkit: Build the Sample App` to compile the code.
  - `Azure AI Speech Toolkit: Run the Sample App` to run the sample.

## Option 2: Manual Setup and Run (Advanced)
Follow these steps if you prefer not to use VS Code.

### Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC with a [supported Linux distribution](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=linux).
* See
the [Linux platform requirements](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?tabs=linux&pivots=programming-language-cpp#platform-requirements)
and [how to use compressed audio](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-use-codec-compressed-audio-input-streams?tabs=linux&pivots=programming-language-cpp)
for installing the required dependencies.

### Build the sample

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
  * Replace the variable `subscriptionKey` with your own subscription key.
  * Replace the variable `endpoint` with the endpoint for your Speech resource. You can find this endpoint in the Azure Portal under your Speech resource's "Keys and Endpoint" section.
    For example, the endpoint might look like `https://westus.api.cognitive.microsoft.com` if your resource is in the West US region. Make sure the endpoint in your code matches the one in your Azure resource, otherwise you'll get a 401 unauthorized access error.
* Run the command `make` to build the sample, the resulting executable will be called `compressed-audio-input`.

### Run the sample

1. To run the sample, you'll need to configure the loader's library path to point to the Speech SDK library.

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
2. Export speech resource key and region as environment variables. For example, use `westus` as `SERVICE_REGION` if you are using the 30-day free trial subscription.

    ```sh
    export SPEECH_RESOURCE_KEY="{your_speech_service_key}"
    export SERVICE_REGION="{your_speech_service_region}"
    ```

3. Run the application:

    ```sh
    ./compressed-audio-input <path to MP3 or Opus file>
    ```

## References

* [Compressed audio input article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-use-codec-compressed-audio-input-streams)
* [Speech SDK API reference for C++](https://aka.ms/csspeech/cppref)
