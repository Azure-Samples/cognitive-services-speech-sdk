//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Util;

using Flurl;
using Flurl.Http;
using Microsoft.SpeechServices.CommonLib.Public.Interface;
using System;

public abstract class HttpClientConfigBase
{
    public HttpClientConfigBase(IRegionConfig regionConfig, string subKey)
    {
        ArgumentNullException.ThrowIfNull(regionConfig);
        this.RegionConfig = regionConfig;
        this.SubscriptionKey = subKey;
    }

    public virtual Uri RootUrl
    {
        get
        {
            // Use APIM for public API.
            var url = this.RegionConfig.EndpointUrl
                .AppendPathSegment(RouteBase);

            if (this.IsApiVersionInUrlSegment)
            {
                url = url.AppendPathSegment(this.ApiVersion);
            }
            else
            {
                url = url.SetQueryParam(CommonPublicConst.Http.ParameterNames.ApiVersion, this.ApiVersion);
            }

            return url.ToUri();
        }
    }

    public virtual bool IsApiVersionInUrlSegment => false;

    public virtual string ApiVersion { get; set; }

    public abstract string RouteBase { get; }

    public IRegionConfig RegionConfig { get; set; }

    public string SubscriptionKey { get; set; }
}
