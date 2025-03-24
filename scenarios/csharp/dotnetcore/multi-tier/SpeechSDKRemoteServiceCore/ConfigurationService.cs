//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace SpeechSDKSamples.Remote.Service.Core
{
    /// <summary>
    /// A basic configuration object to represent the minimal keys needed to connect to the speech service.
    /// </summary>
    public class BasicConfigurationService : IConfigurationService
    {
        public Uri SpeechServiceEndpoint { get; set; }
        public string SpeechServiceRegion { get; set; }
        public string SpeechServiceKey { get; set; }
    }
}
