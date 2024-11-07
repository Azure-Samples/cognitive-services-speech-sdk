# Video translation

Video translation client tool and API sample code

## Run the Sample within VS Code
- Install "Azure AI Speech Toolkit" extension in VS Code.
- Download this sample from sample gallery to local machine.
- Trigger "Azure AI Speech Toolkit: Configure Azure Speech Resources" command from command palette to select an **eastus** regional speech resource.
- Trigger "Azure AI Speech Toolkit: Build the Sample App" command from command palette to build the sample.
- Trigger "Azure AI Speech Toolkit: Run the Sample App" command from command palette to run the sample.

Video translation currently only supports Azure Speech resources in the **eastus** region.

As a sample, only execute commands with the mode parameter is CreateTranslationAndIterationAndWaitUntilTerminated:
```
dotnet VideoTranslationSample/VideoTranslationSample/bin/Debug/net7.0/Microsoft.SpeechServices.VideoTranslation.ApiSampleCode.PublicPreview.dll -mode CreateTranslationAndIterationAndWaitUntilTerminated -apiVersion 2024-05-20-preview -subscriptionKey <YourSubscriptionKey> -region <YourSubscriptionRegion> -videoFileAzureBlobUrl YourVideoFileAzureBlobSASUrl -sourceLocale <YourVideoLocale> -targetLocale <OutputVideoLocale> -voiceKind <TTSSynthesisVoiceKind> -translationId <YourTranslationIdString> -iterationId <YourIterationIdString>
```
Check more modes and corresponding arguments usage in [Command line sample](#Command-line-sample) and [Command line tool arguments](#Command-line-tool-arguments) sections.

# Solution:
   [VideoTranslationApiSampleCode.sln](VideoTranslationSample/VideoTranslationSample.sln)


# API sample:

## Usage:
   For RESTful API usage reference below API core library class.

## RESTful API core library:
   Translation API core library: [TranslationClient.cs](Common/VideoTranslationLib.PublicPreview.Base/HttpClient/TranslationClient.cs)

   Iteration API core library: [TranslationClient.cs](Common/VideoTranslationLib.PublicPreview.Base/HttpClient/IterationClient.cs)

   Operation API core library: [TranslationClient.cs](Common/VideoTranslationLib.PublicPreview.Base/HttpClient/OperationClient.cs)

# For project CommonLib
   Do not upgrade Flurl to version 4.0 because it does not support NewtonJson for ReceiveJson.

# Supported OS
   ## Windows
      Microsoft.VideoTranslationClient.exe

# Runn tool on Windows prerequisite:
   [Install dotnet 7.0](https://dotnet.microsoft.com/en-us/download/dotnet/7.0)

# Command line sample
   | Description | Command Sample |
   | ------------ | -------------- |
   | Upload the new video file for translation and run the first iteration of the translation. | -mode CreateTranslationAndIterationAndWaitUntilTerminated -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview -sourceLocale zh-CN -targetLocales en-US -VoiceKind PersonalVoice -videoFileAzureBlobUrl VideoFileAzureBlobUrl |
   | Create a translation for the video file. | -mode CreateTranslation -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview -sourceLocale zh-CN -targetLocale en-US -voiceKind PlatformVoice -translationId translationId -videoFileAzureBlobUrl VideoFileAzureBlobUrl |
   | Query the translations. | -mode QueryTranslations -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview |
   | Query the translation by ID. | -mode QueryTranslation -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview -translationId translationId |
   | Delete the translation by ID. | -mode DeleteTranslation -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview -translationId translationId |
   | Create an iteration for the translation. | -mode CreateIteration -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview -translationId translationId -iterationId iterationId  |
   | Query the iterations. | -mode QueryIterations -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview -translationId translationId |
   | Query the iteration by ID. | -mode QueryIteration -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview -translationId translationId -iterationId iterationId |

# Command line tool arguments
   | Argument | Supported Values Sample | Description |
   | -------- | ---------------- | ----------- |
   | -region  | eastus | Provide the region of the API request. |
   | -subscriptionKey | | Provide your speech resource key. |
   | -apiVersion | 2024-05-20-preview | Provide the version of the API request. |
   | -VoiceKind | PlatformVoice/PersonalVoice | Synthesize TTS for the translated target video using either PlatformVoice or PersonalVoice. |
   | -sourceLocale | en-US | The locale of the input video file. |
   | -targetLocales | en-US | Target locale of the translation. |
   | -translationId | MyTranslateVideo1FromZhCNToEnUS2024050601 | Translation ID. |
   | -iterationId | MyFirstIteration2024050601 | Iteration ID. |
   | -videoFileAzureBlobUrl |  | Please proivde video file URL, with or without SAS, which is hosted in an Azure storage blob. |
   | -webvttFileAzureBlobUrl |  | Please provide the WebVTT file URL, with or without SAS, which is hosted in an Azure storage blob. It is optional for the first iteration of the translation but required from the second iteration. |
   | -webvttFileKind | TargetLocaleSubtitle/SourceLocaleSubtitle/MetadataJson | Please specify the kind of WebVTT file with a value of TargetLocaleSubtitle, SourceLocaleSubtitle, or MetadataJson. |
   | -subtitleMaxCharCountPerSegment | 100 | Please specify the maximum display character count per segment for the subtitles. |
   | -speakerCount | 1 | Please specify the speaker count of the video. |
   | -exportSubtitleInVideo | false | Please indicate whether to export subtitles in the video. |

# Best practice
   ## Escape char for argument -videoFileAzureBlobUrl and -webvttFileAzureBlobUrl
   If you run a client sample tool in a Windows CMD (Command Prompt) and there is an & in the URL arguments (for example, a SAS token in an Azure blob URL), the & needs to be converted to ^& to escape it.

   For example, if the actual URL for the argument videoFileAzureBlobUrl is https://a/b?c&d, then when you run the command in the Windows shell, you need to run the command like this:

      -videoFileAzureBlobUrl "https://a/b?c^&d"

   ## How to retry?
   If you initiate a command to create a translation or iteration job and subsequently restart Windows, the job will continue to run on the server side. To check the status of the translation/iteration job, you can use the query translation/iteration tool command or the API, providing the specific translation/iteration ID.