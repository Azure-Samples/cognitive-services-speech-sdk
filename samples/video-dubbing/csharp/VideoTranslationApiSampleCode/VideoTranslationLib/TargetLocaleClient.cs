//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation;

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Net;
using System.Threading.Tasks;
using System.Xml.Linq;
using Flurl;
using Flurl.Http;
using Microsoft.SpeechServices.CommonLib.Enums;
using Microsoft.SpeechServices.DataContracts;
using Microsoft.SpeechServices.VideoTranslation.DataContracts.DTOs;
using Microsoft.SpeechServices.VideoTranslation.Enums;
using Microsoft.SpeechServices.CommonLib.Util;
using System.IO;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation;

public class TargetLocaleClient<TDeploymentEnvironment> : VideoTranslationClientBase<TDeploymentEnvironment>
    where TDeploymentEnvironment : Enum
{
    public TargetLocaleClient(TDeploymentEnvironment environment, string subKey)
        : base(environment, subKey)
    {
    }

    public override string ControllerName => "VideoFileTargetLocales";

    public async Task<PaginatedResources<VideoFileTargetLocaleBrief>> QueryTargetLocalesAsync(
        int skip = 0,
        int top = 100,
        string orderby = "lastActionDateTime desc")
    {
        var url = this.BuildRequestBase();
        return await this.RequestWithRetryAsync(async () =>
        {
            return await url
                .SetQueryParam("skip", skip)
                .SetQueryParam("top", top)
                .SetQueryParam("orderby", orderby)
                .GetAsync()
                .ReceiveJson<PaginatedResources<VideoFileTargetLocaleBrief>>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<Uri> UpdateTargetLocaleEdittingWebvttFileAsync(
        Guid videoFileId,
        CultureInfo targetLocale,
        VideoTranslationWebVttFilePlainTextKind? kind,
        string webvttFilePath)
    {
        if (string.IsNullOrEmpty(webvttFilePath))
        {
            throw new ArgumentNullException(webvttFilePath);
        }

        if (!File.Exists(webvttFilePath))
        {
            throw new FileNotFoundException(webvttFilePath);
        }

        var url = this.BuildRequestBase()
            .AppendPathSegment(videoFileId.ToString())
            .AppendPathSegment(targetLocale.Name)
            .AppendPathSegment("webvtt");
        if ((kind ?? VideoTranslationWebVttFilePlainTextKind.None) != VideoTranslationWebVttFilePlainTextKind.None)
        {
            url = url.SetQueryParam("kind", kind.Value.AsString());
        }

        return await this.RequestWithRetryAsync(async () =>
            {
                return await url
                    .PostMultipartAsync(mp =>
                    {
                        mp.AddFile("webVttFile", webvttFilePath);
                    });
            })
            .ReceiveJson<Uri>()
            .ConfigureAwait(false);
    }

}
