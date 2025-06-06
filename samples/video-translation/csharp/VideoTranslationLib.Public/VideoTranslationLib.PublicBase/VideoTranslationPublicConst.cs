//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib;

public static class VideoTranslationPublicConst
{
    public static class ArgumentDescription
    {
        public const string Region = "specify speech resource region.";

        public const string SubscriptionKey = "Specify speech resource key for authentication by key, then don't need specify customDomainName.";

        public const string CustomDomainName = "Specify custom domain name part of the endpoint: https://[customDomainName].cognitiveservices.azure.com/, then don't need specify subscriptionKey.";

        public const string ManagedIdentityClientId = "Specify managed identity client ID, only optional available when specify custom domain.";

        public const string ApiVersion = "Specify API version.";

        public const string TranslationId = "Specify translation ID.";

        public const string SourceLocale = "The source locale of the video file. Locale code follows BCP-47. You can find the text to speech locale list here https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts , if not specified, the source locale will be auto-detected from the video file, the auto detect is only supported after version 2025-05-20.";
        public const string TargetLocale = "The target locale of the translation. Locale code follows BCP-47. You can find the text to speech locale list here https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts.";
        public const string VoiceKind = "Translation voice kind with value: PlatformVoice or PersonalVoice";

        public const string VideoFileAzureBlobUrl = "Translation video file Azure blob url, .mp4 file format, maxmum 5GB file size and 4 hours video duration. Provide the input media file using either videoFileUrl or audioFileUrl, these parameters are mutually exclusive—only one of them is required. If both are provided, the request will be considered invalid.";

        public const string AudioFileAzureBlobUrl = "Translation audio file Azure blob url, .mp3 or .wav file format, maxmum 5GB file size and 4 hours video duration. Provide the input media file using either videoFileUrl or audioFileUrl, these parameters are mutually exclusive—only one of them is required. If both are provided, the request will be considered invalid.";

        public const string EnableLipSync = "Indicate whether to enable lip sync, if not provided, the default value is false to disable the lip sync.";
        public const string IterationId = "Specify iteration ID.";
        public const string IterationName = "Specify iteration name.";
        public const string TranslationName = "Specify translation name.";
        public const string TranslationDescription = "Specify translation description.";
        public const string SpeakerCount = "Number of speakers in the video, if not provided, it will be auto-detected from the video file.";
        public const string SubtitleMaxCharCountPerSegment = "Subtitle max display character count per segment, if not provided, it will use the language dependent default value.";
        public const string ExportSubtitleInVideo = "Export subtitle in video, if not specified, the default value is false, it will not burn subtitle to the translated video file.";
        public const string ExportTargetLocaleAdvancedSubtitleFile = "This parameter, when enabled, allows the API to export subtitles in the Advanced SubStation Alpha format. The subtitle file can specify font styles and colors, which helps in addressing character display issues in certain target locales such as Arabic (Ar), Japanese (Ja), Korean (Ko), and Chinese (Ch). By using this parameter, you can ensure that the subtitles are visually appealing and correctly rendered across different languages and regions, if not specified, iteration response will not include advanced subtitle.";
        public const string WebvttFileAzureBlobUrl = "Specify webvtt file Azure blob URL, it is required from secondary iteration, please specify webvttFileKind as well.";
        public const string WebvttFileKind = "Specify webvtt file kind: SourceLocaleSubtitle, TargetLocaleSubtitle or MetadataJson, it is required from secondary iteration.";
        public const string TtsCustomLexiconFileUrl = "Translate with TTS custom lexicon for speech synthesis. Provide the custom lexicon file using either ttsCustomLexiconFileUrl or ttsCustomLexiconFileIdInAudioContentCreation. These parameters are mutually exclusive—only one of them is required. If both are provided, the request will be considered invalid.";
        public const string TtsCustomLexiconFileIdInAudioContentCreation = "Translate with TTS custom lexicon for speech synthesis. Provide the custom lexicon file using either ttsCustomLexiconFileUrl or ttsCustomLexiconFileIdInAudioContentCreation. These parameters are mutually exclusive—only one of them is required. If both are provided, the request will be considered invalid.";
        public const string EnableVideoSpeedAdjustment = "This parameter allows for the adjustment of video playback speed to ensure better alignment with translated audio. When enabled, the API can slow down or speed up the video to match the timing of the translated audio, providing a more synchronized and seamless viewing experience, if not specified, video speed will not be adjusted.";
        public const string EnableOcrCorrectionFromSubtitle = "Indicate whether to allow the API to correct the speech recognition (SR) results using the subtitles from the original video file. By leveraging the existing subtitles, the API can enhance the accuracy of the transcribed text, ensuring that the final output is more precise and reliable, if not specified, translation will not do correction from OCR subtitle.";

        public const string SubtitlePrimaryRgbaColor = "This parameter specifies the primary color of the subtitles in the video translation output. The value should be provided in the format <rr><gg><bb>, #<rr><gg><bb>, <rr><gg><bb><aa> or #<rr><gg><bb><aa>, where <rr> represents the red component of the color, <gg> represents the green component, <bb> represents the blue component, <aa> represents the alpha component. For example, EBA205 or #EBA205  would set the subtitle color to a specific shade of yellow. This parameter allows for customization of subtitle appearance to enhance readability and visual appeal, if not specified, it will use default white color.";
        public const string SubtitleOutlineRgbaColor = "This parameter specifies the outline color of the subtitles in the video translation output. The value should be provided in the format <rr><gg><bb>, #<rr><gg><bb>, <rr><gg><bb><aa> or #<rr><gg><bb><aa>, where <rr> represents the red component of the color, <gg> represents the green component, <bb> represents the blue component, <aa> represents the alpha component. For example, EBA205 or #EBA205  would set the subtitle color to a specific shade of yellow. This parameter allows for customization of subtitle appearance to enhance readability and visual appeal, if not specified, it will use default black color.";
        public const string SubtitleFontSize = "This parameter specifies the font size of subtitles in the video translation output between 5 and 30, if not specified, it will use the language dependent default value.";
        public const string EnableEmotionalPlatformVoice = "This parameter specifies whether to enable emotion for platform voice. By default, the server determines whether to apply emotion based on the target locale to optimize quality. If not specified, the API will automatically decide whether to enable emotional expression on the server side.";

        public static class EventHub
        {
            public const string IsEnabled = "Indicate whether to enable sending event to EventHub.";
            public const string EventHubNamespaceHostName = "EventHub namespace host name, for example: [YourNamespace].servicebus.windows.net";

            public const string EventHubName = "EventHub name.";
            public const string EnabledEvents = "Enabled event kinds for notification, for example: TranslationCompletion,IterationCompletion";
        }
    }
}
