//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Globalization;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Conversation
{
    /// <summary>
    /// Class that defines conversation transcriber event arguments.
    /// Added in version 1.5.0
    /// </summary>
    public class ConversationTranscriptionEventArgs : RecognitionEventArgs
    {
        internal ConversationTranscriptionEventArgs(IntPtr eventHandlerPtr)
            : base(eventHandlerPtr)
        {
            IntPtr result = IntPtr.Zero;
            ThrowIfFail(Internal.Recognizer.recognizer_recognition_event_get_result(eventHandle, out result));
            Result = new ConversationTranscriptionResult(result);
        }

        /// <summary>
        /// Represents the conversation transcription result.
        /// </summary>
        public ConversationTranscriptionResult Result { get; }

        /// <summary>
        /// Returns a string that represents the session id and the conversation transcription result event.
        /// </summary>
        /// <returns>A string that represents the conversation transcription event.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture, "SessionId:{0} ResultId:{1} Reason:{2} UserId:<{3}> Recognized text:<{4}>.",
                SessionId, Result.ResultId, Result.Reason, Result.UserId, Result.Text);
        }
    }

    /// <summary>
    /// Defines payload of conversation transcriber canceled result events.
    /// </summary>
    public sealed class ConversationTranscriptionCanceledEventArgs : ConversationTranscriptionEventArgs
    {
        internal ConversationTranscriptionCanceledEventArgs(IntPtr eventHandlerPtr)
            : base(eventHandlerPtr)
        {
            var cancellation = CancellationDetails.FromResult(Result);
            Reason = (CancellationReason)cancellation.Reason;
            ErrorCode = (CancellationErrorCode)cancellation.ErrorCode;
            ErrorDetails = cancellation.ErrorDetails;
        }

        /// <summary>
        /// The reason the result was canceled.
        /// </summary>
        public CancellationReason Reason { get; }

        /// <summary>
        /// The error code in case of an unsuccessful recognition (<see cref="Reason"/> is set to Error).
        /// If Reason is not Error, ErrorCode returns NoError.
        /// </summary>
        public CancellationErrorCode ErrorCode { get; }

        /// <summary>
        /// The error message in case of an unsuccessful recognition (<see cref="Reason"/> is set to Error).
        /// </summary>
        public string ErrorDetails { get; }

        /// <summary>
        /// Returns a string that represents the session id and the conversation transcription result event.
        /// </summary>
        /// <returns>A string that represents the conversation transcriber event.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"SessionId:{0} ResultId:{1} CancellationReason:{2}. CancellationErrorCode:{3}", SessionId, Result.ResultId, Reason, ErrorCode);
        }
    }
}
