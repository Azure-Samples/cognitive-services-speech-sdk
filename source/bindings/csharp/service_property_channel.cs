//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines channels used to pass property settings to service.
    /// Added in version 1.5.0.
    /// </summary>
    public enum ServicePropertyChannel
    {
        /// <summary>
        /// Uses URI query parameter to pass property settings to service.
        /// </summary>
        UriQueryParameter = 0,

        /// <summary>
        /// Uses HttpHeader to set a key/value in a HTTP header.
        /// </summary>
        HttpHeader = 1
    }
}
