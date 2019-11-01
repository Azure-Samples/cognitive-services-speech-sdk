//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Transcription
{
    /// <summary>
    /// Defines a payload for <see cref="ConversationTranslator.Transcribing"/>, and
    /// <see cref="ConversationTranslator.Transcribed"/> events.
    /// Added in 1.8.0.
    /// </summary>
    public class ConversationTranslationEventArgs : RecognitionEventArgs
    {
        /// <summary>
        /// Creates a new instance.
        /// </summary>
        /// <param name="handle">The event handle.</param>
        internal ConversationTranslationEventArgs(IntPtr handle)
            : base(handle)
        {
            IntPtr resultHandle;
            ThrowIfFail(Internal.Recognizer.recognizer_recognition_event_get_result(eventHandle, out resultHandle));
            Result = new ConversationTranslationResult(resultHandle);
        }

        /// <summary>
        /// Gets the conversation translation result.
        /// </summary>
        public ConversationTranslationResult Result { get; }

        /// <summary>
        /// Returns a string that represents the conversation translation event.
        /// </summary>
        /// <returns>The string representation.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture, "{0} Result:{1}", base.ToString(), Result.ToString());
        }
    }

    /// <summary>
    /// Defines a payload for the <see cref="ConversationTranslator.Canceled"/> event.
    /// Added in 1.8.0.
    /// </summary>
    public sealed class ConversationTranslationCanceledEventArgs : ConversationTranslationEventArgs
    {
        /// <summary>
        /// Creates a new instance.
        /// </summary>
        /// <param name="handle">The event handle.</param>
        internal ConversationTranslationCanceledEventArgs(IntPtr handle)
            : base(handle)
        {
            var cancellation = CancellationDetails.FromResult(Result);
            Reason = cancellation.Reason;
            ErrorCode = cancellation.ErrorCode;
            ErrorDetails = cancellation.ErrorDetails;
        }

        /// <summary>
        /// The reason the recognition was canceled.
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
    }
}
