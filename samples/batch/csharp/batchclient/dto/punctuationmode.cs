//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    /// <summary>
    /// The kind of punctuation to apply.
    /// </summary>
    public enum PunctuationMode
    {
        /// <summary>
        /// No punctuation.
        /// </summary>
        None = 0,

        /// <summary>
        /// Dictated punctuation marks only, i.e., explicit punctuation.
        /// </summary>
        Dictated = 1 << 0,

        /// <summary>
        /// Automatic punctuation.
        /// </summary>
        Automatic = 1 << 1,

        /// <summary>
        /// Dictated punctuation marks or automatic punctuation.
        /// </summary>
        DictatedAndAutomatic = PunctuationMode.Dictated | PunctuationMode.Automatic
    }
}
