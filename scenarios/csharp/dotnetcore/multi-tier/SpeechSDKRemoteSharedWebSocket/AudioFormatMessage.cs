//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text.Json;

namespace SpeechSDKSamples.Remote.WebSocket.Shared
{
    public class AudioFormatMessage : JsonMessage
    {
        public AudioFormatMessage(ReadOnlyMemory<byte> bytes)
        {
            var newObj = JsonSerializer.Deserialize<AudioFormatMessage>(bytes.Span);
            if(null ==  newObj)
            {
                throw new ArgumentException("Data did not deserialize to an AudioFormatMessagew", "bytes");
            }

            this.ChannelCount = newObj.ChannelCount;
            this.BitsPerSample = newObj.BitsPerSample;
            this.SamplesPerSecond = newObj.SamplesPerSecond;
        }

        public AudioFormatMessage() { }

        public int BitsPerSample { get; set; }
        public int SamplesPerSecond { get; set; }
        public int ChannelCount { get; set; }
    }
}
