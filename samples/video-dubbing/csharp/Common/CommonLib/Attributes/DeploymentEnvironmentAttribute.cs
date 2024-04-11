// <copyright file="DeploymentEnvironmentAttribute.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.CommonLib.Attributes;

using System;
using System.IO;
using System.Linq;
using System.Reflection;
using Microsoft.SpeechServices.CommonLib.Enums;
using Microsoft.SpeechServices.CustomVoice;
using Microsoft.SpeechServices.CommonLib.Extensions;

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

    public string PortalAddress
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

        return default;
    }

    public Uri GetApiBaseUrl()
    {
        Uri url = null;
        if (!string.IsNullOrEmpty(this.PortalAddress))
        {
            url = new Uri($"https://{this.PortalAddress}/");
        }

        return url;
    }
}
