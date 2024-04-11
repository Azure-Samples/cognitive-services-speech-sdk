//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation;

using Flurl;
using Flurl.Http;
using Microsoft.SpeechServices.Common.Client;
using Microsoft.SpeechServices.CommonLib.Enums;
using Microsoft.SpeechServices.CommonLib.Util;
using Microsoft.SpeechServices.DataContracts;
using Microsoft.SpeechServices.VideoTranslation.DataContracts.DTOs;
using Newtonsoft.Json;
using Polly;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Net;
using System.Threading.Tasks;

public class VideoTranslationMetadataClient<TDeploymentEnvironment> : VideoTranslationClientBase<TDeploymentEnvironment>
    where TDeploymentEnvironment : Enum
{
    public VideoTranslationMetadataClient(TDeploymentEnvironment environment, string subKey)
        : base(environment, subKey)
    {
    }

    public override string ControllerName => "VideoTranslationMetadata";

    public async Task<VideoTranslationMetadata> QueryMetadataAsync()
    {
        var url = this.BuildRequestBase();

        return await this.RequestWithRetryAsync(async () =>
        {
             return await url.GetAsync()
                .ReceiveJson<VideoTranslationMetadata>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }
}
