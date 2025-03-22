//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace SpeechSDKSamples.Remote.WebSocket.Shared
{
    public interface IMessage
    {
        ReadOnlyMemory<byte> ToMemory();
        
        void ToStream(System.IO.Stream stream);
    }
}
