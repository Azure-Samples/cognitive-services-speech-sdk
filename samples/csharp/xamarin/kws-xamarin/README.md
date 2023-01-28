# Sample: Start speech recognition by using a keyword with the Speech SDK for Xamarin

This Speech SDK sample demonstrates how to start speech recognition by using a keyword in Universal Windows Platform (UWP), Android and iOS under [Xamarin](https://visualstudio.microsoft.com/xamarin/) under Windows PC and Visual Studio 2019.

> Note:
> The Speech SDK for Xamarin supports UWP (x86, x64, ARM/ARM64), Android (x86, x64 and ARM32/64) and iOS (x64 simulator and ARM64 device).

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Windows PC with Windows 10 Fall Creators Update (10.0; Build 16299) or later and with a working microphone.
* [Microsoft Visual Studio 2019](https://www.visualstudio.com/), Community Edition or higher is preferred.
* [Xamarin installation to Visual Studio](https://docs.microsoft.com/xamarin/get-started/installation/?pivots=windows).
* [Xamarin Android installation on Windows](https://docs.microsoft.com/xamarin/android/get-started/installation/windows).
* [Xamarin iOS installation on Windows](https://docs.microsoft.com/xamarin/ios/get-started/installation/windows/?pivots=windows).
* To target Android: an Android device (ARM32/64, x86; API 23: Android 6.0 Marshmallow or higher) [enabled for development](https://developer.android.com/studio/debug/dev-options) with a working microphone.
* To target iOS: an iOS device (ARM64) or an iOS simulator (x64) [enabled for development](https://docs.microsoft.com/xamarin/ios/get-started/installation/device-provisioning/) with a working microphone.
* Familiarize with [how to create custom wake word by using Speech Service](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-devices-sdk-create-kws)
  * The sample includes ready made keyword `Computer` model in `kws.table` file which can be found under asset/resource folders inside the Xamarin platform projects.
* [Speech SDK recommendations for the microphone](https://aka.ms/sdsdk-microphone).
  * The Speech SDK may work fine for your needs with conventional microphones, however we can guarantee optimal functionality only with microphones listed in the recommendations above.

## Prepare the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Start Microsoft Visual Studio 2019 and select **File** \> **Open** \> **Project/Solution**.
* Navigate to the folder containing this sample (under quickstart\csharp-xamarin), and select the solution file contained within it.
* After Visual Studio solution is launched, acknowledge that sample has a following projects:
  * helloworld - main app/UI project which is common to all platform specific projects
  * helloworld.Android - project which contains Android specific customizations
  * helloworld.iOS - project which contains iOS specific customizations
  * helloworld.UWP - project which contains UWP specific customizations
* For each project, ensure the project references or dependencies contain reference to `Microsoft.CognitiveServices.Speech` nuget package. In case it is not referenced, then go to under `Tools -> Nuget Package Manager -> Manage Nuget Packages For Solution` and install the latest version of Speech SDK for each project.
  > Note: `Microsoft.CognitiveServices.Speech` nuget package contains bitcode disabled iOS binary. If you need bitcode enabled iOS binary then use `Microsoft.CognitiveServices.Speech.Xamarin.iOS` nuget for the iOS project.
* Edit the `MainPage.xaml.cs` source under the main helloworld project:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.

## Build and run the sample for UWP
* Set the active solution configuration and platform to the desired values under **Build** \> **Configuration Manager**:
  * On a 64-bit Windows installation, choose `x64` as active solution platform.
  * On a 32-bit Windows installation, choose `x86` as active solution platform.
* Set the startup projects to `helloworld.UWP (Universal Windows)`
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.
* To run the sample app, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.
* When you see the `Enable microphone` button, click it to give microphone access for the app.
* Then select the `Start speech recognition via keyword` button and say aloud keyword `Computer` and some speech phrase, e.g. `What's the weather like?`. Expected result is that the keyword `Computer` will be recognized first and speech phrase get transcribed after that.

## Build and run the sample for Android
* Set the active solution configuration and platform to the desired values under **Build** \> **Configuration Manager**:
  * Choose `Any CPU` as active solution platform. Android will use the correct architecture automatically.
* Set the startup projects to `helloworld.Android`
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.
* To run the sample app, make sure your Android device or simulator has been setup and set the target device accordingly and press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.
* When you see the `Enable microphone` button, click it to give microphone access for the app.
* Then select the `Start speech recognition via keyword` button and say aloud keyword `Computer` and some speech phrase, e.g. `What's the weather like?`. Expected result is that the keyword `Computer` will be recognized first and speech phrase get transcribed after that.
> NOTE: If you will see an error message that `libMicrosoft.CognitiveServices.Speech.core.kws.so` extension do not load correctly, try removing bin and obj directories from the Xamarin.Android solution and launching the solution again

## Build and run the sample for iOS
* Set the active solution configuration and platform to the desired values under **Build** \> **Configuration Manager**:
  * Choose `iPhone` as active solution platform when you want to launch app on iPhone device.
  * Choose `iPhoneSimulator` as active solution platform when you want to launch app on iPhone simulator.
* Set the startup projects to `helloworld.iOS`
* Plug an iPhone device into your PC or use iPhoneSimulator of your choice.
* For iPhone device, open Info.plist file under `helloworld.iOS` project and make sure **Application** > **Bundle Identifier** in that file matches to your apple device provisioning profile.
* For iPhone device, open `Helloworld.iOS` project properties, select **iOS Bundle Signing** and under that select **Manual Provisioning** as a scheme. Under **Manual Provisioning** section, select your iPhone device **Signing Identity** > **Iphone Developer: Your email address (ID)**. After that select **Provisioning Profile** > **iOS Team Provisioning Profile `Your app's Bundle Identifier`**.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.
> Note:
> If you get build warning `MSB3341: Could not resolve reference "libMicrosoft.CognitiveServices.Speech.core.a". If this reference is required by your code, you may get compilation errors`, that is known cosmetic issue in Xamarin and will not cause harm, just continue forward.
* To run the sample app, make sure your iPhone device or iPhoneSimulator has been setup and set the target device accordingly and press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.
* When you see the `Enable microphone` button, click it to give microphone access for the app.
* Then select the `Start speech recognition via keyword` button and say aloud keyword `Computer` and some speech phrase, e.g. `What's the weather like?`. Expected result is that the keyword `Computer` will be recognized first and speech phrase get transcribed after that.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-csharp-xamarin)
* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
