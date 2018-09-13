//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using Microsoft.CognitiveServices.Speech;

namespace Microsoft.CognitiveServices.Speech.Translation
{
    /// <summary>
    /// Defines tranlsation result.
    /// </summary>
    public sealed class TranslationTextResult : SpeechRecognitionResult
    {
        internal TranslationTextResult(Internal.TranslationTextResult result) : base(result)
        {
            translationResultImpl = result;
            translationTextResultMap = new Dictionary<string, string>();

            var map = result.Translations;
            foreach (var element in map)
            {
                translationTextResultMap.Add(element.Key, element.Value);
            }
        }

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
            var text = string.Format(CultureInfo.InvariantCulture, "ResultId:{0} Reason:{1}, Recognized text:<{2}>.\n", ResultId, Reason, Text);
            foreach (var element in Translations)
            {
                text += $"    Translation in {element.Key}: <{element.Value}>.\n";
            }
            return text;
        }

        // Hold the reference
        private Internal.TranslationTextResult translationResultImpl;

        private Dictionary<string, string> translationTextResultMap;
    }
}
