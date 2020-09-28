//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Linq;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static SPXTEST;
    using static Config;

    [TestClass]
    public class LanguageConfigTests : RecognitionTestBase
    {
        private SpeechRecognitionTestsHelper helper;
        private AudioConfig audioInput;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            LoggingTestBaseInit(context);
            BaseClassInit(context);
        }

        [ClassCleanup]
        new public static void TestClassCleanup()
        {
            LoggingTestBaseCleanup();
        }

        [TestInitialize]
        public void Initialize()
        {
            helper = new SpeechRecognitionTestsHelper();
            audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
        }

        [RetryTestMethod]
        public void TestSourceLanguageConfig()
        {
            SPXTEST_THROWS<ArgumentNullException>(() =>
            {
                SourceLanguageConfig.FromLanguage(null);
            });

            SPXTEST_THROWS<ArgumentNullException>(() =>
            {
                SourceLanguageConfig.FromLanguage("");
            });

            SPXTEST_THROWS<ArgumentNullException>(() =>
            {
                SourceLanguageConfig.FromLanguage(Language.DE_DE, null);
            });

            SPXTEST_THROWS<ArgumentNullException>(() =>
            {
                SourceLanguageConfig.FromLanguage(Language.CA_ES, "");
            });

            SPXTEST_ISNOTNULL(SourceLanguageConfig.FromLanguage(Language.DE_DE));
            SPXTEST_ISNOTNULL(SourceLanguageConfig.FromLanguage(Language.DE_DE, "CustomEndpointId"));
        }

        [RetryTestMethod]
        public void TestAutoDetectSourceLanguageConfig()
        {
            SPXTEST_THROWS<ArgumentNullException>(() =>
            {
                AutoDetectSourceLanguageConfig.FromLanguages(null);
            });

            SPXTEST_THROWS<ArgumentNullException>(() =>
            {
                AutoDetectSourceLanguageConfig.FromLanguages(new string[] { });
            });


            SPXTEST_THROWS<ArgumentNullException>(() =>
            {
                AutoDetectSourceLanguageConfig.FromLanguages(new string[] { null });
            });

            SPXTEST_THROWS<ArgumentNullException>(() =>
            {
                AutoDetectSourceLanguageConfig.FromSourceLanguageConfigs(null);
            });

            SPXTEST_THROWS<ArgumentNullException>(() =>
            {
                AutoDetectSourceLanguageConfig.FromSourceLanguageConfigs(new SourceLanguageConfig[] { });
            });


            SPXTEST_THROWS<ArgumentNullException>(() =>
            {
                AutoDetectSourceLanguageConfig.FromSourceLanguageConfigs(new SourceLanguageConfig[] { null });
            });

            SPXTEST_ISNOTNULL(AutoDetectSourceLanguageConfig.FromLanguages(new string[] { Language.DE_DE, Language.CA_ES }));
            SPXTEST_ISNOTNULL(AutoDetectSourceLanguageConfig.FromSourceLanguageConfigs(
                new SourceLanguageConfig[] {
                    SourceLanguageConfig.FromLanguage(Language.EN),
                    SourceLanguageConfig.FromLanguage(Language.DE_DE, "custom endpointId") }));

        }

        [RetryTestMethod]
        public async Task TestCreateRecognizerWithLanguageConfig()
        {
            using (var speechRecognizer = new SpeechRecognizer(this.defaultConfig, Language.DE_DE, audioInput))
            {
                Assert.IsInstanceOfType(speechRecognizer, typeof(SpeechRecognizer));
                SPXTEST_ARE_EQUAL(Language.DE_DE, speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
                await helper.CompleteRecognizeOnceAsync(speechRecognizer).ConfigureAwait(false);
                var connectionUrl = speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                SPXTEST_ISTRUE(connectionUrl.Contains("language=" + Language.DE_DE), "specified language (should be de-DE) in " + connectionUrl);
            }
        }

        [RetryTestMethod]
        public async Task TestCreateRecognizerWithSourceLanguageConfig()
        {
            var sourceLanguageConfig = SourceLanguageConfig.FromLanguage(Language.DE_DE);
            using (var speechRecognizer = new SpeechRecognizer(this.defaultConfig, sourceLanguageConfig, audioInput))
            {
                Assert.IsInstanceOfType(speechRecognizer, typeof(SpeechRecognizer));
                SPXTEST_ARE_EQUAL(Language.DE_DE, speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
                await helper.CompleteRecognizeOnceAsync(speechRecognizer).ConfigureAwait(false);
                var connectionUrl = speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                SPXTEST_ISTRUE(connectionUrl.Contains("language=" + Language.DE_DE), "specified language (should be de-DE) in " + connectionUrl);
            }
        }

        [RetryTestMethod]
        public async Task TestCreateRecognizerWithSourceLanguageConfigAndEndpointId()
        {
            var sourceLanguageConfig = SourceLanguageConfig.FromLanguage(Language.DE_DE, "endpoint1");
            using (var speechRecognizer = new SpeechRecognizer(this.defaultConfig, sourceLanguageConfig, audioInput))
            {
                Assert.IsInstanceOfType(speechRecognizer, typeof(SpeechRecognizer));
                SPXTEST_ARE_EQUAL(Language.DE_DE, speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
                SPXTEST_ARE_EQUAL("endpoint1", speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_EndpointId));
                await helper.CompleteRecognizeOnceAsync(speechRecognizer).ConfigureAwait(false);
                var connectionUrl = speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                SPXTEST_ISTRUE(connectionUrl.Contains("language=" + Language.DE_DE), "specified language (should be de-DE) in " + connectionUrl);
                SPXTEST_ISTRUE(connectionUrl.Contains("cid=endpoint1"), "specified cid (should be endpoint1) in " + connectionUrl);
            }
        }

        [RetryTestMethod]
        public void TestCreateRecognizerWithAutoDetectSourceLanguageConfig()
        {
            var autoDetectSourceLanguages = new string[] { Language.DE_DE, Language.CA_ES };
            var autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromLanguages(autoDetectSourceLanguages);
            using (var speechRecognizer = new SpeechRecognizer(this.defaultConfig, autoDetectSourceLanguageConfig, audioInput))
            {
                Assert.IsInstanceOfType(speechRecognizer, typeof(SpeechRecognizer));
                var languages = speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_AutoDetectSourceLanguages);
                SPXTEST_ISNOTNULL(languages);
                SPXTEST_ISTRUE(Enumerable.SequenceEqual(autoDetectSourceLanguages.OrderBy(t => t), languages.Split(',').OrderBy(t => t)));
                SPXTEST_ARE_EQUAL("", speechRecognizer.Properties.GetProperty(Language.DE_DE + "SPEECH-ModelId"));
                SPXTEST_ARE_EQUAL("", speechRecognizer.Properties.GetProperty(Language.CA_ES + "SPEECH-ModelId"));
            }

            var sourceLanguageConfigs = new SourceLanguageConfig[]
            {
                SourceLanguageConfig.FromLanguage(Language.CA_ES),
                SourceLanguageConfig.FromLanguage(Language.DE_DE, "customendpoint1")
            };

            autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromSourceLanguageConfigs(sourceLanguageConfigs);
            using (var speechRecognizer = new SpeechRecognizer(this.defaultConfig, autoDetectSourceLanguageConfig, audioInput))
            {
                Assert.IsInstanceOfType(speechRecognizer, typeof(SpeechRecognizer));
                var languages = speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_AutoDetectSourceLanguages);
                SPXTEST_ISNOTNULL(languages);
                SPXTEST_ISTRUE(Enumerable.SequenceEqual(autoDetectSourceLanguages.OrderBy(t => t), languages.Split(',').OrderBy(t => t)));
                SPXTEST_ARE_EQUAL("customendpoint1", speechRecognizer.Properties.GetProperty(Language.DE_DE + "SPEECH-ModelId"));
                SPXTEST_ARE_EQUAL("", speechRecognizer.Properties.GetProperty(Language.CA_ES + "SPEECH-ModelId"));
            }

            this.defaultConfig.EndpointId = "CustomEndpointId2";
            var foundException = false;
            try
            {
                var speechRecognizer = new SpeechRecognizer(this.defaultConfig, AutoDetectSourceLanguageConfig.FromSourceLanguageConfigs(sourceLanguageConfigs), audioInput);
            }
            catch (Exception ex)
            {
                foundException = true;
                SPXTEST_ISTRUE(ex is ApplicationException);
                SPXTEST_ISTRUE(ex.Message.Contains("Invalid argument exception: EndpointId on SpeechConfig is unsupported for auto detection source language scenario. Please set per language endpointId through SourceLanguageConfig and use it to construct AutoDetectSourceLanguageConfig."));
            }
            SPXTEST_ISTRUE(foundException);
        }
    }
}
