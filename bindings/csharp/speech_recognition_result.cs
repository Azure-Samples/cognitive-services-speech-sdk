//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace Carbon.Recognition.Speech
{
    /// <summary>
    /// Defines result of speech recognition.
    /// </summary>
    public class SpeechRecognitionResult
    {
        internal SpeechRecognitionResult(Internal.SpeechRecognitionResult result)
        {
            this.ResultId = result.ResultId;
            this.Reason = GetReason(result.Reason);
            this.Text = result.Text;
        }

        /// <summary>
        /// Specifies the result identifier.
        /// </summary>
        public string ResultId { get; }

        /// <summary>
        /// Specifies status of the result.
        /// </summary>
        public SpeechRecognitionReason Reason { get; }

        /// <summary>
        /// Presents the recognized text in the result.
        /// </summary>
        public string Text { get; }

        // public PayloadItems Payload { get; }

        private SpeechRecognitionReason GetReason(Internal.Reason reasonInternal)
        {
            SpeechRecognitionReason reason;

            switch (reasonInternal)
            {
                case Internal.Reason.Recognized:
                    reason = SpeechRecognitionReason.Recognized;
                    break;

                case Internal.Reason.IntermediateResult:
                    reason = SpeechRecognitionReason.IntermediateResult;
                    break;

                case Internal.Reason.NoMatch:
                    reason = SpeechRecognitionReason.NoMatch;
                    break;

                case Internal.Reason.Canceled:
                    reason = SpeechRecognitionReason.Canceled;
                    break;

                case Internal.Reason.OtherRecognizer:
                    reason = SpeechRecognitionReason.OtherRecognizer;
                    break;

                default:
                    throw new IndexOutOfRangeException("Unknown reason returned.");
            }

            return reason;
        }
    }
}
