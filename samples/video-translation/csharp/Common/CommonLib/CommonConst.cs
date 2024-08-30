//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib;

using System;
using System.Collections.Generic;

public static class CommonConst
{
    public static class VideoTranslation
    {
        public static class ApiVersions
        {
            public const string PublicPreviewApiVersion20240520Preview = "2024-05-20-preview";
        }
    }

    public static class Http
    {
        public static readonly TimeSpan OperationQueryDuration = TimeSpan.FromSeconds(3);

        public static class Headers
        {
            public const string OperationId = "Operation-Id";
            public const string OperationLocation = "Operation-Location";

            public readonly static IEnumerable<string> OperationHeaders = new[]
            {
                OperationId,
                OperationLocation,
            };
        }

        public static class MimeType
        {
            public const string HttpAudioBasic = "audio/basic";
            public const string HttpAudioSilk = "audio/SILK";
            public const string HttpAudioSilk24K = "audio/SILK; samplerate=24000";
            public const string HttpAudioXwave = "audio/x-wav";
            public const string TextXml = "text/xml";
            public const string Text = "text/plain";
            public const string TextJson = "application/json";
            public const string HttpSsmlXml = "application/ssml+xml";
            public const string HttpAudioMpeg = "audio/mpeg";
            public const string OpusAudio16K = "audio/ogg; codecs=opus; rate=16000";
            public const string OpusAudio24K = "audio/ogg; codecs=opus; rate=24000";
            public const string OpusAudio48K = "audio/ogg; codecs=opus; rate=48000";
            public const string Zip = "application/zip";
            public const string AudioMp3 = "audio/mpeg3";
        }
    }
}
