//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using Carbon.Recognition;

namespace Carbon.Recognition.Translation
{
    /// <summary>
    /// Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
    /// </summary>
    public class TranslationSynthesisResult
    {
        // BUG: this is hack for making documentation going.
        internal TranslationSynthesisResult(Internal.TranslationSynthesisResult result)
        {

        }

        /// <summary>
        /// translated text in the target language.
        /// </summary>
        public byte[] AudioData { get; }

        /// <summary>
        /// Returns a string that represents the speech recognition result.
        /// </summary>
        /// <returns>A string that represents the speech recognition result.</returns>
        public override string ToString()
        {
            throw new NotImplementedException();
        }
    }
}
