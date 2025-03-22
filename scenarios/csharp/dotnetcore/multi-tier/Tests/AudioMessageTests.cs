//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.VisualStudio.TestTools.UnitTesting;
using SpeechSDKSamples.Remote.WebSocket.Shared;
using System.Threading.Tasks;

namespace SpeechSDKSamples.RemoteClient.Core.Tests
{
    [TestClass]
    public class AudioMessageTests
    {
        [TestMethod]
        public async Task RoundTripSerialization()
        {
            var bytes = new byte[] { 1, 2, 3, 4 };

            var audioMessage = new AudioMessage(bytes);

            var messageAsBytes = await MessageUtility.SerializeMessageAsync(audioMessage);

            var audioMessage2 = MessageUtility.DeserializeMessageByType<AudioMessage>(messageAsBytes);
    
            CollectionAssert.AreEqual(bytes, audioMessage2.AudioData.ToArray());
        }
    }
}