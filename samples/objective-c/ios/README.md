# Additional samples for Objective-C on iOS

Theses samples demonstrate more advanced techniques for speech recognition with the Microsoft Cognitive Services Speech SDK.
For an introduction to the SDK, please refer to the [quickstart articles for speech recognition](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-objectivec-ios) and [speech synthesis](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-text-to-speech-objectivec-ios) on the SDK documentation page for step-by-step instructions.

## Connection object sample

The [connection sample](./connection-sample) shows how to establish a connection to the speech service independent from starting a recognition task.
This can be used to reduce the latency on the first recognition task.
The sample shows how to implement this for speech recognition from microphone.

## Example app with various samples

This [sample app](./speech-samples) shows various other techniques that can be used with speech, translation and intent recognizers:

- using phrase hints to improve recognition results
- using continuous recognition
- using push and pull audio streams

The [synthesis sample app](./synthesis-samples) shows various other techniques that can be used with speech synthesizers:

- using push and pull audio streams / audio data stream to fetch the synthesized audio streamingly to lower the latency
- using event handlers to handle synthesis events

## Prerequisites

- A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
- A Mac with Xcode installed as iOS development environment. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-objectivec) for details on system requirements and setup.

## Get the code for the samples

- [Download the sample code to your development machine.](/README.md#get-the-samples)

## Get the Speech SDK for iOS

**By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**

The Cognitive Services Speech SDK for iOS can be downloaded as a zip-file from https://aka.ms/csspeech/iosbinary. Download and unpack the files to the directory of this `README.md`, i.e. parallel to the samples directories.

## Build the samples

Open either Xcode project with the sample code.
This loads the project in Xcode.
Next, add your subscription details to the `<sample name>/ViewController.m` file:

1. Replace the string `YourSubscriptionKey` with your subscription key.
2. Replace the string `YourServiceRegion` with the region associated with your subscription (for example, `westus` for the free trial subscription).

To build the sample app and check if all the paths are set correctly, choose **Product** > **Build** from the menu.

## Run the samples

To run the sample, click the `Play` button, or select **Product** > **Run** from the menu.
In the simulator window that opens, you can interact with the sample application.

## References

- [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-objectivec-ios)
- [TTS Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-text-to-speech-objectivec-ios)
- [Speech SDK API reference for Objective-C](https://aka.ms/csspeech/objectivecref)
