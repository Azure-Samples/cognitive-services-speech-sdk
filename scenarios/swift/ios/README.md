# Additional samples for Swift on iOS

Theses samples demonstrate more advanced techniques for speech recognition with the Microsoft Cognitive Services Speech SDK.
For an introduction to the SDK, please refer to the [quickstart articles for speech recognition](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started-speech-to-text?pivots=programming-languages-objectivec-swift) and [speech synthesis](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started-text-to-speech?pivots=programming-languages-objectivec-swift) on the SDK documentation page for step-by-step instructions.


## Example app with various samples

This [speech sample app](./language-learning) shows techniques for pronunciation assessment.


## Prerequisites

- A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
- A Mac with Xcode installed as iOS development environment. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-swift) for details on system requirements and setup.

## Get the code for the samples

- [Download the sample code to your development machine.](/README.md#get-the-samples)

## Get the Speech SDK for iOS

**By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**

The Cognitive Services Speech SDK for iOS is distributed as an XCFramework bundle through [Swift Package Manager](https://swift.org/package-manager/).

## Install the SDK using Swift Package Manager

If `language-learning/speech-samples.xcodeproj` already lists `speech-sdk-spm` under **Package Dependencies**, no installation is needed.
Otherwise, replace the CI-only local `{{{PACKAGE_PATH}}}` reference with the published SDK:

1. Open `language-learning/speech-samples.xcodeproj` in Xcode.
1. In the project's **Package Dependencies**, remove the unresolved local Speech SDK package reference (`{{{PACKAGE_PATH}}}`).
1. Choose **File** > **Add Package Dependencies...** and enter `https://github.com/microsoft/speech-sdk-spm`.
1. Keep the default version rule (**Up to Next Major Version**) or choose a specific version, then click **Add Package**.
1. Select **MicrosoftCognitiveServicesSpeech-iOS**, add it to the `speech-samples` app target, and click **Add Package**. The `Differ` dependency is already configured in the project.

## Build the samples

Add your subscription details to the `language-learning/speech-samples/ViewController.swift` file:

1. Replace the string `YourSubscriptionKey` with your subscription key.
2. Replace the string `YourServiceRegion` with the [region](https://docs.microsoft.com/azure/cognitive-services/speech-service/regions) associated with your subscription (for example, `westus` for the free trial subscription).

To build the sample app and check if all the paths are set correctly, choose **Product** > **Build** from the menu.

## Run the samples

To run the sample, click the `Play` button, or select **Product** > **Run** from the menu.
In the simulator window that opens, you can interact with the sample application.

## References

* [Speech SDK API reference for Objective-C](https://aka.ms/csspeech/objectivecref)
