//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static Config;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public class LanguageConfigTests : RecognitionTestBase
    {
        private SpeechRecognitionTestsHelper helper;
        private AudioConfig audioInput;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInit(context);
        }

        [TestInitialize]
        public void Initialize()
        {
            helper = new SpeechRecognitionTestsHelper();
            audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
        }

        [TestMethod]
        public void TestSourceLanguageConfig()
        {
            Assert.ThrowsException<ArgumentNullException>(() =>
            {
                SourceLanguageConfig.FromLanguage(null);
            });

            Assert.ThrowsException<ArgumentNullException>(() =>
            {
                SourceLanguageConfig.FromLanguage("");
            });

            Assert.ThrowsException<ArgumentNullException>(() =>
            {
                SourceLanguageConfig.FromLanguage(Language.DE_DE, null);
            });

            Assert.ThrowsException<ArgumentNullException>(() =>
            {
                SourceLanguageConfig.FromLanguage(Language.CA_ES, "");
            });

            Assert.IsNotNull(SourceLanguageConfig.FromLanguage(Language.DE_DE));
            Assert.IsNotNull(SourceLanguageConfig.FromLanguage(Language.DE_DE, "CustomEndpointId"));
        }

        [TestMethod]
        public void TestAutoDetectSourceLanguageConfig()
        {
            Assert.ThrowsException<ArgumentNullException>(() =>
            {
                AutoDetectSourceLanguageConfig.FromLanguages(null);
            });

            Assert.ThrowsException<ArgumentNullException>(() =>
            {
                AutoDetectSourceLanguageConfig.FromLanguages(new string[] { });
            });


            Assert.ThrowsException<ArgumentNullException>(() =>
            {
                AutoDetectSourceLanguageConfig.FromLanguages(new string[] { null });
            });

            Assert.ThrowsException<ArgumentNullException>(() =>
            {
                AutoDetectSourceLanguageConfig.FromSourceLanguageConfigs(null);
            });

            Assert.ThrowsException<ArgumentNullException>(() =>
            {
                AutoDetectSourceLanguageConfig.FromSourceLanguageConfigs(new SourceLanguageConfig[] { });
            });


            Assert.ThrowsException<ArgumentNullException>(() =>
            {
                AutoDetectSourceLanguageConfig.FromSourceLanguageConfigs(new SourceLanguageConfig[] { null });
            });

            Assert.IsNotNull(AutoDetectSourceLanguageConfig.FromLanguages(new string[] { Language.DE_DE, Language.CA_ES }));
            Assert.IsNotNull(AutoDetectSourceLanguageConfig.FromSourceLanguageConfigs(
                new SourceLanguageConfig[] {
                    SourceLanguageConfig.FromLanguage(Language.EN),
                    SourceLanguageConfig.FromLanguage(Language.DE_DE, "custom endpointId") }));

        }

        [TestMethod]
        public async Task TestCreateRecognizerWithLanguageConfig()
        {
            using (var speechRecognizer = new SpeechRecognizer(this.defaultConfig, Language.DE_DE, audioInput))
            {
                Assert.IsInstanceOfType(speechRecognizer, typeof(SpeechRecognizer));
                Assert.AreEqual(Language.DE_DE, speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
                await helper.CompleteRecognizeOnceAsync(speechRecognizer).ConfigureAwait(false);
                var connectionUrl = speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsTrue(connectionUrl.Contains("language=" + Language.DE_DE), "specified language (should be de-DE) in " + connectionUrl);
            }
        }

        [TestMethod]
        public async Task TestCreateRecognizerWithSourceLanguageConfig()
        {
            var sourceLanguageConfig = SourceLanguageConfig.FromLanguage(Language.DE_DE);
            using (var speechRecognizer = new SpeechRecognizer(this.defaultConfig, sourceLanguageConfig, audioInput))
            {
                Assert.IsInstanceOfType(speechRecognizer, typeof(SpeechRecognizer));
                Assert.AreEqual(Language.DE_DE, speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
                await helper.CompleteRecognizeOnceAsync(speechRecognizer).ConfigureAwait(false);
                var connectionUrl = speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsTrue(connectionUrl.Contains("language=" + Language.DE_DE), "specified language (should be de-DE) in " + connectionUrl);
            }
        }

        [TestMethod]
        public async Task TestCreateRecognizerWithSourceLanguageConfigAndEndpointId()
        {
            var sourceLanguageConfig = SourceLanguageConfig.FromLanguage(Language.DE_DE, "endpoint1");
            using (var speechRecognizer = new SpeechRecognizer(this.defaultConfig, sourceLanguageConfig, audioInput))
            {
                Assert.IsInstanceOfType(speechRecognizer, typeof(SpeechRecognizer));
                Assert.AreEqual(Language.DE_DE, speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
                Assert.AreEqual("endpoint1", speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_EndpointId));
                await helper.CompleteRecognizeOnceAsync(speechRecognizer).ConfigureAwait(false);
                var connectionUrl = speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsTrue(connectionUrl.Contains("language=" + Language.DE_DE), "specified language (should be de-DE) in " + connectionUrl);
                Assert.IsTrue(connectionUrl.Contains("cid=endpoint1"), "specified cid (should be endpoint1) in " + connectionUrl);
            }
        }

        [TestMethod]
        public void TestCreateRecognizerWithAutoDetectSourceLanguageConfig()
        {
            var autoDetectSourceLanguages = new string[] { Language.DE_DE, Language.CA_ES };
            var autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromLanguages(autoDetectSourceLanguages);
            using (var speechRecognizer = new SpeechRecognizer(this.defaultConfig, autoDetectSourceLanguageConfig, audioInput))
            {
                Assert.IsInstanceOfType(speechRecognizer, typeof(SpeechRecognizer));
                var languages = speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_AutoDetectSourceLanguages);
                Assert.IsNotNull(languages);
                Assert.IsTrue(Enumerable.SequenceEqual(autoDetectSourceLanguages.OrderBy(t => t), languages.Split(',').OrderBy(t => t)));
                Assert.AreEqual("", speechRecognizer.Properties.GetProperty(Language.DE_DE + "SPEECH-ModelId"));
                Assert.AreEqual("", speechRecognizer.Properties.GetProperty(Language.CA_ES + "SPEECH-ModelId"));
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
                Assert.IsNotNull(languages);
                Assert.IsTrue(Enumerable.SequenceEqual(autoDetectSourceLanguages.OrderBy(t => t), languages.Split(',').OrderBy(t => t)));
                Assert.AreEqual("customendpoint1", speechRecognizer.Properties.GetProperty(Language.DE_DE + "SPEECH-ModelId"));
                Assert.AreEqual("", speechRecognizer.Properties.GetProperty(Language.CA_ES + "SPEECH-ModelId"));
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
                Assert.IsTrue(ex is ApplicationException);
                Assert.IsTrue(ex.Message.Contains("Invalid argument exception: EndpointId on SpeechConfig is unsupported for auto detection source language scenario. Please set per language endpointId through SourceLanguageConfig and use it to construct AutoDetectSourceLanguageConfig."));
            }
            Assert.IsTrue(foundException);
        }
    }
}
