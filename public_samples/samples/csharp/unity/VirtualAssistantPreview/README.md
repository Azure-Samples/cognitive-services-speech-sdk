# Quickstart: Interact with a bot in C# Unity

This sample demonstrates how to connect to a BotFramework bot, send and receive activities, and recognize speech in Unity C# using the Speech SDK.

## Prerequisites

* This sample is targeted for Unity 2018.3 or later. To install, pick an appropriate option in the [Unity store](https://store.unity.com).
  * If you are not familiar with Unity, it is recommended to study Unity manuals before starting your app development
  * See [Windows documentation under Unity manual](https://docs.unity3d.com/Manual/Windows.html)
  * See [Android documentation under Unity manual](https://docs.unity3d.com/Manual/android.html)
    >Note:
    >[UWP ARM64 support was only added in Unity 2019.1](https://blogs.unity3d.com/2019/04/16/introducing-unity-2019-1/#universal)
* An Azure subscription key for the Speech Service. Get one for free. [Get one for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A previously created bot configured with the [Direct Line Speech channel](https://docs.microsoft.com/azure/bot-service/bot-service-channel-connect-directlinespeech)
* A Windows PC with Windows 10 Fall Creators Update (10.0; Build 16299) or later and with a working microphone.
* [Microsoft Visual Studio 2017](https://www.visualstudio.com/), Community Edition or higher.
    > For ARM64 support, install the [optional build tools, and Windows 10 SDK for ARM/ARM64](https://blogs.windows.com/buildingapps/2018/11/15/official-support-for-windows-10-on-arm-development/)
* To target Android: an Android device (ARM32/64, x86; API 23: Android 6.0 Marshmallow or higher) enabled for development  with a working microphone.

## Download Speech SDK plugin and sample

* **By downloading the plugin and sample you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license201809).**
* The Speech SDK for Unity (Beta) is packaged as a Unity asset package (.unitypackage). Download it from [here](https://aka.ms/csspeech/unitypackage).
* Also, [download the sample code to your development PC.](/README.md#get-the-samples)

## Prepare the sample

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
* Select the `SpeechBotConnectorSample` scene:
  * In the `Project Window` (bottom-left by default), navigate to **Assets** > **Scenes** and double-click on the `SpeechBotConnectorSample` scene to open it up.
  * Remove "Untitled Scene" if it exists
* Select `Canvas` and view the SpeechBotConnectorSample script component - it will have several empty fields.
  * Replace `Subscription Key` with your Cognitive Services subscription key.
  * Replace `Region` with the service region of your subscription. For example, `westus2` (As part of preview, the only-supported region is 'westus2')
  * Replace `Connection Id` with your bot connection id.
* You can alternatively edit the above settings in the `SpeechBotConnectorSample.cs` source.
  * Inside Unity editor, in the `Project` window under **Assets** > **SpeechSDK** > **Scripts** directory, double-click the file to make edits.

## Run the sample

There are various options to run this sample.
Start by running it in the Unity Editor (first option), then explore other options depending on your needs.

### Build and run the sample via Unity editor for Universal Windows Platform

* Open **File** \> **Build Settings**.
* Select **Universal Windows Platform** and Click **Switch Platform**.
* Press **Play** button on Unity editor.
  * When the app runs, the state indicator text should read "SpeechBotConnector created".
  * When ready, press the 'Start' button to begin the bot interaction.
    * While speaking, you should see the recognition results being displayed and hear TTS output if "speak" responses are received.
  * The console output also provides debug messages.

### Build and run the sample as a stand-alone desktop application

* Open **File** > **Build Settings**.
* Select **PC, Mac & Linux Standalone** as platform.
* If this wasn't the active platform before, you have to select **Switch Platform** (and wait a bit).
* Ensure the **Target Platform** field is set to **Windows** and pick the option you need from the **Architecture** field.
* Select **Build**.
* In the folder picker, create and select a new build folder.
* Once the build has finished, the Windows Explorer launches with the folder containing the application `SpeechBotConnectorSample.exe`. **Do not launch it yet**
* Navigate to the `SpeechBotConnectorSample_Data/Plugins` folder in your build output folder. Copy the DLL `Microsoft.CognitiveServices.Speech.core.dll` into the `SpeechBotConnectorSample_Data/Managed` folder.
* Double-click on `SpeechBotConnectorSample.exe` in the build output folder to start the application.
* When the app runs, the state indicator text should read "SpeechBotConnector created".
* When ready, press the 'Start' button to begin the bot interaction.
  * While speaking, you should see the recognition results being displayed and hear TTS output if "speak" responses are received.
* Close the app to stop.

### Build and run the sample for Universal Windows Platform

* Open **File** \> **Build Settings**.
* Select **Universal Windows Platform** and Click **Switch Platform**.
* Select **Player Settings**, Click **Other Settings**, Select **.NET 4.x Equivalent** as **Scripting Runtime Version**.
* Select **IL2CPP** as Scripting Backend.
* Under **Publishing Settings** \> **capabilities**, make sure following capabilities are selected.
  * InternetClient
  * InternetClientServer
  * Microphone
* From **Build Settings**, click **Build**, create a directory e.g. `UWP` under the SpeechBotConnectorSample project and press **Select**.
* After successfull build, use Windows explorer and double click generated `SpeechBotConnectorSample.sln` file under the `UWP` folder and that should open Visual Studio project.
* Inside Visual Studio project, change build target to x64 or x86
* Build SpeechBotConnectorSample solution
* After successfull build, deploy the project
  * NOTE: Sometimes in order to succesfully deploy, you may need to generate Visual Assets under Package.manifest, there under
          Visual Assets -> Asset Generator, select thumbnails as source from Assets folder and generate.
* Launch SpeechBotConnectorSample application
  * Accept consent query for microphone
  * Press the 'Start' button and say something. You should see the recognized text on-screen.

### Build and run the sample for Android platform

* Complete your [Android environment setup in Unity](https://docs.unity3d.com/Manual/android-sdksetup.html), including the Android NDK setup.
* Open **File** > **Build Settings**.
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
  * Then, click the button and speak an English phrase or sentence into your device's microphone. Your speech is transmitted to the Speech service and transcribed to text, which appears in the window.
  * Close the app to stop speech recognition.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-unity)
* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
