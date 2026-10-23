# Quickstart: Synthesize audio in Objective-C on macOS using the Speech SDK

This sample demonstrates how to create a macOS app in Objective-C using the Cognitive Services Speech SDK to synthesize audio from text input.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A macOS machine with [Xcode](https://geo.itunes.apple.com/us/app/xcode/id497799835?mt=12) installed. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-objectivec) for details on system requirements and setup.

## Get the Code for the Sample App

* [Download the sample code to your development PC.](/README.md#get-the-samples)

## Get the Speech SDK for macOS

**By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**

The Cognitive Services Speech SDK for macOS is distributed as an XCFramework bundle.
It can be added to Xcode projects via [Swift Package Manager](https://swift.org/package-manager/) (recommended) or as a [CocoaPod](https://cocoapods.org/).

## Install the SDK using Swift Package Manager (Recommended)

The Speech SDK is published as a Swift package at [microsoft/speech-sdk-spm](https://github.com/microsoft/speech-sdk-spm). To add it to an Xcode project:

1. In Xcode, choose **File** > **Add Package Dependencies...**.
1. Enter the package URL `https://github.com/microsoft/speech-sdk-spm` in the search field.
1. Keep the default dependency rule (**Up to Next Major Version**, pre-filled with the latest release), or choose a specific version, then click **Add Package**.
1. The package exposes three products — select **only one**, matching your scenario, add it to your app target, and click **Add Package**:
   * **MicrosoftCognitiveServicesSpeech-macOS** — standard macOS SDK
   * **MicrosoftCognitiveServicesSpeech-iOS** — standard iOS SDK
   * **MicrosoftCognitiveServicesSpeechEmbedded-iOS** — iOS SDK with on-device (embedded) speech

Then import the SDK in your source with `#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>`.

## Install the SDK as a CocoaPod (Alternative)

1. Install the CocoaPod dependency manager as described in its [installation instructions](https://guides.cocoapods.org/using/getting-started.html).
1. Navigate to the directory of the downloaded sample app (`helloworld`) in a terminal.
1. Run the command `pod install`. This will generate a `helloworld.xcworkspace` Xcode workspace containing both the sample app and the Speech SDK as a dependency.

## Build and Run the Sample

1. Open the project in Xcode:
   - **Swift Package Manager:** Open `helloworld.xcodeproj`
   - **CocoaPods:** After running `pod install`, open `helloworld.xcworkspace`
1. Make the following changes in the `AppDelegate.m` file:
    1. Replace the string `YourSubscriptionKey` with your subscription key.
    1. Replace the string `YourServiceRegion` with the [region](https://docs.microsoft.com/azure/cognitive-services/speech-service/regions) associated with your subscription (for example, `westus` for the free trial subscription).
1. Make the debug output visible (**View** > **Debug Area** > **Activate Console**).
1. Build and run the example code by selecting **Product** -> **Run** from the menu or clicking the **Play** button.
1. After you input some text and click the button in the app, you should hear the synthesized audio played.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-text-to-speech-objectivec-macos)
* [Speech SDK API reference for Objective-C](https://aka.ms/csspeech/objectivecref)
