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
using System.Security.Principal;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    internal class Config
    {
        private TestContext _testContext { get; }

        private static string _inRoomAudioEndPoint;
        public static string InRoomAudioEndPoint => _inRoomAudioEndPoint;

        private static string _onlineAudioEndPoint;
        public static string OnlineAudioEndPoint => _onlineAudioEndPoint;

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

        public Config(TestContext testContext)
        {
            _testContext = testContext;

            InitializeFromJson();
            InitializeFromRunsettings();
            TestData.AudioDir = Path.Combine(InputDir, "audio");
            TestData.KwsDir = Path.Combine(InputDir, "kws");
            Console.WriteLine("region: " + Region);
            Console.WriteLine("input directory: " + InputDir);
        }

        private void ResolveConfigValue(object input, ref string configParamToUpdate, string configParamName)
        {
            string value = input as string;

            if (!string.IsNullOrWhiteSpace(value))
            {
                configParamToUpdate = value;
            }

            // Disabled for now, until all conflicting names are reconciled
            //if(string.IsNullOrWhiteSpace(configParamToUpdate))
            //{
            //    throw new InvalidOperationException(configParamName + " was null/empty");
            //}
        }

        private void InitializeFromRunsettings()
        {
            ResolveConfigValue(_testContext.Properties[SettingNames.InputDir], ref _inputDir, nameof(InputDir));
            ResolveConfigValue(_testContext.Properties[SettingNames.Region], ref _region, nameof(Region));
            ResolveConfigValue(_testContext.Properties[SettingNames.DeploymentId], ref _deploymentId, nameof(DeploymentId));
            ResolveConfigValue(_testContext.Properties[SettingNames.ConversationTranscriptionEndpoint], ref _conversationTranscriptionEndpoint, nameof(ConversationTranscriptionEndpoint));
            ResolveConfigValue(_testContext.Properties[SettingNames.ConversationTranscriptionPPEKey], ref _conversationTranscriptionPPEKey, nameof(ConversationTranscriptionPPEKey));
            ResolveConfigValue(_testContext.Properties[SettingNames.ConversationTranscriptionPRODKey], ref _conversationTranscriptionPRODKey, nameof(ConversationTranscriptionPRODKey));
            ResolveConfigValue(_testContext.Properties[SettingNames.UnifiedSpeechSubscriptionKey], ref _unifiedSpeechSubscriptionKey, nameof(UnifiedSpeechSubscriptionKey));
            ResolveConfigValue(_testContext.Properties[SettingNames.LanguageUnderstandingSubscriptionKey], ref _languageUnderstandingSubscriptionKey, nameof(LanguageUnderstandingSubscriptionKey));
            ResolveConfigValue(_testContext.Properties[SettingNames.LanguageUnderstandingHomeAutomationAppId], ref _languageUnderstandingHomeAutomationAppId, nameof(LanguageUnderstandingHomeAutomationAppId));
            ResolveConfigValue(_testContext.Properties[SettingNames.LanguageUnderstandingServiceRegion], ref _languageUnderstandingServiceRegion, nameof(LanguageUnderstandingServiceRegion));
            ResolveConfigValue(_testContext.Properties[SettingNames.DialogSubscriptionKey], ref _dialogSubscriptionKey, nameof(DialogSubscriptionKey));
            ResolveConfigValue(_testContext.Properties[SettingNames.DialogFunctionalTestBot], ref _dialogFunctionalTestBot, nameof(DialogFunctionalTestBot));
            ResolveConfigValue(_testContext.Properties[SettingNames.DialogRegion], ref _dialogRegion, nameof(DialogRegion));
            ResolveConfigValue(_testContext.Properties[SettingNames.SpeechRegionForConversationTranscription], ref _speechRegionForConversationTranscription, nameof(SpeechRegionForConversationTranscription));
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
                    _inRoomAudioEndPoint = configSettings[SettingNames.InRoomAudioEndPoint];
                    _onlineAudioEndPoint = configSettings[SettingNames.OnlineAudioEndPoint];
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
                }
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
