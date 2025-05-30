//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Util;

using Azure.Core;
using Azure.Identity;
using Flurl;
using Microsoft.SpeechServices.CommonLib.Public.Interface;
using System;
using System.Threading.Tasks;

public abstract class HttpClientConfigBase
{
    private string oAuthToken;

    public HttpClientConfigBase(
        IRegionConfig regionConfig,
        Guid? managedIdentityClientId)
    {
        ArgumentNullException.ThrowIfNull(regionConfig);
        this.RegionConfig = regionConfig;
        this.ManagedIdentityClientId = managedIdentityClientId;
    }

    public virtual Url RootAddress => new Url(this.RegionConfig.EndpointUrl);

    public virtual bool UseOAuth => true;

    public virtual Uri RootUrl
    {
        get
        {
            // Use APIM for public API.
            var url = this.RootAddress;
            url = url.AppendPathSegment(RouteBase);

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

    public virtual async Task<string> AcquireOAuthTokenAsync()
    {
        if (string.IsNullOrEmpty(this.oAuthToken))
        {
            // The default access token lifetime for a managed identity is 8 hours.Some tenants have a managed identity token lifetime of 24 hours 1.
            // Managed Identity access tokens expire in 24 hours.Tokens acquired via the App Authentication library are refreshed when less than 5 minutes remains until they expire 1.
            // Use Azure Identity SDK to acquire token
            var options = new DefaultAzureCredentialOptions();
            if ((this.ManagedIdentityClientId ?? Guid.Empty) != Guid.Empty)
            {
                options.ManagedIdentityClientId = this.ManagedIdentityClientId.Value.ToString();
            }

            var credential = new DefaultAzureCredential(options);
            var tokenRequestContext = new TokenRequestContext(new[] { "https://cognitiveservices.azure.com/.default" });
            this.oAuthToken = (await credential.GetTokenAsync(tokenRequestContext)).Token;
        }

        return this.oAuthToken;
    }

    public virtual bool IsApiVersionInUrlSegment => false;

    public virtual string ApiVersion { get; set; }

    public abstract string RouteBase { get; }

    public IRegionConfig RegionConfig { get; set; }

    public Guid? ManagedIdentityClientId { get; set; }
}
