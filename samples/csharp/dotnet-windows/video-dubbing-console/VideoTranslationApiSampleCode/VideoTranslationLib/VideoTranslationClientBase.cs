//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation;

using System;
using Microsoft.SpeechServices.CommonLib.Util;

public abstract class VideoTranslationClientBase<TDeploymentEnvironment> : HttpClientBase<TDeploymentEnvironment>
    where TDeploymentEnvironment : Enum
{
    public VideoTranslationClientBase(TDeploymentEnvironment environment, string subKey)
        : base(environment, subKey)
    {
    }

    public override string RouteBase => "videotranslation";
}
