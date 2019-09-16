//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Intent;
using Microsoft.CognitiveServices.Speech.Translation;

namespace Microsoft.CognitiveServices.Speech.Conversation
{
    /// <summary>
    /// Defines how the service handles allocated resources after the conversation transcriber is stopped.
    /// Added in version 1.7.0.
    /// </summary>
    public enum ResourceHandling
    {
        /// <summary>
        /// The service destroys allocated resources.
        /// </summary>
        DestroyResources = 0,

        /// <summary>
        /// The service keeps allocated resources.
        /// </summary>
        KeepResources = 1
    }
}
