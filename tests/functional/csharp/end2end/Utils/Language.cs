//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System.Collections.Generic;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils
{
    sealed class Language
    {
        public static string EN => "en-US";
        public static string FR => "fr";
        public static string TR => "tr";
        public static string ES => "es";
        public static string DE => "de";
        public static string ZH => "zh-Hans";
        public static string DE_DE => "de-DE";
        public static string CA_ES => "ca-ES";
        public static string ZH_CN => "zh-CN";

        // List of speech-to-text codes are copied from https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support [7 dec 2018]
        public static readonly string[] Speech2TextAll =
        {
            "ar-EG", "ca-ES", "da-DK", "de-DE", "en-AU", "en-CA", "en-GB", "en-IN", "en-NZ", "en-US", "es-ES", "es-MX", "fi-FI", "fr-CA", "fr-FR",
            "hi-IN", "it-IT", "ja-JP", "ko-KR", "nb-NO", "nl-NL", "pl-PL", "pt-BR", "pt-PT", "ru-RU", "sv-SE", "zh-CN", "zh-HK", "zh-TW", "th-TH"
        };

        // List of text languages are copied from https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support [7 dec 2018]
        public static readonly string[] TextLangAll =
        {
            "af", "ar", "bn", "bs", "bg", "yue", "ca", "zh-Hans", "zh-Hant", "hr", "cs", "da", "nl", "en", "et", "fj", "fil", "fi", "fr", "de", "el",
            "ht", "he", "hi", "mww", "hu", "id", "it", "ja", "sw", "tlh", "ko", "lv", "lt", "mg", "ms", "mt", "nb", "fa", "pl", "pt", "otq",
            "ro", "ru", "sm", "sr-Cyrl", "sr-Latn", "sk", "sl", "es", "sv", "ty", "ta", "th", "to", "tr", "uk", "ur", "vi", "cy", "yua"
        };

        public static IEnumerable<object[]> LocaleAndLang
        {
            get
            {
                foreach (var locale in Speech2TextAll)
                {
                    foreach (var lang in TextLangAll)
                    {
                        yield return new object[] { locale, lang };
                    }
                }
            }
        }
    }
}
