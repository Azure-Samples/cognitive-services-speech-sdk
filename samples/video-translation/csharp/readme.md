# Video translation

Video translation client tool and API sample code

# Supported OS
## Windows prerequisite:
   Install [dotnet 8.0](https://dotnet.microsoft.com/download/dotnet/8.0)
   
   Run tool: VideoTranslationSample.exe [verb] [arguments]

## Linux prerequisite:
   Install [dotnet 8.0](https://dotnet.microsoft.com/download/dotnet/8.0)

   Run tool: dotnet VideoTranslationSample.dll [verb] [arguments]

# Work Flow

   1. Upload video file to Azure storage blob, and then create translation and run first iteration with below command:

   > VideoTranslationSample createTranslationAndIterationAndWaitUntilTerminated --region [RegionIdentifier] --subscriptionKey [YourSpeechResourceKey] --apiVersion [ApiVersion] --sourceLocale [SourceLocale] --targetLocales [TargetLocale] --voiceKind [PersonalVoice/PlatformVoice] -videoFileAzureBlobUrl [VideoFileAzureBlobUrl]

   2. Download translated metadata json webvtt file to local, and modify it for content editing.
   You can use [Subtitle Edit Tool](https://github.com/SubtitleEdit/subtitleedit) for assistance.

   3. Upload modified metadata json webvtt file to Azure storage blob, and then create another iteration with below command:
   > VideoTranslationSample createIterationAndWaitUntilTerminated --region [RegionIdentifier] --subscriptionKey [YourSpeechResourceKey] --apiVersion [ApiVersion] --translationId [TranslationId] -iterationId [IterationId]

# Solution:
   [VideoTranslationApiSampleCode.sln](VideoTranslationSample.sln)

## Authentication
There are 4 options for authentication: 1. speech resource key, 2. User account, 3. managed identity.
### 1. Subscription key
The endpoint (and key) could be got from the Keys and Endpoint page in the Speech service resource.

Run the tool with argument:

      --subscriptionKey [YourSpeechResourceKey]

### 2. System-assigned managed identity
   1. Assing [System-Assigned Managed Identity] or [Your Azure Resource Running The Tool] to the [speech resource] with role [Cognitive Services Speech Contributor].

   2. [Enable custom domain for the speech resource](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-configure-azure-ad-auth?tabs=portal&pivots=programming-language-csharp#create-a-custom-domain-name), then you will get custom domain url like this: https://[YourCustomDomainName].cognitiveservices.azure.com/

   3. Run the tool in [Your Azure Resoruce].

   4. Run the tool with argument:

      --customDomainName [YourCustomDomainName]

### 3. User-assigned managed identity
   1. Assing [User-Assigned Managed Identity] to the [speech resource] with role [Cognitive Services Speech Contributor].

   2. [Enable custom domain for the speech resource](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-configure-azure-ad-auth?tabs=portal&pivots=programming-language-csharp#create-a-custom-domain-name), then you will get custom domain url like this: https://[YourCustomDomainName].cognitiveservices.azure.com/

   3. Run the tool in [Your Azure Resoruce].

   4. Run the tool with argument:

      --customDomainName [YourCustomDomainName] --managedIdentityClientId [YourUserAssignedManagedIdentityClientId]

### 4. User account
For using user account(default credential) need below steps:

   1. Assing [your account] to the [speech resource] with role [Cognitive Services Speech Contributor].

   2. [Enable custom domain for the speech resource](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-configure-azure-ad-auth?tabs=portal&pivots=programming-language-csharp#create-a-custom-domain-name), then you will get custom domain url like this: https://[YourCustomDomainName].cognitiveservices.azure.com/

   3. Run the tool with your account.

   4. Run the tool with argument:
   
      --customDomainName [YourCustomDomainName]

# API sample:

## Usage:
   For RESTful API usage reference below API core library class.

## RESTful API core library:
   Translation API core library: [TranslationClient.cs](VideoTranslationLib.Public/VideoTranslationLib.ReferenceFiles/HttpClient/TranslationClient.cs)

   Iteration API core library: [IterationClient.cs](VideoTranslationLib.Public/VideoTranslationLib.ReferenceFiles/HttpClient/IterationClient.cs)

   Operation API core library: [OperationClient.cs](VideoTranslationLib.Public/VideoTranslationLib.ReferenceFiles/HttpClient/OperationClient.cs)

# For project CommonLib
   Do not upgrade Flurl to version 4.0 because it does not support NewtonJson for ReceiveJson.

# Command Line Usage
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

# Command line tool arguments
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

## Below arguments are only available after API version: 2025-05-20
| Argument | Description |
| --- | --- |
| --source_locale | Support optional for auto detect |
| --audio_file_blob_url | Support translate audio file |  
| --tts_custom_lexicon_file_url | Translate with TTS custom lexicon for TTS synthesis, provide the custom lexicon file with URL, ttsCustomLexiconFileUrl and ttsCustomLexiconFileIdInAudioContentCreation are conflict, only one of them are required. |
|  --tts_custom_lexicon_file_id_in_audio_content_creation | Translate with TTS custom lexicon for TTS synthesis, provide the custom lexicon file with file ID in Audio Content Creation, ttsCustomLexiconFileUrl and ttsCustomLexiconFileIdInAudioContentCreation are conflict, only one of them are required. |
| --enable_video_speed_adjustment | This parameter allows for the adjustment of video playback speed to ensure better alignment with translated audio. When enabled, the API can slow down or speed up the video to match the timing of the translated audio, providing a more synchronized and seamless viewing experience. |
| --enable_ocr_correction_from_subtitle | Indicate whether allow the API to correct the speech recognition (SR) results using the subtitles from the original video file. By leveraging the existing subtitles, the API can enhance the accuracy of the transcribed text, ensuring that the final output is more precise and reliable. |
| --export_target_locale_advanced_subtitle_file | This parameter, when enabled, allows the API to export subtitles in the Advanced SubStation Alpha format. The subtitle file can specify font styles and colors, which helps in addressing character display issues in certain target locales such as Arabic (Ar), Japanese (Ja), Korean (Ko), and Chinese (Ch). By using this parameter, you can ensure that the subtitles are visually appealing and correctly rendered across different languages and regions. |
| --subtitle_primary_color | This parameter specifies the primary color of the subtitles in the video translation output. The value should be provided in the format <rr><gg><bb>, #<rr><gg><bb>, <rr><gg><bb><aa> or #<rr><gg><bb><aa>, where <rr> represents the red component of the color, <gg> represents the green component, <bb> represents the blue component, <aa> represents the alpha component. For example, EBA205 or #EBA205  would set the subtitle color to a specific shade of yellow. This parameter allows for customization of subtitle appearance to enhance readability and visual appeal. |
| --subtitle_outline_color | This parameter specifies the outline color of the subtitles in the video translation output. The value should be provided in the format <rr><gg><bb>, #<rr><gg><bb>, <rr><gg><bb><aa> or #<rr><gg><bb><aa>, where <rr> represents the red component of the color, <gg> represents the green component, <bb> represents the blue component, <aa> represents the alpha component. For example, EBA205 or #EBA205  would set the subtitle color to a specific shade of yellow. This parameter allows for customization of subtitle appearance to enhance readability and visual appeal. |
| --subtitle_font_size | This parameter specifies the font size of subtitles in the video translation output. |

# Argument definitions
## Supported regions
   https://learn.microsoft.com/azure/ai-services/speech-service/regions

   Use region identifier for the region argument.

## Supported API versions:
* 2024-05-20-preview

## Supported VoiceKind
* PlatformVoice
* PersonalVoice

# Best practice
## Escape char for URL arguments in Windows
   For arguments:  --videoFileAzureBlobUrl and --webvttFileAzureBlobUrl
   If you run a client sample tool in a Windows shell and there is an & in the URL arguments (for example, a SAS token in an Azure blob URL), the & needs to be converted to ^& to escape it.

   For example, if the actual URL for the argument videoFileAzureBlobUrl is https://a/b?c&d, then when you run the command in the Windows shell, you need to run the command like this:

      --videoFileAzureBlobUrl "https://a/b?c^&d"

## How to retry?
   If you initiate a command to create a translation or iteration job and subsequently restart Windows, the job will continue to run on the server side. To check the status of the translation/iteration job, you can use the query translation/iteration tool command or the API, providing the specific translation/iteration ID.

# Trouble shooting:
## Permission error
1. If you got this response when you want to use [System-assigned Managed Identity]:

      {"error":{"code":"PermissionDenied","message": "Principal does not have access to API/Operation."}}

Please check you have granted [your resoruce] to the [Cognitive Services Speech Contributor] role of the [speech resource]

2. If you got this response when you want to use [User-assigned Managed Identity]:

```
      Failed to run with exception: DefaultAzureCredential failed to retrieve a token from the included credentials. See the troubleshooting guide for more information. https://aka.ms/azsdk/net/identity/defaultazurecredential/troubleshoot
      - EnvironmentCredential authentication unavailable. Environment variables are not fully configured. See the troubleshooting guide for more information. https://aka.ms/azsdk/net/identity/environmentcredential/troubleshoot
      - WorkloadIdentityCredential authentication unavailable. The workload options are not fully configured. See the troubleshooting guide for more information. https://aka.ms/azsdk/net/identity/workloadidentitycredential/troubleshoot
      - ManagedIdentityCredential authentication unavailable. The requested identity has not been assigned to this resource.
      - VisualStudioCredential authentication failed: Visual Studio Token provider can't be accessed at C:\Users\poleli\AppData\Local\.IdentityService\AzureServiceAuth\tokenprovider.json
      - AzureCliCredential authentication failed: Azure CLI not installed
      - AzurePowerShellCredential authentication failed: Az.Accounts module >= 2.2.0 is not installed.
      - AzureDeveloperCliCredential authentication failed: Azure Developer CLI could not be found.
      Failure with exit code: -1
```

Please proivde user-assigned managed identity when run the command.

   --managedIdentityClientId [YourUserAssignedManagedIdentityClientId]