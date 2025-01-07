//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Public.Interface;

using System;

public class ApimApiRegionConfig : IRegionConfig
{
    public ApimApiRegionConfig(string regionIdentifier)
    {
        ArgumentNullException.ThrowIfNull(regionIdentifier);
        this.RegionIdentifier = regionIdentifier;
    }

    public string RegionIdentifier { get; private set; }

    public Uri EndpointUrl => new Uri($"https://{HostName}");

    private string HostName => $"{this.RegionIdentifier}.api.cognitive.microsoft.com";
}
