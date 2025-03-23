//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text.Json;

namespace SpeechSDKSamples.Remote.WebSocket.Shared
{
    public class AudioAcknowledgedMessage : JsonMessage
    {
        public AudioAcknowledgedMessage(ReadOnlyMemory<byte> data)
        {
            var newObj = JsonSerializer.Deserialize<AudioAcknowledgedMessage>(data.Span);
            if (null == newObj)
            {
                throw new ArgumentException("Data did not deserialize to an AudioAcknowledgedMessage", "bytes");
            }

            AcknowledgedAudio = newObj.AcknowledgedAudio;
        }

        public AudioAcknowledgedMessage() { }

        public TimeSpan AcknowledgedAudio { get; set; }
    }
}
