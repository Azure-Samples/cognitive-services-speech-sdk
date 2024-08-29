# Video Dubbing

Video dubbing client tool and API sample code

# Solution:
   [VideoTranslationApiSampleCode.sln](VideoTranslationSample/VideoTranslationSample.sln)


# API sample:

## RESTful API library:
   Translation API: [TranslationClient.cs](Common/VideoTranslationLib.PublicPreview.Base/HttpClient/TranslationClient.cs)

   Iteration API: [TranslationClient.cs](Common/VideoTranslationLib.PublicPreview.Base/HttpClient/IterationClient.cs)

   Operation API: [TranslationClient.cs](Common/VideoTranslationLib.PublicPreview.Base/HttpClient/OperationClient.cs)

# For project CommonLib
   Not upgrade Flurl to 4.0 due to 4.0 doesn't support NewtonJson for ReceiveJson.

# Supported OS
   ## Windows
      Microsoft.VideoTranslationClient.exe
   ## Linux:
      $HOME/.dotnet/dotnet ./Microsoft.VideoTranslationClient.dll

# Runn tool on Linux prerequisite
   Install dotnet 7.0 on Linux:
      wget https://dot.net/v1/dotnet-install.sh -O dotnet-install.sh
      chmod +x ./dotnet-install.sh
      ./dotnet-install.sh --channel 7.0
      export DOTNET_ROOT=$HOME/.dotnet
   [Install .NET on Linux without using a package manager - .NET | Microsoft Learn](https://learn.microsoft.com/en-us/dotnet/core/install/linux-scripted-manual#scripted-install)


# Runn tool on Windows prerequisite:
   [Install dotnet 7.0](https://dotnet.microsoft.com/en-us/download/dotnet/7.0)

# Command line sample
   | Description | Command Sample |
   | ------------ | -------------- |
   | Upload new video file for translation, and run first iteration of the translation | -mode CreateTranslationAndIterationAndWaitUntilTerminated -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview -sourceLocale zh-CN -targetLocales en-US -VoiceKind PersonalVoice -videoFileAzureBlobUrl VideoFileAzureBlobUrl |
   | Create translation for a video file. | -mode CreateTranslation -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview -sourceLocale zh-CN -targetLocale en-US -voiceKind PlatformVoice -translationId translationId -videoFileAzureBlobUrl VideoFileAzureBlobUrl |
   | Query translations. | -mode QueryTranslations -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview |
   | Query translation by ID. | -mode QueryTranslation -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview -translationId translationId |
   | Delete translation by ID. | -mode DeleteTranslation -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview -translationId translationId |
   | Create iteration for a translation. | -mode CreateIteration -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview -translationId translationId -iterationId iterationId  |
   | Query iterations. | -mode QueryIterations -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview -translationId translationId |
   | Query iteration by ID. | -mode QueryIteration -region eastus -subscriptionKey subscriptionKey -apiVersion 2024-05-20-preview -translationId translationId -iterationId iterationId |

# Command line tool arguments
   | Argument | Supported Values Sample | Description |
   | -------- | ---------------- | ----------- |
   | -region  | eastus | Supported regions |
   | -subscriptionKey | | Your speech resource key |
   | -apiVersion | 2024-05-20-preview | API version |
   | -VoiceKind | PlatformVoice/PersonalVoice | For trnaslated target video, synthesis TTS with either PlatformVoice or PersonalVoice. |
   | -sourceLocale | en-US | Video file source locale, supported source locales can be queried by running tool with QueryMetadata mode. |
   | -targetLocales | en-US | translation target locale, supported source locales can be queried by running tool with QueryMetadata mode. |
   | -translationId | MyTranslateVideo1FromZhCNToEnUS2024050601 | Translation ID. |
   | -iterationId | MyFirstIteration2024050601 | Iteration ID. |
   | -videoFileAzureBlobUrl |  | Video file URL with SAS(or not) which is hosted in Azure storage blob. |
   | -webvttFileAzureBlobUrl |  | Webvtt file URL with SAS(or not) which is hosted in Azure storage blob. |
   | -webvttFileKind | TargetLocaleSubtitle/SourceLocaleSubtitle/MetadataJson | Webvtt file kind. |
   | -subtitleMaxCharCountPerSegment | 100 | Subtitle max char count per segment. |
   | -speakerCount | 1 | Speaker count of the video. |
   | -enableLipSync | false | Enable lip sync. |
   | -exportSubtitleInVideo | false | Export subtitle in video. |
