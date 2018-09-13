//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Diagnostics;
using System.Globalization;
using System.Collections.Generic;
using Microsoft.CognitiveServices.Speech;

namespace Microsoft.CognitiveServices.Speech.Translation
{
    /// <summary>
    /// Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
    /// </summary>
    public sealed class TranslationSynthesisResult
    {
        internal TranslationSynthesisResult(Internal.TranslationSynthesisResult result)
        {
            resultImpl = result;

            Audio = new byte[result.Audio.Count];
            result.Audio.CopyTo(Audio);
        }

        /// <summary>
        /// The voice output of the translated text in the target language.
        /// </summary>
        public byte[] Audio { get; }

        /// <summary>
        /// Returns a string that represents the synthesis result.
        /// </summary>
        /// <returns>A string that represents the synthesis result.</returns>
        public override string ToString()
        {
            var text = string.Format(CultureInfo.InvariantCulture, "AudioSize:{0}", Audio.Length);
            if (Audio.Length == 0)
            {
                text += string.Format(CultureInfo.InvariantCulture, " (end of synthesis data)");
            }
            return text;
        }

        // Hold the referece.
        private Internal.TranslationSynthesisResult resultImpl;
    }
}
