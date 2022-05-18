# C++ Captioning scenario app for Windows

In this sample, you create captions from audio input using the speech-to-text capability of the Speech SDK for C++ on Windows.

## Prerequisites

> [!div class="checklist"]
> * Azure subscription - [Create one for free](https://azure.microsoft.com/free/cognitive-services)
> * <a href="https://ms.portal.azure.com/#create/Microsoft.CognitiveServicesSpeechServices"  title="Create a Speech resource"  target="_blank">Create a Speech resource</a> in the Azure portal. You can use the free pricing tier (`F0`) to try the service, and upgrade later to a paid tier for production.
> * Get the resource key and region. After your Speech resource is deployed, select **Go to resource** to view and manage keys. For more information about Cognitive Services resources, see [Get the keys for your resource](~/articles/cognitive-services/cognitive-services-apis-create-account.md#get-the-keys-for-your-resource). 
* [Microsoft Visual Studio 2019](https://www.visualstudio.com/), Community Edition or higher.

## Set up the environment

Install the **Desktop development with C++** workload in Visual Studio and the **NuGet package manager** component in Visual Studio. In Visual Studio, select **Tools** \> **Get Tools and Features** to open the Visual Studio Installer. The **Desktop development with C++** workload is under the **Workloads** tab. **NuGet package manager** is under the **Individual components** tab in the **Code tools** section.

The Speech SDK is available as a [NuGet package](https://www.nuget.org/packages/Microsoft.CognitiveServices.Speech) and implements .NET Standard 2.0. You install the Speech SDK in the next section of this article, but first check the [platform-specific installation instructions](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/setup-platform?tabs=dotnet%2Cwindows%2Cjre%2Cbrowser&pivots=programming-language-cpp) for any more requirements.

> [!NOTE]
> By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its [license agreement](https://aka.ms/csspeech/license201809).

If you wish to create captions from compressed audio files such as MP4 files, you must also install [GStreamer](~/articles/cognitive-services/speech-service/how-to-use-codec-compressed-audio-input-streams.md).

> [!NOTE]
> - When you install GStreamer on a Windows computer, you must create environment variables named `GST_PLUGIN_PATH` and `GSTREAMER_ROOT_X86_64`, as well as adding `C:\gstreamer\1.0\msvc_x86_64\bin` to your PATH environment variable (see the previous link for more information). Be sure to make these changes to your system environment variables, not your user environment variables. After making changes to your system environment variables, you must reboot your computer for the changes to take effect.
> - If you install the 64-bit version of GStreamer, you must also [configure your C++ project to target 64-bit platforms](https://docs.microsoft.com/en-us/cpp/build/how-to-configure-visual-cpp-projects-to-target-64-bit-platforms?view=msvc-170).
> If one or more of these conditions is not met, you might receive the error `SPXERR_GSTREAMER_NOT_FOUND_ERROR` when you try to use GStreamer with the captioning scenario app.

## Build and run the sample

1. Download or copy the [scenarios/cpp/windows/captioning/](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/scenarios/go/captioning/) sample files from GitHub into a local directory. 
1. Open `captioning.sln` in Visual Studio.  
1. Install the Speech SDK in your project with the NuGet package manager.
    ```powershell
    Install-Package Microsoft.CognitiveServices.Speech
    ```
    > [!NOTE]
    > If you are seeing red squigglies from IntelliSense for Speech SDK APIs,
    > right-click in your editor window and select **Rescan** > **Rescan Solution**.
1. Make sure the compiler is set to **ISO C++17 Standard (/std:c++17)** at **Project** > **Properties** > **General** > **C++ Language Standard**.
1. Set the active solution configuration and platform to the desired values under **Build** \> **Configuration Manager**:
    - On a 64-bit Windows installation, choose `x64` as active solution platform.
    - On a 32-bit Windows installation, choose `x86` as active solution platform.
1. Enter your preferred command line arguments at **Project** > **Properties** > **Debugging** > **Command Arguments**. See [usage and arguments](#usage-and-arguments) for the available options. Here is an example. Replace `YourSubscriptionKey` with your Speech resource key, and replace `YourServiceRegion` with your Speech resource region:
    ```
    --key YourSubscriptionKey --region YourServiceRegion --input c:\caption\caption.this.mp4 --format any --output c:\caption\caption.output.txt - --srt --recognizing --threshold 5 --profanity mask --phrases "Contoso;Jessie;Rehaan"
    ```
    Replace `YourSubscriptionKey` with your Speech resource key, and replace `YourServiceRegion` with your Speech resource [region](https://aka.ms/csspeech/region) (for example, `westus` or `northeurope`.) Make sure that the paths specified by `--input` and `--output` are valid. Otherwise you must change the paths.
1. Build and run the console application. The output file with complete captions is written to `c:\caption\caption.output.txt`. Intermediate results are shown in the console:
    ```console
    00:00:00,180 --> 00:00:01,600
    Welcome to
    
    00:00:00,180 --> 00:00:01,820
    Welcome to applied
    
    00:00:00,180 --> 00:00:02,420
    Welcome to applied mathematics
    
    00:00:00,180 --> 00:00:02,930
    Welcome to applied mathematics course
    
    00:00:00,180 --> 00:00:03,100
    Welcome to applied Mathematics course 2
    
    00:00:00,180 --> 00:00:03,230
    Welcome to applied Mathematics course 201.
    ```

## Usage and arguments

Usage: `captioning --key <key> --region <region> --input <input file>`

Connection options include:

- `--key`: Your Speech resource key. 
- `--region REGION`: Your Speech resource region. Examples: `westus`, `eastus`

Input options include:

- `--input FILE`: Input audio from file. The default input is the microphone. 
- `--format FORMAT`: Use compressed audio format. Valid only with `--file`. Valid values are `alaw`, `any`, `flac`, `mp3`, `mulaw`, and `ogg_opus`. The default value is `any`. This option is not available with the JavaScript captioning sample. For compressed audio files such as MP4, install GStreamer and see [How to use compressed input audio](~/articles/cognitive-services/speech-service/how-to-use-codec-compressed-audio-input-streams.md).

Language options include:

- `--languages LANG1,LANG2`: Enable language identification for specified languages. For example: `en-US,ja-JP`. This option is only available with the C++, C#, and Python captioning samples. For more information, see [Language identification](~/articles/cognitive-services/speech-service/language-identification.md).

Recognition options include:

- `--recognizing`: Output `Recognizing` event results. The default output is `Recognized` event results only. These are always written to the console, never to an output file. The `--quiet` option overrides this. For more information, see [Get speech recognition results](~/articles/cognitive-services/speech-service/get-speech-recognition-results.md).

Accuracy options include:

- `--phrases PHRASE1;PHRASE2`: You can specify a list of phrases to be recognized, such as `Contoso;Jessie;Rehaan`. For more information, see [Improve recognition with phrase list](~/articles/cognitive-services/speech-service/improve-accuracy-phrase-list.md).

Output options include:

- `--help`: Show this help and stop
- `--output FILE`: Output captions to the specified `file`. This flag is required.
- `--srt`: Output captions in SRT (SubRip Text) format. The default format is WebVTT (Web Video Text Tracks). For more information about SRT and WebVTT caption file formats, see [Caption output format](~/articles/cognitive-services/speech-service/captioning-concepts.md#caption-output-format).
- `--quiet`: Suppress console output, except errors.
- `--profanity OPTION`: Valid values: raw, remove, mask. For more information, see [Profanity filter](~/articles/cognitive-services/speech-service/captioning-concepts.md#profanity-filter) concepts.
- `--threshold NUMBER`: Set stable partial result threshold. The default value with this code example is `3`. For more information, see [Get partial results](~/articles/cognitive-services/speech-service/captioning-concepts.md#get-partial-results) concepts.

## Clean up resources

You can use the [Azure portal](~/articles/cognitive-services/cognitive-services-apis-create-account.md#clean-up-resources) or [Azure Command Line Interface (CLI)](~/articles/cognitive-services/cognitive-services-apis-create-account-cli.md#clean-up-resources) to remove the Speech resource you created.

## References

* [Speech SDK API reference for C++](https://aka.ms/csspeech/cppref)
* [Package (NuGet)](https://www.nuget.org/packages/Microsoft.CognitiveServices.Speech)
* [Additional Samples on GitHub](https://aka.ms/speech/github-cpp)
