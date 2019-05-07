//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System.Collections.Generic;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    sealed class Voice
    {
        public static readonly string FR = "Microsoft Server Speech Text to Speech Voice (fr-CA, Caroline)";
        public static readonly string DE = "Microsoft Server Speech Text to Speech Voice (de-DE, HeddaRUS)";

        // List of voice names are copied from https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support [7 Dec 2018]
        public static readonly Dictionary<string, List<string>> All = new Dictionary<string, List<string>>
        {
            ["ar"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (ar-EG, Hoda)", "Microsoft Server Speech Text to Speech Voice (ar-SA, Naayf)" },
            ["bg"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (bg-BG, Ivan)" },
            ["ca"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (ca-ES, HerenaRUS)" },
            ["cs"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (cs-CZ, Jakub)" },
            ["da"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (da-DK, HelleRUS)" },
            ["de"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (de-AT, Michael)", "Microsoft Server Speech Text to Speech Voice (de-CH, Karsten)",
                "Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)", "Microsoft Server Speech Text to Speech Voice (de-DE, HeddaRUS)",
                "Microsoft Server Speech Text to Speech Voice (de-DE, Stefan, Apollo)" },
            ["el"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (el-GR, Stefanos)" },
            ["en"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (en-AU, Catherine)", "Microsoft Server Speech Text to Speech Voice (en-AU, HayleyRUS)",
                "Microsoft Server Speech Text to Speech Voice (en-CA, Linda)", "Microsoft Server Speech Text to Speech Voice (en-CA, HeatherRUS)",
                "Microsoft Server Speech Text to Speech Voice (en-GB, Susan, Apollo)", "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)",
                "Microsoft Server Speech Text to Speech Voice (en-GB, George, Apollo)", "Microsoft Server Speech Text to Speech Voice (en-IE, Sean)",
                "Microsoft Server Speech Text to Speech Voice (en-IN, Heera, Apollo)", "Microsoft Server Speech Text to Speech Voice (en-IN, PriyaRUS)",
                "Microsoft Server Speech Text to Speech Voice (en-IN, Ravi, Apollo)", "Microsoft Server Speech Text to Speech Voice (en-US, ZiraRUS)",
                "Microsoft Server Speech Text to Speech Voice (en-US, JessaRUS)", "Microsoft Server Speech Text to Speech Voice (en-US, BenjaminRUS)",
                "Microsoft Server Speech Text to Speech Voice (en-US, Jessa24kRUS)", "Microsoft Server Speech Text to Speech Voice (en-US, Guy24kRUS)" },
            ["es"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (es-ES, Laura, Apollo)", "Microsoft Server Speech Text to Speech Voice (es-ES, HelenaRUS)",
                "Microsoft Server Speech Text to Speech Voice (es-ES, Pablo, Apollo)", "Microsoft Server Speech Text to Speech Voice (es-MX, HildaRUS)",
                "Microsoft Server Speech Text to Speech Voice (es-MX, Raul, Apollo)" },
            ["fi"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (fi-FI, HeidiRUS)" },
            ["fr"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (fr-CA, Caroline)", "Microsoft Server Speech Text to Speech Voice (fr-CA, HarmonieRUS)",
                "Microsoft Server Speech Text to Speech Voice (fr-CH, Guillaume)", "Microsoft Server Speech Text to Speech Voice (fr-FR, Julie, Apollo)",
                "Microsoft Server Speech Text to Speech Voice (fr-FR, HortenseRUS)", "Microsoft Server Speech Text to Speech Voice (fr-FR, Paul, Apollo)" },
            ["he"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (he-IL, Asaf)" },
            ["hi"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (hi-IN, Kalpana, Apollo)", "Microsoft Server Speech Text to Speech Voice (hi-IN, Kalpana)",
                "Microsoft Server Speech Text to Speech Voice (hi-IN, Hemant)" },
            ["hr"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (hr-HR, Matej)" },
            ["hu"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (hu-HU, Szabolcs)" },
            ["id"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (id-ID, Andika)" },
            ["it"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (it-IT, Cosimo, Apollo)", "Microsoft Server Speech Text to Speech Voice (it-IT, LuciaRUS)" },
            ["ja"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (ja-JP, Ayumi, Apollo)", "Microsoft Server Speech Text to Speech Voice (ja-JP, Ichiro, Apollo)",
                "Microsoft Server Speech Text to Speech Voice (ja-JP, HarukaRUS)" },
            ["ko"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (ko-KR, HeamiRUS)" },
            ["ms"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (ms-MY, Rizwan)" },
            ["nb"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (nb-NO, HuldaRUS)" },
            ["nl"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (nl-NL, HannaRUS)" },
            ["pl"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (pl-PL, PaulinaRUS)" },
            ["pt"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (pt-BR, HeloisaRUS)", "Microsoft Server Speech Text to Speech Voice (pt-BR, Daniel, Apollo)",
                "Microsoft Server Speech Text to Speech Voice (pt-PT, HeliaRUS)" },
            ["ro"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (ro-RO, Andrei)" },
            ["ru"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (ru-RU, Irina, Apollo)", "Microsoft Server Speech Text to Speech Voice (ru-RU, Pavel, Apollo)",
                "Microsoft Server Speech Text to Speech Voice (ru-RU, EkaterinaRUS)" },
            ["sk"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (sk-SK, Filip)" },
            ["sl"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (sl-SI, Lado)" },
            ["sv"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (sv-SE, HedvigRUS)" },
            ["ta"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (ta-IN, Valluvar)" },
            ["te"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (te-IN, Chitra)" },
            ["th"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (th-TH, Pattara)" },
            ["tr"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (tr-TR, SedaRUS)" },
            ["vi"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (vi-VN, An)" },
            ["zh"] = new List<string>{ "Microsoft Server Speech Text to Speech Voice (zh-CN, HuihuiRUS)", "Microsoft Server Speech Text to Speech Voice (zh-CN, Yaoyao, Apollo)",
                "Microsoft Server Speech Text to Speech Voice (zh-CN, Kangkang, Apollo)", "Microsoft Server Speech Text to Speech Voice (zh-HK, Tracy, Apollo)",
                "Microsoft Server Speech Text to Speech Voice (zh-HK, TracyRUS)", "Microsoft Server Speech Text to Speech Voice (zh-HK, Danny, Apollo)",
                "Microsoft Server Speech Text to Speech Voice (zh-TW, Yating, Apollo)", "Microsoft Server Speech Text to Speech Voice (zh-TW, HanHanRUS)",
                "Microsoft Server Speech Text to Speech Voice (zh-TW, Zhiwei, Apollo)" }
        };

        public static IEnumerable<object[]> LangAndSynthesis
        {
            get
            {
                foreach (var lang_voices in All)
                {
                    foreach (var voice in lang_voices.Value)
                    {
                        yield return new object[] { lang_voices.Key, voice };
                    }
                }
            }
        }
    }
}
