//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Collections.Generic;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils
{
    sealed class Voice
    {
        // List of voice names are copied from https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support#standard-voices [17 Aug 2020]
        // NOTE: Since we are using a North Europe subscription, neural TTS voices are not supported and so are not listed here
        public static readonly IReadOnlyDictionary<string, string[]> All = new Dictionary<string, string[]>(StringComparer.OrdinalIgnoreCase)
        {
            ["ar-EG"] = new string[] { "ar-EG-Hoda" },
            ["ar-SA"] = new string[] { "ar-SA-Naayf" },
            ["bg-BG"] = new string[] { "bg-BG-Ivan" },
            ["ca-ES"] = new string[] { "ca-ES-HerenaRUS" },
            ["cs-CZ"] = new string[] { "cs-CZ-Jakub" },
            ["da-DK"] = new string[] { "da-DK-HelleRUS" },
            ["de-AT"] = new string[] { "de-AT-Michael" },
            ["de-CH"] = new string[] { "de-CH-Karsten" },
            ["de-DE"] = new string[] { "de-DE-Hedda", "de-DE-HeddaRUS", "de-DE-Stefan-Apollo" },
            ["el-GR"] = new string[] { "el-GR-Stefanos" },
            ["en-AU"] = new string[] { "en-AU-Catherine", "en-AU-HayleyRUS" },
            ["en-CA"] = new string[] { "en-CA-Linda", "en-CA-HeatherRUS" },
            ["en-GB"] = new string[] { "en-GB-Susan-Apollo", "en-GB-HazelRUS", "en-GB-George-Apollo" },
            ["en-IE"] = new string[] { "en-IE-Sean" },
            ["en-IN"] = new string[] { "en-IN-Heera-Apollo", "en-IN-PriyaRUS", "en-IN-Ravi-Apollo" },
            ["en-US"] = new string[] { "en-US-ZiraRUS", "en-US-AriaRUS", "en-US-BenjaminRUS", "en-US-Guy24kRUS" },
            ["es-ES"] = new string[] { "es-ES-Laura-Apollo", "es-ES-HelenaRUS", "es-ES-Pablo-Apollo" },
            ["es-MX"] = new string[] { "es-MX-HildaRUS", "es-MX-Raul-Apollo" },
            ["fi-FI"] = new string[] { "fi-FI-HeidiRUS" },
            ["fr-CA"] = new string[] { "fr-CA-Caroline", "fr-CA-HarmonieRUS" },
            ["fr-CH"] = new string[] { "fr-CH-Guillaume" },
            ["fr-FR"] = new string[] { "fr-FR-Julie-Apollo", "fr-FR-HortenseRUS", "fr-FR-Paul-Apollo" },
            ["he-IL"] = new string[] { "he-IL-Asaf" },
            ["hi-IN"] = new string[] { "hi-IN-Kalpana-Apollo", "hi-IN-Kalpana", "hi-IN-Hemant" },
            ["hr-HR"] = new string[] { "hr-HR-Matej" },
            ["hu-HU"] = new string[] { "hu-HU-Szabolcs" },
            ["id-ID"] = new string[] { "id-ID-Andika" },
            ["it-IT"] = new string[] { "it-IT-Cosimo-Apollo", "it-IT-LuciaRUS" },
            ["ja-JP"] = new string[] { "ja-JP-Ayumi-Apollo", "ja-JP-Ichiro-Apollo", "ja-JP-HarukaRUS" },
            ["ko-KR"] = new string[] { "ko-KR-HeamiRUS" },
            ["ms-MY"] = new string[] { "ms-MY-Rizwan" },
            ["nb-NO"] = new string[] { "nb-NO-HuldaRUS" },
            ["nl-NL"] = new string[] { "nl-NL-HannaRUS" },
            ["pl-PL"] = new string[] { "pl-PL-PaulinaRUS" },
            ["pt-BR"] = new string[] { "pt-BR-HeloisaRUS", "pt-BR-Daniel-Apollo" },
            ["pt-PT"] = new string[] { "pt-PT-HeliaRUS" },
            ["ro-RO"] = new string[] { "ro-RO-Andrei" },
            ["ru-RU"] = new string[] { "ru-RU-Irina-Apollo", "ru-RU-Pavel-Apollo", "ru-RU-EkaterinaRUS" },
            ["sk-SK"] = new string[] { "sk-SK-Filip" },
            ["sl-SI"] = new string[] { "sl-SI-Lado" },
            ["sv-SE"] = new string[] { "sv-SE-HedvigRUS" },
            ["ta-IN"] = new string[] { "ta-IN-Valluvar" },
            ["te-IN"] = new string[] { "te-IN-Chitra" },
            ["th-TH"] = new string[] { "th-TH-Pattara" },
            ["tr-TR"] = new string[] { "tr-TR-SedaRUS" },
            ["vi-VN"] = new string[] { "vi-VN-An" },
            ["zh-CN"] = new string[] { "zh-CN-HuihuiRUS", "zh-CN-Yaoyao-Apollo", "zh-CN-Kangkang-Apollo" },
            ["zh-HK"] = new string[] { "zh-HK-Tracy-Apollo", "zh-HK-TracyRUS", "zh-HK-Danny-Apollo" },
            ["zh-TW"] = new string[] { "zh-TW-Yating-Apollo", "zh-TW-HanHanRUS", "zh-TW-Zhiwei-Apollo" }
        };

        public static readonly string FR = All["fr-CA"][0];
        public static readonly string DE = All["de-DE"][1];

        public static IEnumerable<object[]> LangAndSynthesis
        {
            get
            {
                foreach (var lang_voices in All)
                {
                    foreach (var voice in lang_voices.Value)
                    {
                        if (!UNSUPPORTED_TTS_VOICE_CODES.Contains(voice))
                        {
                            yield return new object[] { lang_voices.Key, voice };
                        }
                    }
                }
            }
        }

        /// <summary>
        /// These are codes that are listed on the web page above as supported but don't work. For example we get
        /// a canceled event with the following error details: "Synthesis service failed with code:  - Could not
        /// identify the voice 'en-US-AriaRUS'"
        /// </summary>
        private static readonly ISet<string> UNSUPPORTED_TTS_VOICE_CODES = new HashSet<string>(StringComparer.OrdinalIgnoreCase)
        {
            "en-US-AriaRUS",
            "en-US-Guy24kRUS",
            "te-IN-Chitra"
        };
    }
}
