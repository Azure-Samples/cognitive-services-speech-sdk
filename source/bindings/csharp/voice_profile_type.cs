//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Output format.
    /// </summary>
    public enum VoiceProfileType
    {
        /// <summary>
        /// Text independent speaker identification.
        /// </summary>
        TextIndependentIdentification = 1,

        /// <summary>
        ///  Text dependent speaker verification.
        /// </summary>
        TextDependentVerification = 2,

        /// <summary>
        /// Text independent verification.
        /// </summary>
        TextIndependentVerification = 3
    }
}
