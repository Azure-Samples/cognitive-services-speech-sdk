//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Http;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    internal class Config
    {
        private TestContext _testContext { get; }

        private static string _inRoomAudioEndpoint;
        public static string InRoomAudioEndpoint => _inRoomAudioEndpoint;

        private static string _onlineAudioEndpoint;
        public static string OnlineAudioEndpoint => _onlineAudioEndpoint;

        private static string _inputDir;
        public static string InputDir => _inputDir;

        private static string _region;
        public static string Region => _region;

        private static string _deploymentId;
        public static string DeploymentId => _deploymentId;

        private static string _conversationTranscriptionSubscriptionKey;
        public static string ConversationTranscriptionSubscriptionKey => _conversationTranscriptionSubscriptionKey;

        private static string _speechRegionForConversationTranscription;
        public static string SpeechRegionForConversationTranscription => _speechRegionForConversationTranscription;

        private static string _conversationTranscriptionEndpoint;
        public static string ConversationTranscriptionEndpoint => _conversationTranscriptionEndpoint;

        private static string _conversationTranscriptionPPEKey;
        public static string ConversationTranscriptionPPEKey => _conversationTranscriptionPPEKey;

        private static string _conversationTranscriptionPRODKey;
        public static string ConversationTranscriptionPRODKey => _conversationTranscriptionPRODKey;

        private static string _unifiedSpeechSubscriptionKey;
        public static string UnifiedSpeechSubscriptionKey => _unifiedSpeechSubscriptionKey;

        private static string _languageUnderstandingSubscriptionKey;
        public static string LanguageUnderstandingSubscriptionKey => _languageUnderstandingSubscriptionKey;

        private static string _languageUnderstandingHomeAutomationAppId;
        public static string LanguageUnderstandingHomeAutomationAppId => _languageUnderstandingHomeAutomationAppId;

        private static string _languageUnderstandingServiceRegion;
        public static string LanguageUnderstandingServiceRegion => _languageUnderstandingServiceRegion;

        private static string _dialogSubscriptionKey;
        public static string DialogSubscriptionKey => _dialogSubscriptionKey;

        private static string _dialogFunctionalTestBot;
        public static string DialogFunctionalTestBot => _dialogFunctionalTestBot;

        private static string _dialogRegion;
        public static string DialogRegion => _dialogRegion;

        public static string Endpoint { get; private set; }
        public static string ConversationTranslatorSubscriptionKey { get; private set; }
        public static string ConversationTranslatorRegion { get; private set; }
        public static string ConversationTranslatorHost { get; private set; }
        public static string ConversationTranslatorSpeechEndpoint { get; private set; }
        public static string ConversationTranslatorClientId { get; private set; }

        public Config(TestContext testContext)
        {
            _testContext = testContext;

            InitializeFromJson();
            TestData.AudioDir = Path.Combine(InputDir, "audio");
            TestData.KwsDir = Path.Combine(InputDir, "kws");
            Console.WriteLine("region: " + Region);
            Console.WriteLine("input directory: " + InputDir);
        }

        private static void InitializeFromJson()
        {
            Dictionary<string, string> configSettings;

            if (File.Exists("./test.settings.json"))
            {
                using (FileStream fileStream = new FileStream("./test.settings.json", FileMode.Open, FileAccess.Read))
                {
                    using (StreamReader streamReader = new StreamReader(fileStream))
                    {
                        using (JsonTextReader jsonTextReader = new JsonTextReader(streamReader))
                        {
                            JsonSerializer jsonSerializer = new JsonSerializer();
                            configSettings = jsonSerializer.Deserialize<Dictionary<string, string>>(jsonTextReader);
                        }
                    }
                }

                if (configSettings != null)
                {
                    _inRoomAudioEndpoint = configSettings[SettingNames.InRoomAudioEndpoint];
                    _onlineAudioEndpoint = configSettings[SettingNames.OnlineAudioEndpoint];
                    _inputDir = configSettings[SettingNames.InputDir];
                    _region = configSettings[SettingNames.Region];
                    _deploymentId = configSettings[SettingNames.DeploymentId];

                    _conversationTranscriptionSubscriptionKey = configSettings[SettingNames.ConversationTranscriptionSubscriptionKey];
                    _speechRegionForConversationTranscription = configSettings[SettingNames.SpeechRegionForConversationTranscription];
                    _conversationTranscriptionEndpoint = configSettings[SettingNames.ConversationTranscriptionEndpoint];
                    _conversationTranscriptionPPEKey = configSettings[SettingNames.ConversationTranscriptionPPEKey];
                    _conversationTranscriptionPRODKey = configSettings[SettingNames.ConversationTranscriptionPRODKey];

                    _unifiedSpeechSubscriptionKey = configSettings[SettingNames.UnifiedSpeechSubscriptionKey];

                    _languageUnderstandingSubscriptionKey = configSettings[SettingNames.LanguageUnderstandingSubscriptionKey];
                    _languageUnderstandingHomeAutomationAppId = configSettings[SettingNames.LanguageUnderstandingHomeAutomationAppId];
                    _languageUnderstandingServiceRegion = configSettings[SettingNames.LanguageUnderstandingServiceRegion];

                    _dialogSubscriptionKey = configSettings[SettingNames.DialogSubscriptionKey];
                    _dialogFunctionalTestBot = configSettings[SettingNames.DialogFunctionalTestBot];
                    _dialogRegion = configSettings[SettingNames.DialogRegion];

                    Endpoint = configSettings[SettingNames.Endpoint];
                    ConversationTranslatorSubscriptionKey = configSettings[nameof(ConversationTranslatorSubscriptionKey)];
                    ConversationTranslatorRegion = configSettings[nameof(ConversationTranslatorRegion)];
                    ConversationTranslatorHost = configSettings[nameof(ConversationTranslatorHost)];
                    ConversationTranslatorSpeechEndpoint = configSettings[nameof(ConversationTranslatorSpeechEndpoint)];
                    ConversationTranslatorClientId = configSettings[nameof(ConversationTranslatorClientId)];
                }
            }
            else
            {
                Console.WriteLine($"test.settings.json file not found searched: {InputDir}");
                Console.WriteLine($"Current working directory: {Directory.GetCurrentDirectory()}");
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
