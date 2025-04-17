//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;
using System;

public partial class BaseOptions
{
    [Option('r', "region", Required = true, HelpText = "Specify region")]
    public string Region { get; set; }

    [Option('s', "subscriptionKey", Required = false, HelpText = "Specify speech resource key for authentication by key, then don't need specify customDomainName")]
    public string SubscriptionKey { get; set; }

    [Option('d', "customDomainName", Required = false, HelpText = "Specify custom domain name part of the endpoint: https://[customDomainName].cognitiveservices.azure.com/, then don't need specify subscriptionKey")]
    public string customDomainName { get; set; }

    [Option('m', "managedIdentityClientId", Required = false, HelpText = "Specify managed identity client ID, only optional available when specify custom domain.")]
    public Guid ManagedIdentityClientId { get; set; }

    [Option('v', "apiVersion", Required = true, HelpText = "Specify API version.")]
    public string ApiVersion { get; set; }
}
