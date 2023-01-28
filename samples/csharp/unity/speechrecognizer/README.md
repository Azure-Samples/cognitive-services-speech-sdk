# Speech Recognition, translation and intent recognition using C# & Unity

This sample demonstrates how to use the Microsoft Cognitive Service Speech SDK in [Unity](https://unity3d.com/) experiences, including the following scenarios:
* Speech recognition with live hypotheses (i.e. interim results)
* Speech recognition with translation in multiple languages simultaneously.
* Intent recognition via speech input using Natural Language Understanding

> Note:
> The Speech SDK for Unity supports Windows Desktop (x86 and x64) or Universal Windows Platform (x86, x64, ARM/ARM64), Android (x86, ARM32/64), iOS (x64 simulator, ARM64), Mac (x64) and Linux (x64).

## Prerequisites

* This sample is targeted for Unity 2020.3 or later. To install, pick an appropriate option in the [Unity store](https://store.unity.com/).
  > Note:
  > If you are not familiar with Unity, it is recommended to study the [Unity User Manual](https://docs.unity3d.com/Manual/UnityManual.html) before starting your app development.
* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Windows PC with Windows 10 Fall Creators Update (10.0; Build 16299) or later and with a working microphone.
* A Mac device with Xcode and Unity installed for iOS development.
* [Microsoft Visual Studio 2019](https://www.visualstudio.com/), Community Edition or higher.
* Access to your computer's microphone.
* To target Android: an Android device (ARM32/64, x86; API 23: Android 6.0 Marshmallow or higher) [enabled for development](https://developer.android.com/studio/debug/dev-options) with a working microphone.
* To target iOS: an iOS device (ARM64) [enabled for development](https://learn.unity.com/tutorial/building-for-mobile) with a working microphone.

## Download the Speech SDK for Unity

* **By downloading the Speech SDK for Unity you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* The Speech SDK for Unity is packaged as a Unity asset package (.unitypackage).
  Download it from [here](https://aka.ms/csspeech/unitypackage).

## Import the Speech SDK plugin for Unity in the sample 

* Open the sample project in Unity:
  * Start Unity. In the launch window, under the **Projects** tab select **Open**.
  * Select the folder which contains this sample.
  * After a bit of time, the Unity Editor window should pop up.
    > Note: do not worry about the error  `The type or namespace name
    > 'CognitiveServices' does not exist in the namespace 'Microsoft' (are you
    > missing an assembly reference?)` after opening the sample. This error
    > gets resolved in the next step by importing the Speech SDK.
* Import the Speech SDK:
  * Import the Speech SDK by selecting **Assets** > **Import Package** > **Custom Package**.
  * In the file picker, select the Speech SDK .unitypackage file that you downloaded before.
  * Ensure that all files are selected and click **Import**.
  * NOTE: If you have installed Unity only for specific platform e.g. Unity CloudBuild for Android, be aware that you may need to remove other 
  * platform (iOS, Mac, WSA) binaries from the Speech SDK Plugins folder to avoid possible conflicts with the same library name.

## SCENE 1: Prepare & run the Speech Recognition & Translation sample scene

* Select the `SpeechRecognition` sample scene:
  * In the Project Window (bottom left by default), navigate to **Assets** > **SpeechSDKSample** > **Scenes** and double-click on the `SpeechRecognition` scene to open it up.
* Update the sample with your subscription data using one of the following options:
   * OPTION 1 - VIA INSPECTOR:
      * Select the `SpeechManager` game object in the scene hierarchy.
      * Enter the following values in the `Speech Recognition (Script) component in the Inspector:
        * Enter your own subscription key in the Inspector field labeled `Speech Service API Key`.
        * Enter the service region of your subscription in the Inspector field labeled `Speech Service Region`. For example, replace with `westus` if you are using the 30-day free trial subscription.
   * OPTION 2 - VIA SCRIPT:
      * In the Project Window, navigate to **Assets** > **SpeechSDKSample** > **Scripts** and double-click the `SpeechRecognition` C# script to edit it. Note: you can configure which code editor will be launched under **Edit** > **Preferences**, for details see [here](https://docs.unity3d.com/Manual/Preferences.html).
      * Perform the following edits:
        * Replace the string `YourSubscriptionKey` with your own subscription key.
        * Replace the string `YourServiceRegion` with the service region of your subscription. For example, replace with `westus` if you are using the 30-day free trial subscription.
        * Save the file.
* Run the sample scene in the Unity Editor:
  * Press the **Play** button in the Unity Editor toolbar (below the menu bar).
    1. In the Unity Editor's Game Window, click the `Continuous Recognition` button and speak an English phrase or sentence into your device's microphone. Your speech is transmitted to the Speech service and transcribed to text, which appears in the window.
    2. The sample uses continuous recognition, allowing you to speak multiple utterances without repeatedly pressing any button.
    3. The Speech Service provides live hypotheses (i.e. interim results) while the user speaks, which the sample displays on screen in near real-time as they are received.
    4. The end of the utterance is automatically detected by the server, at which point the final result is displayed. 
    5. Check also the [Console Window](https://docs.unity3d.com/Manual/Console.html) for debug messages.
    6. Click the `Stop Recognition` button when done.
    7. To experiment with speech translation, use one of the three dropdown lists to select target languages for translation.
        * Note: the source language in this sample is currently fixed to "en-us" (English US). You can edit the `fromLanguage` class variable in the `SpeechRecognition` script to select a different source language.
        * Note: the languages in the dropdown lists do *not* represent the full list of supported languages. Refer to [Language support](https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support) section of the Cognitive Services Speech documentation to see the full list of supported languages for the various speech services available.
    8. Make sure the `Enable Translation` checkbox is selected.
    9. Repeat steps 1 through 6 above to test speech recognition. This time, the recognized utterances are displayed in the source language as well as the three selected target languages simultaneously. 
  * Click the **Play** button again to stop running the app.

> Note:
> the `SpeechRecognition` C# script is used as a component of the `SpeechManager` game object in the `SpeechRecognition` scene hierarchy.

## SCENE 2: Prepare & run the Intent Recognition sample scene

> Note:
> this sample scene requires that you first create a new LUIS app for natural language understanding. Get started at [https://www.luis.ai/](https://www.luis.ai/) to first familiarize yourself with Cognitive Services Language Understanding (LUIS). Also refer to the [LUIS documentation here](https://docs.microsoft.com/azure/cognitive-services/luis/). A sample app definition is included with this sample to easily get you started. Also note that this sample uses credentials for the LUIS service, which are *NOT* the same credentials used in the speech sample above. More details are provided below.

### Prepare the LUIS app

* Follow the [instructions here](https://docs.microsoft.com/azure/cognitive-services/luis/luis-how-to-start-new-app) to create a new LUIS app from file. Use the file `LUIS-App-Export-Unity-FunWithShapes.json` located in the root of this sample to create your LUIS app.
* Follow the [instructions here](https://docs.microsoft.com/azure/cognitive-services/luis/get-started-portal-deploy-app) to deploy your app in the LUIS portal.
* Once your app has been trained and published, keep your LUIS App Id, App key and service region info handy since you'll need them below.

### Prepare the Unity sample

* Select the `IntentRecognition` sample scene:
  * In the Project Window (bottom left by default), navigate to **Assets** > **SpeechSDKSample** > **Scenes** and double-click on the `IntentRecognition` scene to open it up.
* Update the sample with your subscription data using one of the following options:
   * OPTION 1 - VIA INSPECTOR:
      * Select the `SpeechManager` game object in the scene hierarchy.
      * Enter the following values in the `Intent Recognition (Script) component in the Inspector:
        * Enter your LUIS App Id in the Inspector field labeled `LUIS App Id`.
        * Enter your LUIS App Key in the Inspector field labeled `LUIS App Key`.
        * Enter the service region of your subscription in the Inspector field labeled `LUIS Region`. For example, replace with `westus` if you are using the 30-day free trial subscription.
   * OPTION 2 - VIA SCRIPT:
      * In the Project Window, navigate to **Assets** > **SpeechSDKSample** > **Scripts** and double-click the `IntentRecognition` C# script to edit it. Note: you can configure which code editor will be launched under **Edit** > **Preferences**, for details see [here](https://docs.unity3d.com/Manual/Preferences.html).
      * Perform the following edits:
        * Replace the string `YourLUISAppId` with your own LUIS App Id.
        * Replace the string `YourLUISAppKey` with your own LUIS App Key.
        * Replace the string `YourLUISServiceRegion` with the service region of your subscription. For example, replace with `westus` if you are using the 30-day free trial subscription.
        * Save the file.
* Run the sample scene in the Unity Editor:
  * Press the **Play** button in the Unity Editor toolbar (below the menu bar).
    1. In the Unity Editor's Game Window, click the `Intent Recognition` button and speak an English phrase or sentence into your device's microphone. Your speech is transmitted to the Speech service and transcribed to text, which appears in the window. If the utterance matches a desired intent as defined in the LUIS app, the scene shapes will change size or color to reflect the user command. Try some of the following commands and feel free to experiment with your own commands using natural language:
        * "*Please make the ball yellow*"
        * "*Ok now I'd like the tube color to be purple*"
        * "*Can you make the cube bigger?*"
        * "*Actually the cube should be even bigger*"
        * "*And finally I want the ball to be smaller*"
    2. The sample uses continuous recognition, allowing you to speak multiple commands without repeatedly pressing any button.
    3. The Speech Service provides live hypotheses (i.e. interim results) while the user speaks, which the sample displays on screen in near real-time as they are received.
    4. The end of the utterance is automatically detected by the server, at which point the final result is displayed and the commands is processed by the sample if a matched intent was found. 
    5. Check also the [Console Window](https://docs.unity3d.com/Manual/Console.html) for debug messages.
    6. Click the `Stop Recognition` button when done.
  * Click the **Play** button again to stop running the app.

> Note:
> the `IntentRecognition` C# script is used as a component of the `SpeechManager` game object in the `IntentRecognition` scene hierarchy.

## Building the sample scenes

There are various options to run the scenes in this sample.
Start by running the scenes in the Unity Editor (see above), then explore the other build options below depending on your needs.
Note: the instructions below are the same regardless of the sample scene you wish to build and deploy.

### Build and run the sample as a stand-alone desktop application

* Open **File** > **Build Settings**.
* Select the scene to build in the `Scenes in Build` list at the top of the dialog. Note: this sample is designed to only deploy one scene at a time since no scene navigation controls are included.
* Select **PC, Mac & Linux Standalone** as platform.
* If this wasn't the active platform before, you have to select **Switch Platform** (and wait a bit).
* Ensure the **Target Platform** field is set to **Windows** and pick the option you need from the **Architecture** field.
* Select **Build**.
* In the folder picker, create and select a new build folder.
* Once the build has finished, the Windows Explorer launches with the folder containing the application `SpeechRecognitionSample.exe`. **Do not launch it yet**
* Navigate to the `SpeechRecognitionSample_Data/Plugins` folder in your build output folder. Copy ALL the DLLs beginning with `Microsoft.CognitiveServices.Speech` into the `SpeechRecognitionSample_Data/Managed` folder.
* Double-click on `SpeechRecognitionSample.exe` in the build output folder to start the application.
* Then, follow the `SCENE 1` run instructions outlined above to test the scene.
* Close the app to stop speech recognition.

### Build and run the sample as Universal Windows Platform application

* Open **File** > **Build Settings**.
* Select the scene to build in the `Scenes in Build` list at the top of the dialog. Note: this sample is designed to only deploy one scene at a time since no scene navigation controls are included.
* Select **Universal Windows Platform** as platform.
* If this wasn't the active platform before, you have to select **Switch Platform** (and wait a bit).
* Ensure that **Target Platform** and **Architecture** are set appropriately.
* Click **Player Settings...** to open them up in the Inspector Window (on the right by default).
* Navigate to the section **Other Settings** in the **Settings for Universal Windows Platform** tab:
  * Select **.NET 4.x Equivalent** as **Scripting Runtime Version**.
  * Select **IL2CPP** as **Scripting Backend**.
* In the section **Publishing Settings**, review that checkmarks are set for the these capabilities in the **Capabilities** control:
  * `InternetClient`
  * `InternetClientServer`
  * `Microphone`
* Back in the **Build Settings** window, click **Build And Run**.
* In the folder picker, create and select a new folder.
* After the build completes, the UWP app launches:
  * When you see the `Enable microphone` button, click it.
  * Then, follow the `SCENE 1` run instructions outlined above to test the scene.
  * Close the app to stop speech recognition.

### Build and run the sample for Android platform

* Complete your [Android environment setup in Unity](https://docs.unity3d.com/Manual/android-sdksetup.html), including the Android NDK setup.
* Open **File** > **Build Settings**.
* Select the scene to build in the `Scenes in Build` list at the top of the dialog. Note: this sample is designed to only deploy one scene at a time since no scene navigation controls are included.
* Select **Android** as platform.
* If this wasn't the active platform before, you have to select **Switch Platform** (and wait a bit).
* Click **Player Settings...** to open them up in the Inspector Window (on the right by default).
* Navigate to the **Other Settings** of the **Settings for Android** tab.
* Review the information under **Identification** and **Configuration** and make sure they are correct for your environment, in particular:
  * **Minimum API Level**
  * **Target API Level**
  * **Scripting Runtime Version** (set to **.NET 4.x**)
  * **Scripting Backend** (set to **IL2CPP**)
  * **Target Architectures**
* Connect your [developer-enabled](https://developer.android.com/studio/debug/dev-options) Android device to your PC via USB.
* Back in the **Build Settings** window, click **Build And Run**.
* After the build completes, the app launches on your Android device:
  * When you see the prompt asking for microphone permission, agree to it.
  * Then, follow the `SCENE 1` run instructions outlined above to test the scene.
  * Close the app to stop speech recognition.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-csharp-unity)
* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
* [LUIS documentation](https://docs.microsoft.com/azure/cognitive-services/luis/)
* [LUIS portal](https://luis.ai)
