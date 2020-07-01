//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Diagnostics.Contracts;
using System.Globalization;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Contains auto detected source language result
    /// Added in 1.9.0
    /// </summary>
    public class AutoDetectSourceLanguageResult
    {
        /// <summary>
        /// Presents the detected language
        /// </summary>
        public string Language { get; }

        /// <summary>
        /// Creates an instance of AutoDetectSourceLanguageResult object for the speech recognition result.
        /// </summary>
        /// <param name="result">The speech recongition result.</param>
        /// <returns>A new AutoDetectSourceLanguageResult instance</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1062", Justification = "Validated in internal constructor.")]
        public static AutoDetectSourceLanguageResult FromResult(SpeechRecognitionResult result)
        {
            return new AutoDetectSourceLanguageResult(result);
        }

        /// <summary>
        /// Creates an instance of AutoDetectSourceLanguageResult object for any recognition result.
        /// </summary>
        /// <param name="result">The recongition result.</param>
        /// <returns>A new AutoDetectSourceLanguageResult instance</returns>
        internal AutoDetectSourceLanguageResult(RecognitionResult result)
        {
            ThrowIfNull(result);
            this.Language = result.Properties.GetProperty(PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult);
        }
    }
}
