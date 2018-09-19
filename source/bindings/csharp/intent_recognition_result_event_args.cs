//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System.Globalization;
namespace Microsoft.CognitiveServices.Speech.Intent
{
    /// <summary>
    /// Define payload of intent recognizing/recognized events.
    /// </summary>
    public class IntentRecognitionEventArgs : RecognitionEventArgs
    {
        internal IntentRecognitionEventArgs(Internal.IntentRecognitionEventArgs e)
            : base(e)
        {
            eventArgImpl = e;
            Result = new IntentRecognitionResult(e.GetResult());
        }

        /// <summary>
        /// Represents the intent recognition result.
        /// </summary>
        public IntentRecognitionResult Result { get; }

        /// <summary>
        /// Returns a string that represents the session id and the intent recognition result event.
        /// </summary>
        /// <returns>A string that represents the intent recognition result event.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture, "SessionId:{0} ResultId:{1} Reason:{2} IntentId:<{3}> Recognized text:<{4}>.", 
                SessionId, Result.ResultId, Result.Reason, Result.IntentId, Result.Text);
        }

        // Hold the reference.
        private Internal.IntentRecognitionEventArgs eventArgImpl;
    }

    /// <summary>
    /// Define payload of intent recognition canceled result events.
    /// </summary>
    public sealed class IntentRecognitionCanceledEventArgs : IntentRecognitionEventArgs
    {
        internal IntentRecognitionCanceledEventArgs(Internal.IntentRecognitionCanceledEventArgs e)
            : base(e)
        {
            eventArgImpl = e;

            var cancellation = e.GetCancellationDetails();
            Reason = (CancellationReason)cancellation.Reason;
            ErrorDetails = cancellation.ErrorDetails;
        }

        /// <summary>
        /// The reason the result was canceled.
        /// </summary>
        public CancellationReason Reason { get; }

        /// <summary>
        /// In case of an unsuccessful recognition, provides a details of why the occurred error.
        /// This field is only filled-out if the reason canceled (<see cref="CancellationReason"/>) is set to Error.
        /// </summary>
        public string ErrorDetails { get; }

        /// <summary>
        /// Returns a string that represents the session id and the intent recognition result event.
        /// </summary>
        /// <returns>A string that represents the intent recognition result event.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture, "SessionId:{0} ResultId:{1} CancellationReason:{2}.", 
                SessionId, Result.ResultId, Reason);
        }

        // Hold the reference.
        private Internal.IntentRecognitionCanceledEventArgs eventArgImpl;
    }
}
