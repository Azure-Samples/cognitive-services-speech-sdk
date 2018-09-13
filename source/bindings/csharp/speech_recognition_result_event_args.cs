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
            return string.Format(CultureInfo.InvariantCulture,"SessionId:{0} ResultId:{1} Reason:{2} Recognized text:<{3}>.", SessionId, Result.ResultId, Result.Reason, Result.Text);
        }

        // Hold the reference
        private Internal.SpeechRecognitionEventArgs evenArgImpl;
    }

    /// <summary>
    /// Define payload of speech recognition canceled result events.
    /// </summary>
    public sealed class SpeechRecognitionCanceledEventArgs : System.EventArgs
    {
        internal SpeechRecognitionCanceledEventArgs(Internal.SpeechRecognitionCanceledEventArgs e)
        {
            evenArgImpl = e;
            Result = new SpeechRecognitionResult(e.GetResult());
            SessionId = e.SessionId;
            
            var cancellation = e.GetCancellationDetails();
            Reason = (CancellationReason)cancellation.Reason;
            ErrorDetails = cancellation.ErrorDetails;
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
        /// The reason the recognition was canceled.
        /// </summary>
        public CancellationReason Reason { get; }

        /// <summary>
        /// In case of an unsuccessful recognition, provides a details of why the occurred error.
        /// This field is only filled-out if the reason canceled (<see cref="Reason"/>) is set to Error.
        /// </summary>
        public string ErrorDetails { get; }

        /// <summary>
        /// Returns a string that represents the speech recognition result event.
        /// </summary>
        /// <returns>A string that represents the speech recognition result event.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"SessionId:{0} ResultId:{1} CancellationReason:{2}.", SessionId, Result.ResultId, Reason);
        }

        // Hold the reference
        private Internal.SpeechRecognitionCanceledEventArgs evenArgImpl;
    }
}
