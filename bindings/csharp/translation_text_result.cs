//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using System.Collections.Generic;
using Microsoft.CognitiveServices.Speech;

namespace Microsoft.CognitiveServices.Speech.Translation
{
    /// <summary>
    /// Defines tranlsation result.
    /// </summary>
    public sealed class TranslationTextResult : SpeechRecognitionResult
    {
        // BUG: this is hack for making documentation going.
        internal TranslationTextResult(Internal.TranslationTextResult result) : base(result)
        {
            TranslationStatus = result.TranslationStatus == Internal.TranslationStatus.Success ? TranslationStatus.Success : TranslationStatus.Error;
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
        public TranslationStatus TranslationStatus { get; }

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
            var text = string.Format(CultureInfo.InvariantCulture,
                "ResultId:{0} RecognitionStatus:{1}, TranslationStatus: {2}, Recognized text:<{3}>.\n", 
                ResultId, Reason, TranslationStatus, RecognizedText);
            foreach (var element in Translations)
            {
                text += string.Format(CultureInfo.InvariantCulture, "    Translation in {0}: <{1}>.\n", element.Key, element.Value);
            }
            return text;
        }

        private Dictionary<string, string> translationTextResultMap;
    }
}
