//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation;

using Microsoft.SpeechServices.CommonLib.Public.Interface;
using Microsoft.SpeechServices.CommonLib.Util;
using System;

public class VideoTranslationPublicPreviewHttpClientConfig :
    HttpSpeechClientConfigBase
{
    public VideoTranslationPublicPreviewHttpClientConfig(
        IRegionConfig regionConfig,
        string subKey,
        string customDomainName,
        Guid? managedIdentityClientId)
        : base(regionConfig, subKey, customDomainName, managedIdentityClientId)
    {
    }

    public override string RouteBase => "videotranslation";

    public override bool IsApiVersionInUrlSegment => false;
}
