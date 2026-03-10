//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;
using Microsoft.SpeechServices.CommonLib;
using System;

public partial class BaseOptions
{
    [Option('r', "region", Required = true, HelpText = CommonPublicConst.ArgumentDescription.Region)]
    public string Region { get; set; }

    [Option('s', "subscriptionKey", Required = false, HelpText = CommonPublicConst.ArgumentDescription.SubscriptionKey)]
    public string SubscriptionKey { get; set; }

    [Option('d', "customDomainName", Required = false, HelpText = CommonPublicConst.ArgumentDescription.CustomDomainName)]
    public string CustomDomainName { get; set; }

    [Option('m', "managedIdentityClientId", Required = false, HelpText = CommonPublicConst.ArgumentDescription.ManagedIdentityClientId)]
    public Guid ManagedIdentityClientId { get; set; }

    [Option('v', "apiVersion", Required = true, HelpText = CommonPublicConst.ArgumentDescription.ApiVersion)]
    public string ApiVersion { get; set; }

    public bool? AdjustWebvttAlignmentApiParameter(string apiVersion, bool? adjustWebvttAlignment)
    {
        if (string.Equals(
                CommonPublicConst.ApiVersions.ApiVersion20240520Preview,
                apiVersion,
                StringComparison.OrdinalIgnoreCase) ||
            string.Equals(
                CommonPublicConst.ApiVersions.ApiVersion20250520,
                apiVersion,
                StringComparison.OrdinalIgnoreCase))
        {
            // For API version 2025-05-20 and previous, always keep the parameter null, since the parameter is not released in public doc for those versions.
            // This is for backward compatible for user who keep using old API version with latest client tool.
            return null;
        }
        else
        {
            // For API version 2026-03-01 and later, the default value is true if this parameter is not provided.
            return adjustWebvttAlignment;
        }
    }
}
