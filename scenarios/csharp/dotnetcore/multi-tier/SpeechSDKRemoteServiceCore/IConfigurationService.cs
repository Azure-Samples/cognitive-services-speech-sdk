//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace SpeechSDKSamples.Remote.Service.Core
{
    /// <summary>
    /// The interface needed to repsent the minimum parameters needed to connect to the speech service.
    /// </summary>
    public interface IConfigurationService
    {
        Uri SpeechServiceEndpoint { get; set; }
        string SpeechServiceRegion { get; set; }
        string SpeechServiceKey { get; set; }
    }
}
