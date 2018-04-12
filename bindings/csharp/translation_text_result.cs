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
    /// Defines tranlsation result.
    /// </summary>
    public class TranslationTextResult : Speech.SpeechRecognitionResult
    {
        // BUG: this is hack for making documentation going.
        internal TranslationTextResult(Internal.TranslationTextResult result) : base(null)
        {

        }

        /// <summary>
        /// Specifies translation status.
        /// </summary>
        public RecognitionStatus TranslationStatus { get; }

        /// <summary>
        /// Presents the translation results. Each item in the list is a key value pair, where key is the language name in BCP-47 format,
        /// and value is the translation text in the specified language.
        /// </summary>
        public IReadOnlyList<KeyValuePair<string, string>> Translations { get; }

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
