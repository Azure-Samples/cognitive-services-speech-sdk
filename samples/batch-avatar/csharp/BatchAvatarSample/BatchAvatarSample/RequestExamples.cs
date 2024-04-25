//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchAvatarSample;

using BatchAvatarSample.dto;
using System;
using System.Collections.Generic;

internal static class RequestExamples
{
    public static BatchAvatarRequest SsmlRequest = new()
    {
        InputKind = "Ssml",
        Inputs =
            [
                new BatchAvatarInput
                {
                    Content = """
                    <speak version="1.0" xmlns="http://www.w3.org/2001/10/synthesis" xmlns:mstts="http://www.w3.org/2001/mstts" xmlns:emo="http://www.w3.org/2009/10/emotionml" xml:lang="en-US"><voice name="en-US-AvaMultilingualNeural">hello, this is my talking avatar</voice></speak>
                    """,
                },
            ],
        AvatarConfig = new()
        {
            TalkingAvatarCharacter = "lisa",
            TalkingAvatarStyle = "graceful-sitting",
            VideoFormat = "mp4",
            VideoCodec = "h264",
            SubtitleType = "soft_embedded",
            BackgroundColor = "#FFFFFFFF",
        },
    };

    public static BatchAvatarRequest CustomVoiceRequest = new()
    {
        InputKind = "PlainText",
        Inputs =
            [
                new BatchAvatarInput
                {
                    Content = "Hi, I'm a virtual assistant created by Microsoft.",
                },
            ],
        // Replace with your custom voice name and deployment ID if you want to use custom voice.
        // Multiple voices are supported, the mixture of custom voices and platform voices is allowed.
        // Invalid voice name or deployment ID will be rejected.
        CustomVoices = new Dictionary<string, Guid>
        {
            ["YOUR_CUSTOM_VOICE_NAME"]  = Guid.Parse("YOUR_CUSTOM_VOICE_DEPLOYMENT_ID"),
        },
        SynthesisConfig = new()
        {
            Voice = "YOUR_CUSTOM_VOICE_NAME",
        },
        AvatarConfig = new()
        {
            TalkingAvatarCharacter = "lisa",
            TalkingAvatarStyle = "graceful-sitting",
            VideoFormat = "mp4",
            VideoCodec = "h264",
            SubtitleType = "soft_embedded",
            BackgroundColor = "#FFFFFFFF",
        },
    };
}
