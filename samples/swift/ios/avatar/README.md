# TTS Avatar real-time API sample for iOS

This sample demonstrates usage of Azure text-to-speech avatar real-time API, with Swift using the Speech SDK for iOS.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Mac with [Xcode](https://developer.apple.com/xcode/) installed as iOS development environment. These samples target iOS versions 12.0 or later.
* An iOS device with iOS 12.0 or later [enabled for development](https://developer.apple.com/documentation/xcode/enabling-developer-mode-on-a-device) with a working speaker.

## Install the SDK as a CocoaPod

* Install the CocoaPod dependency manager as described in its [installation instructions](https://guides.cocoapods.org/using/getting-started.html).
* Navigate to the directory of the downloaded sample app (e.g. `speech-samples`) in a terminal.
* Run the command `pod install`. This will generate a Xcode workspace containing both the sample app and the Speech SDK as a dependency. This workspace will be used in the following.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Open either Xcode project with the sample code.
* Edit the `<sample name>/ViewController.swift` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
  * Update below parameters if you want to use the avatar service with different configurations:
    * `avatarCharacter` - The character of the avatar. By default it's lisa, and you can update this value to use a different avatar.
    * `avatarStyle` - The style of the avatar. You can update this value to use a different avatar style. This parameter is optional for custom avatar.
    * `customAvatar` - Set this to `true` if you are using a custom avatar.
    * `ttsVoice` - The voice of the TTS. Here is the [available TTS voices list](https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts#supported-languages).
    * `ttsEndpointID` - The deployment ID (also called endpoint ID) of your custom voice. If you are not using a custom voice, please leave it empty.
* To build the sample app and check if all the paths are set correctly, choose **Product** > **Build** from the menu.

## Run the sample

* Connect your iOS device to your development PC.
* Choose **Product** > **Destination** from the menu, pick your iOS device.
* Click the `Play` button, or select **Product** > **Run** from the menu
* On your iOS device, use the buttons in the user interface to run the sample.
  * Touch `Start Session` button to setup video connection with Azure TTS Talking Avatar service. If everything goes well, you should see a live video with an avatar being shown on the screen.
  * Type some text in the text box on top of the screen and touch `Speak` button to send the text to Azure TTS Talking Avatar service. The service will synthesize the text to talking avatar video, and send the video stream back to the browser. The application will play the video stream. You should see the avatar speaking the text you typed with mouth movement, and hear the voice which is synchronized with the mouth movement.
  * You can either continue to type text in the text box and let the avatar speak that text by touching `Speak` button, or touch `Stop Session` button to stop the video connection with Azure TTS Talking Avatar service. If you touch `Stop Session` button, you can touch `Start Session` button to start a new video connection with Azure TTS Talking Avatar service.

## Note on iOS permission handling

Please note that the Speech SDK requires two permissions, i.e., the INTERNET permission. For iOS, you add these by specifying NSAppTransportSecurity and NSExceptionDomains in your Info.plist file if needed. iOS handles permissions for sensitive data (like microphone access) through the system’s privacy settings, where users can grant or revoke these at any time.

## References

* [Speech SDK API reference for Objective-C](https://aka.ms/csspeech/objectivecref)
