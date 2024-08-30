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

    public string CustomizedApiHostName { get; set; }

    public int ApiPort { get; set; }

    public string CogSvcAuthHostName { get; set; }

    public string ApimRegionIdentifier { get; set; }

    public string CustomizedTtsFrontendInternalHostName { get; set; }

    public string CustomizedFrontendCogSvcVoiceListEndpoint { get; set; }

    public string CustomizedSpeechPortalHost { get; set; }

    public string ApimHostName
    {
        get
        {
            var region = string.IsNullOrEmpty(this.ApimRegionIdentifier) ? this.RegionIdentifier : this.ApimRegionIdentifier;

            if (region == "local")
            {
                return this.ApiAddress;
            }

            return $"{region}.api.cognitive.microsoft.com";
        }
    }

    public string TtsFrontendInternalHostName
    {
        get
        {
            if (!string.IsNullOrEmpty(this.CustomizedTtsFrontendInternalHostName))
            {
                return this.CustomizedTtsFrontendInternalHostName;
            }

            return $"{this.RegionIdentifier}.tts-frontend.speech.microsoft.com";
        }
    }

    public Uri SpeechPortalUrl
    {
        get
        {
            if (!string.IsNullOrEmpty(this.CustomizedSpeechPortalHost))
            {
                return new Uri($"https://{this.CustomizedSpeechPortalHost}");
            }

            return new Uri($"https://{this.RegionIdentifier}.api.speech.microsoft.com");
        }
    }

    public Uri FrontendCogSvcMetadataApiEndpoint
    {
        get
        {
            return new Uri($"https://{this.TtsFrontendInternalHostName}/synthesize/internal");
        }
    }

    public Uri FrontendCogSvcVoiceListEndpoint
    {
        get
        {
            if (!string.IsNullOrEmpty(CustomizedFrontendCogSvcVoiceListEndpoint))
            {
                return new Uri(this.CustomizedFrontendCogSvcVoiceListEndpoint);
            }

            return new Uri($"https://{this.TtsFrontendInternalHostName}/synthesize/list/cognitive-service/voices");
        }
    }

    public Uri GetFrontendHiddenVoiceListEndpointUri(string trustedToken)
    {
        return new Uri($"https://{this.TtsFrontendInternalHostName}/synthesize/studio9/list/hidden/voices?TrustedClientToken={trustedToken}");
    }

    public Uri GetFrontendCogSvcStudio9Endpoint(string trustedToken)
    {
        return new Uri($"https://{this.TtsFrontendInternalHostName}/synthesize/studio9?TrustedClientToken={trustedToken}");
    }

    public string ApiAddress
    {
        get
        {
            var address = ApiHostName;
            if (!string.IsNullOrEmpty(address) && this.ApiPort != 0)
            {
                address = $"{address}:{this.ApiPort}";
            }

            return address;
        }
    }

    public string ApiHostName
    {
        get
        {
            if (!string.IsNullOrEmpty(this.CustomizedApiHostName))
            {
                return this.CustomizedApiHostName;
            }
            else if (!string.IsNullOrEmpty(this.RegionIdentifier))
            {
                return $"{RegionIdentifier}.customvoice.api.speech.microsoft.com";
            }

            return string.Empty;
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

    public Uri CogSvcAuthHostUrl
    {
        get
        {
            if (!string.IsNullOrEmpty(this.CogSvcAuthHostName))
            {
                return new Uri(this.CogSvcAuthHostName);
            }

            if (string.IsNullOrEmpty(this.RegionIdentifier))
            {
                throw new ArgumentNullException(nameof(this.RegionIdentifier));
            }

            return new Uri($"{this.RegionIdentifier}.api.cognitive.microsoft.com");
        }
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

    public Uri GetBackendApiBaseUrl()
    {
        Uri url = null;
        if (!string.IsNullOrEmpty(this.ApiAddress))
        {
            url = new Uri($"https://{this.ApiAddress}/");
        }

        return url;
    }
}
