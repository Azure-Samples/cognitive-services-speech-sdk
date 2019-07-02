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
    using static DialogServiceConnectorTestsHelper;

    [TestClass]
    public class DialogServiceConnectorTests : DialogServiceConnectorTestBase
    {
        private DialogServiceConnectorTestsHelper helper;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInitialize(context);
        }

        [TestInitialize]
        public void Initialize()
        {
            helper = new DialogServiceConnectorTestsHelper();
        }

        [DataTestMethod]
        public async Task ValidBaselineListenOnce()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var dialogServiceConnector = TrackSessionId(new DialogServiceConnector(this.dialogServiceConfig, audioInput)))
            {
                await helper.CompleteListenOnceAsync(dialogServiceConnector).ConfigureAwait(false);
            }
        }

        [DataTestMethod]
        public async Task ValidSendActivity()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var dialogServiceConnector = TrackSessionId(new DialogServiceConnector(this.dialogServiceConfig, audioInput)))
            {
                await helper.CompleteSendActivity(dialogServiceConnector).ConfigureAwait(false);
            }
        }


    }
}
