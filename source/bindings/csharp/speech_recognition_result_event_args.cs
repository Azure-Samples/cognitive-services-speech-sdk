//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System.Globalization;
namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Define payload of speech intermediate/final result events.
    /// </summary>
    public sealed class SpeechRecognitionResultEventArgs : System.EventArgs
    {
        internal SpeechRecognitionResultEventArgs(Internal.SpeechRecognitionEventArgs e)
        {
            evenArgImpl = e;
            Result = new SpeechRecognitionResult(e.GetResult());
            SessionId = e.SessionId;
        }

        /// <summary>
        /// Specifies the recognition result.
        /// </summary>
        public SpeechRecognitionResult Result { get; }

        /// <summary>
        /// Specifies the session identifier.
        /// </summary>
        public string SessionId { get; }

        /// <summary>
        /// Returns a string that represents the speech recognition result event.
        /// </summary>
        /// <returns>A string that represents the speech recognition result event.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"SessionId:{0} ResultId:{1} Status:{2} Recognized text:<{3}>.", SessionId, Result.ResultId, Result.RecognitionStatus, Result.Text);
        }

        // Hold the reference
        private Internal.SpeechRecognitionEventArgs evenArgImpl;
    }
}
