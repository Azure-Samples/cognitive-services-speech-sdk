
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static Config;

    [TestClass]
    public class KwsRecognizerTests : RecognitionTestBase
    {

        [TestMethod]
        public async Task TestKeywordFound()
        {
            var tcs = new TaskCompletionSource<bool>();
            var cancellation = new TaskCompletionSource<bool>();

            Console.WriteLine($"Setting keyword file path to {AudioUtterancesMap[AudioUtteranceKeys.SECRET_KEYWORDS].FilePath.GetRootRelativePath()}");
            var str = AudioUtterancesMap[AudioUtteranceKeys.SECRET_KEYWORDS].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = new KeywordRecognizer(audioInput))
            {
                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());
                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                    {
                        if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Secret.ModelKeyword))
                        {
                            Console.WriteLine("Final result EXPECTED: " + e.Result.Text.ToString());
                            tcs.TrySetResult(true);
                            return;
                        }
                    }
                    Console.WriteLine("Received unexpected result.");
                    tcs.TrySetResult(false);
                };
                recognizer.Canceled += (s, e) =>
                {
                    cancellation.TrySetResult(true);
                    Console.WriteLine("Canceled: " + e.ToString());
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Secret.ModelFile);
                var result = await recognizer.RecognizeOnceAsync(model);

                Assert.IsTrue(result.Reason == ResultReason.RecognizedKeyword, "Wrong result reason.");
                Assert.IsTrue(result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Secret.ModelKeyword), "Wrong result contents.");

                var hasCompleted = Task.WaitAny(tcs.Task, cancellation.Task, Task.Delay(30000));

                Assert.AreEqual(0, hasCompleted, "keyword not detected within timeout");
                Assert.IsTrue(tcs.Task.Result, "keyword not detected within timeout");
                /* Give some time so the stream is filled */
                await Task.Delay(500);
                var stream = AudioDataStream.FromResult(result);
                Assert.IsTrue(stream.CanReadData(100), "Can't read data from the stream");
                var buffer = new byte[100];
                Assert.AreEqual(100u, stream.ReadData(buffer));
                stream.DetachInput();
            }
        }
    }
}
