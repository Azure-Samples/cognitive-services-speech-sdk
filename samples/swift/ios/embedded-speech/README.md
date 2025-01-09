# Embedded Speech Samples for Swift on iOS

These samples demonstrate how to use embedded speech functionalities with the Microsoft Cognitive Services Speech SDK and Swift as the programming language.

For an introduction to the embedded Speech SDK, please refer to the [article for embedded speech](https://learn.microsoft.com/azure/ai-services/speech-service/embedded-speech).

## Prerequisites

- A Mac with Xcode installed as an iOS development environment. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-swift) for details on system requirements and setup.

## Get the Code for the Samples

- [Download the sample code to your development machine.](/README.md#get-the-samples)

## Get the Speech SDK for iOS

The Cognitive Services Speech SDK with embedded speech support for iOS can be downloaded as a zip file from [https://aka.ms/csspeech/iosbinaryembedded](https://aka.ms/csspeech/iosbinaryembedded). Download and unpack the files to the directory of this `README.md`, i.e., parallel to the samples directories.

## Build and Run the Sample

1. Open the Xcode project with the sample code.
2. Build and run the sample in Xcode. This will load the project in Xcode, and you can interact with the sample application using the simulator.

## Usage

The `embedded-speech-to-text` sample demonstrates how to perform speech recognition on iOS using embedded speech models. It allows you to initiate speech recognition from the device's microphone and receive real-time recognition results.

The `embedded-text-to-speech` sample demonstrates how to convert text to speech on iOS using embedded speech synthesis models. It allows you to input text and hear the corresponding synthesized speech output.

The `embedded-speech-translation` sample demonstrates how to perform speech translation on iOS using embedded speech translation models. It allows you to speak in one language and receive real-time translations in another language, leveraging the embedded translation models.

## References

- [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-objectivec-ios)
- [Speech SDK API reference for Objective-C](https://aka.ms/csspeech/objectivecref)
