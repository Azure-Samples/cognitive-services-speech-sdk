# Using codec compressed audio input with the Speech SDK on iOS (Swift)

The Speech SDK's **Compressed Audio Input Stream** API provides a way to stream compressed audio to the Speech Service using a push stream.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Mac with [Xcode](https://geo.itunes.apple.com/us/app/xcode/id497799835?mt=12) and [CocoaPods](https://cocoapods.org/) installed. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-swift) for details on system requirements and setup.

## Get the Code for the Sample App

* [Download the sample code to your development PC.](/README.md#get-the-samples)

## Get the Audio Sample File

Copy `whatstheweatherlike.mp3` from the Objective-C companion sample into this directory (alongside `CompressedStreamsSample.xcodeproj`):

```
cp ../../../objective-c/ios/compressed-streams/CompressedStreamsSample/whatstheweatherlike.mp3 .
```

## Install Dependencies

Run CocoaPods to install the Speech SDK:

```bash
pod install
```

## Configure the Sample

Open `CompressedStreamsSample.xcworkspace` in Xcode and edit `CompressedStreamsSample/ViewController.swift`:

1. Replace `YourSubscriptionKey` with your Speech service subscription key.
2. Replace `YourServiceRegion` with the [region](https://docs.microsoft.com/azure/cognitive-services/speech-service/regions) associated with your subscription (for example, `westus`).

## Run the Sample

Build and run the app on a physical iOS device or simulator. Tap the **Recognize from MP3 file** button. You should see the recognized text "What's the weather like?" appear on screen.

## References

* [Speech SDK quickstart for Swift on iOS](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-swift)
* [Speech SDK API reference for Objective-C/Swift](https://aka.ms/csspeech/objectivecref)
* [Companion Objective-C sample](../../objective-c/ios/compressed-streams)
