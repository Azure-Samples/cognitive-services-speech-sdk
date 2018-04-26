//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech.Recognition
{
    /// <summary>
    /// Defines constants used in speech recognition and translation.
    /// </summary>
    public static class ParameterNames
    {
        /// <summary>
        /// The name of parameter `SpeechSubscriptionKey`
        /// </summary>
        public const string SpeechSubscriptionKey = "SPEECH-SubscriptionKey";

        /// <summary>
        /// The name of parameter `SpeechAuthToken`
        /// </summary>
        public const string SpeechAuthToken = "SPEECH-AuthToken";

        /// <summary>
        /// The name of parameter `SpeechRpsToken`
        /// </summary>
        public const string SpeechRpsToken = "SPEECH-RpsToken";

        /// <summary>
        /// The name of parameter `SpeechModelId`
        /// </summary>
        public const string SpeechModelId = "SPEECH-ModelId";

        /// <summary>
        /// The name of parameter `Region`
        /// </summary>
        public const string Region = "SPEECH-Region";

        /// <summary>
        /// The name of parameter `Language`.
        /// </summary>
        public const string SpeechRecognitionLanguage = "SPEECH-RecoLanguage";

        /// <summary>
        /// The name of parameter `RecognitionMode`.
        /// </summary>
        public const string SpeechRecognitionMode = "SPEECH-RecoMode";

        /// <summary>
        /// The name of parameter `SpeechEndpoint`.
        /// </summary>
        public const string SpeechEndpoint = "SPEECH-Endpoint";

        /// <summary>
        /// The name of parameter `TRANSLATION-FromLanguage` which describes the source language of audio.
        /// </summary>
        public const string TranslationFromLanguage = "TRANSLATION-FromLanguage";

        /// <summary>
        /// The name of parameter `TRANSLATION-ToLanguage` which describes target languages for translation.
        /// </summary>
        public const string TranslationToLanguages = "TRANSLATION-ToLanguages";

        /// <summary>
        /// The name of parameter `TRANSLATION-Features` describes which features are desired for translation.
        /// </summary>
        public const string TranslationFeatures = "TRANSLATION-Features";

        /// <summary>
        /// The name of parameter `TRANSLATION-Voice` describes which voice is used for text-to-speech.
        /// </summary>
        public const string TranslationVoice = "TRANSLATION-Voice";
    }
}
