# Video translation

Video translation client tool and API sample code

## Option 1: Run the Sample Easily in Visual Studio Code (Recommended)
This is the easiest way to get started. The Azure AI Speech Toolkit extension automates setup, environment configuration, build, and run.

- Install [Azure AI Speech Toolkit](https://marketplace.visualstudio.com/items?itemName=ms-azureaispeech.azure-ai-speech-toolkit) extension in VS Code.
- In extension's panel, click `View Samples` and download this sample from sample gallery.
- From the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`), run:
  - `Azure AI Speech Toolkit: Configure Azure Speech Resources` to select an **eastus** regional speech resource. (Video translation currently only supports Azure Speech resources in the **eastus** region.) (one-time setup).
  - `Azure AI Speech Toolkit: Configure and Setup the Sample App` to prepare the project (one-time setup).
  - `Azure AI Speech Toolkit: Build the Sample App` to compile the code.
  - `Azure AI Speech Toolkit: Run the Sample App` to run the sample.

      Upload an input video to Azure Blob Storage and copy the Blob SAS URL. Paste the Azure Blob URL when the run task terminal ask for it. Interactively enter other arguments in the run task terminal. The `Azure AI Speech Toolkit: Run the Sample App` command will run in mode `CreateTranslationAndIterationAndWaitUntilTerminated`:

      ```
      dotnet VideoTranslationSample/VideoTranslationSample/bin/Debug/net7.0/Microsoft.SpeechServices.VideoTranslation.ApiSampleCode.PublicPreview.dll -mode CreateTranslationAndIterationAndWaitUntilTerminated -apiVersion 2024-05-20-preview -subscriptionKey <YourSubscriptionKey> -region <YourSubscriptionRegion> -videoFileAzureBlobUrl <YourVideoFileAzureBlobSASUrl> -sourceLocale <YourVideoLocale> -targetLocale <OutputVideoLocale> -voiceKind <TTSSynthesisVoiceKind> -translationId <YourTranslationIdString> -iterationId <YourIterationIdString>
      ```
      
      Check more modes and arguments usage in [Command line sample](#Command-line-sample) and [Command line tool arguments](#Command-line-tool-arguments) sections. Try them out in new terminal.

## Option 2: Manual Setup and Run (Advanced)
Follow these steps if you prefer not to use VS Code.

### Supported OS:
- Windows: 
   - Install [dotnet 8.0](https://dotnet.microsoft.com/download/dotnet/8.0)
   - Run tool: `VideoTranslationSample.exe [verb] [arguments]`
- Linux:
   - Install [dotnet 8.0](https://dotnet.microsoft.com/download/dotnet/8.0)
   - Run tool: `dotnet VideoTranslationSample.dll [verb] [arguments]`

### Solution:
   [VideoTranslationApiSampleCode.sln](VideoTranslationSample.sln)

## Work Flow

   1. Upload video file to Azure storage blob, and then create translation and run first iteration with below command:

   > VideoTranslationSample createTranslationAndIterationAndWaitUntilTerminated --region [RegionIdentifier] --subscriptionKey [YourSpeechResourceKey] --apiVersion [ApiVersion] --sourceLocale [SourceLocale] --targetLocales [TargetLocale] --voiceKind [PersonalVoice/PlatformVoice] -videoFileAzureBlobUrl [VideoFileAzureBlobUrl]

   2. Download translated metadata json webvtt file to local, and modify it for content editing.
   You can use [Subtitle Edit Tool](https://github.com/SubtitleEdit/subtitleedit) for assistance.

   3. Upload modified metadata json webvtt file to Azure storage blob, and then create another iteration with below command:
   > VideoTranslationSample createIterationAndWaitUntilTerminated --region [RegionIdentifier] --subscriptionKey [YourSpeechResourceKey] --apiVersion [ApiVersion] --translationId [TranslationId] -iterationId [IterationId]


## API sample:

### Usage:
   For RESTful API usage reference below API core library class.

### RESTful API core library:
   Translation API core library: [TranslationClient.cs](VideoTranslationLib.Public/VideoTranslationLib.ReferenceFiles/HttpClient/TranslationClient.cs)

   Iteration API core library: [IterationClient.cs](VideoTranslationLib.Public/VideoTranslationLib.ReferenceFiles/HttpClient/IterationClient.cs)

   Operation API core library: [OperationClient.cs](VideoTranslationLib.Public/VideoTranslationLib.ReferenceFiles/HttpClient/OperationClient.cs)

## For project CommonLib
   Do not upgrade Flurl to version 4.0 because it does not support NewtonJson for ReceiveJson.



## Command Line Usage
   | Description | Command line arguments |
   | ------------ | -------------- |
   | Upload the new video file for translation and run the first iteration of the translation. | createTranslationAndIterationAndWaitUntilTerminated --region [RegionIdentifier] --subscriptionKey [YourSpeechResourceKey] --apiVersion [ApiVersion] --sourceLocale [SourceLocale] --targetLocales [TargetLocale] --voiceKind [PersonalVoice/PlatformVoice] --videoFileAzureBlobUrl [VideoFileAzureBlobUrl] |
   | Create a translation for the video file. | createTranslation --region [RegionIdentifier] --subscriptionKey [YourSpeechResourceKey] --apiVersion [ApiVersion] --sourceLocale [SourceLocale] --targetLocale [TargetLocale] --voiceKind [PersonalVoice/PlatformVoice] --translationId [TranslationId] --videoFileAzureBlobUrl [VideoFileAzureBlobUrl] |
   | Query the translations. | queryTranslations --region [RegionIdentifier] --subscriptionKey [YourSpeechResourceKey] --apiVersion [ApiVersion] |
   | Query the translation by ID. | queryTranslation --region [RegionIdentifier] --subscriptionKey [YourSpeechResourceKey] --apiVersion [ApiVersion] --translationId [TranslationId] |
   | Delete the translation by ID. | deleteTranslation --region [RegionIdentifier] --subscriptionKey [YourSpeechResourceKey] --apiVersion [ApiVersion] --translationId [TranslationId] |
   | Create an iteration for the translation and wait until its terminated. | createIterationAndWaitUntilTerminated --region [RegionIdentifier] --subscriptionKey [YourSpeechResourceKey] --apiVersion [ApiVersion] --translationId [TranslationId] --iterationId [IterationId]  |
   | Query the iterations. | queryIterations --region [RegionIdentifier] --subscriptionKey [YourSpeechResourceKey] --apiVersion [ApiVersion] --translationId [TranslationId] |
   | Query the iteration by ID. | queryIteration --region [RegionIdentifier] --subscriptionKey [YourSpeechResourceKey] --apiVersion [ApiVersion] --translationId [TranslationId] --iterationId [IterationId] |

## Command line tool arguments
   | Argument | Is Required | Supported Values Sample | Description |
   | -------- | -------- | ---------------- | ----------- |
   | --region  | True | eastus | Provide the region of the API request. |
   | --subscriptionKey | True | | Provide your speech resource key. |
   | --apiVersion | True | 2024-05-20-preview | Provide the version of the API request. |
   | --voiceKind | True | PlatformVoice/PersonalVoice | Synthesize TTS for the translated target video using either PlatformVoice or PersonalVoice. |
   | --sourceLocale | True | en-US | The locale of the input video file. |
   | --targetLocales | True | en-US | Target locale of the translation. |
   | --translationId | True | MyTranslateVideo1FromZhCNToEnUS2024050601 | Translation ID. |
   | --iterationId | True | MyFirstIteration2024050601 | Iteration ID. |
   | --videoFileAzureBlobUrl | True | URL | Please proivde video file URL, with or without SAS, which is hosted in an Azure storage blob. |
   | --webvttFileAzureBlobUrl | False | URL | Please provide the WebVTT file URL, with or without SAS, which is hosted in an Azure storage blob. It is optional for the first iteration of the translation but required from the second iteration. |
   | --webvttFileKind | False | TargetLocaleSubtitle/SourceLocaleSubtitle/MetadataJson | Please specify the kind of WebVTT file with a value of TargetLocaleSubtitle, SourceLocaleSubtitle, or MetadataJson. |
   | --subtitleMaxCharCountPerSegment | False | 100 | Please specify the maximum display character count per segment for the subtitles. |
   | --speakerCount | False | 1 | Please specify the speaker count of the video. |
   | --exportSubtitleInVideo | False | False | Please indicate whether to export subtitles in the video. |

## Argument definitions
### Supported regions
   https://learn.microsoft.com/azure/ai-services/speech-service/regions

   Use region identifier for the region argument.

### Supported API versions:
* 2024-05-20-preview

### Supported VoiceKind
* PlatformVoice
* PersonalVoice

## Best practice
### Escape char for URL arguments in Windows
   For arguments:  --videoFileAzureBlobUrl and --webvttFileAzureBlobUrl
   If you run a client sample tool in a Windows shell and there is an & in the URL arguments (for example, a SAS token in an Azure blob URL), the & needs to be converted to ^& to escape it.

   For example, if the actual URL for the argument videoFileAzureBlobUrl is https://a/b?c&d, then when you run the command in the Windows shell, you need to run the command like this:

      --videoFileAzureBlobUrl "https://a/b?c^&d"

### How to retry?
   If you initiate a command to create a translation or iteration job and subsequently restart Windows, the job will continue to run on the server side. To check the status of the translation/iteration job, you can use the query translation/iteration tool command or the API, providing the specific translation/iteration ID.