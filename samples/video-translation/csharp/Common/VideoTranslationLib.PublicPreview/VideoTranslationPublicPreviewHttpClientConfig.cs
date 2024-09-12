//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation;

using Flurl;
using Microsoft.SpeechServices.CommonLib.Attributes;
using Microsoft.SpeechServices.CommonLib.Extensions;
using Microsoft.SpeechServices.CommonLib.Util;
using System;

public class VideoTranslationPublicPreviewHttpClientConfig<TDeploymentEnvironment> :
    HttpClientConfigBase<TDeploymentEnvironment>
    where TDeploymentEnvironment : Enum
{
    public VideoTranslationPublicPreviewHttpClientConfig(TDeploymentEnvironment environment, string subKey)
        : base(environment, subKey)
    {
    }

    public override string RouteBase => "videotranslation";

    public override Uri RootUrl
    {
        get
        {
            return this.BaseUrl
                .AppendPathSegment(RouteBase)
                .ToUri();
        }
    }

    public override Uri BaseUrl
    {
        get
        {
            return this.Environment.GetAttributeOfType<DeploymentEnvironmentAttribute>()?.GetApimApiBaseUrl();
        }
    }
}
