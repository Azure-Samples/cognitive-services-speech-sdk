# Conversation transcription (aka real-time diarization) sample for Swift on iOS

This sample demonstrates conversation transcription (aka real-time diarization) with the Microsoft Cognitive Services Speech SDK.
For an introduction to the SDK, please refer to the [quickstart articles for speech recognition](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started-speech-to-text?pivots=programming-languages-objectivec-swift) on the SDK documentation page for step-by-step instructions.

## Prerequisites

- A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
- A Mac with Xcode installed as iOS development environment. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-swift) for details on system requirements and setup.

## Get the code for the samples

- [Download the sample code to your development machine.](/README.md#get-the-samples)

## Get the Speech SDK for iOS

**By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**

The Cognitive Services Speech SDK for iOS is distributed as an XCFramework bundle.
It can be added to Xcode projects via [Swift Package Manager](https://swift.org/package-manager/) (recommended) or as a [CocoaPod](https://cocoapods.org/).

## Install the SDK using Swift Package Manager (Recommended)

The Speech SDK is published as a Swift package at [microsoft/speech-sdk-spm](https://github.com/microsoft/speech-sdk-spm). To add it to an Xcode project:

1. In Xcode, choose **File** > **Add Package Dependencies...**.
1. Enter the package URL `https://github.com/microsoft/speech-sdk-spm` in the search field.
1. Keep the default dependency rule (**Up to Next Major Version**, pre-filled with the latest release), or choose a specific version, then click **Add Package**.
1. The package exposes three products — select **only one**, matching your scenario, add it to your app target, and click **Add Package**:
   - **MicrosoftCognitiveServicesSpeech-iOS** — standard iOS SDK
   - **MicrosoftCognitiveServicesSpeechEmbedded-iOS** — iOS SDK with on-device (embedded) speech
   - **MicrosoftCognitiveServicesSpeech-macOS** — standard macOS SDK

Then import the SDK in your source with `import MicrosoftCognitiveServicesSpeech`.

## Install the SDK as a CocoaPod (Alternative)

1. Install the CocoaPod dependency manager as described in its [installation instructions](https://guides.cocoapods.org/using/getting-started.html).
1. Navigate to the directory of the downloaded sample app (e.g. `speech-samples`) in a terminal.
1. Run the command `pod install`. This will generate a Xcode workspace containing both the sample app and the Speech SDK as a dependency.

## Build the samples

1. Open the project in Xcode:
   - **Swift Package Manager:** Open the `.xcodeproj` file
   - **CocoaPods:** After running `pod install`, open the `.xcworkspace` file
1. Add your subscription details to the `<sample name>/ViewController.swift` file:

1. Replace the string `YourSubscriptionKey` with your subscription key.
2. Replace the string `YourServiceRegion` with the [region](https://docs.microsoft.com/azure/cognitive-services/speech-service/regions) associated with your subscription (for example, `westus` for the free trial subscription).

To build the sample app and check if all the paths are set correctly, choose **Product** > **Build** from the menu.

## Run the samples

To run the sample, click the `Play` button, or select **Product** > **Run** from the menu.
In the simulator window that opens, you can interact with the sample application.

## References

* [Speech SDK API reference for Objective-C](https://aka.ms/csspeech/objectivecref)
