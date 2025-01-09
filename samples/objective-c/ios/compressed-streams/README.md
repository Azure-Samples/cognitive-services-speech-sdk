# Using codec compressed audio input with the Speech SDK on iOS

The Speech SDK's **Compressed Audio Input Stream** API provides a way to stream compressed audio to the Speech Service using a pull or push stream.

**NOTE**: The compressed audio support using Gstreamer is disabled in the latest Speech SDK due to instability problems.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Mac with [Xcode](https://geo.itunes.apple.com/us/app/xcode/id497799835?mt=12) installed as iOS development environment. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-objectivec) for details on system requirements and setup.

## Get the Code for the Sample App

* [Download the sample code to your development PC.](/README.md#get-the-samples)

## Get the Speech SDK for iOS

**By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**

The Cognitive Services Speech SDK for iOS can be downloaded as a zip-file from https://aka.ms/csspeech/iosbinary. Download and copy the files to the directory of this `README.md`, i.e. parallel to the `CompressedStreamsSample` directory that contains the Xcode sample project.

## Prerequisites to using codec compressed audio input

Handling compressed audio is implemented using [GStreamer](gstreamer.freedesktop.org).
For licensing reasons, these functions can not be shipped with the SDK, but a wrapper library containing these functions needs to be built by application developers and shipped with the apps using the SDK.
To build this wrapper library, first download and install the [GStreamer SDK](https://gstreamer.freedesktop.org/data/pkg/ios/1.16.0/gstreamer-1.0-devel-1.16.0-ios-universal.pkg).
Open the [GStreamerWrapper](./GStreamerWrapper) project in Xcode and build it for the **Generic iOS Device** target -- it will not work to build it for a specific target.
The build step will generate a dynamic framework bundle with a dynamic library for all necessary architectures with the name of `GStreamerWrapper.framework`.
This framework needs to be included in all apps using compressed streams with the Speech Services SDK.

The sample [CompressedStreamsSample](./CompressedStreamsSample) app expects both the `GStreamerWrapper.framework` you just built and the framework of the Cognitive Services Speech SDK in the directory containing this README file. Copy them there.

Open the `CompressedStreamsSample/CompressedStreamsSample.xcodeproj` file.
This loads the project in Xcode.
Next, add your subscription details to the `CompressedStreamsSample/ViewController.m` file:

1. Replace the string `YourSubscriptionKey` with your subscription key.
1. Replace the string `YourServiceRegion` with the [region](https://docs.microsoft.com/azure/cognitive-services/speech-service/regions) associated with your subscription (for example, `westus` for the free trial subscription).

To build the sample app and check if all the paths are set correctly, choose **Product** > **Build** from the menu.

## Run the Sample

To run the sample, click the `Play` button, or select **Product** > **Run** from the menu.
In the simulator window that opens, after you click the "Recognize!" button in the app, you should see the recognized text from the audio file "What's the weather like?" on the lower part of the simulated screen.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-objectivec-ios)
* [Speech SDK API reference for Objective-C](https://aka.ms/csspeech/objectivecref)
