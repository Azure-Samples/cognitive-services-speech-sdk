//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text.Json;
using SpeechSDKSamples.Remote.Core;

namespace SpeechSDKSamples.Remote.WebSocket.Shared
{
    public class RecognitionStoppedMessage : JsonMessage
    {
        public RecognitionStoppedMessage() { }

        public RecognitionStoppedMessage(ReadOnlyMemory<byte> bytes)
        {
            var newObj = JsonSerializer.Deserialize<RecognitionStoppedMessage>(bytes.Span);
            if (null == newObj)
            {
                throw new ArgumentException("Data did not deserialize to an AudioFormatMessagew", "bytes");
            }

            this.Reason = newObj.Reason;
            this.ErrorMessage = newObj.ErrorMessage;
            this.ErrorCode = newObj.ErrorCode;
        }

        public RecognitionStoppedReason Reason { get; set; }
        public string ErrorMessage { get; set; }
        public int ErrorCode { get; set; }

    }
}
