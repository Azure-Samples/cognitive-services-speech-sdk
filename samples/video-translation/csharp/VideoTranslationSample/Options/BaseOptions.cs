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

    [Option('d', "CustomDomainName", Required = false, HelpText = CommonPublicConst.ArgumentDescription.CustomDomainName)]
    public string CustomDomainName { get; set; }

    [Option('m', "managedIdentityClientId", Required = false, HelpText = CommonPublicConst.ArgumentDescription.ManagedIdentityClientId)]
    public Guid ManagedIdentityClientId { get; set; }

    [Option('v', "apiVersion", Required = true, HelpText = CommonPublicConst.ArgumentDescription.ApiVersion)]
    public string ApiVersion { get; set; }
}
