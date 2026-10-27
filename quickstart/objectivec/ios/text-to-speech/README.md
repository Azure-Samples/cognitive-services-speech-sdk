# Quickstart: Synthesize audio in Objective-C on iOS

This sample demonstrates how to create a iOS app in Objective-C using the Cognitive Services Speech SDK to synthesize audio from text input.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Mac with [Xcode](https://geo.itunes.apple.com/us/app/xcode/id497799835?mt=12) installed as iOS development environment. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-objectivec) for details on system requirements and setup.

## Get the Code for the Sample App

* [Download the sample code to your development PC.](/README.md#get-the-samples)

## Get the Speech SDK for iOS

**By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**

The Cognitive Services Speech SDK for iOS is distributed as an XCFramework bundle.
It can be added to Xcode projects via [Swift Package Manager](https://swift.org/package-manager/).

## Install the SDK using Swift Package Manager

The Speech SDK is published as a Swift package at [microsoft/speech-sdk-spm](https://github.com/microsoft/speech-sdk-spm).
If the sample project already lists `speech-sdk-spm` under **Package Dependencies**, no installation is needed.
Otherwise, remove any unresolved local Speech SDK package reference and add the published package:

1. In Xcode, choose **File** > **Add Package Dependencies...**.
1. Enter the package URL `https://github.com/microsoft/speech-sdk-spm` in the search field.
1. Keep the default dependency rule (**Up to Next Major Version**, pre-filled with the latest release), or choose a specific version, then click **Add Package**.
1. The package exposes three products — select **only one**, matching your scenario, add it to your app target, and click **Add Package**:
   * **MicrosoftCognitiveServicesSpeech-iOS** — standard iOS SDK
   * **MicrosoftCognitiveServicesSpeechEmbedded-iOS** — iOS SDK with on-device (embedded) speech
   * **MicrosoftCognitiveServicesSpeech-macOS** — standard macOS SDK

Then import the SDK in your source with `#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>`.

## Build and Run the Sample

1. Open `helloworld.xcodeproj` in Xcode.
1. Add your subscription details to the `ViewController.m` file:
   1. Replace the string `YourSubscriptionKey` with your subscription key.
   1. Replace the string `YourServiceRegion` with the [region](https://docs.microsoft.com/azure/cognitive-services/speech-service/regions) associated with your subscription (for example, `westus` for the free trial subscription).
1. Make the debug output visible (**View** > **Debug Area** > **Activate Console**).
1. Select the test destination (**Product** > **Destination**) to choose a device connected to your macOS machine or a iOS simulator.
1. Build and run the example code by selecting **Product** -> **Run** from the menu or clicking the **Play** button.
1. After you input some text and click the button in the app, you should hear the synthesized audio played.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-text-to-speech-objectivec-ios)
* [Speech SDK API reference for Objective-C](https://aka.ms/csspeech/objectivecref)
