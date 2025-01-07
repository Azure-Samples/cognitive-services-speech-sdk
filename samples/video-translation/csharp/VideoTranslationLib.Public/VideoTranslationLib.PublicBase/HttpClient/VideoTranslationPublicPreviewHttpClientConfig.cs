//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation;

using Flurl;
using Microsoft.SpeechServices.CommonLib;
using Microsoft.SpeechServices.CommonLib.Public.Interface;
using Microsoft.SpeechServices.CommonLib.Util;
using System;

public class VideoTranslationPublicPreviewHttpClientConfig :
    HttpClientConfigBase
{
    public VideoTranslationPublicPreviewHttpClientConfig(IRegionConfig regionConfig, string subKey)
        : base(regionConfig, subKey)
    {
    }

    public override string RouteBase => "videotranslation";

    public override bool IsApiVersionInUrlSegment => false;
}
