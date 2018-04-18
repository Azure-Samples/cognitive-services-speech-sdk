//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using Microsoft.CognitiveServices.Speech.Recognition;

namespace Microsoft.CognitiveServices.Speech.Recognition.Translation
{
    /// <summary>
    /// Defines tranlsation result.
    /// </summary>
    public sealed class TranslationTextResult : Speech.SpeechRecognitionResult
    {
        // BUG: this is hack for making documentation going.
        internal TranslationTextResult(Internal.TranslationTextResult result) : base(result)
        {
            translationTextResultMap = new Dictionary<string, string>();
            //Todo: add translation result
            var map = result.Translations;
            foreach (var element in map)
            {
                translationTextResultMap.Add(element.Key, element.Value);
            }
        }

        /// <summary>
        /// Specifies translation status.
        /// </summary>
        public RecognitionStatus TranslationStatus { get; }

        /// <summary>
        /// Presents the translation results. Each item in the dictionary represents translation result in one of target languages, where the key 
        /// is the name of the target language, in BCP-47 format, and the value is the translation text in the specified language.
        /// </summary>
        public IReadOnlyDictionary<string, string> Translations { get { return translationTextResultMap; } }

        /// <summary>
        /// Returns a string that represents the speech recognition result.
        /// </summary>
        /// <returns>A string that represents the speech recognition result.</returns>
        public override string ToString()
        {
            throw new NotImplementedException();
        }

        private Dictionary<string, string> translationTextResultMap;
    }
}
