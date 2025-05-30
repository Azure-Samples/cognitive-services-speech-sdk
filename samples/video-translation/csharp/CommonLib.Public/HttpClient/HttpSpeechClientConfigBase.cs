//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Util;

using Flurl;
using Microsoft.SpeechServices.CommonLib.Public.Interface;
using System;

public abstract class HttpSpeechClientConfigBase : HttpClientConfigBase
{
    public HttpSpeechClientConfigBase(
        IRegionConfig regionConfig,
        string subKey,
        string customDomainName,
        Guid? managedIdentityClientId)
        : base(regionConfig, managedIdentityClientId)
    {
        ArgumentNullException.ThrowIfNull(regionConfig);
        this.RegionConfig = regionConfig;
        this.SubscriptionKey = subKey;
        this.CustomDomainName = customDomainName;
        this.ManagedIdentityClientId = managedIdentityClientId;
    }

    public override bool UseOAuth
    {
        get
        {
            return string.IsNullOrEmpty(this.SubscriptionKey) && !string.IsNullOrEmpty(this.CustomDomainName);
        }
    }

    public override Url RootAddress
    {
        get
        {
            return !string.IsNullOrEmpty(this.CustomDomainName) ?
                new Url($"https://{this.CustomDomainName}.cognitiveservices.azure.com/") :
                base.RootAddress;
        }
    }

    public string SubscriptionKey { get; set; }

    // For speech resource, OAuth only avaible when CustomDomainName enabled.
    public string CustomDomainName { get; set; }
}
