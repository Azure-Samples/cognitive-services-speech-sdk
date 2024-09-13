# Video translation

Video translation client tool and API sample code

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
   Not upgrade Flurl to 4.0 due to 4.0 doesn't support NewtonJson for ReceiveJson.

# Supported OS
   ## Windows
      Microsoft.VideoTranslationClient.exe

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
   | -webvttFileAzureBlobUrl |  | Webvtt file URL with SAS(or not) which is hosted in Azure storage blob, it is optional for the first iteration of the translation, but required from the second iteration. |
   | -webvttFileKind | TargetLocaleSubtitle/SourceLocaleSubtitle/MetadataJson | Webvtt file kind. |
   | -subtitleMaxCharCountPerSegment | 100 | Subtitle max char count per segment. |
   | -speakerCount | 1 | Speaker count of the video. |
   | -exportSubtitleInVideo | false | Export subtitle in video. |

# Best practice
   ## Escape char for argument -videoFileAzureBlobUrl and -webvttFileAzureBlobUrl
   If you run client sample tool in windows shell, if there is & in the URL arguments for example SAS in Azure blob URL, it need to be converted to && for escape.

   For example if the actual URL for argument videoFileAzureBlobUrl is htts://a/b?c&d, then if  you run the command in windows shell, you need run the command like this:

      -videoFileAzureBlobUrl "htts://a/b?c&&d"

   ## How to retry?
   If you run command to create an translation or iteration, and then restart windows, the translation/iteration job will still run at server side, you can then query translation/iteration job status by using the query translation/iteration tool commmand or API with the iteration translation/iteration ID.