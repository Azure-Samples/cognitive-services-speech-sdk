//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Carbon.Recognition.Intent
{
    /// <summary>
    /// Defines result of intent recognition.
    /// </summary>
    public class IntentRecognitionResult : Carbon.Recognition.Speech.SpeechRecognitionResult
    {
        internal IntentRecognitionResult(Internal.IntentRecognitionResult result)
            : base(result)
        {
            this.IntentId = result.IntentId;
        }

        /// <summary>
        /// A string that represents the intent identifier being recognized.
        /// </summary>
        public string IntentId { get; }

        /// <summary>
        /// Returns a string that represents the intent recognition result.
        /// </summary>
        /// <returns>A string that represents the intent recognition result.</returns>
        public override string ToString()
        {
            return string.Format("ResultId:{0} Status:{1} IntentId:<{2}> Recognized text:<{3}>.", ResultId, Reason, IntentId, RecognizedText);
        }
    }
}
