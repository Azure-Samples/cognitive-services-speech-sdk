//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using SpeechSDKSamples.Remote.WebSocket.Shared;
using System.Text.Json;

namespace SpeechSDKSamples.Remote.Service;

public class DisplayText : JsonMessage
{
    public DisplayText() { }

    public DisplayText(ReadOnlyMemory<byte> data)
    {
        var newObj = JsonSerializer.Deserialize<DisplayText>(data.Span);
        if (null == newObj)
        {
            throw new ArgumentException("Data did not deserialize to an DisplayText", "bytes");
        }

        Text = newObj.Text;
        DisplayType = newObj.DisplayType;
    }

    public DisplayType DisplayType {get;set;}
    public string Text { get; set; }
}

public enum DisplayType
{
    Intermedaite,
    Final
}
