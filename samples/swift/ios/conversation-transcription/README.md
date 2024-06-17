# Conversation transcription (aka real-time diarization) sample for Swift on iOS

This sample demonstrates conversation transcription (aka real-time diarization) with the Microsoft Cognitive Services Speech SDK.
For an introduction to the SDK, please refer to the [quickstart articles for speech recognition](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started-speech-to-text?pivots=programming-languages-objectivec-swift) on the SDK documentation page for step-by-step instructions.

## Prerequisites

- A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
- A Mac with Xcode installed as iOS development environment. The Speech SDK supports iOS versions 11.0 or later.

## Get the code for the samples

- [Download the sample code to your development machine.](/README.md#get-the-samples)

## Get the Speech SDK for iOS

**By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**

The Cognitive Services Speech SDK for iOS is distributed as a xcframework bundle.
It can be used in Xcode projects as a [CocoaPod](https://cocoapods.org/), or downloaded directly [here](https://aka.ms/csspeech/iosbinary) and linked manually. This guide uses a CocoaPod.

## Install the SDK as a CocoaPod

1. Install the CocoaPod dependency manager as described in its [installation instructions](https://guides.cocoapods.org/using/getting-started.html).
1. Navigate to the directory of the downloaded sample app (e.g. `speech-samples`) in a terminal.
1. Run the command `pod install`. This will generate a Xcode workspace containing both the sample app and the Speech SDK as a dependency. This workspace will be used in the following.

## Build the samples

Open either Xcode project with the sample code.
This loads the project in Xcode.
Next, add your subscription details to the `<sample name>/ViewController.swift` file:

1. Replace the string `YourSubscriptionKey` with your subscription key.
2. Replace the string `YourServiceRegion` with the [region](https://docs.microsoft.com/azure/cognitive-services/speech-service/regions) associated with your subscription (for example, `westus` for the free trial subscription).

To build the sample app and check if all the paths are set correctly, choose **Product** > **Build** from the menu.

## Run the samples

To run the sample, click the `Play` button, or select **Product** > **Run** from the menu.
In the simulator window that opens, you can interact with the sample application.

## References

* [Speech SDK API reference for Objective-C](https://aka.ms/csspeech/objectivecref)
