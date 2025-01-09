# Quickstart: Recognize speech from a microphone in Objective-C on iOS

This sample demonstrates how to recognize speech with Objective-C using the Speech SDK for iOS.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/speech-to-text-from-microphone-langs/objectivec-ios) on the SDK documentation page for step-by-step instructions.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Mac with [Xcode](https://geo.itunes.apple.com/us/app/xcode/id497799835?mt=12) installed as iOS development environment. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-objectivec) for details on system requirements and setup.

## Get the Code for the Sample App

* [Download the sample code to your development PC.](/README.md#get-the-samples)

## Get the Speech SDK for iOS

**By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**

The Cognitive Services Speech SDK for iOS can be downloaded as a zip-file from https://aka.ms/csspeech/iosbinary. Download and copy the files to the `speechsdk` directory in the directory of this `README.md`, i.e. parallel to the `helloworld` directory that contains the Xcode project.

## Build the Sample

Open the `helloworld.xcodeproj` from the sample code.
This loads the project in Xcode.
Next, add your subscription details to the `helloworld/ViewController.m` file:

1. Replace the string `YourSubscriptionKey` with your subscription key.

1. Replace the string `YourServiceRegion` with the [region](https://docs.microsoft.com/azure/cognitive-services/speech-service/regions) associated with your subscription (for example, `westus` for the free trial subscription).

To build the sample app and check if all the paths are set correctly, choose **Product** > **Build** from the menu.

## Run the Sample

To run the sample, click the `Play` button, or select **Product** > **Run** from the menu.
In the simulator window that opens, after you click the "Recognize!" button in the app, you should see the recognized text from the audio file "What's the weather like?" on the lower part of the simulated screen.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-objectivec-ios)
* [Speech SDK API reference for Objective-C](https://aka.ms/csspeech/objectivecref)
