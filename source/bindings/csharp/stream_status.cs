//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines the possible status of audio data stream.
    /// Added in version 1.4.0
    /// </summary>
    public enum StreamStatus
    {
        /// <summary>
        /// The audio data stream status is unknown
        /// </summary>
        Unknown = 0,

        /// <summary>
        /// The audio data stream contains no data
        /// </summary>
        NoData = 1,

        /// <summary>
        /// The audio data stream contains partial data of a speak request
        /// </summary>
        PartialData = 2,

        /// <summary>
        /// The audio data stream contains all data of a speak request
        /// </summary>
        AllData = 3,

        /// <summary>
        /// The audio data stream was cancelled
        /// </summary>
        Canceled = 4
    }
}
