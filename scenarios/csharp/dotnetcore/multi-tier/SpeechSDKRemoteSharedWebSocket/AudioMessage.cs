//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.IO;

namespace SpeechSDKSamples.Remote.WebSocket.Shared
{
    public class AudioMessage : IMessage
    {
        public ReadOnlyMemory<byte> AudioData { get; set; } = new byte[0];

        public AudioMessage(ReadOnlyMemory<byte> data)
        {
            AudioData = data;
        }

        public ReadOnlyMemory<byte> ToMemory()
        {
            return AudioData.ToArray();
        }

        public void ToStream(Stream stream)
        {
            stream.Write(AudioData.ToArray(), 0, AudioData.Length);
        }
    }
}
