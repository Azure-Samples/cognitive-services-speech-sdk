//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    /// <summary>
    /// The mode used for profanity filtering.
    /// </summary>
    public enum ProfanityFilterMode
    {
        /// <summary>
        /// Disable profanity filtering.
        /// </summary>
        None,

        /// <summary>
        /// Remove profanity.
        /// </summary>
        Removed,

        /// <summary>
        /// Add "profanity" XML tags&lt;/Profanit&gt;
        /// </summary>
        Tags,

        /// <summary>
        /// Mask the profanity with * except for the first letter, e.g., f***
        /// </summary>
        Masked
    }
}
