//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Dialog;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static DialogServiceConnectorTestsHelper;

    [TestClass]
    public class DialogServiceConnectorTests : DialogServiceConnectorTestBase
    {
        private DialogServiceConnectorTestsHelper helper;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            LoggingTestBaseInit(context);
            BaseClassInitialize(context);
        }

        [ClassCleanup]
        public static void TestClassCleanup()
        {
            LoggingTestBaseCleanup();
        }

        [TestInitialize]
        public void Initialize()
        {
            helper = new DialogServiceConnectorTestsHelper();
        }

        [DataTestMethod]
        public async Task ValidBaselineListenOnce()
        {
            var audioInput = AudioConfig.FromWavFileInput(Config.AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var dialogServiceConnector = TrackSessionId(new DialogServiceConnector(this.dialogServiceConfig, audioInput)))
            {
                await helper.CompleteListenOnceAsync(dialogServiceConnector).ConfigureAwait(false);
            }
        }

        [DataTestMethod]
        public async Task ValidSendActivity()
        {
            var audioInput = AudioConfig.FromWavFileInput(Config.AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var dialogServiceConnector = TrackSessionId(new DialogServiceConnector(this.dialogServiceConfig, audioInput)))
            {
                await helper.CompleteSendActivity(dialogServiceConnector).ConfigureAwait(false);
            }
        }


    }
}
