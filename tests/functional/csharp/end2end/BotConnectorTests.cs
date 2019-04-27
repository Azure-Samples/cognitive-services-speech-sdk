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
using Microsoft.CognitiveServices.Speech.Dialog;

using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static AssertHelpers;
    using static BotConnectorTestsHelper;

    [TestClass]
    public class BotConnectorTests : BotConnectorTestBase
    {
        private BotConnectorTestsHelper helper;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInitialize(context);
        }

        [TestInitialize]
        public void Initialize()
        {
            helper = new BotConnectorTestsHelper();
        }

        [Ignore] //Temporarily removing during stand-up, tracked by task 1755114
        [DataTestMethod]
        public async Task ValidBaselineBotListenOnce()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var botConnector = TrackSessionId(new SpeechBotConnector(this.botConnectorConfig, audioInput)))
            {
                await helper.CompleteListenOnceAsync(botConnector).ConfigureAwait(false);
            }
        }

        [Ignore] // Disabling temporarily due to intermitent crash, bug 1758969
        [DataTestMethod]
        public async Task ValidSendActivity()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var botConnector = TrackSessionId(new SpeechBotConnector(this.botConnectorConfig, audioInput)))
            {
                await helper.CompleteSendActivity(botConnector).ConfigureAwait(false);
            }
        }


    }
}
