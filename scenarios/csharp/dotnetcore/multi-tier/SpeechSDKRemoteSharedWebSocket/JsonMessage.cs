//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.IO;
using System.Text;
using System.Text.Json;

namespace SpeechSDKSamples.Remote.WebSocket.Shared
{
    public class JsonMessage : IMessage
    {
        public ReadOnlyMemory<byte> ToMemory()
        {
            var json = JsonSerializer.Serialize(this, this.GetType());
            var bytes = Encoding.UTF8.GetBytes(json);
            return new ReadOnlyMemory<byte>(bytes);
        }

        public void ToStream(Stream stream)
        {
            throw new NotImplementedException();
        }
    }
}
