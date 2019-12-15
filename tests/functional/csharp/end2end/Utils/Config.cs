//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Http;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils
{
    public struct SubscriptionRegion
    {
        public string Key { get; set; }
        public string Region { get; set; }
    }

    public struct Utterance
    {
        public string Text { get; set; }
        public string ProfanityRaw { get; set; }
        public string ProfanityMasked { get; set; }
        public string ProfanityMaskedPattern { get; set; }
        public string ProfanityRemoved { get; set; }
        public string ProfanityTagged { get; set; }
        public int? AudioOffset { get; set; }
        public int? AudioDuration { get; set; }
        public int? TextOffset { get; set; }
        public int? SsmlOffset { get; set; }
    }

    public struct AudioEntry
    {
        public string FilePath { get; set; }
        public string NativeLanguage { get; set; }
        public IDictionary<string, Utterance[]> Utterances { get; set; }
    };

    public struct SubscriptionsRegionsKeys
    {
        public static string UNIFIED_SPEECH_SUBSCRIPTION => "UnifiedSpeechSubscription";
        public static string DIALOG_SUBSCRIPTION => "DialogSubscription";
        public static string LANGUAGE_UNDERSTANDING_SUBSCRIPTION => "LanguageUnderstandingSubscription";
        public static string CONVERSATION_TRANSCRIPTION_PROD_SUBSCRIPTION => "ConversationTranscriptionPRODSubscription";
        public static string CONVERSATION_TRANSCRIPTION_PPE_SUBSCRIPTION => "ConversationTranscriptionPPESubscription";
        public static string CONVERSATION_TRANSLATOR => "ConversationTranslatorSubscription";
    }

    // Default settings keys
    public struct DefaultSettingKeys
    {
        public static string ENDPOINT => "Endpoint";
        public static string DIALOG_FUNCTIONAL_TEST_BOT => "DialogFunctionalTestBot";
        public static string LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID => "LanguageUnderstandingHomeAutomationAppId";
        public static string INROOM_AUDIO_ENDPOINT => "InRoomAudioEndpoint";
        public static string CONVERSATION_TRANSCRIPTION_ENDPOINT => "ConversationTranscriptionEndpoint";
        public static string LONG_RUNNING => "LongRunning";
        public static string DEPLOYMENT_ID => "DeploymentId";
        public static string ONLINE_AUDIO_ENDPOINT => "OnlineAudioEndpoint";
        public static string INPUT_DIR => "InputDir";
        public static string CONVERSATION_TRANSLATOR_HOST => "ConversationTranslatorHost";
        public static string CONVERSATION_TRANSLATOR_SPEECH_ENDPOINT => "ConversationTranslatorSpeechEndpoint";
        public static string CONVERSATION_TRANSLATOR_CLIENTID => "ConversationTranslatorClientId";
    }

    // Audio file and utterances keys
    public struct AudioUtteranceKeys
    {
        public static string SINGLE_UTTERANCE_ENGLISH => "SingleUtteranceEnglish";
        public static string SINGLE_UTTERANCE_CHINESE => "SingleUtteranceChinese";
        public static string SINGLE_UTTERANCE_MP3 => "SingleUtteranceMP3";
        public static string SINGLE_UTTERANCE_OPUS => "SingleUtteranceOPUS";
        public static string SINGLE_UTTERANCE_A_LAW => "SingleUtteranceALaw";
        public static string SINGLE_UTTERANCE_MU_LAW => "SingleUtteranceMULaw";
        public static string SINGLE_UTTERANCE_FLAC => "SingleUtteranceFLAC";
        public static string SINGLE_UTTERANCE_3X => "SingleUtterance3x";
        public static string SINGLE_UTTERANCE_MULTIPLE_TURNS => "SingleUtteranceMultipleTurns";
        public static string SINGLE_UTTERANCE_CATALAN => "SingleUtteranceCatalan";
        public static string MULTIPLE_UTTERANCE_ENGLISH => "MultipleUtteranceEnglish";
        public static string AUDIO_44_1KHZ => "Audio441Khz";
        public static string AUDIO_11_KHZ => "Audio11Khz";
        public static string HEY_CORTANA => "HeyCortana";
        public static string SINGLE_UTTERANCE_GERMAN => "SingleUtteranceGerman";
        public static string INTENT_UTTERANCE => "IntentUtterance";
        public static string AMBIGUOUS_SPEECH => "AmbiguousSpeech";
        public static string COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1 => "ComputerKeywordWithSingleUtterance1"; // Used to be accept
        public static string COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_2 => "ComputerKeywordWithSingleUtterance2";
        public static string COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_2X => "ComputerKeywordWithSingleUtterance2x"; // Used to be acceptx2
        public static string COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_3 => "ComputerKeywordWithSingleUtterance3";
        public static string COMPUTER_KEYWORD_WITH_MULTIPLE_TURNS => "ComputerKeywordWithMultipleTurns";
        public static string SECRET_KEYWORDS => "SecretKeywords";
        public static string CONVERSATION_BETWEEN_TWO_PERSONS_ENGLISH => "ConversationBetweenTwoPersonsEnglish";
        public static string PERSON_ENROLLMENT_ENGLISH_1 => "PersonEnrollmentEnglish1";
        public static string PERSON_ENROLLMENT_ENGLISH_2 => "PersonEnrollmentEnglish2";
        public static string SINGLE_UTTERANCE_WITH_SPECIAL_CHARACTER => "SingleUtteranceWithSpecialCharacter";
        public static string SHORT_SILENCE => "ShortSilence";
        public static string SINGLE_UTTERANCE_WITH_PUNCTUATION => "SingleUtteranceWithPunctuation";
        public static string PROFANTITY_SINGLE_UTTERANCE_ENGLISH_1 => "ProfanitySingleUtteranceEnglish1";
        public static string PROFANITY_SINGLE_UTTERANCE_ENGLISH_2 => "ProfanitySingleUtteranceEnglish2";
    }
    internal class Config
    {
        private static IDictionary<string, string> _defaultSettingsMap;
        private static IDictionary<string, AudioEntry> _audioUtterancesMap;
        private static IDictionary<string, SubscriptionRegion> _subscriptionsRegionsMap;

        public static IDictionary<string, string> DefaultSettingsMap => _defaultSettingsMap;
        public static IDictionary<string, AudioEntry> AudioUtterancesMap => _audioUtterancesMap;
        public static IDictionary<string, SubscriptionRegion> SubscriptionsRegionsMap => _subscriptionsRegionsMap;

        private TestContext _testContext { get; }

        public Config(TestContext testContext)
        {
            _testContext = testContext;

            InitializeFromJson();
        }

        private static void InitializeFromJson()
        {
            Deserialize("test.defaults.json", out _defaultSettingsMap);
            Deserialize("test.audio.utterances.json", out _audioUtterancesMap);
            Deserialize("test.subscriptions.regions.json", out _subscriptionsRegionsMap);
        }

        private static void Deserialize<T>(string filePath, out T data)
        {
            if (File.Exists(filePath))
            {
                using (FileStream fileStream = new FileStream(filePath, FileMode.Open, FileAccess.Read))
                {
                    using (StreamReader streamReader = new StreamReader(fileStream))
                    {
                        using (JsonTextReader jsonTextReader = new JsonTextReader(streamReader))
                        {
                            JsonSerializer jsonSerializer = new JsonSerializer();
                            try
                            {
                                data = jsonSerializer.Deserialize<T>(jsonTextReader);
                            }
                            catch (JsonException jsonException)
                            {
                                Console.WriteLine($"Deserializing test.defaults.json threw an exception {jsonException.Message}");
                                throw;
                            }
                        }
                    }
                }
            }
            else
            {
                Console.WriteLine($"test.defaults.json file not found searched: {DefaultSettingsMap[DefaultSettingKeys.INPUT_DIR]}");
                Console.WriteLine($"Current working directory: {Directory.GetCurrentDirectory()}");
                throw new InvalidOperationException($"json file {filePath} not found!");
            }
        }
        public static List<byte[]> GetByteArraysForFilesWithSamePrefix(string dir, string prefix)
        {
            List<byte[]> byteArrList = new List<byte[]>();
            FileInfo[] fileInfos = new DirectoryInfo(dir).GetFiles(string.Concat(prefix, "*.*"));
            for (int i = 1; i <= fileInfos.Length; i++)
            {
                byteArrList.Add(File.ReadAllBytes(Path.Combine(dir, string.Concat(prefix, i, ".wav"))));
            }
            return byteArrList;
        }

        public static async Task<string> GetToken(string key, string region)
        {
            using (HttpClient client = new HttpClient())
            {
                client.DefaultRequestHeaders.Add("Ocp-Apim-Subscription-Key", key);
                UriBuilder uriBuilder = new UriBuilder($"https://{region}.api.cognitive.microsoft.com/sts/v1.0");
                uriBuilder.Path += "/issueToken";

                using (HttpResponseMessage result = await client.PostAsync(uriBuilder.Uri.AbsoluteUri, null))
                {
                    Console.WriteLine("Token Uri: {0}", uriBuilder.Uri.AbsoluteUri);
                    if (result.IsSuccessStatusCode)
                    {
                        return await result.Content.ReadAsStringAsync();
                    }
                    else
                    {
                        throw new HttpRequestException($"Cannot get token from {uriBuilder.ToString()}. Error: {result.StatusCode}");
                    }
                }
            }
        }
    }
}
