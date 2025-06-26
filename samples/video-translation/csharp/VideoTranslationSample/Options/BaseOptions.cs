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
    [Option('r', "region", Required = true, HelpText = VideoTranslationPublicConst.ArgumentDescription.Region)]
    public string Region { get; set; }

    [Option('s', "subscriptionKey", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.SubscriptionKey)]
    public string SubscriptionKey { get; set; }

    [Option('d', "customDomainName", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.CustomDomainName)]
    public string customDomainName { get; set; }

    [Option('m', "managedIdentityClientId", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.ManagedIdentityClientId)]
    public Guid ManagedIdentityClientId { get; set; }

    [Option('v', "apiVersion", Required = true, HelpText = VideoTranslationPublicConst.ArgumentDescription.ApiVersion)]
    public string ApiVersion { get; set; }
}
