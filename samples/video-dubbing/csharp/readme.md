# Video Dubbing

Video dubbing client tool and API sample code

# Solution:
   [VideoTranslationApiSampleCode.sln](VideoTranslationApiSampleCode/VideoTranslationApiSampleCode.sln)


# API sample:

## Video file API:
   Metadata API: [VideoFileClient.cs](VideoTranslationApiSampleCode/VideoTranslationLib/VideoTranslationMetadataClient.cs)

   Video file API: [VideoFileClient.cs](VideoTranslationApiSampleCode/VideoTranslationLib/VideoFileClient.cs)

   Translation API: [VideoTranslationClient.cs](VideoTranslationApiSampleCode/VideoTranslationLib/VideoTranslationClient.cs)

   Target locale API: [TargetLocaleClient.cs](VideoTranslationApiSampleCode/VideoTranslationLib/TargetLocaleClient.cs)

# For project CommonLib
   Not upgrade Flurl to 4.0 due to 4.0 doesn't support NewtonJson for ReceiveJson.

# Supported OS
   ## Windows
      Microsoft.SpeechServices.VideoTranslation.exe
   ## Linux:
      $HOME/.dotnet/dotnet ./Microsoft.SpeechServices.VideoTranslation.dll

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
   | Query video dubbing metadata | -mode QueryMetadata -region eastus -subscriptionKey ... |
   | Upload new video file, and translate the file to new locale | -mode UploadVideoOrAudioFileAndCreateTranslation -region eastus -subscriptionKey ... -sourceLocale zh-CN -targetLocales en-US -VoiceKind PersonalVoice -sourceVideoOrAudioFilePath YourVideoFileLocalPath |
   | Query single translation by ID. | -mode QueryTranslation -region eastus -subscriptionKey ... -id TranslationId |
   | Query all translations of the speech resource. | -mode QueryTranslations -region eastus -subscriptionKey ... |
   | Delete single translation by ID. | -mode DeleteTranslation -region eastus -subscriptionKey ... -id TranslationId |
   | Upload video file | -mode UploadVideoOrAudioFile -region eastus -subscriptionKey -sourceLocale zh-CN -sourceVideoOrAudioFilePath YourVideoFileLocalPath. |
   | Delete single video file by ID. |-mode DeleteVideoOrAudioFile -region eastus -subscriptionKey ... -id TranslationId -deleteAssociations |
   | query all uploaded video files. | -mode QueryVideoOrAudioFiles -region eastus -subscriptionKey ... |
   | query single video file by ID | -mode QueryVideoOrAudioFile -environment eastus -subscriptionKey ... -id VideoFileId |

# Command line tool arguments
   | Argument | Supported Values Sample | Description |
   | -------- | ---------------- | ----------- |
   | -region  | eastus | Supported regions |
   | -subscriptionKey | | Your speech resource key |
   | -VoiceKind | PlatformVoice/PersonalVoice | For trnaslated target video, synthesis TTS with either PlatformVoice or PersonalVoice. |
   | -profileName | [Supported profiles](#Supported-profiles) | Translation pipeline profile |
   | -sourceLocale | en-US | Video file source locale, supported source locales can be queried by running tool with QueryMetadata mode. |
   | -targetLocales | en-US | translation target locale, supported source locales can be queried by running tool with QueryMetadata mode. |

# Supported profiles
   | Profile name | Description | When to use? |
   | ------------ | ----------- | ------------ |
   | DynamicAlignmentProfile-Experiment-V1  | Optimized background audio extraction, translation content, length, alignment adjustment and personal voice TTS synthesis. | Full translatio. |
   | UserInputWebvttAlignmentProfile-Experiment-V1 | Optimized background audio extraction, translation content, length, alignment adjustment and personal voice TTS synthesis. | Translate with webvtt content editting. |
   | GreedyAlignmentProfile-Experiment-V1 | Gender sensitive translation. | Gender sensitve text translation. |
