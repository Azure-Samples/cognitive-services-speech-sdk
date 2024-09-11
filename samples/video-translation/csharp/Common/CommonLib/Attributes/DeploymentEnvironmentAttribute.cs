//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Attributes;

using Microsoft.SpeechServices.CommonLib.Extensions;
using System;
using System.IO;

[AttributeUsage(AttributeTargets.Field, Inherited = false, AllowMultiple = false)]
public sealed class DeploymentEnvironmentAttribute : Attribute
{
    public DeploymentEnvironmentAttribute(
        string regionIdentifier)
    {
        this.RegionIdentifier = regionIdentifier;
    }

    public string RegionIdentifier { get; internal set; }

    public string ApimHostName
    {
        get
        {
            return $"{this.RegionIdentifier}.api.cognitive.microsoft.com";
        }
    }

    public static TDeploymentEnvironment ParseFromRegionIdentifier<TDeploymentEnvironment>(string regionIdentifier)
        where TDeploymentEnvironment : Enum
    {
        if (string.IsNullOrEmpty(regionIdentifier))
        {
            throw new ArgumentNullException(nameof(regionIdentifier));
        }

        foreach (TDeploymentEnvironment environment in Enum.GetValues(typeof(TDeploymentEnvironment)))
        {
            var attribute = environment.GetAttributeOfType<DeploymentEnvironmentAttribute>();
            if (string.Equals(attribute?.RegionIdentifier, regionIdentifier, StringComparison.OrdinalIgnoreCase))
            {
                return environment;
            }
        }

        throw new NotSupportedException($"Not supported region: {regionIdentifier}");
    }

    public Uri GetApimApiBaseUrl()
    {
        Uri url = null;
        if (!string.IsNullOrEmpty(this.ApimHostName))
        {
            url = new Uri($"https://{this.ApimHostName}/");
        }

        return url;
    }
}
