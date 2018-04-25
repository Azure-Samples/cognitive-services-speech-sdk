//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System.Globalization;
using Microsoft.CognitiveServices.Speech.Recognition;

namespace Microsoft.CognitiveServices.Speech.Recognition.Speech
{
    /// <summary>
    /// Defines result of speech recognition.
    /// </summary>
    public class SpeechRecognitionResult
    {
        internal SpeechRecognitionResult(Internal.RecognitionResult result)
        {
            this.ResultId = result.ResultId;
            this.RecognizedText = result.Text;
            this.Reason = (RecognitionStatus)((int)result.Reason);
        }

        /// <summary>
        /// Specifies the result identifier.
        /// </summary>
        public string ResultId { get; }

        /// <summary>
        /// Specifies status of the result.
        /// </summary>
        public RecognitionStatus Reason { get; }

        /// <summary>
        /// Presents the recognized text in the result.
        /// </summary>
        public string RecognizedText { get; }

        /// <summary>
        /// Returns a string that represents the speech recognition result.
        /// </summary>
        /// <returns>A string that represents the speech recognition result.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"ResultId:{0} Status:{1} Recognized text:<{2}>.", ResultId, Reason, RecognizedText);
        }
    }
}
