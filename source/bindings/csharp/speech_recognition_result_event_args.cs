//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System.Globalization;
namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Define payload of speech recognizing/recognized events.
    /// </summary>
    public class SpeechRecognitionEventArgs : RecognitionEventArgs
    {
        internal SpeechRecognitionEventArgs(Internal.SpeechRecognitionEventArgs e)
            : base(e)
        {
            eventArgImpl = e;
            Result = new SpeechRecognitionResult(e.GetResult());
        }

        /// <summary>
        /// Specifies the recognition result.
        /// </summary>
        public SpeechRecognitionResult Result { get; }

        /// <summary>
        /// Returns a string that represents the speech recognition result event.
        /// </summary>
        /// <returns>A string that represents the speech recognition result event.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"SessionId:{0} ResultId:{1} Reason:{2} Recognized text:<{3}>.", SessionId, Result.ResultId, Result.Reason, Result.Text);
        }

        // Hold the reference
        private Internal.SpeechRecognitionEventArgs eventArgImpl;
    }

    /// <summary>
    /// Define payload of speech recognition canceled result events.
    /// </summary>
    public sealed class SpeechRecognitionCanceledEventArgs : SpeechRecognitionEventArgs
    {
        internal SpeechRecognitionCanceledEventArgs(Internal.SpeechRecognitionCanceledEventArgs e)
            : base(e)
        {
            eventArgImpl = e;
            var cancellation = e.GetCancellationDetails();
            Reason = (CancellationReason)cancellation.Reason;
            ErrorCode = (CancellationErrorCode)cancellation.ErrorCode;
            ErrorDetails = cancellation.ErrorDetails;
        }

        /// <summary>
        /// The reason the recognition was canceled.
        /// </summary>
        public CancellationReason Reason { get; }

        /// <summary>
        /// The error code in case of an unsuccessful recognition (Reason<see cref="Reason"/> is set to Error).
        /// If Reason is not Error, ErrorCode returns NoError.
        /// Added in version 1.1.0.
        /// </summary>
        public CancellationErrorCode ErrorCode { get; }

        /// <summary>
        /// The error message in case of an unsuccessful recognition (Reason<see cref="Reason"/> is set to Error).
        /// This field is only filled-out if Reason is Error.
        /// </summary>
        public string ErrorDetails { get; }

        /// <summary>
        /// Returns a string that represents the speech recognition result event.
        /// </summary>
        /// <returns>A string that represents the speech recognition result event.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"SessionId:{0} ResultId:{1} CancellationReason:{2}. CancellationErrorCode:{3}", SessionId, Result.ResultId, Reason, ErrorCode);
        }

        // Hold the reference
        private Internal.SpeechRecognitionCanceledEventArgs eventArgImpl;
    }
}
