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
    using static DialogConnectorTestsHelper;

    [TestClass]
    public class DialogConnectorTests : DialogConnectorTestBase
    {
        private DialogConnectorTestsHelper helper;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInitialize(context);
        }

        [TestInitialize]
        public void Initialize()
        {
            helper = new DialogConnectorTestsHelper();
        }

        [Ignore] //Temporarily removing during stand-up, tracked by task 1755114
        [DataTestMethod]
        public async Task ValidBaselineListenOnce()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var dialogConnector = TrackSessionId(new DialogConnector(this.dialogConfig, audioInput)))
            {
                await helper.CompleteListenOnceAsync(dialogConnector).ConfigureAwait(false);
            }
        }

        [Ignore] // Disabling temporarily due to intermitent crash, bug 1758969
        [DataTestMethod]
        public async Task ValidSendActivity()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var dialogConnector = TrackSessionId(new DialogConnector(this.dialogConfig, audioInput)))
            {
                await helper.CompleteSendActivity(dialogConnector).ConfigureAwait(false);
            }
        }


    }
}
